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
#include <bluetooth/audio_output.h>
#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BT_DEBUG_AVRCP)
#define LOG_MODULE_NAME bt_avrcp
#include "common/log.h"

#include "hci_core.h"
#include "conn_internal.h"
#include "l2cap_internal.h"
#include "avrcp_internal.h"

#define AVRCP_TG_EVENT_REPORT(event, param)                                                                            \
    do {                                                                                                               \
        if (avrcp_tg_cb) {                                                                                             \
            avrcp_tg_cb(event, param);                                                                                 \
        }                                                                                                              \
    } while (0);

static bt_prf_avrcp_tg_cb_t avrcp_tg_cb;
static uint16_t             avrcp_tg_cap_bit_mask;
static uint8_t              avrcp_tg_rn_tl[BT_PRF_AVRCP_NOTIFICATION_MAX_EVT];

#if CONFIG_BT_AVRCP_VOL_CONTROL
static uint8_t                       avrcp_vol = 50;
static const bt_a2dp_audio_output_t *avrcp_audio_output;
#endif

static u8_t avrcp_tg_get_cap(u8_t *param)
{
    u8_t i;
    u8_t cnt = 0;

    for (i = 0; i < 16; i++) {
        if ((1 << i) & avrcp_tg_cap_bit_mask) {
            param[cnt++] = i;
        }
    }

    return cnt;
}

static bt_prf_avrcp_ctype_response_t avrcp_tg_get_response_from_ctype(u8_t ctype)
{
    switch (ctype) {
        case BT_AVRCP_CTYPE_CONTROL:
            return BT_AVRCP_RESPONSE_ACCEPTED;
        case BT_AVRCP_CTYPE_STATUS:
            return BT_AVRCP_RESPONSE_IMPLEMENTED_STABLE;
        case BT_AVRCP_CTYPE_NOTIFY:
            return BT_AVRCP_RESPONSE_INTERIM;
        default:
            return -1;
    }

    return -1;
}

