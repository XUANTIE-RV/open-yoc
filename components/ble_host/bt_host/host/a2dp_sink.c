/**
 * Copyright (c) 2022  Alibaba Group Holding Limited
 */

#include <ble_os.h>
#include <string.h>
#include <errno.h>
#include <atomic.h>
#include <misc/byteorder.h>
#include <misc/util.h>

#include <aos/bt.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/l2cap.h>
#include <bluetooth/sdp.h>
#include <bluetooth/conn.h>
#include <bluetooth/a2dp-codec.h>
#include <bluetooth/audio_decode.h>
#if CONFIG_BT_A2DP_AUDIO_OUTPUT_INSIDE
#include <bluetooth/audio_output.h>
#endif

#define BT_DBG_ENABLED  IS_ENABLED(CONFIG_BT_DEBUG_A2DP)
#define LOG_MODULE_NAME bt_a2dp
#include "common/log.h"

#include "hci_core.h"
#include "conn_internal.h"
#include "avdtp_internal.h"
#include "a2dp_internal.h"

#ifndef CONFIG_BT_AUDIO_FRAME_NUM
#define CONFIG_BT_AUDIO_FRAME_NUM 6
#endif

#if CONFIG_BT_A2DP_DECODE_SBC
#define BT_A2DP_AUDIO_MAX_FRAME_LEN (BT_A2DP_SBC_MAX_CHANNELS * BT_A2DP_SBC_MAX_BANDS * BT_A2DP_SBC_MAX_BLOCKS)
#else
#define BT_A2DP_AUDIO_MAX_FRAME_LEN 0
#endif

#define A2DP_NO_SPACE (-1)

#define A2DP_EVENT_CALL(evt, param)                                                                                    \
    do {                                                                                                               \
        if (a2dp_cb) {                                                                                                 \
            a2dp_cb(evt, param);                                                                                       \
        }                                                                                                              \
    } while (0);

#define A2DP_DATA_CALL(data, size)                                                                                     \
    do {                                                                                                               \
        if (a2dp_data_cb) {                                                                                            \
            a2dp_data_cb((u8_t *)data, size);                                                                          \
        }                                                                                                              \
    } while (0);

static void acl_connected(struct bt_conn *conn, u8_t err);

#if CONFIG_BT_A2DP_DECODE_SBC
/** Stream end point sbc media capability */
static const bt_prf_a2dp_codec_sbc_params_t sbc_param = {
    .config[0] = BT_PRF_A2DP_SBC_SAMP_FREQ_16000 | BT_PRF_A2DP_SBC_SAMP_FREQ_32000 | BT_PRF_A2DP_SBC_SAMP_FREQ_44100
                 | BT_PRF_A2DP_SBC_SAMP_FREQ_48000 | BT_PRF_A2DP_SBC_CH_MODE_MONO | BT_PRF_A2DP_SBC_CH_MODE_DUAL
                 | BT_PRF_A2DP_SBC_CH_MODE_STREO | BT_PRF_A2DP_SBC_CH_MODE_JOINT,
    .config[1] = BT_PRF_A2DP_SBC_BLK_LEN_4 | BT_PRF_A2DP_SBC_BLK_LEN_8 | BT_PRF_A2DP_SBC_BLK_LEN_12
                 | BT_PRF_A2DP_SBC_BLK_LEN_16 | BT_PRF_A2DP_SBC_SUBBAND_4 | BT_PRF_A2DP_SBC_SUBBAND_8
                 | BT_PRF_A2DP_SBC_ALLOC_MTHD_SNR | BT_PRF_A2DP_SBC_ALLOC_MTHD_LOUDNESS,
    .min_bitpool = 0,
    .max_bitpool = 250,
};

/** Stream end point media capability */
static const struct bt_avdtp_media_codec_cap media_cap = {
    .media_type     = BT_AVDTP_AUDIO,
    .codec_type     = BT_A2DP_SBC,
    .codec_info_len = sizeof(sbc_param),
    .codec_info     = (u8_t *)&sbc_param,
};

/** Stream end point capability */
static const struct bt_avdtp_cap cap = {
    BIT(BT_AVDTP_SERVICE_CAT_MEDIA_TRANSPORT) | BIT(BT_AVDTP_SERVICE_CAT_MEDIA_CODEC),
    (struct bt_avdtp_media_codec_cap *)&media_cap,
};

