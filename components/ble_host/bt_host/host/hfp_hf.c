/* hfp_hf.c - Hands free Profile - Handsfree side handling */

/*
 * Copyright (c) 2015-2016 Intel Corporation
 * Copyright (c) 2022  Alibaba Group Holding Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <atomic.h>
#include <errno.h>
#include <aos/bt.h>
#include <misc/byteorder.h>
#include <misc/printk.h>
#include <misc/util.h>

#if (defined(CONFIG_BT_HFP_HF) && CONFIG_BT_HFP_HF)
#if CONFIG_BT_HFP_AUDIO_I2S
#include <bluetooth/audio_hfp.h>
#endif
#include <bluetooth/conn.h>
#include <bluetooth/hfp_hf.h>
#include <bluetooth/rfcomm.h>
#include <bluetooth/sdp.h>
#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BT_DEBUG_HFP_HF)
#define LOG_MODULE_NAME bt_hfp_hf
#include "common/log.h"

#include "at.h"
#include "hci_core.h"
#include "conn_internal.h"
#include "hfp_internal.h"
#include "l2cap_internal.h"
#include "rfcomm_internal.h"

#define MAX_IND_STR_LEN 17

#define HFP_REPORT_EVENT(evt, param)                                                                                   \
    do {                                                                                                               \
        if (bt_prf_hfp_hf_cb) {                                                                                        \
            bt_prf_hfp_hf_cb(evt, param);                                                                              \
        }                                                                                                              \
    } while (0);

static bt_prf_hfp_hf_cb_t bt_prf_hfp_hf_cb = NULL;
static struct bt_hfp_hf * hfp_hf           = NULL;
#if CONFIG_BT_HFP_AUDIO_I2S
static const audio_hfp_ops_t *audio_hfp = NULL;
#endif

NET_BUF_POOL_FIXED_DEFINE(hf_pool, CONFIG_BT_BR_MAX_CONN + 1, BT_RFCOMM_BUF_SIZE(BT_HF_CLIENT_MAX_PDU), NULL);

#define SDP_CLIENT_USER_BUF_LEN 512
NET_BUF_POOL_DEFINE(hfp_sdp_client_pool, CONFIG_BT_MAX_CONN, SDP_CLIENT_USER_BUF_LEN, BT_BUF_USER_DATA_MIN, NULL);

static struct bt_hfp_hf bt_hfp_hf_pool[CONFIG_BT_BR_MAX_CONN];

/* The order should follow the enum hfp_hf_ag_indicators */
static const struct {
    char *name;
    u32_t min;
    u32_t max;
} ag_ind[] = {
    { "service", 0, 1 },   /* HF_SERVICE_IND */
    { "call", 0, 1 },      /* HF_CALL_IND */
    { "callsetup", 0, 3 }, /* HF_CALL_SETUP_IND */
    { "callheld", 0, 2 },  /* HF_CALL_HELD_IND */
    { "signal", 0, 5 },    /* HF_SINGNAL_IND */
    { "roam", 0, 1 },      /* HF_ROAM_IND */
    { "battchg", 0, 5 },   /* HF_BATTERY_IND */
};

static void hf_acl_connected(struct bt_conn *conn, u8_t err);
static void hfp_hf_connected(struct bt_rfcomm_dlc *dlc);
static void hfp_hf_disconnected(struct bt_rfcomm_dlc *dlc);
static void hfp_hf_recv(struct bt_rfcomm_dlc *dlc, struct net_buf *buf);
static void slc_completed(struct at_client *hf_at);
static int  hfp_slc_state_machine(struct bt_hfp_hf *hf, enum at_result result, enum at_cme cme_err);
static u8_t bt_hfp_hf_sdp_cb(struct bt_conn *conn, struct bt_sdp_client_result *result);

static struct bt_sdp_attribute handfree_attrs[] = {
    BT_SDP_NEW_SERVICE,
    BT_SDP_LIST(BT_SDP_ATTR_SVCLASS_ID_LIST, BT_SDP_TYPE_SIZE_VAR(BT_SDP_SEQ8, 6),
                BT_SDP_DATA_ELEM_LIST(
                    {
                        BT_SDP_TYPE_SIZE(BT_SDP_UUID16),
                        BT_SDP_ARRAY_16(BT_SDP_HANDSFREE_SVCLASS),
                    },
                    { BT_SDP_TYPE_SIZE(BT_SDP_UUID16), BT_SDP_ARRAY_16(BT_SDP_GENERIC_AUDIO_SVCLASS) }, )),
    BT_SDP_LIST(BT_SDP_ATTR_PROTO_DESC_LIST, BT_SDP_TYPE_SIZE_VAR(BT_SDP_SEQ8, 12),
                BT_SDP_DATA_ELEM_LIST(
                    {
                        BT_SDP_TYPE_SIZE_VAR(BT_SDP_SEQ8, 3),
                        BT_SDP_DATA_ELEM_LIST(
                            {
                                BT_SDP_TYPE_SIZE(BT_SDP_UUID16),
                                BT_SDP_ARRAY_16(BT_SDP_PROTO_L2CAP),
                            }, ),
                    },
                    {
                        BT_SDP_TYPE_SIZE_VAR(BT_SDP_SEQ8, 5),
                        BT_SDP_DATA_ELEM_LIST(
                            {
                                BT_SDP_TYPE_SIZE(BT_SDP_UUID16),
                                BT_SDP_ARRAY_16(BT_SDP_PROTO_RFCOMM),
                            },
                            {
                                BT_SDP_TYPE_SIZE(BT_SDP_UINT8),
                                BT_SDP_ARRAY_8(BT_RFCOMM_CHAN_HFP_HF),
                            }, ),
                    }, )),
    BT_SDP_LIST(BT_SDP_ATTR_PROFILE_DESC_LIST, BT_SDP_TYPE_SIZE_VAR(BT_SDP_SEQ8, 8),
                BT_SDP_DATA_ELEM_LIST(
                    {
                        BT_SDP_TYPE_SIZE_VAR(BT_SDP_SEQ8, 6),
                        BT_SDP_DATA_ELEM_LIST(
                            {
                                BT_SDP_TYPE_SIZE(BT_SDP_UUID16),
                                BT_SDP_ARRAY_16(BT_SDP_HANDSFREE_SVCLASS),
                            },
                            {
                                BT_SDP_TYPE_SIZE(BT_SDP_UINT16), BT_SDP_ARRAY_16(BT_HFP_HF_VERSION), /* version 1.8*/
                            }, ),
                    }, )),
    BT_SDP_SUPPORTED_FEATURES(BT_HFP_HF_SUPPORTED_FEATURES),
};