/** AVRCP Interface callbacks */
static const int avrcp_tg_data_handle(struct bt_avrcp *session, uint8_t tl, struct net_buf *buf)
{
    int                                     event = -1;
    u8_t                                    resp_param[17];
    u8_t                                    cap_id;
    u16_t                                   param_len;
    struct net_buf *                        resp_buf = NULL;
    struct bt_avc_hdr *                     avc_hdr;
    struct bt_avrcp_hdr *                   avrcp_hdr;
    bt_prf_avrcp_tg_cb_param_t              report_param;
    struct bt_avrcp_pass_througt_packet *   pt;
    struct bt_avc_unit_info_resp_packet *   unit_info;
    struct bt_avc_subunit_info_resp_packet *subunit_info;

    if (buf->len < sizeof(*avc_hdr)) {
        BT_ERR("ERROR: len");
        return -EINVAL;
    }

    avc_hdr = net_buf_pull_mem(buf, sizeof(*avc_hdr));

    BT_DBG("opcode 0x%02x", avc_hdr->opcode);

    switch (avc_hdr->opcode) {
        case BT_AVCTP_OPCODE_UNIT_INFO:
            resp_buf              = avrcp_create_pdu(BT_AVCTP_RESPONSE, BT_AVCTP_OPCODE_UNIT_INFO,
                                        BT_AVRCP_RESPONSE_IMPLEMENTED_STABLE, tl);
            unit_info             = net_buf_add(resp_buf, sizeof(*unit_info));
            unit_info->constant_7 = 7;
            unit_info->unit       = 0;
            unit_info->unit_type  = BT_AVRCP_SUBUNIT_TYPE_PANEL;
            sys_put_be24(BT_SIG_AVC_COMPANY_ID, unit_info->company_id);
            break;
        case BT_AVCTP_OPCODE_SUBUNIT_INFO:
            resp_buf                     = avrcp_create_pdu(BT_AVCTP_RESPONSE, BT_AVCTP_OPCODE_SUBUNIT_INFO,
                                        BT_AVRCP_RESPONSE_IMPLEMENTED_STABLE, tl);
            subunit_info                 = net_buf_add(resp_buf, sizeof(*subunit_info));
            subunit_info->extention_code = 7;
            subunit_info->rfa1           = 0;
            subunit_info->rfa2           = 0;
            subunit_info->page           = 0;
            subunit_info->max_subunit_id = 0;
            subunit_info->subunit_type   = BT_AVRCP_SUBUNIT_TYPE_PANEL;
            memset(subunit_info->constant_FF, 0xFF, 3);
            break;
        case BT_AVCTP_OPCODE_PASS_THROUGH:
            pt                                    = net_buf_pull_mem(buf, sizeof(*pt));
            report_param.psth_cmd.operation_id    = pt->operation_id;
            report_param.psth_cmd.operation_state = pt->operation_state;
            event                                 = BT_PRF_AVRCP_TG_PASSTHROUGH_CMD_EVT;
            resp_buf = bt_avrcp_create_pass_through_pdu(pt->operation_id, pt->operation_state, tl,
                                                        BT_AVRCP_RESPONSE_ACCEPTED);

            break;
        case BT_AVCTP_OPCODE_VENDOR_DEPENDENT:
            avrcp_hdr = net_buf_pull_mem(buf, sizeof(*avrcp_hdr));
            param_len = net_buf_pull_be16(buf);

            if (param_len != buf->len) {
                BT_ERR("ERROR: param_len");
                return -EINVAL;
            }

            BT_DBG("pdu id 0x%02x, param_len %d", avrcp_hdr->pdu_id, param_len);

            switch (avrcp_hdr->pdu_id) {
                case BT_AVRCP_PDU_ID_GET_CAPABILITIES:
                    if (buf->len < 1) {
                        return -EINVAL;
                    }
                    cap_id = net_buf_pull_u8(buf);
                    switch (cap_id) {
                        case BT_AVRCP_CAPABLITITY_EVENT_ID:
                            resp_param[0] = cap_id;
                            resp_param[1] = avrcp_tg_get_cap(resp_param + 2);
                            resp_buf      = bt_avrcp_create_vendor_depenent_pdu(
                                avrcp_hdr->pdu_id, resp_param, resp_param[1] + 2, tl,
                                avrcp_tg_get_response_from_ctype(avc_hdr->cr));
                            break;
                        case BT_AVRCP_CAPABLITITY_COMPANY_ID:
                            resp_param[0] = cap_id;
                            resp_param[1] = 0;
                            /** no supported company for default */
                            resp_buf = bt_avrcp_create_vendor_depenent_pdu(
                                avrcp_hdr->pdu_id, resp_param, 2, tl, avrcp_tg_get_response_from_ctype(avc_hdr->cr));
                            break;
                        default:
                            resp_param[0] = BT_AVRCP_STATUS_INVALID_PARAMETER;
                            resp_buf      = bt_avrcp_create_vendor_depenent_pdu(avrcp_hdr->pdu_id, resp_param, 1, tl,
                                                                           BT_AVRCP_RESPONSE_REJECTED);
                            break;
                    }
                    break;
                case BT_AVRCP_PDU_ID_REGISTER_NOTIFICATION:
                    if (buf->len < 5) {
                        return -EINVAL;
                    }
                    report_param.reg_ntf.event_id                 = net_buf_pull_u8(buf);
                    report_param.reg_ntf.interval                 = net_buf_pull_be32(buf);
                    avrcp_tg_rn_tl[report_param.reg_ntf.event_id] = tl;
                    if ((1 << report_param.reg_ntf.event_id) & avrcp_tg_cap_bit_mask) {
#if CONFIG_BT_AVRCP_VOL_CONTROL
                        if (report_param.reg_ntf.event_id == BT_PRF_AVRCP_NOTIFICATION_VOLUME_CHANGE) {
                            resp_param[0] = report_param.reg_ntf.event_id;
                            resp_param[1] = avrcp_vol;
                            resp_buf      = bt_avrcp_create_vendor_depenent_pdu(avrcp_hdr->pdu_id, resp_param, 2, tl,
                                                                           BT_AVRCP_RESPONSE_INTERIM);
                            break;
                        }
#endif
                        event = BT_PRF_AVRCP_TG_REGISTER_NOTIFICATION_EVT;
                    } else if (report_param.reg_ntf.event_id >= BT_PRF_AVRCP_NOTIFICATION_MAX_EVT
                               || report_param.reg_ntf.event_id < BT_PRF_AVRCP_NOTIFICATION_PLAY_STATUS_CHANGE)
                    {
                        resp_param[0] = BT_AVRCP_STATUS_INVALID_PARAMETER;
                        resp_buf      = bt_avrcp_create_vendor_depenent_pdu(avrcp_hdr->pdu_id, resp_param, 1, tl,
                                                                       BT_AVRCP_RESPONSE_REJECTED);
                    } else {
                        resp_param[0] = avrcp_hdr->pdu_id;
                        resp_buf      = bt_avrcp_create_vendor_depenent_pdu(avrcp_hdr->pdu_id, resp_param, 1, tl,
                                                                       BT_AVRCP_RESPONSE_NOT_IMPLEMENTED);
                    }
                    break;
                case BT_AVRCP_PDU_ID_SET_ABSOLUTE_VOLUME:
                    if (buf->len != 1) {
                        resp_param[0] = BT_AVRCP_STATUS_INVALID_COMMAND;
                        resp_buf      = bt_avrcp_create_vendor_depenent_pdu(avrcp_hdr->pdu_id, resp_param, 1, tl,
                                                                       BT_AVRCP_RESPONSE_REJECTED);
                        break;
                    }
                    report_param.set_abs_vol.volume = net_buf_pull_u8(buf) & 0x7F;
                    event                           = BT_PRF_AVRCP_TG_SET_ABSOLUTE_VOLUME_CMD_EVT;
                    resp_param[0]                   = report_param.set_abs_vol.volume;
                    resp_buf = bt_avrcp_create_vendor_depenent_pdu(avrcp_hdr->pdu_id, resp_param, 1, tl,
                                                                   avrcp_tg_get_response_from_ctype(avc_hdr->cr));

#if CONFIG_BT_AVRCP_VOL_CONTROL
                    /** set device volume inside stack */
                    avrcp_vol = report_param.set_abs_vol.volume;
                    avrcp_audio_output->vol(avrcp_vol);
#endif
                    break;
                case BT_AVRCP_PDU_ID_REQUEST_CONTINUING_RESPONSE:
                case BT_AVRCP_PDU_ID_REQUEST_ABORT_CONTINUING_RESPONSE:
                case BT_AVRCP_PDU_ID_GET_PLAY_STATUS:
                case BT_AVRCP_PDU_ID_SET_ADDRESSED_PLAYER:
                case BT_AVRCP_PDU_ID_SET_BROWSED_PLAYER:
                case BT_AVRCP_PDU_ID_GET_FOLDER_ITEMS:
                case BT_AVRCP_PDU_ID_CHANGE_PATH:
                case BT_AVRCP_PDU_ID_GET_ITEM_ATTRIBUTES:
                case BT_AVRCP_PDU_ID_PLAY_ITEM:
                case BT_AVRCP_PDU_ID_GET_TOTAL_NUMBER_OF_ITEMS:
                case BT_AVRCP_PDU_ID_SEARCH:
                case BT_AVRCP_PDU_ID_ADD_TO_NOW_PLAYING:
                case BT_AVRCP_PDU_ID_LIST_PLAYER_APPLICATION_SETTING_ATTRIBUTES:
                case BT_AVRCP_PDU_ID_LIST_PLAYER_APPLICATION_SETTING_VALUE:
                case BT_AVRCP_PDU_ID_GET_CURRENT_PLAYER_APPLICATION_SETTING_VALUE:
                case BT_AVRCP_PDU_ID_SET_PLAYER_APPLICATION_SETTING_VALUE:
                case BT_AVRCP_PDU_ID_GET_PLAYER_APPLICATION_SETTING_ATTRIBUTE_TEXT:
                case BT_AVRCP_PDU_ID_GET_PLAYER_APPLICATION_SETTING_VALUE_TEXT:
                case BT_AVRCP_PDU_ID_INFORM_DISPLAYABLE_CHARACTER_SET:
                case BT_AVRCP_PDU_ID_INFORM_BATTERY_STATUS_OF_CT:
                case BT_AVRCP_PDU_ID_GET_ELEMENT_ATTRIBUTES:
                    BT_ERR("Not supported pdu id %d", avrcp_hdr->pdu_id);
                    resp_param[0] = avrcp_hdr->pdu_id;
                    resp_buf      = bt_avrcp_create_vendor_depenent_pdu(avrcp_hdr->pdu_id, resp_param, 1, tl,
                                                                   BT_AVRCP_RESPONSE_NOT_IMPLEMENTED);
                    break;
                default:
                    BT_ERR("Invalid pdu id %d", avrcp_hdr->pdu_id);
                    resp_param[0] = BT_AVRCP_STATUS_INVALID_COMMAND;
                    resp_buf      = bt_avrcp_create_vendor_depenent_pdu(avrcp_hdr->pdu_id, resp_param, 1, tl,
                                                                   BT_AVRCP_RESPONSE_REJECTED);
                    break;
            }
            break;
        default:
            break;
    }

    if (event >= 0) {
        AVRCP_TG_EVENT_REPORT(event, &report_param);
    }

    if (resp_buf) {
        bt_avrcp_send(resp_buf);
    }

    return 0;
}