/** Stream end point sbc media configuration */
static bt_prf_a2dp_codec_sbc_params_t sbc_param_conf;

/** Stream end point media configuration */
static struct bt_avdtp_media_codec_cap media_cap_conf = {
    .codec_info_len = sizeof(sbc_param_conf),
    .codec_info = (u8_t *)&sbc_param_conf,
};

/** Stream end point configuration */
static struct bt_avdtp_cap conf = {
    .media_codec = &media_cap_conf,
};

/** Stream end point */
static struct bt_avdtp_seid_lsep sink_sbc_sep = {
    .caps = (struct bt_avdtp_cap *)&cap,
    .conf = &conf,
};
#endif

/** Connections */
static struct bt_a2dp a2dp_connection[CONFIG_BT_BR_MAX_CONN];

static bt_prf_a2dp_cb_t a2dp_cb;

static bt_prf_a2dp_sink_data_cb_t a2dp_data_cb;

#if CONFIG_BT_A2DP_AUDIO_OUTPUT_INSIDE
static const bt_a2dp_audio_output_t *a2dp_audio_output;
#if CONFIG_BT_A2DP_DECODE_SBC
static s16_t a2dp_audio_buf[BT_A2DP_AUDIO_MAX_FRAME_LEN * CONFIG_BT_AUDIO_FRAME_NUM];
#endif
#endif

static bt_a2dp_audio_decode_t *a2dp_audio_decode;

static void a2d_reset(struct bt_a2dp *a2dp_conn)
{
    (void)memset(a2dp_conn, 0, sizeof(struct bt_a2dp));
}

static struct bt_a2dp *get_new_connection(struct bt_conn *conn, int *errno)
{
    s8_t i, free;

    free = A2DP_NO_SPACE;

    /* Find a space */
    for (i = 0; i < CONFIG_BT_BR_MAX_CONN; i++) {
        if (a2dp_connection[i].session.br_chan.chan.conn == conn) {
            BT_DBG("Conn already exists");
            *errno = -EALREADY;
            return &a2dp_connection[i];
        }

        if (!a2dp_connection[i].session.br_chan.chan.conn && free == A2DP_NO_SPACE) {
            free = i;
        }
    }

    if (free == A2DP_NO_SPACE) {
        BT_DBG("More a2dp_connection cannot be supported");
        *errno = -ENOMEM;
        return NULL;
    }

    /* Clean the memory area before returning */
    a2d_reset(&a2dp_connection[free]);

    return &a2dp_connection[free];
}

static struct bt_a2dp *find_connection(struct bt_conn *conn)
{
    s8_t i;

    if (!conn) {
        BT_ERR("Invalid Input (err: %d)", -EINVAL);
        return NULL;
    }

    for (i = 0; i < CONFIG_BT_BR_MAX_CONN; i++) {
        if (a2dp_connection[i].session.br_chan.chan.conn == conn) {
            return &a2dp_connection[i];
        }
    }

    return NULL;
}

static void a2dp_report_connection_state(struct bt_conn *conn, uint8_t state)
{
    bt_prf_a2dp_cb_param_t param;

    param.conn_stat.peer_addr.type = BT_DEV_ADDR_TYPE_PUBLIC;
    memcpy(param.conn_stat.peer_addr.val, conn->br.dst.val, BT_STACK_BD_ADDR_LEN);
    param.conn_stat.state = state;
    A2DP_EVENT_CALL(BT_PRF_A2DP_CONNECTION_STATE_EVT, &param);
}

static void a2dp_report_audio_state(struct bt_conn *conn, uint8_t state)
{
    bt_prf_a2dp_cb_param_t param;

    param.conn_stat.peer_addr.type = BT_DEV_ADDR_TYPE_PUBLIC;
    memcpy(param.audio_stat.peer_addr.val, conn->br.dst.val, BT_STACK_BD_ADDR_LEN);
    param.audio_stat.state = state;
    A2DP_EVENT_CALL(BT_PRF_A2DP_AUDIO_STATE_EVT, &param);
}

static int a2dp_accept(struct bt_conn *conn, struct bt_avdtp **session)
{
    struct bt_a2dp *a2dp_conn;
    int             err = 0;

    a2dp_conn = get_new_connection(conn, &err);
    if (!a2dp_conn) {
        return err;
    }

    *session = &(a2dp_conn->session);
    BT_DBG("session: %p", &(a2dp_conn->session));

    return err;
}