static struct bt_sdp_record handfree_rec = BT_SDP_RECORD(handfree_attrs);

static struct bt_rfcomm_dlc_ops hfp_hf_ops = {
    .connected    = hfp_hf_connected,
    .disconnected = hfp_hf_disconnected,
    .recv         = hfp_hf_recv,
};

static struct bt_sdp_discover_params discov_hfpag = {
    .uuid = BT_UUID_DECLARE_16(BT_SDP_HANDSFREE_AGW_SVCLASS),
    .func = bt_hfp_hf_sdp_cb,
    .pool = &hfp_sdp_client_pool,
};

static void hf_slc_disconnect(struct at_client *hf_at)
{
    struct bt_hfp_hf *hf = CONTAINER_OF(hf_at, struct bt_hfp_hf, at);
    int               err;

    BT_ERR("SLC error: disconnecting");
    err = bt_rfcomm_dlc_disconnect(&hf->rfcomm_dlc);
    if (err) {
        BT_ERR("Rfcomm: Unable to disconnect :%d", -err);
    }
}

static int hfp_hf_send_cmd(struct bt_hfp_hf *hf, at_resp_cb_t resp, at_finish_cb_t finish, const char *format, ...)
{
    struct net_buf *buf;
    va_list         vargs;
    int             ret;

    /* register the callbacks */
    at_register(&hf->at, resp, finish);

    buf = bt_rfcomm_create_pdu(&hf_pool);
    if (!buf) {
        BT_ERR("No Buffers!");
        return -ENOMEM;
    }

    va_start(vargs, format);
    ret = vsnprintf((char *)buf->data, (net_buf_tailroom(buf) - 1), format, vargs);
    if (ret < 0) {
        BT_ERR("Unable to format variable arguments");
        return ret;
    }
    va_end(vargs);

    net_buf_add(buf, ret);
    net_buf_add_u8(buf, '\r');

    ret = bt_rfcomm_dlc_send(&hf->rfcomm_dlc, buf);
    if (ret < 0) {
        BT_ERR("Rfcomm send error :(%d)", ret);
        return ret;
    }

    return 0;
}

static int brsf_handle(struct at_client *hf_at)
{
    struct bt_hfp_hf *hf = CONTAINER_OF(hf_at, struct bt_hfp_hf, at);
    u32_t             val;
    int               ret;

    ret = at_get_number(hf_at, &val);
    if (ret < 0) {
        BT_ERR("Error getting value");
        return ret;
    }

    hf->ag_features = val;

    return 0;
}

static int brsf_resp(struct at_client *hf_at, struct net_buf *buf)
{
    int err;

    BT_DBG("");

    err = at_parse_cmd_input(hf_at, buf, "BRSF", brsf_handle, AT_CMD_TYPE_NORMAL);
    if (err < 0) {
        /* Returning negative value is avoided before SLC connection
         * established.
         */
        BT_ERR("Error parsing CMD input");
        hf_slc_disconnect(hf_at);
    }

    return 0;
}

static void cind_handle_values(struct at_client *hf_at, u32_t index, char *name, u32_t min, u32_t max)
{
    struct bt_hfp_hf *hf = CONTAINER_OF(hf_at, struct bt_hfp_hf, at);
    int               i;

    BT_DBG("index: %u, name: %s, min: %u, max:%u", index, name, min, max);

    for (i = 0; i < ARRAY_SIZE(ag_ind); i++) {
        if (strcmp(name, ag_ind[i].name) != 0) {
            continue;
        }
        if (min != ag_ind[i].min || max != ag_ind[i].max) {
            BT_ERR("%s indicator min/max value not matching", name);
        }

        hf->ind_table[index] = i;
        break;
    }
}

static int cind_handle(struct at_client *hf_at)
{
    u32_t index = 0U;

    /* Parsing Example: CIND: ("call",(0,1)) etc.. */
    while (at_has_next_list(hf_at)) {
        char  name[MAX_IND_STR_LEN];
        u32_t min, max;

        if (at_open_list(hf_at) < 0) {
            BT_ERR("Could not get open list");
            goto error;
        }

        if (at_list_get_string(hf_at, name, sizeof(name)) < 0) {
            BT_ERR("Could not get string");
            goto error;
        }

        if (at_open_list(hf_at) < 0) {
            BT_ERR("Could not get open list");
            goto error;
        }

        if (at_list_get_range(hf_at, &min, &max) < 0) {
            BT_ERR("Could not get range");
            goto error;
        }

        if (at_close_list(hf_at) < 0) {
            BT_ERR("Could not get close list");
            goto error;
        }

        if (at_close_list(hf_at) < 0) {
            BT_ERR("Could not get close list");
            goto error;
        }

        cind_handle_values(hf_at, index, name, min, max);
        index++;
    }

    return 0;
error:
    BT_ERR("Error on CIND response");
    hf_slc_disconnect(hf_at);
    return -EINVAL;
}

static int cind_resp(struct at_client *hf_at, struct net_buf *buf)
{
    int err;

    err = at_parse_cmd_input(hf_at, buf, "CIND", cind_handle, AT_CMD_TYPE_NORMAL);
    if (err < 0) {
        BT_ERR("Error parsing CMD input");
        hf_slc_disconnect(hf_at);
    }

    return 0;
}