static void avrcp_report_connection(uint8_t connected, uint8_t *addr)
{
    bt_prf_avrcp_tg_cb_param_t resp;

    resp.conn_stat.connected = connected;
    memcpy(resp.conn_stat.peer_addr.val, addr, BT_STACK_BD_ADDR_LEN);

    AVRCP_TG_EVENT_REPORT(BT_PRF_AVRCP_TG_CONNECTION_STATE_EVT, &resp);
}

static int avrcp_connected(struct bt_avrcp *session)
{
    avrcp_report_connection(1, session->br_chan.chan.conn->br.dst.val);

    return 0;
}

static int avrcp_disconnected(struct bt_avrcp *session)
{
    avrcp_report_connection(0, session->br_chan.chan.conn->br.dst.val);

    return 0;
}

static const struct bt_avrcp_ind_cb cb_ind = {
    .connected    = avrcp_connected,
    .disconnected = avrcp_disconnected,
};

/** AVRCP target event callback */
static const struct bt_avrcp_event_cb avrcp_event_tg_cb = {
    .ind  = &cb_ind,
    .data = avrcp_tg_data_handle,
};

/** AVRCP target SDP attribute table */
static struct bt_sdp_attribute avrcp_tg_attrs[] = {
    BT_SDP_NEW_SERVICE,
    BT_SDP_LIST(BT_SDP_ATTR_SVCLASS_ID_LIST, BT_SDP_TYPE_SIZE_VAR(BT_SDP_SEQ8, 3),
                BT_SDP_DATA_ELEM_LIST(
                    {
                        BT_SDP_TYPE_SIZE(BT_SDP_UUID16),
                        BT_SDP_ARRAY_16(BT_SDP_AV_REMOTE_TARGET_SVCLASS),
                    }, )),
    BT_SDP_LIST(
        BT_SDP_ATTR_PROTO_DESC_LIST, BT_SDP_TYPE_SIZE_VAR(BT_SDP_SEQ8, 16),
        BT_SDP_DATA_ELEM_LIST(
            {
                BT_SDP_TYPE_SIZE_VAR(BT_SDP_SEQ8, 6),
                BT_SDP_DATA_ELEM_LIST({ BT_SDP_TYPE_SIZE(BT_SDP_UUID16), BT_SDP_ARRAY_16(BT_SDP_PROTO_L2CAP) },
                                      { BT_SDP_TYPE_SIZE(BT_SDP_UINT16), BT_SDP_ARRAY_16(BT_L2CAP_PSM_AVCTP) }, ),
            },
            {
                BT_SDP_TYPE_SIZE_VAR(BT_SDP_SEQ8, 6),
                BT_SDP_DATA_ELEM_LIST({ BT_SDP_TYPE_SIZE(BT_SDP_UUID16), BT_SDP_ARRAY_16(BT_SDP_PROTO_AVCTP) },
                                      {
                                          BT_SDP_TYPE_SIZE(BT_SDP_UINT16),
                                          BT_SDP_ARRAY_16(BT_AVCTP_VERSION) // version
                                      }, ),
            }, )),
    BT_SDP_LIST(
        BT_SDP_ATTR_PROFILE_DESC_LIST, BT_SDP_TYPE_SIZE_VAR(BT_SDP_SEQ8, 8),
        BT_SDP_DATA_ELEM_LIST(
            {
                BT_SDP_TYPE_SIZE_VAR(BT_SDP_SEQ8, 6),
                BT_SDP_DATA_ELEM_LIST({ BT_SDP_TYPE_SIZE(BT_SDP_UUID16), BT_SDP_ARRAY_16(BT_SDP_AV_REMOTE_SVCLASS) },
                                      {
                                          BT_SDP_TYPE_SIZE(BT_SDP_UINT16),
                                          BT_SDP_ARRAY_16(BT_AVRCP_VERSION) // version
                                      }, ),
            }, )),
    BT_SDP_SUPPORTED_FEATURES(0x0002),
};