static int a2dp_set_config_ind(struct bt_avdtp *session, struct bt_avdtp_cap *conf)
{
#if CONFIG_BT_A2DP_DECODE_SBC
    u8_t                   channel     = 0;
    u8_t                   block       = 0;
    u8_t                   subband     = 0;
    u32_t                  sample_rate = 0;
    u8_t                   channel_bit;
    u8_t                   block_bit;
    u8_t                   subband_bit;
    u8_t                   sample_rate_bit;
    bt_prf_a2dp_cb_param_t param;

    bt_prf_a2dp_codec_sbc_params_t *sbc = (bt_prf_a2dp_codec_sbc_params_t *)conf->media_codec->codec_info;

    channel_bit     = sbc->config[0] & 0x0f;
    subband_bit     = sbc->config[1] & 0x0c;
    block_bit       = sbc->config[1] & 0xf0;
    sample_rate_bit = sbc->config[0] & 0xf0;

    /* Channel Mode */
    switch (channel_bit) {
        case BT_PRF_A2DP_SBC_CH_MODE_MONO:
            channel = 1;
            break;
        case BT_PRF_A2DP_SBC_CH_MODE_DUAL:
        case BT_PRF_A2DP_SBC_CH_MODE_STREO:
        case BT_PRF_A2DP_SBC_CH_MODE_JOINT:
            channel = 2;
            break;
    }

    /* Subbands */
    switch (subband_bit) {
        case BT_PRF_A2DP_SBC_SUBBAND_8:
            subband = 8;
            break;
        case BT_PRF_A2DP_SBC_SUBBAND_4:
            subband = 4;
            break;
    }

    /* Block Length */
    switch (block_bit) {
        case BT_PRF_A2DP_SBC_BLK_LEN_4:
            block = 4;
            break;
        case BT_PRF_A2DP_SBC_BLK_LEN_8:
            block = 8;
            break;
        case BT_PRF_A2DP_SBC_BLK_LEN_12:
            block = 12;
            break;
        case BT_PRF_A2DP_SBC_BLK_LEN_16:
            block = 16;
            break;
    }

    /* Sampling Frequency */
    switch (sample_rate_bit) {
        case BT_PRF_A2DP_SBC_SAMP_FREQ_16000:
            sample_rate = 16000;
            break;
        case BT_PRF_A2DP_SBC_SAMP_FREQ_32000:
            sample_rate = 32000;
            break;
        case BT_PRF_A2DP_SBC_SAMP_FREQ_44100:
            sample_rate = 44100;
            break;
        case BT_PRF_A2DP_SBC_SAMP_FREQ_48000:
            sample_rate = 48000;
            break;
    }

    BT_DBG("channel: %d, subband: %d, block: %d, sample_rate: %d", channel, subband, block, sample_rate);
#if CONFIG_BT_A2DP_AUDIO_OUTPUT_INSIDE
    audio_output_conf_t audio_conf;

    audio_conf.channel     = channel;
    audio_conf.sample_rate = sample_rate;
    audio_conf.buf         = a2dp_audio_buf;
    /** frame_len = num of block * num of subband * channel * samplebit / 8 */
    audio_conf.frame_len = block * subband * channel * 2;
    audio_conf.frame_num = CONFIG_BT_AUDIO_FRAME_NUM;

    a2dp_audio_output->conf(&audio_conf);
#endif

    param.audio_cfg.peer_addr.type = BT_DEV_ADDR_TYPE_PUBLIC;
    memcpy(param.audio_cfg.peer_addr.val, &(session->br_chan.chan.conn->br.dst), BT_STACK_BD_ADDR_LEN);
    memcpy(&param.audio_cfg.sbc, sbc, sizeof(*sbc));

    A2DP_EVENT_CALL(BT_PRF_A2DP_AUDIO_SBC_CFG_EVT, &param);
#endif
    return 0;
}

static int a2dp_open_ind(struct bt_avdtp *session)
{
    BT_DBG("");
    a2dp_report_connection_state(session->br_chan.chan.conn, BT_PRF_A2DP_CONNECTION_STATE_CONNECTED);

#if CONFIG_BT_A2DP_AUDIO_OUTPUT_INSIDE
    a2dp_audio_output->open();
#endif

    return 0;
}