static void ag_indicator_handle_values(struct at_client *hf_at, u32_t index, u32_t value)
{
    struct bt_hfp_hf *       hf = CONTAINER_OF(hf_at, struct bt_hfp_hf, at);
    bt_prf_hfp_hf_cb_param_t param;

    BT_DBG("Index :%u, Value :%u", index, value);

    if (index >= ARRAY_SIZE(ag_ind)) {
        BT_ERR("Max only %lu indicators are supported", ARRAY_SIZE(ag_ind));
        return;
    }

    if (value > ag_ind[hf->ind_table[index]].max || value < ag_ind[hf->ind_table[index]].min) {
        BT_ERR("Indicators out of range - value: %u", value);
        return;
    }

    memset(&param, 0, sizeof(bt_prf_hfp_hf_cb_param_t));

    switch (hf->ind_table[index]) {
        case HF_SERVICE_IND:
            param.service_availability.status = value;
            HFP_REPORT_EVENT(BT_PRF_HFP_HF_CIND_SERVICE_AVAILABILITY_EVT, &param);
            break;
        case HF_CALL_IND:
            param.call.status = value;
            HFP_REPORT_EVENT(BT_PRF_HFP_HF_CIND_CALL_EVT, &param);
            break;
        case HF_CALL_SETUP_IND:
            param.call_setup.status = value;
            HFP_REPORT_EVENT(BT_PRF_HFP_HF_CIND_CALL_SETUP_EVT, &param);
            break;
        case HF_CALL_HELD_IND:
            param.call_held.status = value;
            HFP_REPORT_EVENT(BT_PRF_HFP_HF_CIND_CALL_HELD_EVT, &param);
            break;
        case HF_SINGNAL_IND:
            param.signal_strength.value = value;
            HFP_REPORT_EVENT(BT_PRF_HFP_HF_CIND_SIGNAL_STRENGTH_EVT, &param);
            break;
        case HF_ROAM_IND:
            param.roaming.status = value;
            HFP_REPORT_EVENT(BT_PRF_HFP_HF_CIND_ROAMING_STATUS_EVT, &param);
            break;
        case HF_BATTERY_IND:
            param.battery_level.value = value;
            HFP_REPORT_EVENT(BT_PRF_HFP_HF_CIND_BATTERY_LEVEL_EVT, &param);
            break;
        default:
            BT_ERR("Unknown AG indicator");
            break;
    }
}

static int cind_status_handle(struct at_client *hf_at)
{
    u32_t index = 0U;

    while (at_has_next_list(hf_at)) {
        u32_t value;
        int   ret;

        ret = at_get_number(hf_at, &value);
        if (ret < 0) {
            BT_ERR("could not get the value");
            return ret;
        }

        ag_indicator_handle_values(hf_at, index, value);

        index++;
    }

    return 0;
}

static int cind_status_resp(struct at_client *hf_at, struct net_buf *buf)
{
    int err;

    err = at_parse_cmd_input(hf_at, buf, "CIND", cind_status_handle, AT_CMD_TYPE_NORMAL);
    if (err < 0) {
        BT_ERR("Error parsing CMD input");
        hf_slc_disconnect(hf_at);
    }

    return 0;
}

static int chld_handle(struct at_client *hf_at)
{
    struct bt_hfp_hf *hf = CONTAINER_OF(hf_at, struct bt_hfp_hf, at);

    /* Parsing Example: +CHLD: (0,1,1x,2,2x,3...) etc.. */
    if (at_open_list(hf_at) < 0) {
        BT_ERR("Could not get open list");
        goto error;
    }

    while (at_has_next_list(hf_at)) {
#ifdef NEVER
        ret = at_get_number(hf_at, &value);
        if (ret == 0) {
            printf("%s, %d, value %d\r\n", __FUNCTION__, __LINE__, value);
            continue;
        }
#endif /* NEVER */

        if (strncmp("0", &hf_at->buf[hf_at->pos], 1) == 0) {
            hf->chld_features |= BT_HF_CLIENT_CHLD_REL;
            hf_at->pos += 2; /* the number itself and the , need to be skiped*/
            continue;
        } else if (strncmp("1x", &hf_at->buf[hf_at->pos], 2) == 0) {
            hf->chld_features |= BT_HF_CLIENT_CHLD_REL_X;
            hf_at->pos += 3;
            continue;
        } else if (strncmp("1", &hf_at->buf[hf_at->pos], 1) == 0) {
            hf->chld_features |= BT_HF_CLIENT_CHLD_REL_ACC;
            hf_at->pos += 2;
            continue;
        } else if (strncmp("2x", &hf_at->buf[hf_at->pos], 2) == 0) {
            hf->chld_features |= BT_HF_CLIENT_CHLD_PRIV_X;
            hf_at->pos += 3;
            continue;
        } else if (strncmp("2", &hf_at->buf[hf_at->pos], 1) == 0) {
            hf->chld_features |= BT_HF_CLIENT_CHLD_HOLD_ACC;
            hf_at->pos += 2;
            continue;
        } else if (strncmp("3", &hf_at->buf[hf_at->pos], 1) == 0) {
            hf->chld_features |= BT_HF_CLIENT_CHLD_MERGE;
            hf_at->pos += 2;
            continue;
        } else if (strncmp("4", &hf_at->buf[hf_at->pos], 1) == 0) {
            hf->chld_features |= BT_HF_CLIENT_CHLD_MERGE_DETACH;
            hf_at->pos += 2;
            continue;
        } else {
            if (at_close_list(hf_at) < 0) {
                BT_ERR("Could not get close list");
                goto error;
            }
        }
    }

    return 0;

error:
    BT_ERR("Error on CHLD response");
    hf_slc_disconnect(hf_at);
    return -EINVAL;
}

static int chld_resp(struct at_client *hf_at, struct net_buf *buf)
{
    int err;

    BT_DBG("");

    err = at_parse_cmd_input(hf_at, buf, "CHLD", chld_handle, AT_CMD_TYPE_NORMAL);
    if (err < 0) {
        /* Returning negative value is avoided before SLC connection
         * established.
         */
        BT_ERR("Error parsing CMD input");
        hf_slc_disconnect(hf_at);
    }

    return 0;
}

