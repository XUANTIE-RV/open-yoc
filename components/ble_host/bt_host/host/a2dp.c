/** @file
 * @brief Advance Audio Distribution Profile.
 */

/*
 * Copyright (c) 2015-2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <ble_os.h>
#include <string.h>
#include <errno.h>
#include <atomic.h>
#include <misc/byteorder.h>
#include <misc/util.h>
#include <misc/printk.h>
#include <assert.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/l2cap.h>
#include <bluetooth/avdtp.h>
#include <bluetooth/a2dp.h>
#include <bluetooth/a2dp-codec.h>
#include <oi_codec_sbc.h>

#define BT_DBG_ENABLED  1 // IS_ENABLED(CONFIG_BT_DEBUG_A2DP)
#define LOG_MODULE_NAME bt_a2dp
#include "common/log.h"

#include "hci_core.h"
#include "conn_internal.h"
#include "avdtp_internal.h"
#include "a2dp_internal.h"

#define A2DP_NO_SPACE (-1)

struct bt_a2dp {
    struct bt_avdtp session;
};

/** Stream end point sbc media capability */
static const struct bt_a2dp_codec_sbc_params sbc_param = {
    .config[0] = A2DP_SBC_SAMP_FREQ_16000 | A2DP_SBC_SAMP_FREQ_32000
                 | A2DP_SBC_SAMP_FREQ_44100 | A2DP_SBC_SAMP_FREQ_48000
                 | A2DP_SBC_CH_MODE_MONO | A2DP_SBC_CH_MODE_DUAL
                 | A2DP_SBC_CH_MODE_STREO | A2DP_SBC_CH_MODE_JOINT,
    .config[1] = A2DP_SBC_BLK_LEN_4 | A2DP_SBC_BLK_LEN_8 | A2DP_SBC_BLK_LEN_12
                 | A2DP_SBC_BLK_LEN_16 | A2DP_SBC_SUBBAND_4 | A2DP_SBC_SUBBAND_8
                 | A2DP_SBC_ALLOC_MTHD_SNR | A2DP_SBC_ALLOC_MTHD_LOUDNESS,
    .min_bitpool = 0,
    .max_bitpool = 250,
};

/** Stream end point media capability */
static const struct bt_avdtp_media_codec_cap media_cap = {
    .media_type     = BT_AVDTP_AUDIO,
    .codec_type     = BT_AVDTP_SBC,
    .codec_info_len = sizeof(struct bt_a2dp_codec_sbc_params),
    .codec_info     = (u8_t *)&sbc_param,
};

/** Stream end point capability */
static const struct bt_avdtp_cap cap = {
    BIT(BT_AVDTP_SERVICE_CAT_MEDIA_TRANSPORT) | BIT(BT_AVDTP_SERVICE_CAT_MEDIA_CODEC),
    (struct bt_avdtp_media_codec_cap *)&media_cap,
};

/** Stream end point sbc media configuration */
static struct bt_a2dp_codec_sbc_params sbc_param_conf;

/** Stream end point media configuration */
static struct bt_avdtp_media_codec_cap media_cap_conf = {
    .codec_info = (u8_t *)&sbc_param_conf,
};

/** Stream end point configuration */
static struct bt_avdtp_cap conf = {
    .media_codec = &media_cap_conf,
};

/** Stream end point */
static struct bt_avdtp_seid_lsep sink_sep = {
    .caps = (struct bt_avdtp_cap *)&cap,
    .conf = &conf,
};

/** Connections */
static struct bt_a2dp connection[CONFIG_BT_BR_MAX_CONN];

static void a2d_reset(struct bt_a2dp *a2dp_conn)
{
    (void)memset(a2dp_conn, 0, sizeof(struct bt_a2dp));
}

static struct bt_a2dp *get_new_connection(struct bt_conn *conn, int *errno)
{
    s8_t i, free;

    free = A2DP_NO_SPACE;

    if (!conn) {
        BT_ERR("Invalid Input (err: %d)", -EINVAL);
        *errno = -EINVAL;
        return NULL;
    }

    /* Find a space */
    for (i = 0; i < CONFIG_BT_BR_MAX_CONN; i++) {
        if (connection[i].session.br_chan.chan.conn == conn) {
            BT_DBG("Conn already exists");
            *errno = -EALREADY;
            return &connection[i];
        }

        if (!connection[i].session.br_chan.chan.conn && free == A2DP_NO_SPACE) {
            free = i;
        }
    }

    if (free == A2DP_NO_SPACE) {
        BT_DBG("More connection cannot be supported");
        *errno = -ENOMEM;
        return NULL;
    }

    /* Clean the memory area before returning */
    a2d_reset(&connection[free]);