static int a2dp_start_ind(struct bt_avdtp *session)
{
    BT_DBG("");
    a2dp_report_audio_state(session->br_chan.chan.conn, BT_PRF_A2DP_AUDIO_STATE_STARTED);

#if CONFIG_BT_A2DP_AUDIO_OUTPUT_INSIDE
    a2dp_audio_output->start();
#endif

    return 0;
}

static int a2dp_suspend_ind(struct bt_avdtp *session)
{
    BT_DBG("");
    a2dp_report_audio_state(session->br_chan.chan.conn, BT_PRF_A2DP_AUDIO_STATE_REMOTE_SUSPEND);

#if CONFIG_BT_A2DP_AUDIO_OUTPUT_INSIDE
    a2dp_audio_output->stop();
#endif
    return 0;
}

static int a2dp_close_ind(struct bt_avdtp *session)
{
    BT_DBG("");
    a2dp_report_connection_state(session->br_chan.chan.conn, BT_PRF_A2DP_CONNECTION_STATE_DISCONNECTING);

#if CONFIG_BT_A2DP_AUDIO_OUTPUT_INSIDE
    a2dp_audio_output->close();
#endif

    return 0;
}

static int a2dp_abort_ind(struct bt_avdtp *session)
{
    BT_DBG("");

    return 0;
}

/** stream data recived */
static int a2dp_data(struct bt_avdtp_stream *stream, struct bt_avdtp_seid_lsep *sep, struct net_buf *buf)
{
    struct bt_a2dp_sbc_payload_hdr *hdr;
    bt_a2dp_audio_decode_t *        audio_decode = a2dp_audio_decode;
    int                             i;
    u8_t *                          in_data;
    u32_t                           in_size;
    int                             ret;

    hdr     = net_buf_pull_mem(buf, sizeof(*hdr));
    in_size = buf->len;
    in_data = net_buf_pull_mem(buf, buf->len);

    for (i = 0; i < hdr->nof; i++) {
#if CONFIG_BT_A2DP_AUDIO_OUTPUT_INSIDE
        s16_t *out_data;
        u32_t  out_size;
        a2dp_audio_output->get_write_frame_buf((void *)&out_data, &out_size);
#else
        static u16_t out_data[BT_A2DP_AUDIO_MAX_FRAME_LEN];
        u32_t        out_size = sizeof(out_data);
#endif
        for (; audio_decode; audio_decode = audio_decode->next) {
            if (audio_decode->codec_type == sep->caps->media_codec->codec_type) {
                ret = audio_decode->decode((void **)&in_data, &in_size, out_data, &out_size);

                if (ret < 0) {
                    BT_ERR("decode failed %d", ret);
                    return -1;
                }
                break;
            }
        }

#if CONFIG_BT_A2DP_AUDIO_OUTPUT_INSIDE
        a2dp_audio_output->write_done(out_data, out_size);
#endif

        A2DP_DATA_CALL(out_data, out_size);

#ifdef BT_A2DP_AUDIO_DUMP_DEBUG
        static uint8_t  pcm_data[0x100000];
        static uint32_t pcm_data_len = 0;
        if (pcm_data_len + out_size < sizeof(pcm_data)) {
            memcpy(pcm_data + pcm_data_len, out_data, out_size);
            pcm_data_len = pcm_data_len + out_size;
        }
#endif

        BT_DBG("num of frame: %d, in_size: %d, out_size: %d", hdr->nof, in_size, out_size);
    }

    return 0;
}

static int a2dp_connected(struct bt_avdtp *session)
{
    a2dp_report_connection_state(session->br_chan.chan.conn, BT_PRF_A2DP_CONNECTION_STATE_CONNECTING);

    return 0;
}

static int a2dp_disconnected(struct bt_avdtp *session)
{
    int reason = session->br_chan.chan.conn->err;
    BT_DBG("err %d", session->br_chan.chan.conn->err);

    if (reason == BT_HCI_ERR_CONN_TIMEOUT) {
        a2dp_report_connection_state(session->br_chan.chan.conn, BT_PRF_A2DP_CONNECTION_STATE_DISCONNECTED_ABNORMAL);
    } else {
        a2dp_report_connection_state(session->br_chan.chan.conn, BT_PRF_A2DP_CONNECTION_STATE_DISCONNECTED_NORMAL);
    }

    return 0;
}

