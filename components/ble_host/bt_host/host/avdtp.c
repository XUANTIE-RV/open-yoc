/*
 * Audio Video Distribution Protocol
 *
 * Copyright (c) 2022  Alibaba Group Holding Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#include <ble_os.h>
#include <string.h>
#include <errno.h>
#include <atomic.h>
#include <misc/byteorder.h>
#include <misc/util.h>

#include <bluetooth/hci.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/l2cap.h>
#if (defined(CONFIG_BT_AVDTP) && CONFIG_BT_AVDTP)
#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BT_DEBUG_AVDTP)
#define LOG_MODULE_NAME bt_avdtp
#include "common/log.h"

#include "hci_core.h"
#include "conn_internal.h"
#include "l2cap_internal.h"
#include "avdtp_internal.h"

#define AVDTP_REPORT_EVENT(call, param)                                                                                \
    do {                                                                                                               \
        if (avdtp_event_cb && avdtp_event_cb->ind && avdtp_event_cb->ind->call) {                                      \
            avdtp_event_cb->ind->call(param);                                                                          \
        }                                                                                                              \
    } while (0);

#ifndef CONFIG_BT_AVDTP_MAX_STREAM
#define CONFIG_BT_AVDTP_MAX_STREAM 1
#endif

static struct bt_avdtp_event_cb *avdtp_event_cb;

static struct bt_avdtp_seid_lsep *avdtp_lseps;

static struct bt_avdtp_stream avdtp_streams[CONFIG_BT_AVDTP_MAX_STREAM];

#define AVDTP_CHAN(_ch)        CONTAINER_OF(_ch, struct bt_avdtp, br_chan.chan)
#define AVDTP_STREAM_CHAN(_ch) CONTAINER_OF(_ch, struct bt_avdtp_stream, chan.chan)
#define AVDTP_KWORK(_work)     CONTAINER_OF(_work, struct bt_avdtp_req, timeout_work)

#define AVDTP_TIMEOUT K_SECONDS(6)

static void avdtp_discover_resp(struct bt_avdtp *session, struct net_buf *buf, u8_t tid, u8_t signal_id);
static void avdtp_get_cap_resp(struct bt_avdtp *session, struct net_buf *buf, u8_t tid, u8_t signal_id);
static void avdtp_set_conf_resp(struct bt_avdtp *session, struct net_buf *buf, u8_t tid, u8_t signal_id);
static void avdtp_get_conf_resp(struct bt_avdtp *session, struct net_buf *buf, u8_t tid, u8_t signal_id);
static void avdtp_reconfig_resp(struct bt_avdtp *session, struct net_buf *buf, u8_t tid, u8_t signal_id);
static void avdtp_start_suspend_resp(struct bt_avdtp *session, struct net_buf *buf, u8_t tid, u8_t signal_id);
static void avdtp_open_close_resp(struct bt_avdtp *session, struct net_buf *buf, u8_t tid, u8_t signal_id);
static void avdtp_abord_resp(struct bt_avdtp *session, struct net_buf *buf, u8_t tid, u8_t signal_id);
#if (defined(CONFIG_BT_AVDTP_DELAY_REPORT) && CONFIG_BT_AVDTP_DELAY_REPORT)
static void avdtp_delayreport_resp(struct bt_avdtp *session, struct net_buf *buf, u8_t tid, u8_t signal_id);
#endif

static void bt_avdtp_l2cap_connected(struct bt_l2cap_chan *chan);
static void bt_avdtp_l2cap_disconnected(struct bt_l2cap_chan *chan);
static void bt_avdtp_l2cap_encrypt_changed(struct bt_l2cap_chan *chan, u8_t status);
static int  bt_avdtp_l2cap_recv(struct bt_l2cap_chan *chan, struct net_buf *buf);

static const struct bt_l2cap_chan_ops avdtp_l2cap_ops = {
    .connected      = bt_avdtp_l2cap_connected,
    .disconnected   = bt_avdtp_l2cap_disconnected,
    .encrypt_change = bt_avdtp_l2cap_encrypt_changed,
    .recv           = bt_avdtp_l2cap_recv,
};

static const struct {
    u8_t sig_id;
    void (*func)(struct bt_avdtp *session, struct net_buf *buf, u8_t tid, u8_t signal_id);
} handler[] = {
    {
        BT_AVDTP_DISCOVER,
        avdtp_discover_resp,
    },
    {
        BT_AVDTP_GET_CAPABILITIES,
        avdtp_get_cap_resp,
    },
    {
        BT_AVDTP_SET_CONFIGURATION,
        avdtp_set_conf_resp,
    },
    {
        BT_AVDTP_GET_CONFIGURATION,
        avdtp_get_conf_resp,
    },
    {
        BT_AVDTP_RECONFIGURE,
        avdtp_reconfig_resp,
    },
    {
        BT_AVDTP_OPEN,
        avdtp_open_close_resp,
    },
    {
        BT_AVDTP_START,
        avdtp_start_suspend_resp,
    },
    {
        BT_AVDTP_CLOSE,
        avdtp_open_close_resp,
    },
    {
        BT_AVDTP_SUSPEND,
        avdtp_start_suspend_resp,
    },
    {
        BT_AVDTP_ABORT,
        avdtp_abord_resp,
    },
    {
        BT_AVDTP_GET_ALL_CAPABILITIES,
        avdtp_get_cap_resp,
    },
#if (defined(CONFIG_BT_AVDTP_DELAY_REPORT) && CONFIG_BT_AVDTP_DELAY_REPORT)
    {
        BT_AVDTP_DELAYREPORT,
        avdtp_delayreport_resp,
    },
#endif
};

static u8_t avdtp_get_tid()
{
    static u8_t tid;

    /** Loop for 16*/
    tid %= 16;

    return tid++;
}