static int ciev_handle(struct at_client *hf_at)
{
    u32_t index, value;
    int   ret;

    ret = at_get_number(hf_at, &index);
    if (ret < 0) {
        BT_ERR("could not get the Index");
        return ret;
    }
    /* The first element of the list shall have 1 */
    if (!index) {
        BT_ERR("Invalid index value '0'");
        return 0;
    }

    ret = at_get_number(hf_at, &value);
    if (ret < 0) {
        BT_ERR("could not get the value");
        return ret;
    }

    ag_indicator_handle_values(hf_at, (index - 1), value);

    return 0;
}

static int ring_handle(struct at_client *hf_at)
{
    if (bt_prf_hfp_hf_cb) {
        bt_prf_hfp_hf_cb(BT_PRF_HFP_HF_RING_IND_EVT, NULL);
    }

    return 0;
}

static int vgs_handle(struct at_client *hf_at)
{
    bt_prf_hfp_hf_cb_param_t param;
    u32_t                    vol;
    int                      ret;

    ret = at_get_number(hf_at, &vol);
    if (ret < 0) {
        BT_ERR("could not get the Index");
        return ret;
    }

    BT_DBG("vol: %d", vol);

    param.volume_control.type   = BT_PRF_HFP_HF_VOLUME_CONTROL_TARGET_SPK;
    param.volume_control.volume = vol;
    HFP_REPORT_EVENT(BT_PRF_HFP_HF_VOLUME_CONTROL_EVT, &param);

#if CONFIG_BT_HFP_AUDIO_I2S
    if (audio_hfp) {
        audio_hfp->vol(vol);
    }
#endif

    return 0;
}

static int bsir_handle(struct at_client *hf_at)
{
    bt_prf_hfp_hf_cb_param_t param;
    u32_t                    bsir;
    int                      ret;

    ret = at_get_number(hf_at, &bsir);
    if (ret < 0) {
        BT_ERR("could not get the Index");
        return ret;
    }

    BT_DBG("bsir %d", bsir);

    param.bsir.state = bsir;
    HFP_REPORT_EVENT(BT_PRF_HFP_HF_BSIR_EVT, &param);

    return 0;
}

static int btrh_handle(struct at_client *hf_at)
{
    bt_prf_hfp_hf_cb_param_t param;
    u32_t                    btrh;
    int                      ret;

    ret = at_get_number(hf_at, &btrh);
    if (ret < 0) {
        BT_ERR("could not get the Index");
        return ret;
    }

    BT_DBG("btrh %d", btrh);

    param.btrh.status = btrh;
    HFP_REPORT_EVENT(BT_PRF_HFP_HF_BTRH_EVT, &param);

    return 0;
}

static int ccwa_handle(struct at_client *hf_at)
{
    bt_prf_hfp_hf_cb_param_t param;
    int                      ret;
    char                     num[MAX_IND_STR_LEN];

    ret = at_list_get_string(hf_at, num, sizeof(num));
    if (ret < 0) {
        BT_ERR("could not get the Index");
        return ret;
    }

    param.ccwa.number = num;
    HFP_REPORT_EVENT(BT_PRF_HFP_HF_CCWA_EVT, &param);

    return 0;
}

static const struct at_unsolicited handlers[] = {
    {
        "CIEV",
        AT_CMD_TYPE_UNSOLICITED,
        ciev_handle,
    },
    {
        "RING",
        AT_CMD_TYPE_OTHER,
        ring_handle,
    },
    {
        "VGS",
        AT_CMD_TYPE_UNSOLICITED,
        vgs_handle,
    },
    {
        "BSIR",
        AT_CMD_TYPE_UNSOLICITED,
        bsir_handle,
    },
    {
        "BTRH",
        AT_CMD_TYPE_UNSOLICITED,
        btrh_handle,
    },
    {
        "CCWA",
        AT_CMD_TYPE_UNSOLICITED,
        ccwa_handle,
    },
};

static int cmd_complete(struct at_client *hf_at, enum at_result result, enum at_cme cme_err)
{
    struct bt_hfp_hf *hf = CONTAINER_OF(hf_at, struct bt_hfp_hf, at);

    BT_DBG("result %d, err %d", result, cme_err);

    if (hf->connection_state < BT_PRF_HFP_HF_CONNECTION_STATE_SLC_CONNECTED) {
        return hfp_slc_state_machine(hf, result, cme_err);
    }

    return 0;
}

static int hfp_hf_send_at_brsf(struct bt_hfp_hf *hf)
{
    int err = 0;

    err = hfp_hf_send_cmd(hf, brsf_resp, cmd_complete, "AT+BRSF=%u", hf->hf_features);
    if (err < 0) {
        hf_slc_disconnect(&hf->at);
        return err;
    }

    hf->last_cmd = HFP_HF_AT_BRSF;

    return 0;
}

static int hfp_hf_send_at_bac(struct bt_hfp_hf *hf)
{
    int err = 0;

    err = hfp_hf_send_cmd(hf, NULL, cmd_complete, "AT+BAC=1"); /* currently only support CVSD*/
    if (err < 0) {
        hf_slc_disconnect(&hf->at);
        return err;
    }

    hf->last_cmd = HFP_HF_AT_BAC;

    return 0;
}

static int hfp_hf_send_at_cind(struct bt_hfp_hf *hf, bool status)
{
    int   err    = 0;
    char *at_str = NULL;

    if (status) {
        at_str = "AT+CIND?";
        err    = hfp_hf_send_cmd(hf, cind_status_resp, cmd_complete, at_str);
    } else {
        at_str = "AT+CIND=?";
        err    = hfp_hf_send_cmd(hf, cind_resp, cmd_complete, at_str);
    }

    if (err < 0) {
        hf_slc_disconnect(&hf->at);
        return err;
    }

    if (status) {
        hf->last_cmd = HFP_HF_AT_CIND_STATUS;
    } else {
        hf->last_cmd = HFP_HF_AT_CIND;
    }

    return 0;
}