    return &connection[free];
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

static int a2dp_set_config_ind(struct bt_avdtp *session, struct bt_avdtp_cap *caps)
{
    BT_DBG("");
    return 0;
}

static int a2dp_open_ind(struct bt_avdtp *session)
{
    BT_DBG("");
    return 0;
}

static int a2dp_start_ind(struct bt_avdtp *session)
{
    BT_DBG("");
    return 0;
}

static int a2dp_suspend_ind(struct bt_avdtp *session)
{
    BT_DBG("");

    return 0;
}

static int a2dp_close_ind(struct bt_avdtp *session)
{
    BT_DBG("");

    return 0;
}

static int a2dp_abort_ind(struct bt_avdtp *session)
{
    BT_DBG("");

    return 0;
}

uint8_t pcm_data[0x100000];
uint32_t pcm_data_len = 0;

/** stream data recived */
static int a2dp_data(struct bt_avdtp_stream *stream, struct net_buf *buf)
{
    struct bt_a2dp_sbc_payload_hdr *    hdr;
    int                                 i;
    static OI_CODEC_SBC_DECODER_CONTEXT decoder_context;
    static OI_UINT32 context_data[CODEC_DATA_WORDS(2, SBC_CODEC_MIN_FILTER_BUFFERS)];
    static OI_INT16  output_buf[SBC_MAX_SAMPLES_PER_FRAME * SBC_MAX_CHANNELS];
    static int       init_flag = 0;
    OI_UINT32        out_size = sizeof(output_buf);
    OI_STATUS        status;
    const OI_BYTE *  oi_data;
    uint32_t         oi_size;

    hdr     = net_buf_pull_mem(buf, sizeof(*hdr));
    oi_size = buf->len;
    oi_data = net_buf_pull_mem(buf, buf->len);

    if (!init_flag) {
        OI_CODEC_SBC_DecoderReset(&decoder_context, context_data, sizeof(context_data),
                                  2, 2, 0, 0);
        init_flag = 1;
    }

    for (i = 0; i < hdr->nof; i++) {
        status   = OI_CODEC_SBC_DecodeFrame(&decoder_context,
                                          &oi_data, &oi_size, output_buf, &out_size);
        if (!OI_SUCCESS(status)) {
            BT_ERR("%s: Decoding failure: %d", __func__, status);
            return false;
        }

        if (pcm_data_len + out_size < sizeof(pcm_data)) {
            memcpy(pcm_data + pcm_data_len, output_buf, out_size);
            pcm_data_len =  pcm_data_len + out_size;
        }

        BT_DBG("oi_size: %d, out_size: %d", oi_size, out_size);
    }

    return 0;
}

/** Callback for incoming requests */
static struct bt_avdtp_ind_cb cb_ind = {
    .set_config_ind = a2dp_set_config_ind,
    .open_ind       = a2dp_open_ind,
    .start_ind      = a2dp_start_ind,
    .suspend_ind    = a2dp_suspend_ind,
    .close_ind      = a2dp_close_ind,
    .abort_ind      = a2dp_abort_ind,
};

/* The above callback structures need to be packed and passed to AVDTP */
static struct bt_avdtp_event_cb avdtp_cb = {
    .ind    = &cb_ind,
    .accept = a2dp_accept,
    .data   = a2dp_data,
};

int bt_a2dp_init(void)
{
    int err;

    /** Register event handlers with AVDTP */
    err = bt_avdtp_register(&avdtp_cb);
    if (err < 0) {
        BT_ERR("A2DP registration failed");
        return err;
    }

    bt_avdtp_register_sep(BT_AVDTP_AUDIO, BT_A2DP_SINK, &sink_sep);

    BT_DBG("A2DP Initialized successfully.");
    return 0;
}

struct bt_a2dp *bt_a2dp_connect(struct bt_conn *conn)
{
    struct bt_a2dp *a2dp_conn;
    int             err;

    a2dp_conn = get_new_connection(conn, &err);
    if (!a2dp_conn) {
        BT_ERR("Cannot allocate memory");
        return NULL;
    }

    err = bt_avdtp_connect(conn, &(a2dp_conn->session));
    if (err < 0) {
        /** If error occurs, undo the saving and return the error */
        a2d_reset(a2dp_conn);
        BT_DBG("AVDTP Connect failed");
        return NULL;
    }

    BT_DBG("Connect request sent");
    return a2dp_conn;
}

int bt_a2dp_register_endpoint(struct bt_a2dp_endpoint *endpoint, u8_t media_type,
                              u8_t role)
{
    int err;

    BT_ASSERT(endpoint);

    err = bt_avdtp_register_sep(media_type, role, &(endpoint->info));
    if (err < 0) {
        return err;
    }

    return 0;
}