static struct bt_conn_cb acl_callbacks = {
    .connected = acl_connected,
};

static void acl_connected(struct bt_conn *conn, u8_t err)
{
    struct bt_a2dp *a2dp_conn;
    int             errno;

    BT_DBG("conn %p err %d", conn, err);

    if (err == 0) {
        a2dp_conn = get_new_connection(conn, &errno);
        if (!a2dp_conn) {
            BT_ERR("Cannot allocate memory");
            return;
        }

        err = bt_avdtp_connect(conn, &(a2dp_conn->session));
        if (err < 0) {
            /** If error occurs, undo the saving and return the error */
            a2d_reset(a2dp_conn);
            BT_DBG("AVDTP Connect failed err %d", err);
            return;
        }

        BT_DBG("Connect request sent");
    } else {
        a2dp_report_connection_state(conn, BT_PRF_A2DP_CONNECTION_STATE_DISCONNECTED_NORMAL);
    }

    /** unregister callback to avoid double notify */
    bt_conn_cb_unregister(&acl_callbacks);
}

/** Callback for incoming requests */
static struct bt_avdtp_ind_cb cb_ind = {
    .connected      = a2dp_connected,
    .disconnected   = a2dp_disconnected,
    .set_config_ind = a2dp_set_config_ind,
    .open_ind       = a2dp_open_ind,
    .start_ind      = a2dp_start_ind,
    .suspend_ind    = a2dp_suspend_ind,
    .close_ind      = a2dp_close_ind,
    .abort_ind      = a2dp_abort_ind,
};

/** The above callback structures need to be packed and passed to AVDTP */
static struct bt_avdtp_event_cb avdtp_cb = {
    .ind    = &cb_ind,
    .accept = a2dp_accept,
    .data   = a2dp_data,
};

static struct bt_sdp_attribute a2dp_attrs[] = {
    BT_SDP_NEW_SERVICE,
    BT_SDP_LIST(
        BT_SDP_ATTR_SVCLASS_ID_LIST, BT_SDP_TYPE_SIZE_VAR(BT_SDP_SEQ8, 3),
        BT_SDP_DATA_ELEM_LIST({ BT_SDP_TYPE_SIZE(BT_SDP_UUID16), BT_SDP_ARRAY_16(BT_SDP_AUDIO_SINK_SVCLASS) }, ) ),
    BT_SDP_LIST(BT_SDP_ATTR_PROTO_DESC_LIST, BT_SDP_TYPE_SIZE_VAR(BT_SDP_SEQ8, 16),
                BT_SDP_DATA_ELEM_LIST(
                    {
                        BT_SDP_TYPE_SIZE_VAR(BT_SDP_SEQ8, 6),
                        BT_SDP_DATA_ELEM_LIST({ BT_SDP_TYPE_SIZE(BT_SDP_UUID16), BT_SDP_ARRAY_16(BT_SDP_PROTO_L2CAP) },
                                              {
                                                  BT_SDP_TYPE_SIZE(BT_SDP_UINT16),
                                                  BT_SDP_ARRAY_16(BT_L2CAP_PSM_AVDTP),
                                              }, ),
                    },
                    {
                        BT_SDP_TYPE_SIZE_VAR(BT_SDP_SEQ8, 6),
                        BT_SDP_DATA_ELEM_LIST({ BT_SDP_TYPE_SIZE(BT_SDP_UUID16), BT_SDP_ARRAY_16(BT_SDP_PROTO_AVDTP) },
                                              {
                                                  BT_SDP_TYPE_SIZE(BT_SDP_UINT16),
                                                  BT_SDP_ARRAY_16(BT_AVDTP_VERSION), // version
                                              }, ),
                    }, ) ),
    BT_SDP_LIST(BT_SDP_ATTR_PROFILE_DESC_LIST, BT_SDP_TYPE_SIZE_VAR(BT_SDP_SEQ8, 8),
                BT_SDP_DATA_ELEM_LIST(
                    {
                        BT_SDP_TYPE_SIZE_VAR(BT_SDP_SEQ8, 6),
                        BT_SDP_DATA_ELEM_LIST(
                            { BT_SDP_TYPE_SIZE(BT_SDP_UUID16), BT_SDP_ARRAY_16(BT_SDP_ADVANCED_AUDIO_SVCLASS) },
                            {
                                BT_SDP_TYPE_SIZE(BT_SDP_UINT16),
                                BT_SDP_ARRAY_16(BT_A2DP_VERSION), // version
                            }, ),
                    }, ) ),
    BT_SDP_SUPPORTED_FEATURES(0x0001),
};