static int avdtp_is_valid_len(u8_t sigid, u32_t len)
{
    u32_t default_min_len = 0;

    switch (sigid) {
        case BT_AVDTP_GET_CAPABILITIES:
        case BT_AVDTP_SET_CONFIGURATION:
        case BT_AVDTP_GET_CONFIGURATION:
        case BT_AVDTP_RECONFIGURE:
        case BT_AVDTP_OPEN:
        case BT_AVDTP_START:
        case BT_AVDTP_CLOSE:
        case BT_AVDTP_SUSPEND:
        case BT_AVDTP_ABORT:
        case BT_AVDTP_GET_ALL_CAPABILITIES:
            default_min_len = 1;
            break;
        default:
            break;
    }

    return len >= default_min_len;
}

static struct bt_avdtp_stream *avdtp_new_stream(struct bt_conn *conn)
{
    s8_t i, free;

    free = -1;

    /* Find a space */
    for (i = 0; i < CONFIG_BT_AVDTP_MAX_STREAM; i++) {
        if (avdtp_streams[i].chan.chan.conn == conn) {
            BT_DBG("Conn already exists");
            return NULL;
        }

        if (!avdtp_streams[i].chan.chan.conn && free == -1) {
            free = i;
        }
    }

    if (free == -1) {
        BT_DBG("More Stream cannot be supported");
        return NULL;
    }

    return &avdtp_streams[free];
}

static struct bt_avdtp_stream *avdtp_find_stream(u8_t seid)
{
    s8_t i;

    for (i = 0; i < CONFIG_BT_AVDTP_MAX_STREAM; i++) {
        if (avdtp_streams[i].lseid == seid) {
            return &avdtp_streams[i];
        }
    }

    return NULL;
}

static void avdtp_clean_stream(u8_t seid)
{
    s8_t i;

    for (i = 0; i < CONFIG_BT_AVDTP_MAX_STREAM; i++) {
        if (avdtp_streams[i].lseid == seid) {
            avdtp_streams[i].lseid = 0;
            avdtp_streams[i].rseid = 0;
            avdtp_streams[i].state = BT_AVDTP_STREAM_STATE_IDLE;
            return;
        }
    }
}

static void avdtp_clean_all_stream()
{
    s8_t i;

    for (i = 0; i < CONFIG_BT_AVDTP_MAX_STREAM; i++) {
        avdtp_streams[i].lseid = 0;
        avdtp_streams[i].rseid = 0;
        avdtp_streams[i].state = BT_AVDTP_STREAM_STATE_IDLE;
    }
}

static struct bt_avdtp_seid_lsep *avdtp_find_lsep(u8_t seid)
{
    struct bt_avdtp_seid_lsep *sep = avdtp_lseps;

    for (; sep != NULL; sep = sep->next) {
        if (sep->sep.id == seid) {
            return sep;
        }
    }

    return NULL;
}

static void avdtp_clear_all_lsep()
{
    struct bt_avdtp_seid_lsep *sep = avdtp_lseps;

    for (; sep != NULL; sep = sep->next) {
        sep->sep.inuse = 0;
    }

    return;
}

static struct net_buf *avdtp_create_pdu(u8_t msg_type, u8_t pkt_type, u8_t sig_id, u8_t tid)
{
    struct net_buf *                buf;
    struct bt_avdtp_single_sig_hdr *hdr;

    buf = bt_l2cap_create_pdu(NULL, 0);

    hdr = net_buf_add(buf, sizeof(*hdr));

    hdr->msgtype   = msg_type;
    hdr->packtype  = pkt_type;
    hdr->tid       = tid;
    hdr->signal_id = sig_id;

    return buf;
}

static int avdtp_send(struct bt_avdtp *session, struct net_buf *buf, struct bt_avdtp_req *req)
{
    int                             result;
    struct bt_avdtp_single_sig_hdr *hdr;

    result = bt_l2cap_chan_send(&session->br_chan.chan, buf);
    if (result < 0) {
        net_buf_unref(buf);
        BT_ERR("L2CAP send fail - result = %d", result);
        return result;
    }

    if (req) {
        hdr = (struct bt_avdtp_single_sig_hdr *)buf->data;

        /*Save the sent request*/
        req->sig = hdr->signal_id;
        req->tid = hdr->tid;
        BT_DBG("sig 0x%02X, tid 0x%02X", req->sig, req->tid);

        session->req = req;
        /* Start timeout work */
        k_delayed_work_submit(&session->req->timeout_work, AVDTP_TIMEOUT);
    }

    BT_DBG("");

    return result;
}

static void avdtp_send_with_no_body(struct bt_avdtp *session, u8_t msg_type, u8_t pkt_type, u8_t sig_id, u8_t tid)
{
    struct net_buf *buf = avdtp_create_pdu(msg_type, pkt_type, sig_id, tid);
    if (!buf) {
        BT_ERR("No Buff available");
        return;
    }

    avdtp_send(session, buf, NULL);

    return;
}