static struct bt_sdp_record avrcp_tg_rec = BT_SDP_RECORD(avrcp_tg_attrs);

/** AVRCP target init function */
bt_stack_status_t bt_prf_avrcp_tg_init(void)
{
    int err;

    err = bt_avrcp_init();

    if (err) {
        BT_ERR("Err: %d", err);
        return err;
    }

    bt_avrcp_tg_register(&avrcp_event_tg_cb);

    err = bt_sdp_register_service(&avrcp_tg_rec);

    if (err) {
        BT_ERR("Err: %d", err);
    }

#if CONFIG_BT_AVRCP_VOL_CONTROL
    avrcp_audio_output = bt_a2dp_audio_output_get_interface();
#endif

    return err;
}

/** AVRCP target register callback function */
void bt_prf_avrcp_tg_register_callback(bt_prf_avrcp_tg_cb_t callback)
{
    avrcp_tg_cb = callback;
}

/** AVRCP target set register notification capability function */
void bt_prf_avrcp_tg_set_rn_evt_cap(uint16_t cap_bit_mask)
{
    avrcp_tg_cap_bit_mask = cap_bit_mask;
}

/** AVRCP target get register notification capability function */
uint16_t bt_prf_avrcp_tg_get_rn_evt_cap(void)
{
    return avrcp_tg_cap_bit_mask;
}