static int hfp_hf_send_at_cmer(struct bt_hfp_hf *hf, bool active)
{
    int err = 0;

    err = hfp_hf_send_cmd(hf, NULL, cmd_complete, "AT+CMER=3,0,0,%d", active);
    if (err < 0) {
        hf_slc_disconnect(&hf->at);
        return err;
    }

    hf->last_cmd = HFP_HF_AT_CMER;

    return 0;
}

static int hfp_hf_send_at_chld(struct bt_hfp_hf *hf, char cmd, uint32_t idx)
{
    int err = 0;
    BT_DBG("");

    if (idx > 0) {
        err = hfp_hf_send_cmd(hf, chld_resp, cmd_complete, "AT+CHLD=%c%u", cmd, idx);
    } else {
        err = hfp_hf_send_cmd(hf, chld_resp, cmd_complete, "AT+CHLD=%c", cmd);
    }

    if (err < 0) {
        hf_slc_disconnect(&hf->at);
        return err;
    }

    hf->last_cmd = HFP_HF_AT_CHLD;

    return 0;
}

static int hfp_hf_send_at_bcc(struct bt_hfp_hf *hf)
{
    int err = 0;
    BT_DBG("");

    err = hfp_hf_send_cmd(hf, NULL, cmd_complete, "AT+BCC");
    if (err < 0) {
        hf_slc_disconnect(&hf->at);
        return err;
    }

    hf->last_cmd = HFP_HF_AT_BCC;

    return 0;
}

static int hfp_hf_send_at_ata(struct bt_hfp_hf *hf)
{
    int err = 0;
    BT_DBG("");

    err = hfp_hf_send_cmd(hf, NULL, cmd_complete, "ATA");
    if (err < 0) {
        hf_slc_disconnect(&hf->at);
        return err;
    }

    hf->last_cmd = HFP_HF_AT_ATA;

    return 0;
}

static int hfp_hf_send_at_chup(struct bt_hfp_hf *hf)
{
    int err = 0;
    BT_DBG("");

    err = hfp_hf_send_cmd(hf, NULL, cmd_complete, "AT+CHUP");
    if (err < 0) {
        hf_slc_disconnect(&hf->at);
        return err;
    }

    hf->last_cmd = HFP_HF_AT_CHUP;

    return 0;
}

static int hfp_hf_send_at_atd(struct bt_hfp_hf *hf, const char *number)
{
    int err = 0;
    BT_DBG("");

    err = hfp_hf_send_cmd(hf, NULL, cmd_complete, "ATD%s;", number);
    if (err < 0) {
        hf_slc_disconnect(&hf->at);
        return err;
    }

    hf->last_cmd = HFP_HF_AT_ATD;

    return 0;
}

static int hfp_hf_send_at_bldn(struct bt_hfp_hf *hf)
{
    int err = 0;
    BT_DBG("");

    err = hfp_hf_send_cmd(hf, NULL, cmd_complete, "AT+BLDN");
    if (err < 0) {
        hf_slc_disconnect(&hf->at);
        return err;
    }

    hf->last_cmd = HFP_HF_AT_BLDN;

    return 0;
}

static int hfp_hf_send_at_vgs(struct bt_hfp_hf *hf, int volume)
{
    int err = 0;
    BT_DBG("");

    err = hfp_hf_send_cmd(hf, NULL, cmd_complete, "AT+VGS=%u", volume);
    if (err < 0) {
        hf_slc_disconnect(&hf->at);
        return err;
    }
#if CONFIG_BT_HFP_AUDIO_I2S
    if (audio_hfp) {
        audio_hfp->vol(volume);
    }
#endif

    hf->last_cmd = HFP_HF_AT_VGS;

    return 0;
}

static int hfp_hf_send_at_vgm(struct bt_hfp_hf *hf, int volume)
{
    int err = 0;
    BT_DBG("");

    err = hfp_hf_send_cmd(hf, NULL, cmd_complete, "AT+VGM=%u", volume);
    if (err < 0) {
        hf_slc_disconnect(&hf->at);
        return err;
    }

    hf->last_cmd = HFP_HF_AT_VGM;

    return 0;
}

static int hfp_slc_state_machine(struct bt_hfp_hf *hf, enum at_result result, enum at_cme cme_err)
{
    struct at_client *hf_at = &hf->at;
    int               err   = 0;

    if (result != AT_RESULT_OK) {
        BT_ERR("SLC Connection ERROR in response");
        hf_slc_disconnect(hf_at);
        return -EINVAL;
    }

    switch (hf->last_cmd) {
        case HFP_HF_AT_NONE:
            err = hfp_hf_send_at_brsf(hf);
            break;

        case HFP_HF_AT_BRSF:
            if (hf->ag_features & BT_HFP_AG_FEATURE_CODEC_NEG) {
                err = hfp_hf_send_at_bac(hf);
                break;
            }

            /*
             * if not support codec negotiation, fall through to next case
             * HFP_HF_AT_BAC
             */

        case HFP_HF_AT_BAC:
            err = hfp_hf_send_at_cind(hf, false);
            break;

        case HFP_HF_AT_CIND:
            err = hfp_hf_send_at_cind(hf, true);
            break;

        case HFP_HF_AT_CIND_STATUS:
            at_register_unsolicited(hf_at, handlers, ARRAY_SIZE(handlers));
            err = hfp_hf_send_at_cmer(hf, true);
            break;

        case HFP_HF_AT_CMER:
            if (hf->ag_features & BT_HFP_AG_FEATURE_3WAY_CALL) {
                err = hfp_hf_send_at_chld(hf, '?', 0);
                break;
            }

            /* if not support 3-way call, fall through to next case HFP_HF_AT_CHLD */
        case HFP_HF_AT_CHLD:
            slc_completed(hf_at);
            break;

        default:
            BT_ERR("HFP HF: failed to create SLC due to unexpected AT command");
            hf_slc_disconnect(hf_at);
            err = -EINVAL;
            break;
    }

    return err;
}