static void avdtp_send_with_errno(struct bt_avdtp *session, u8_t msg_type, u8_t pkt_type, u8_t sig_id, u8_t tid,
                                  u8_t errno, u8_t seid)
{
    struct net_buf *buf = avdtp_create_pdu(msg_type, pkt_type, sig_id, tid);
    if (!buf) {
        BT_ERR("No Buff available");
        return;
    }

    if (sig_id == BT_AVDTP_START || sig_id == BT_AVDTP_SUSPEND) {
        net_buf_add_u8(buf, seid << 2);
    }

    net_buf_add_u8(buf, errno);

    avdtp_send(session, buf, NULL);

    return;
}

static void avdtp_send_with_errno_with_sercat(struct bt_avdtp *session, u8_t msg_type, u8_t pkt_type, u8_t sig_id,
                                              u8_t tid, u8_t errno, u8_t sercat)
{
    struct net_buf *buf = avdtp_create_pdu(msg_type, pkt_type, sig_id, tid);
    if (!buf) {
        BT_ERR("No Buff available");
        return;
    }

    net_buf_add_u8(buf, sercat);
    net_buf_add_u8(buf, errno);

    avdtp_send(session, buf, NULL);

    return;
}

static int avdtp_stream_state_verify(struct bt_avdtp *session, u8_t seid, u8_t sigid, u8_t tid)
{
    struct bt_avdtp_stream *   stream;
    struct bt_avdtp_seid_lsep *sep;
    u8_t                       bad_state = 0;

    BT_DBG("");
    sep    = avdtp_find_lsep(seid);
    stream = avdtp_find_stream(seid);

    if (!sep) {
        avdtp_send_with_errno(session, BT_AVDTP_REJECT, BT_AVDTP_PACKET_TYPE_SINGLE, sigid, tid,
                              BT_AVDTP_ERR_BAD_ACP_SEID, seid);
        BT_ERR("No sep");
        return 1;
    }

    if (!stream) {
        avdtp_send_with_errno(session, BT_AVDTP_REJECT, BT_AVDTP_PACKET_TYPE_SINGLE, sigid, tid, BT_AVDTP_ERR_BAD_STATE,
                              seid);
        BT_ERR("No stream");
        return 1;
    }

    switch (sigid) {
        case BT_AVDTP_RECONFIGURE:
            if (stream->state != BT_AVDTP_STREAM_STATE_OPEN) {
                bad_state = 1;
            }
            break;
        case BT_AVDTP_OPEN:
            if (stream->state != BT_AVDTP_STREAM_STATE_CONFIGURED) {
                bad_state = 1;
            } else {
                stream->state = BT_AVDTP_STREAM_STATE_OPEN;
            }
            break;
        case BT_AVDTP_START:
            if (stream->state != BT_AVDTP_STREAM_STATE_OPEN) {
                bad_state = 1;
            } else {
                stream->state = BT_AVDTP_STREAM_STATE_STREAMING;
            }
            break;
        case BT_AVDTP_CLOSE:
            if (stream->state != BT_AVDTP_STREAM_STATE_STREAMING && stream->state != BT_AVDTP_STREAM_STATE_OPEN) {
                bad_state = 1;
            } else {
                stream->state = BT_AVDTP_STREAM_STATE_CLOSING;
            }
            break;
        case BT_AVDTP_SUSPEND:
            if (stream->state != BT_AVDTP_STREAM_STATE_STREAMING) {
                bad_state = 1;
            } else {
                stream->state = BT_AVDTP_STREAM_STATE_OPEN;
            }
            break;
        default:
            break;
    }

    if (bad_state) {
        avdtp_send_with_errno(session, BT_AVDTP_REJECT, BT_AVDTP_PACKET_TYPE_SINGLE, sigid, tid, BT_AVDTP_ERR_BAD_STATE,
                              seid);
    }

    return bad_state;
}

/** Timeout handler */
static void avdtp_timeout(struct k_work *work)
{
    BT_DBG("Failed Signal_id = %d", (AVDTP_KWORK(work))->sig);

    /* Gracefully Disconnect the Signalling and streaming L2cap chann*/
}

/** AVDTP Discover signal cmd response */
static void avdtp_discover_resp(struct bt_avdtp *session, struct net_buf *buf, u8_t tid, u8_t signal_id)
{
    struct bt_avdtp_seid_lsep *sep = avdtp_lseps;
    struct net_buf *           resp;

    BT_DBG("");

    resp = avdtp_create_pdu(BT_AVDTP_ACCEPT, BT_AVDTP_PACKET_TYPE_SINGLE, BT_AVDTP_DISCOVER, tid);
    if (!resp) {
        BT_ERR("No Buff available");
        return;
    }

    for (; sep != NULL; sep = sep->next) {
        net_buf_add_mem(resp, (uint8_t *)&sep->sep, sizeof(struct bt_avdtp_seid_info));
    }

    avdtp_send(session, resp, NULL);

    return;
}