/** AVRCP target set send register notification response */
bt_stack_status_t bt_prf_avrcp_tg_send_rn_rsp(bt_prf_avrcp_rn_event_ids_t event_id, bt_prf_avrcp_ctype_response_t rsp,
                                              bt_prf_avrcp_rn_param_t *param)
{
    struct net_buf *buf;
    uint8_t         p[12];
    uint16_t        p_len;

    switch (event_id) {
        /** noly support VOLUME_CHANGE for now */
        case BT_PRF_AVRCP_NOTIFICATION_VOLUME_CHANGE:
            p_len = 2;
            p[0]  = event_id;
            p[1]  = param->volume & 0x7F;
            break;
        default:
            return -EINVAL;
    }

    buf = bt_avrcp_create_vendor_depenent_pdu(BT_AVRCP_PDU_ID_REGISTER_NOTIFICATION, p, p_len, avrcp_tg_rn_tl[event_id],
                                              rsp);

    if (buf == NULL) {
        return -ENOMEM;
    }

    return bt_avrcp_send(buf);
}

#if CONFIG_BT_AVRCP_VOL_CONTROL
bt_stack_status_t bt_prf_avrcp_tg_notify_vol_changed(uint8_t volume)
{
    bt_prf_avrcp_rn_param_t p;

    if (volume > BT_AVRCP_MAX_VOLUME) {
        return -EINVAL;
    }

    avrcp_audio_output->vol(volume);
    avrcp_vol = volume;
    p.volume  = volume;

    return bt_prf_avrcp_tg_send_rn_rsp(BT_PRF_AVRCP_NOTIFICATION_VOLUME_CHANGE, BT_AVRCP_RESPONSE_CHANGED_STABLE, &p);
}
#endif