static void bt_hfp_hf_audio_report(u8_t *addr, uint8_t state)
{
    bt_prf_hfp_hf_cb_param_t param;

    param.audio_stat.state = state;
    memcpy(param.audio_stat.peer_addr.val, addr, BT_STACK_BD_ADDR_LEN);

    if (bt_prf_hfp_hf_cb) {
        bt_prf_hfp_hf_cb(BT_PRF_HFP_HF_AUDIO_STATE_EVT, &param);
    }
}

static void bt_hfp_hf_conn_report(struct bt_hfp_hf *hf)
{
    bt_prf_hfp_hf_cb_param_t param;
    u8_t *                   addr = hf->rfcomm_dlc.session->br_chan.chan.conn->br.dst.val;

    param.conn_stat.state     = hf->connection_state;
    param.conn_stat.peer_feat = hf->ag_features;
    param.conn_stat.chld_feat = hf->chld_features;
    memcpy(param.conn_stat.peer_addr.val, addr, BT_STACK_BD_ADDR_LEN);

    if (bt_prf_hfp_hf_cb) {
        bt_prf_hfp_hf_cb(BT_PRF_HFP_HF_CONNECTION_STATE_EVT, &param);
    }
}

static void bt_hfp_hf_conn_report_withmac(u8_t *addr, uint8_t state)
{
    bt_prf_hfp_hf_cb_param_t param;

    param.conn_stat.state     = state;
    param.conn_stat.peer_feat = 0;
    param.conn_stat.chld_feat = 0;
    memcpy(param.conn_stat.peer_addr.val, addr, BT_STACK_BD_ADDR_LEN);

    if (bt_prf_hfp_hf_cb) {
        bt_prf_hfp_hf_cb(BT_PRF_HFP_HF_CONNECTION_STATE_EVT, &param);
    }
}

static void slc_completed(struct at_client *hf_at)
{
    struct bt_hfp_hf *hf  = CONTAINER_OF(hf_at, struct bt_hfp_hf, at);
    int               err = 0;

    hf->connection_state = BT_PRF_HFP_HF_CONNECTION_STATE_SLC_CONNECTED;

    bt_hfp_hf_conn_report(hf);

    BT_DBG("SLC established successfully");

    /* post slc cmd */
    err = hfp_hf_send_cmd(hf, NULL, cmd_complete, "AT+CMEE=%u", 1);
    if (err < 0) {
        hf_slc_disconnect(&hf->at);
        return;
    }

    hf->last_cmd = HFP_HF_AT_CMEE;
}

static int hf_slc_establish(struct bt_hfp_hf *hf)
{
    int err;

    BT_DBG("");

    err = hfp_slc_state_machine(hf, AT_RESULT_OK, 0);
    if (err < 0) {
        hf_slc_disconnect(&hf->at);
        return err;
    }

    return 0;
}

static struct bt_hfp_hf *bt_hfp_hf_lookup_bt_conn(struct bt_conn *conn)
{
    int               i;
    struct bt_hfp_hf *hf;

    for (i = 0; i < ARRAY_SIZE(bt_hfp_hf_pool); i++) {
        hf = &bt_hfp_hf_pool[i];

        if (hf->rfcomm_dlc.session && hf->rfcomm_dlc.session->br_chan.chan.conn == conn) {
            return hf;
        }
    }

    return NULL;
}

static struct bt_hfp_hf *bt_hfp_hf_get_new_connection(int *err)
{
    s8_t              i, free;
    struct bt_hfp_hf *hf;

    free = -1;

    for (i = 0; i < ARRAY_SIZE(bt_hfp_hf_pool); i++) {
        hf = &bt_hfp_hf_pool[i];

        /** FIXME: rfcomm will not reset session in some case */
        if (hf->rfcomm_dlc.session && hf->rfcomm_dlc.session->br_chan.chan.conn) {
            *err = -EALREADY;
            return hf;
        }

        if ((!hf->rfcomm_dlc.session || !hf->rfcomm_dlc.session->br_chan.chan.conn) && free == -1) {
            free = i;
        }
    }

    if (free == -1) {
        BT_DBG("No Free Connection");
        *err = -ENOMEM;
        return NULL;
    }

    memset(&bt_hfp_hf_pool[free], 0, sizeof(*hf));

    return &bt_hfp_hf_pool[free];
}

static void bt_hfp_hf_session_init(struct bt_hfp_hf *hf)
{
    hf->at.buf         = hf->hf_buffer;
    hf->at.buf_max_len = HF_MAX_BUF_LEN;

    hf->rfcomm_dlc.ops                = &hfp_hf_ops;
    hf->rfcomm_dlc.mtu                = BT_HFP_MAX_MTU;
    hf->rfcomm_dlc.required_sec_level = BT_SECURITY_L2;

    /* Set the supported features*/
    hf->hf_features = BT_HFP_HF_SUPPORTED_FEATURES;

    memset(hf->ind_table, -1, HF_MAX_AG_INDICATORS);
}

static void hfp_hf_connected(struct bt_rfcomm_dlc *dlc)
{
    struct bt_hfp_hf *hf = CONTAINER_OF(dlc, struct bt_hfp_hf, rfcomm_dlc);

    BT_ASSERT(hf);

    hf->connection_state = BT_PRF_HFP_HF_CONNECTION_STATE_CONNECTED;

    bt_hfp_hf_conn_report(hf);

    BT_DBG("hf connected");

    hfp_hf = hf;

    hf_slc_establish(hf);
}

static void hfp_hf_disconnected(struct bt_rfcomm_dlc *dlc)
{
    struct bt_hfp_hf *hf = CONTAINER_OF(dlc, struct bt_hfp_hf, rfcomm_dlc);

    hf->connection_state = BT_PRF_HFP_HF_CONNECTION_STATE_DISCONNECTED;
    hf->ag_features      = 0;
    hf->chld_features    = 0;

    bt_hfp_hf_conn_report(hf);

    /** clear hf state */
    hf->last_cmd = HFP_HF_AT_NONE;

    BT_DBG("hf disconnected!");
}

static void hfp_hf_recv(struct bt_rfcomm_dlc *dlc, struct net_buf *buf)
{
    struct bt_hfp_hf *hf = CONTAINER_OF(dlc, struct bt_hfp_hf, rfcomm_dlc);

    if (at_parse_input(&hf->at, buf) < 0) {
        BT_ERR("Parsing failed");
    }
}