/** AVDTP Get capability/Get All capability signal cmd response */
static void avdtp_get_cap_resp(struct bt_avdtp *session, struct net_buf *buf, u8_t tid, u8_t signal_id)
{
    struct bt_avdtp_seid_lsep *      sep;
    struct net_buf *                 resp;
    struct bt_avdtp_cap *            caps;
    struct bt_avdtp_media_codec_cap *media_codec;
    u8_t                             seid;

    BT_DBG("");

    seid = net_buf_pull_u8(buf) >> 2;
    sep  = avdtp_find_lsep(seid);

    if (!sep) {
        BT_ERR("No sep");
        avdtp_send_with_errno(session, BT_AVDTP_REJECT, BT_AVDTP_PACKET_TYPE_SINGLE, signal_id, tid,
                              BT_AVDTP_ERR_BAD_ACP_SEID, 0);
        return;
    }

    resp = avdtp_create_pdu(BT_AVDTP_ACCEPT, BT_AVDTP_PACKET_TYPE_SINGLE, signal_id, tid);
    if (!resp) {
        BT_ERR("No Buff available");
        return;
    }

    caps        = sep->caps;
    media_codec = caps->media_codec;

    /** only support MEDIA_TRANSPORT and MEDIA_CODEC capability */
    if (caps->service_cat_bitmap & BIT(BT_AVDTP_SERVICE_CAT_MEDIA_TRANSPORT)) {
        net_buf_add_u8(resp, BT_AVDTP_SERVICE_CAT_MEDIA_TRANSPORT);
        net_buf_add_u8(resp, 0);
    }

    if (caps->service_cat_bitmap & BIT(BT_AVDTP_SERVICE_CAT_MEDIA_CODEC)) {
        net_buf_add_u8(resp, BT_AVDTP_SERVICE_CAT_MEDIA_CODEC);
        net_buf_add_u8(resp, media_codec->codec_info_len + 2);
        net_buf_add_u8(resp, media_codec->media_type << 4);
        net_buf_add_u8(resp, media_codec->codec_type);
        net_buf_add_mem(resp, media_codec->codec_info, media_codec->codec_info_len);
    }

    avdtp_send(session, resp, NULL);

    return;
}

/** AVDTP Set configuration signal cmd response */
static void avdtp_set_conf_resp(struct bt_avdtp *session, struct net_buf *buf, u8_t tid, u8_t signal_id)
{
    struct bt_avdtp_seid_lsep *      sep;
    struct bt_avdtp_cap *            conf;
    struct bt_avdtp_media_codec_cap *media_codec;
    struct bt_avdtp_stream *         stream;
    u8_t *                           ser_caps;
    u8_t                             acp_seid;
    u8_t                             int_seid;
    u8_t                             ser_cat;
    u8_t                             ser_len;

    BT_DBG("");

    acp_seid = net_buf_pull_u8(buf) >> 2;
    int_seid = net_buf_pull_u8(buf) >> 2;

    sep = avdtp_find_lsep(acp_seid);
    if (!sep) {
        BT_ERR("No sep");
        avdtp_send_with_errno_with_sercat(session, BT_AVDTP_REJECT, BT_AVDTP_PACKET_TYPE_SINGLE,
                                          BT_AVDTP_SET_CONFIGURATION, tid, BT_AVDTP_ERR_BAD_ACP_SEID, 0);
        return;
    }

    /** get a new stream session */
    stream = avdtp_new_stream(session->br_chan.chan.conn);

    if (stream == NULL) {
        BT_ERR("No stream");
        return;
    }

    if (stream->state != BT_AVDTP_STREAM_STATE_IDLE) {
        /** send SEP_IN_USE error */
        avdtp_send_with_errno_with_sercat(session, BT_AVDTP_REJECT, BT_AVDTP_PACKET_TYPE_SINGLE, signal_id, tid,
                                          BT_AVDTP_ERR_SEP_IN_USE, 0);
        return;
    }

    sep->sep.inuse = 1;
    conf           = sep->conf;
    media_codec    = sep->conf->media_codec;

    /** change stream state */
    stream->lseid    = sep->sep.id;
    stream->rseid    = int_seid;
    stream->state    = BT_AVDTP_STREAM_STATE_CONFIGURED;
    session->streams = stream;

    while (buf->len >= 2) {
        ser_cat = net_buf_pull_u8(buf);
        ser_len = net_buf_pull_u8(buf);

        if (buf->len < ser_len) {
            BT_ERR("ser len");
            return;
        }

        ser_caps = net_buf_pull_mem(buf, ser_len);

        if (!(sep->caps->service_cat_bitmap & BIT(ser_cat))) {
            avdtp_send_with_errno_with_sercat(session, BT_AVDTP_REJECT, BT_AVDTP_PACKET_TYPE_SINGLE,
                                              BT_AVDTP_SET_CONFIGURATION, tid, BT_AVDTP_ERR_INVALID_CAPABILITIES,
                                              ser_cat);
            return;
        }

        switch (ser_cat) {
            case BT_AVDTP_SERVICE_CAT_REPORTING:
            case BT_AVDTP_SERVICE_CAT_RECOVERY:
            case BT_AVDTP_SERVICE_CAT_CONTENT_PROTECTION:
            case BT_AVDTP_SERVICE_CAT_HDR_COMPRESSION:
            case BT_AVDTP_SERVICE_CAT_MULTIPLEXING:
            case BT_AVDTP_SERVICE_CAT_DELAYREPORTING:
                break;
            case BT_AVDTP_SERVICE_CAT_MEDIA_TRANSPORT:
                /** for TS AVDTP/SNK/ACP/TRA/BTR/BI-01-C */
                if (ser_len != 0) {
                    avdtp_send_with_errno_with_sercat(session, BT_AVDTP_REJECT, BT_AVDTP_PACKET_TYPE_SINGLE, signal_id,
                                                      tid, BT_AVDTP_ERR_BAD_MEDIA_TRANSPORT_FORMAT, ser_cat);
                    return;
                }
                break;
            case BT_AVDTP_SERVICE_CAT_MEDIA_CODEC:
                if (ser_len != media_codec->codec_info_len + 2) {
                    avdtp_send_with_errno_with_sercat(session, BT_AVDTP_REJECT, BT_AVDTP_PACKET_TYPE_SINGLE, signal_id,
                                                      tid, BT_AVDTP_ERR_INVALID_CAPABILITIES, ser_cat);
                    return;
                }
                media_codec->media_type = ser_caps[0] >> 4;
                media_codec->codec_type = ser_caps[1];
                memcpy(media_codec->codec_info, &ser_caps[2], ser_len - 2);
                conf->media_codec = media_codec;
                break;
            default:
                avdtp_send_with_errno_with_sercat(session, BT_AVDTP_REJECT, BT_AVDTP_PACKET_TYPE_SINGLE,
                                                  BT_AVDTP_SET_CONFIGURATION, tid, BT_AVDTP_ERR_BAD_SERV_CATEGORY,
                                                  ser_cat);
                return;
        }
        conf->service_cat_bitmap |= BIT(ser_cat);
    }

    /** report to upper layer */
    if (avdtp_event_cb && avdtp_event_cb->ind && avdtp_event_cb->ind->set_config_ind) {
        avdtp_event_cb->ind->set_config_ind(session, conf);
    }

    avdtp_send_with_no_body(session, BT_AVDTP_ACCEPT, BT_AVDTP_PACKET_TYPE_SINGLE, BT_AVDTP_SET_CONFIGURATION, tid);

    return;
}