static struct bt_sdp_record a2dp_rec = BT_SDP_RECORD(a2dp_attrs);

/** A2DP profile init */
bt_stack_status_t bt_prf_a2dp_sink_init(void)
{
    bt_stack_status_t err;

    bt_avdtp_init();

    /** Register event handlers with AVDTP */
    err = bt_avdtp_register(&avdtp_cb);
    if (err < 0) {
        BT_ERR("A2DP registration failed");
        return err;
    }

    bt_sdp_register_service(&a2dp_rec);

#if CONFIG_BT_A2DP_AUDIO_OUTPUT_INSIDE
    a2dp_audio_output = bt_a2dp_audio_output_get_interface();
    a2dp_audio_output->init();
#endif

#if CONFIG_BT_A2DP_DECODE_SBC
    bt_avdtp_register_sep(BT_AVDTP_AUDIO, BT_A2DP_SINK, &sink_sbc_sep);
    /** add to decode list if support multil decoder */
    a2dp_audio_decode = bt_a2dp_audio_decode_sbc_get_interface();
    a2dp_audio_decode->init();
#endif

    BT_DBG("A2DP Initialized successfully.");
    return 0;
}

/** A2DP register event callback */
void bt_prf_a2dp_register_callback(bt_prf_a2dp_cb_t callback)
{
    a2dp_cb = callback;
}

/** A2DP register media data callback */
void bt_prf_a2dp_sink_register_data_callback(bt_prf_a2dp_sink_data_cb_t callback)
{
    a2dp_data_cb = callback;
}

/** Connect to remote bluetooth A2DP source */
bt_stack_status_t bt_prf_a2dp_sink_connect(bt_dev_addr_t *peer_addr)
{
    struct bt_a2dp *a2dp_conn;
    int             err;
    struct bt_conn *conn;
    bt_addr_t       addr;

    memcpy(addr.val, peer_addr->val, BT_STACK_BD_ADDR_LEN);
    conn = bt_conn_lookup_addr_br(&addr);

    if (conn == NULL) {
        conn = bt_conn_create_br(&addr, BT_BR_CONN_PARAM_DEFAULT);

        if (!conn) {
            BT_ERR("Connection failed");
            return -ENOMEM;
        } else {

            BT_DBG("Connection pending");

            bt_conn_cb_register(&acl_callbacks);

            /* unref connection obj in advance as app user */
            bt_conn_unref(conn);

            return 0;
        }
    }

    /** unref connection for conn_lookup */
    bt_conn_unref(conn);

    a2dp_conn = get_new_connection(conn, &err);
    if (!a2dp_conn || err == -EALREADY) {
        BT_ERR("Already Connected");
        return -EALREADY;
    }

    err = bt_avdtp_connect(conn, &(a2dp_conn->session));
    if (err < 0) {
        /** If error occurs, undo the saving and return the error */
        a2d_reset(a2dp_conn);
        BT_DBG("AVDTP Connect failed err %d", err);
        return -EINVAL;
    }

    BT_DBG("Connect request sent");

    return 0;
}

/** Disconnect to remote bluetooth A2DP source */
bt_stack_status_t bt_prf_a2dp_sink_disconnect(bt_dev_addr_t *peer_addr)
{
    struct bt_a2dp *a2dp_conn;
    int             err;
    struct bt_conn *conn;
    bt_addr_t       addr;

    memcpy(addr.val, peer_addr->val, BT_STACK_BD_ADDR_LEN);
    conn = bt_conn_lookup_addr_br(&addr);

    if (!conn) {
        BT_DBG("NO Connection");
        return 0;
    }

    /** unref connection for conn_lookup */
    bt_conn_unref(conn);

    a2dp_conn = find_connection(conn);
    if (!a2dp_conn) {
        BT_DBG("NO A2DP Connection");
        return 0;
    }

    err = bt_avdtp_disconnect(&(a2dp_conn->session));
    if (err < 0) {
        /** If error occurs, undo the saving and return the error */
        BT_DBG("AVDTP Disonnect failed");
        return -EINVAL;
    }

    BT_DBG("Disconnect req send");

    return 0;
}