static int bt_hfp_hf_accept(struct bt_conn *conn, struct bt_rfcomm_dlc **dlc)
{
    int               err;
    struct bt_hfp_hf *hf;

    BT_DBG("conn %p", conn);

    hf = bt_hfp_hf_get_new_connection(&err);

    if (!hf) {
        return err;
    }

    bt_hfp_hf_session_init(hf);

    *dlc = &hf->rfcomm_dlc;

    return 0;
}

static void hfp_hf_init(void)
{
    static struct bt_rfcomm_server chan = {
        .channel = BT_RFCOMM_CHAN_HFP_HF,
        .accept  = bt_hfp_hf_accept,
    };

    NET_BUF_POOL_INIT(hf_pool);
    NET_BUF_POOL_INIT(hfp_sdp_client_pool);

    bt_rfcomm_server_register(&chan);

    /* register SDP service */
    bt_sdp_register_service(&handfree_rec);

    BT_DBG("HFP HF initialized successfully");
}

/* SCO/eSCO established cb */
static void sco_connected(struct bt_conn *conn, u8_t err)
{
#if CONFIG_BT_HFP_AUDIO_I2S
    if (audio_hfp) {
        audio_hfp->start();
    }
#endif
    bt_hfp_hf_audio_report(conn->br.dst.val, BT_PRF_HFP_HF_AUDIO_STATE_CONNECTED);
}

static void sco_disconnected(struct bt_conn *conn, u8_t reason)
{
#if CONFIG_BT_HFP_AUDIO_I2S
    if (audio_hfp) {
        audio_hfp->stop();
    }
#endif
    bt_hfp_hf_audio_report(conn->br.dst.val, BT_PRF_HFP_HF_AUDIO_STATE_DISCONNECTED);
}

static struct bt_conn_cb sco_callbacks = {
    .connected    = sco_connected,
    .disconnected = sco_disconnected,
};

static struct bt_conn_cb hf_acl_callbacks = {
    .connected = hf_acl_connected,
};

static void hf_acl_connected(struct bt_conn *conn, u8_t err)
{
    BT_DBG("conn %p err %d", conn, err);

    if (err == 0) {
        err = bt_sdp_discover(conn, &discov_hfpag);

        if (err < 0) {
            BT_ERR("Unable to connect (err %u)", err);
            return;
        }
    } else {
        bt_hfp_hf_conn_report_withmac(conn->br.dst.val, BT_PRF_HFP_HF_CONNECTION_STATE_DISCONNECTED);
    }

    /** unregister callback to avoid double notify */
    bt_conn_cb_unregister(&hf_acl_callbacks);
}

static u8_t bt_hfp_hf_sdp_cb(struct bt_conn *conn, struct bt_sdp_client_result *result)
{
    u16_t             param;
    int               res;
    struct bt_hfp_hf *hf;

    if (result && result->resp_buf) {
        /*
         * Focus to get BT_SDP_ATTR_PROTO_DESC_LIST attribute item to
         * get HFPAG Server Channel Number operating on RFCOMM protocol.
         */
        res = bt_sdp_get_proto_param(result->resp_buf, BT_SDP_PROTO_RFCOMM, &param);
        if (res < 0) {
            BT_DBG("Error getting Server CN, "
                   "err %d",
                   res);
            goto done;
        }
        BT_DBG("HFPAG Server CN param 0x%04x", param);

        hf = bt_hfp_hf_get_new_connection(&res);

        if (!hf || res == -EALREADY) {
            BT_DBG("Already Connected");
            return BT_SDP_DISCOVER_UUID_STOP;
        }

        bt_hfp_hf_session_init(hf);

        res = bt_rfcomm_dlc_connect(conn, &hf->rfcomm_dlc, param);

        if (res < 0) {
            BT_ERR("Unable to connect (err %u)", res);
            return BT_SDP_DISCOVER_UUID_STOP;
        }

        bt_hfp_hf_conn_report_withmac(conn->br.dst.val, BT_PRF_HFP_HF_CONNECTION_STATE_CONNECTING);
    } else {
        BT_DBG("No SDP HFPAG data from remote");
    }
done:
    return BT_SDP_DISCOVER_UUID_CONTINUE;
}

/** Initialize the bluetooth HFP client module */
bt_stack_status_t bt_prf_hfp_hf_init(void)
{
    hfp_hf_init();

    bt_sco_conn_cb_register(&sco_callbacks);

#if CONFIG_BT_HFP_AUDIO_I2S
    audio_hfp = bt_hfp_audio_get_interface();
    if (audio_hfp) {
        audio_hfp->init();
    }
#endif

    return 0;
}

/** Register callback function */
void bt_prf_hfp_hf_register_callback(bt_prf_hfp_hf_cb_t callback)
{
    bt_prf_hfp_hf_cb = callback;
}

/** Connect to remote bluetooth HFP audio gateway(AG) device */
bt_stack_status_t bt_prf_hfp_hf_connect(bt_dev_addr_t *peer_addr)
{
    int             err;
    struct bt_conn *conn = NULL;

    conn = bt_conn_lookup_addr_br((bt_addr_t *)peer_addr->val);
    if (conn == NULL) {
        conn = bt_conn_create_br((bt_addr_t *)peer_addr->val, BT_BR_CONN_PARAM_DEFAULT);

        if (!conn) {
            BT_ERR("Connection failed");
            return -ENOMEM;
        } else {

            BT_DBG("Connection pending");

            bt_conn_cb_register(&hf_acl_callbacks);

            /* unref connection obj in advance as app user */
            bt_conn_unref(conn);

            return 0;
        }
    }

    /** unref connection for conn_lookup */
    bt_conn_unref(conn);

    err = bt_sdp_discover(conn, &discov_hfpag);

    if (err < 0) {
        BT_ERR("Unable to connect (err %u)", err);
        return err;
    }

    BT_DBG("connection pending\n");

    return 0;
}