/** AVDTP Get configuration signal cmd response */
static void avdtp_get_conf_resp(struct bt_avdtp *session, struct net_buf *buf, u8_t tid, u8_t signal_id)
{
    struct bt_avdtp_seid_lsep *      sep;
    struct net_buf *                 resp;
    struct bt_avdtp_cap *            conf;
    struct bt_avdtp_stream *         stream;
    struct bt_avdtp_media_codec_cap *media_codec;
    u8_t                             acp_seid;

    BT_DBG("");

    acp_seid = net_buf_pull_u8(buf) >> 2;
    sep      = avdtp_find_lsep(acp_seid);
    stream   = avdtp_find_stream(acp_seid);

    if (!sep || !stream) {
        BT_ERR("No sep");
        avdtp_send_with_errno(session, BT_AVDTP_REJECT, BT_AVDTP_PACKET_TYPE_SINGLE, BT_AVDTP_GET_CONFIGURATION, tid,
                              BT_AVDTP_ERR_BAD_ACP_SEID, 0);
        return;
    }

    resp = avdtp_create_pdu(BT_AVDTP_ACCEPT, BT_AVDTP_PACKET_TYPE_SINGLE, BT_AVDTP_GET_CONFIGURATION, tid);
    if (!resp) {
        BT_ERR("No Buff available");
        return;
    }

    conf        = sep->conf;
    media_codec = conf->media_codec;

    /** only support MEDIA_TRANSPORT and MEDIA_CODEC capability */
    if (conf->service_cat_bitmap & BIT(BT_AVDTP_SERVICE_CAT_MEDIA_TRANSPORT)) {
        net_buf_add_u8(resp, BT_AVDTP_SERVICE_CAT_MEDIA_TRANSPORT);
        net_buf_add_u8(resp, 0);
    }

    if (conf->service_cat_bitmap & BIT(BT_AVDTP_SERVICE_CAT_MEDIA_CODEC)) {
        net_buf_add_u8(resp, BT_AVDTP_SERVICE_CAT_MEDIA_CODEC);
        net_buf_add_u8(resp, media_codec->codec_info_len + 2);
        net_buf_add_u8(resp, media_codec->media_type << 4);
        net_buf_add_u8(resp, media_codec->codec_type);
        net_buf_add_mem(resp, media_codec->codec_info, media_codec->codec_info_len);
    }

    avdtp_send(session, resp, NULL);

    return;
}

/** AVDTP Reconfig signal cmd response */
static void avdtp_reconfig_resp(struct bt_avdtp *session, struct net_buf *buf, u8_t tid, u8_t signal_id)
{
    struct bt_avdtp_seid_lsep *      sep;
    struct bt_avdtp_cap *            conf;
    struct bt_avdtp_media_codec_cap *media_codec;
    struct bt_avdtp_stream *         stream;
    u8_t *                           ser_caps;
    u8_t                             acp_seid;
    u8_t                             ser_cat;
    u8_t                             ser_len;
    u8_t                             need_report = 0;

    BT_DBG("");

    acp_seid = net_buf_pull_u8(buf) >> 2;

    sep = avdtp_find_lsep(acp_seid);
    if (!sep) {
        BT_ERR("No sep");
        avdtp_send_with_errno_with_sercat(session, BT_AVDTP_REJECT, BT_AVDTP_PACKET_TYPE_SINGLE, signal_id, tid,
                                          BT_AVDTP_ERR_BAD_ACP_SEID, 0);
        return;
    }

    stream = avdtp_find_stream(acp_seid);

    if (stream->state != BT_AVDTP_STREAM_STATE_OPEN) {
        /** send SEP_IN_USE error */
        avdtp_send_with_errno_with_sercat(session, BT_AVDTP_REJECT, BT_AVDTP_PACKET_TYPE_SINGLE, signal_id, tid,
                                          BT_AVDTP_ERR_SEP_IN_USE, 0);
        return;
    }

    if (buf->len < 2) {
        avdtp_send_with_errno_with_sercat(session, BT_AVDTP_REJECT, BT_AVDTP_PACKET_TYPE_SINGLE, signal_id, tid,
                                          BT_AVDTP_ERR_BAD_SERV_CATEGORY, 0);
        return;
    }

    conf        = sep->conf;
    media_codec = sep->conf->media_codec;

    while (buf->len > 1) {
        ser_cat = net_buf_pull_u8(buf);
        ser_len = net_buf_pull_u8(buf);

        if (buf->len < ser_len) {
            BT_ERR("ser len");
            return;
        }

        ser_caps = net_buf_pull_mem(buf, ser_len);

        if (!(sep->caps->service_cat_bitmap & BIT(ser_cat))) {
            avdtp_send_with_errno_with_sercat(session, BT_AVDTP_REJECT, BT_AVDTP_PACKET_TYPE_SINGLE, signal_id, tid,
                                              BT_AVDTP_ERR_BAD_SERV_CATEGORY, ser_cat);
            return;
        }

        switch (ser_cat) {
            case BT_AVDTP_SERVICE_CAT_REPORTING:
            case BT_AVDTP_SERVICE_CAT_RECOVERY:
            case BT_AVDTP_SERVICE_CAT_CONTENT_PROTECTION:
            case BT_AVDTP_SERVICE_CAT_HDR_COMPRESSION:
            case BT_AVDTP_SERVICE_CAT_MULTIPLEXING:
            case BT_AVDTP_SERVICE_CAT_MEDIA_TRANSPORT:
            case BT_AVDTP_SERVICE_CAT_DELAYREPORTING:
                avdtp_send_with_errno_with_sercat(session, BT_AVDTP_REJECT, BT_AVDTP_PACKET_TYPE_SINGLE, signal_id, tid,
                                                  BT_AVDTP_ERR_INVALID_CAPABILITIES, ser_cat);
                return;
            case BT_AVDTP_SERVICE_CAT_MEDIA_CODEC:
                BT_ERR("%d", ser_len);
                if (ser_len != media_codec->codec_info_len + 2) {
                    avdtp_send_with_errno_with_sercat(session, BT_AVDTP_REJECT, BT_AVDTP_PACKET_TYPE_SINGLE, signal_id,
                                                      tid, BT_AVDTP_ERR_INVALID_CAPABILITIES, ser_cat);
                    return;
                }
                need_report             = 1;
                media_codec->media_type = ser_caps[0] >> 4;
                media_codec->codec_type = ser_caps[1];
                memcpy(media_codec->codec_info, &ser_caps[2], ser_len - 2);
                break;
            default:
                avdtp_send_with_errno_with_sercat(session, BT_AVDTP_REJECT, BT_AVDTP_PACKET_TYPE_SINGLE, signal_id, tid,
                                                  BT_AVDTP_ERR_BAD_SERV_CATEGORY, ser_cat);
                return;
        }
        conf->service_cat_bitmap |= BIT(ser_cat);
    }

    /** report to upper layer */
    if (need_report && avdtp_event_cb && avdtp_event_cb->ind->set_config_ind) {
        avdtp_event_cb->ind->set_config_ind(session, conf);
    }

    avdtp_send_with_no_body(session, BT_AVDTP_ACCEPT, BT_AVDTP_PACKET_TYPE_SINGLE, signal_id, tid);

    return;
}

/** AVDTP Open/Close stream signal cmd response */
static void avdtp_open_close_resp(struct bt_avdtp *session, struct net_buf *buf, u8_t tid, u8_t signal_id)
{
    u8_t                       acp_seid;
    struct bt_avdtp_seid_lsep *sep;

    BT_DBG("");

    acp_seid = net_buf_pull_u8(buf) >> 2;

    if (avdtp_stream_state_verify(session, acp_seid, signal_id, tid)) {
        return;
    }

    /** report to upper layer */
    switch (signal_id) {
        case BT_AVDTP_OPEN:
            AVDTP_REPORT_EVENT(open_ind, session);
            break;
        case BT_AVDTP_CLOSE:
            avdtp_clean_stream(acp_seid);

            sep            = avdtp_find_lsep(acp_seid);
            sep->sep.inuse = 0;
            AVDTP_REPORT_EVENT(close_ind, session);
            break;
        default:
            break;
    }

    avdtp_send_with_no_body(session, BT_AVDTP_ACCEPT, BT_AVDTP_PACKET_TYPE_SINGLE, signal_id, tid);

    return;
}

/** AVDTP Start/Stop stream signal cmd response */
static void avdtp_start_suspend_resp(struct bt_avdtp *session, struct net_buf *buf, u8_t tid, u8_t signal_id)
{
    u8_t acp_seid;

    BT_DBG("");

    /** TODO: not support muitl seid*/
    acp_seid = net_buf_pull_u8(buf) >> 2;

    if (avdtp_stream_state_verify(session, acp_seid, signal_id, tid)) {
        return;
    }

    /** report to upper layer */
    switch (signal_id) {
        case BT_AVDTP_START:
            AVDTP_REPORT_EVENT(start_ind, session);
            break;
        case BT_AVDTP_SUSPEND:
            AVDTP_REPORT_EVENT(suspend_ind, session);
            break;
        default:
            break;
    }

    avdtp_send_with_no_body(session, BT_AVDTP_ACCEPT, BT_AVDTP_PACKET_TYPE_SINGLE, signal_id, tid);

    return;
}