/** Disconnect from the remote HFP audio gateway */
bt_stack_status_t bt_prf_hfp_hf_disconnect(bt_dev_addr_t *peer_addr)
{
    int               err;
    struct bt_conn *  conn = NULL;
    struct bt_hfp_hf *hf;

    conn = bt_conn_lookup_addr_br((bt_addr_t *)peer_addr->val);
    if (!conn) {
        BT_DBG("No ACL Connection\n");
        return 0;
    }

    /** unref connection for conn_lookup */
    bt_conn_unref(conn);

    hf = bt_hfp_hf_lookup_bt_conn(conn);

    if (!hf) {
        BT_DBG("No Connection");
        return -EALREADY;
    }

    err = bt_rfcomm_dlc_disconnect(&hf->rfcomm_dlc);
    if (err) {
        BT_ERR("Rfcomm: Unable to disconnect :%d", -err);
        return err;
    }

    bt_hfp_hf_conn_report_withmac(conn->br.dst.val, BT_PRF_HFP_HF_CONNECTION_STATE_DISCONNECTING);

    return 0;
}

/** Create audio connection with remote HFP AG */
bt_stack_status_t bt_prf_hfp_hf_audio_connect(bt_dev_addr_t *peer_addr)
{
    struct bt_conn *  conn = NULL;
    struct bt_hfp_hf *hf;

    conn = bt_conn_lookup_addr_br((bt_addr_t *)peer_addr->val);

    if (!conn) {
        BT_DBG("No ACL Connection\n");
        return 0;
    }

    /** unref connection for conn_lookup */
    bt_conn_unref(conn);

    hf = bt_hfp_hf_lookup_bt_conn(conn);

    if (!hf) {
        BT_DBG("No Connection");
        return -EALREADY;
    }

    if (hf->sco_conn) {
        BT_DBG("SCO Connected");
        return -EALREADY;
    }

    /* notify to AG to start sco establish */
    hfp_hf_send_at_bcc(hf);

    bt_hfp_hf_audio_report(peer_addr->val, BT_PRF_HFP_HF_AUDIO_STATE_CONNECTING);

    return 0;
}

/** Release the established audio connection with remote HFP AG */
bt_stack_status_t bt_prf_hfp_hf_audio_disconnect(bt_dev_addr_t *peer_addr)
{
    struct bt_conn *  conn = NULL;
    struct bt_hfp_hf *hf;

    conn = bt_conn_lookup_addr_br((bt_addr_t *)peer_addr->val);

    if (!conn) {
        BT_DBG("No ACL Connection\n");
        return 0;
    }

    /** unref connection for conn_lookup */
    bt_conn_unref(conn);

    hf = bt_hfp_hf_lookup_bt_conn(conn);

    if (!hf) {
        BT_DBG("No Connection");
        return -EALREADY;
    }

    if (!hf->sco_conn) {
        BT_DBG("No SCO");
        return -EALREADY;
    }

    /** disconect sco connection */
    bt_conn_disconnect(conn, BT_HCI_ERR_REMOTE_USER_TERM_CONN);

    BT_DBG("No Connection");

    return 0;
}

/** Answer an incoming call */
bt_stack_status_t bt_prf_hfp_hf_answer_call(void)
{
    struct bt_hfp_hf *hf = hfp_hf;

    if (hf == NULL) {
        return -EINVAL;
    }

    return hfp_hf_send_at_ata(hf);
}

/** Reject an incoming call */
bt_stack_status_t bt_prf_hfp_hf_reject_call(void)
{
    struct bt_hfp_hf *hf = hfp_hf;

    if (hf == NULL) {
        return -EINVAL;
    }

    return hfp_hf_send_at_chup(hf);
}

/** Place a call with a specified number */
bt_stack_status_t bt_prf_hfp_hf_dial(char *number)
{
    struct bt_hfp_hf *hf = hfp_hf;

    if (hf == NULL) {
        return -EINVAL;
    }

    if (strlen(number) != 0) {
        return hfp_hf_send_at_atd(hf, number);
    } else {
        return hfp_hf_send_at_bldn(hf);
    }
}

/** Send call hold and multiparty commands */
bt_stack_status_t bt_prf_hfp_hf_send_chld_cmd(bt_prf_hfp_hf_chld_type_t chld, int idx)
{
    struct bt_hfp_hf *hf  = hfp_hf;
    int               ret = 0;

    if (hf == NULL) {
        return -EINVAL;
    }

    switch (chld) {
        case BT_PRF_HFP_HF_CHLD_TYPE_REL:
        case BT_PRF_HFP_HF_CHLD_TYPE_REL_ACC:
        case BT_PRF_HFP_HF_CHLD_TYPE_HOLD_ACC:
        case BT_PRF_HFP_HF_CHLD_TYPE_MERGE:
        case BT_PRF_HFP_HF_CHLD_TYPE_MERGE_DETACH:
            ret = hfp_hf_send_at_chld(hf, chld + '0', 0);
            break;
        case BT_PRF_HFP_HF_CHLD_TYPE_REL_X:
            ret = hfp_hf_send_at_chld(hf, '1', idx);
            break;
        case BT_PRF_HFP_HF_CHLD_TYPE_PRIV_X:
            ret = hfp_hf_send_at_chld(hf, '2', idx);
            break;
        default:
            break;
    }

    return ret;
}

/** Volume synchronization with AG */
bt_stack_status_t bt_prf_hfp_hf_vol_update(pbt_prf_hfp_hf_vol_ctrl_target_t type, int volume)
{
    struct bt_hfp_hf *hf  = hfp_hf;
    int               ret = 0;

    if (hf == NULL) {
        return -EINVAL;
    }

    switch (type) {
        case BT_PRF_HFP_HF_VOLUME_CONTROL_TARGET_SPK:
            ret = hfp_hf_send_at_vgs(hf, volume);
            break;
        case BT_PRF_HFP_HF_VOLUME_CONTROL_TARGET_MIC:
            ret = hfp_hf_send_at_vgm(hf, volume);
            break;
        default:
            ret = -EINVAL;
    }

    return ret;
}
#endif