/** AVDTP Abort stream signal cmd response */
static void avdtp_abord_resp(struct bt_avdtp *session, struct net_buf *buf, u8_t tid, u8_t signal_id)
{
    struct bt_avdtp_stream *stream;
    u8_t                    acp_seid;

    BT_DBG("");

    acp_seid = net_buf_pull_u8(buf) >> 2;

    stream = avdtp_find_stream(acp_seid);

    if (!stream) {
        avdtp_send_with_errno(session, BT_AVDTP_REJECT, BT_AVDTP_PACKET_TYPE_SINGLE, BT_AVDTP_ABORT, tid,
                              BT_AVDTP_ERR_BAD_ACP_SEID, 0);
        BT_ERR("No sep");
        return;
    }

    stream->state = BT_AVDTP_STREAM_STATE_ABORTING;

    /** report to upper layer */
    AVDTP_REPORT_EVENT(abort_ind, session)

    avdtp_send_with_no_body(session, BT_AVDTP_ACCEPT, BT_AVDTP_PACKET_TYPE_SINGLE, BT_AVDTP_ABORT, tid);

    return;
}

#if (defined(CONFIG_BT_AVDTP_DELAY_REPORT) && CONFIG_BT_AVDTP_DELAY_REPORT)
/** AVDTP Delay repoty signal cmd response */
static void avdtp_delayreport_resp(struct bt_avdtp *session, struct net_buf *buf, u8_t tid, u8_t signal_id)
{
    BT_DBG("enter");
}
#endif

/** L2CAP Interface callbacks */
static void bt_avdtp_l2cap_connected(struct bt_l2cap_chan *chan)
{
    struct bt_avdtp *session;

    if (!chan) {
        BT_ERR("Invalid AVDTP chan");
        return;
    }

    session = AVDTP_CHAN(chan);
    BT_DBG("chan %p session %p", chan, session);

    AVDTP_REPORT_EVENT(connected, session);

    /** Init the timer */
    if (session->req)
        k_delayed_work_init(&session->req->timeout_work, avdtp_timeout);
}

static void bt_avdtp_l2cap_disconnected(struct bt_l2cap_chan *chan)
{
    struct bt_avdtp *        session = AVDTP_CHAN(chan);
    struct bt_l2cap_br_chan *ch      = CONTAINER_OF(chan, struct bt_l2cap_br_chan, chan);

    BT_DBG("chan %p session %p", chan, session);

    /** Clear the Pending req if set*/

    /** clear all streams state */
    avdtp_clean_all_stream();

    AVDTP_REPORT_EVENT(disconnected, session);

    /** set lsep to unused */
    avdtp_clear_all_lsep();

    ch->chan.conn = NULL;
    ch->tx.cid    = 0;
    ch->rx.cid    = 0;
}

static int bt_avdtp_l2cap_recv(struct bt_l2cap_chan *chan, struct net_buf *buf)
{
    struct bt_avdtp_single_sig_hdr *hdr;
    struct bt_avdtp *               session = AVDTP_CHAN(chan);
    u8_t                            i;

    if (buf->len < sizeof(*hdr)) {
        BT_ERR("Recvd Wrong AVDTP Header");
        return 0;
    }

    hdr = net_buf_pull_mem(buf, sizeof(*hdr));

    BT_DBG("packet_type[0x%02x] msg_type[0x%02x] sig_id[0x%02x] tid[0x%02x]", hdr->packtype, hdr->msgtype,
           hdr->signal_id, hdr->tid);

    /** validate if there is an outstanding resp expected*/
    if (hdr->msgtype != BT_AVDTP_CMD) {
        if (session->req == NULL) {
            BT_WARN("Unexpected peer response");
            return 0;
        }

        if (session->req->sig != hdr->signal_id || session->req->tid != hdr->tid) {
            BT_WARN("Peer mismatch resp, expected sig[0x%02x]"
                    "tid[0x%02x]",
                    session->req->sig, session->req->tid);
            return 0;
        }

        if (session->req->func) {
            session->req->func(session, session->req, buf);
        }
    }

    /** TODO: deal with fragment packet */

    /** validate packet len */
    if (!avdtp_is_valid_len(hdr->signal_id, buf->len)) {
        /** send bad len error code */
        avdtp_send_with_errno(session, BT_AVDTP_REJECT, BT_AVDTP_PACKET_TYPE_SINGLE, hdr->signal_id, hdr->tid,
                              BT_AVDTP_ERR_BAD_LENGTH, 0);
        return 0;
    }

    /** deal with signal request */
    for (i = 0U; i < ARRAY_SIZE(handler); i++) {
        if (hdr->signal_id == handler[i].sig_id) {
            handler[i].func(session, buf, hdr->tid, hdr->signal_id);
            return 0;
        }
    }

    /** no signal_id matched, send general reject error code */
    avdtp_send_with_no_body(session, BT_AVDTP_GEN_REJECT, BT_AVDTP_PACKET_TYPE_SINGLE, hdr->signal_id, hdr->tid);

    return 0;
}

static void bt_avdtp_l2cap_encrypt_changed(struct bt_l2cap_chan *chan, u8_t status)
{
    BT_DBG("");
}

static void bt_avdtp_stream_l2cap_connected(struct bt_l2cap_chan *chan)
{
    BT_DBG("chan %p", chan);
}

static void bt_avdtp_stream_l2cap_disconnected(struct bt_l2cap_chan *chan)
{
    struct bt_avdtp_stream * stream = AVDTP_STREAM_CHAN(chan);
    struct bt_l2cap_br_chan *ch     = CONTAINER_OF(chan, struct bt_l2cap_br_chan, chan);

    stream->state = BT_AVDTP_STREAM_STATE_IDLE;
    stream->rseid = 0;
    stream->lseid = 0;

    ch->chan.conn = NULL;
    ch->tx.cid    = 0;
    ch->rx.cid    = 0;
}

static int bt_avdtp_stream_l2cap_recv(struct bt_l2cap_chan *chan, struct net_buf *buf)
{
    struct bt_avdtp_media_hdr *hdr;
    struct bt_avdtp_seid_lsep *sep;
    struct bt_avdtp_stream *   stream = AVDTP_STREAM_CHAN(chan);

    hdr = net_buf_pull_mem(buf, sizeof(*hdr));
    sep = avdtp_find_lsep(stream->lseid);

    if (sep == NULL) {
        return -1;
    }

    BT_DBG("Sequence Number: %d, Time Stamp %d", hdr->sn, hdr->ts);

    if (avdtp_event_cb->data) {
        avdtp_event_cb->data(stream, sep, buf);
    }

    return 0;
}

static int bt_avdtp_l2cap_accept(struct bt_conn *conn, struct bt_l2cap_chan **chan)
{
    struct bt_avdtp *       session = NULL;
    struct bt_avdtp_stream *stream  = NULL;
    int                     result;

    static const struct bt_l2cap_chan_ops stream_ops = {
        .connected    = bt_avdtp_stream_l2cap_connected,
        .disconnected = bt_avdtp_stream_l2cap_disconnected,
        .recv         = bt_avdtp_stream_l2cap_recv,
    };

    BT_DBG("conn %p", conn);
    /* Get the AVDTP session from upper layer */
    result = avdtp_event_cb->accept(conn, &session);
    /**
     * if AVDTP session is already exists, we assume that this connection is for
     * stream.
     */
    if (result == -EALREADY) {
        /**
         * stream l2cap channel open after set config cmd.
         * so that the head of streams list is the stream need to connect
         */
        stream                = session->streams;
        stream->chan.chan.ops = &stream_ops;
        stream->chan.rx.mtu   = BT_AVDTP_MAX_MTU;
        *chan                 = &stream->chan.chan;
        return 0;
    } else if (result < 0) {
        return result;
    }
    session->br_chan.chan.ops = &avdtp_l2cap_ops;
    session->br_chan.rx.mtu   = BT_AVDTP_MAX_MTU;
    *chan                     = &session->br_chan.chan;

    return 0;
}

/** Application will register its callback */
int bt_avdtp_register(struct bt_avdtp_event_cb *cb)
{
    BT_DBG("");

    if (avdtp_event_cb) {
        return -EALREADY;
    }

    avdtp_event_cb = cb;

    return 0;
}

int bt_avdtp_register_sep(u8_t media_type, u8_t role, struct bt_avdtp_seid_lsep *lsep)
{
    BT_DBG("");

    static u8_t bt_avdtp_seid = BT_AVDTP_MIN_SEID;

    if (!lsep) {
        return -EIO;
    }

    if (bt_avdtp_seid == BT_AVDTP_MAX_SEID) {
        return -EIO;
    }

    lsep->sep.id         = bt_avdtp_seid++;
    lsep->sep.inuse      = 0U;
    lsep->sep.media_type = media_type;
    lsep->sep.tsep       = role;

    lsep->next  = avdtp_lseps;
    avdtp_lseps = lsep;

    return 0;
}

/** AVDTP Init function */
int bt_avdtp_init(void)
{
    static struct bt_l2cap_server avdtp_l2cap = {
        .psm       = BT_L2CAP_PSM_AVDTP,
        .sec_level = BT_SECURITY_L2,
        .accept    = bt_avdtp_l2cap_accept,
    };
    int err;

    BT_DBG("");

    /* Register AVDTP PSM with L2CAP */
    err = bt_l2cap_br_server_register(&avdtp_l2cap);
    if (err < 0) {
        BT_ERR("AVDTP L2CAP Registration failed %d", err);
    }

    return err;
}

/*A2DP Layer interface */
int bt_avdtp_connect(struct bt_conn *conn, struct bt_avdtp *session)
{
    if (!session) {
        return -EINVAL;
    }

    session->br_chan.chan.ops                = &avdtp_l2cap_ops;
    session->br_chan.chan.required_sec_level = BT_SECURITY_L2;
    session->br_chan.rx.mtu                  = BT_AVDTP_MAX_MTU;

    return bt_l2cap_chan_connect(conn, &session->br_chan.chan, BT_L2CAP_PSM_AVDTP);
}

int bt_avdtp_disconnect(struct bt_avdtp *session)
{
    if (!session) {
        return -EINVAL;
    }

    BT_DBG("session %p", session);

    return bt_l2cap_chan_disconnect(&session->br_chan.chan);
}

/** AVDTP Discover signal Request */
int bt_avdtp_discover(struct bt_avdtp *session, bt_avdtp_cmf_cb_t func)
{
    struct net_buf *buf;

    BT_DBG("");
    if (!session) {
        BT_DBG("Callback/Session not valid");
        return -EINVAL;
    }

    buf = avdtp_create_pdu(BT_AVDTP_CMD, BT_AVDTP_PACKET_TYPE_SINGLE, BT_AVDTP_DISCOVER, avdtp_get_tid());
    if (!buf) {
        BT_ERR("No Buff available");
        return -ENOMEM;
    }

    /* Body of the message */
    return avdtp_send(session, buf, NULL);
}
#endif
