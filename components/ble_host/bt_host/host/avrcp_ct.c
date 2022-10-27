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
#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BT_DEBUG_AVRCP)
#define LOG_MODULE_NAME bt_avrcp
#include "common/log.h"

#include "hci_core.h"
#include "conn_internal.h"
#include "l2cap_internal.h"
#include "avrcp_internal.h"

#define AVRCP_PDU_ID_GET_PLAY_STATUS_PACKET_LEN     9
#define AVRCP_PDU_ID_GET_CAPABILITIES_PACKET_LEN    2
#define AVRCP_PDU_ID_SET_ABSOLUTE_VOLUME_PACKET_LEN 1

#ifndef CONFIG_AVRCP_PLAYBACK_SECOND
#define CONFIG_AVRCP_PLAYBACK_SECOND 2
#endif

#define AVRCP_CT_EVENT_REPORT(event, param)                                                                            \
    do {                                                                                                               \
        if (avrcp_ct_cb) {                                                                                             \
            avrcp_ct_cb(event, param);                                                                                 \
        }                                                                                                              \
    } while (0);

static bt_prf_avrcp_ct_cb_t avrcp_ct_cb;

static bt_prf_avrcp_ctype_response_t avrcp_get_ctype_from_pdu_id(u8_t pdu_id)
{
    switch (pdu_id) {
        case BT_AVRCP_PDU_ID_SET_PLAYER_APPLICATION_SETTING_VALUE:
        case BT_AVRCP_PDU_ID_INFORM_DISPLAYABLE_CHARACTER_SET:
        case BT_AVRCP_PDU_ID_INFORM_BATTERY_STATUS_OF_CT:
        case BT_AVRCP_PDU_ID_REQUEST_CONTINUING_RESPONSE:
        case BT_AVRCP_PDU_ID_REQUEST_ABORT_CONTINUING_RESPONSE:
        case BT_AVRCP_PDU_ID_SET_ABSOLUTE_VOLUME:
        case BT_AVRCP_PDU_ID_SET_ADDRESSED_PLAYER:
        case BT_AVRCP_PDU_ID_PLAY_ITEM:
        case BT_AVRCP_PDU_ID_ADD_TO_NOW_PLAYING:
            return BT_AVRCP_CTYPE_CONTROL;
        case BT_AVRCP_PDU_ID_GET_CAPABILITIES:
        case BT_AVRCP_PDU_ID_LIST_PLAYER_APPLICATION_SETTING_ATTRIBUTES:
        case BT_AVRCP_PDU_ID_LIST_PLAYER_APPLICATION_SETTING_VALUE:
        case BT_AVRCP_PDU_ID_GET_CURRENT_PLAYER_APPLICATION_SETTING_VALUE:
        case BT_AVRCP_PDU_ID_GET_PLAYER_APPLICATION_SETTING_ATTRIBUTE_TEXT:
        case BT_AVRCP_PDU_ID_GET_PLAYER_APPLICATION_SETTING_VALUE_TEXT:
        case BT_AVRCP_PDU_ID_GET_ELEMENT_ATTRIBUTES:
        case BT_AVRCP_PDU_ID_GET_PLAY_STATUS:
            return BT_AVRCP_CTYPE_STATUS;
        case BT_AVRCP_PDU_ID_REGISTER_NOTIFICATION:
            return BT_AVRCP_CTYPE_NOTIFY;
        case BT_AVRCP_PDU_ID_SET_BROWSED_PLAYER:
        case BT_AVRCP_PDU_ID_GET_FOLDER_ITEMS:
        case BT_AVRCP_PDU_ID_CHANGE_PATH:
        case BT_AVRCP_PDU_ID_GET_ITEM_ATTRIBUTES:
        case BT_AVRCP_PDU_ID_GET_TOTAL_NUMBER_OF_ITEMS:
        case BT_AVRCP_PDU_ID_SEARCH:
        case BT_AVRCP_PDU_ID_GENERAL_REJECT:
            return -1;
    }

    return -1;
}

/** register notification response handle */
static void avrcp_ct_handle_rn(uint8_t ctype, struct net_buf *buf, uint16_t param_len)
{
    int                        need_report = 0;
    uint8_t                    event_id;
    bt_prf_avrcp_ct_cb_param_t resp;
    bt_prf_avrcp_rn_param_t *  param = &resp.change_ntf.event_parameter;

    switch (ctype) {
        case BT_AVRCP_RESPONSE_INTERIM:
            /** recv INTERIM response, register sucess */
            BT_DBG("rn sucess");
            break;
        case BT_AVRCP_RESPONSE_CHANGED_STABLE:
            if (buf->len < 1) {
                return;
            }
            event_id                 = net_buf_pull_u8(buf);
            resp.change_ntf.event_id = event_id;
            BT_DBG("rn changed %d, len %d", event_id, buf->len);
            switch (event_id) {
                case BT_PRF_AVRCP_NOTIFICATION_PLAY_STATUS_CHANGE:
                    if (buf->len < 1) {
                        return;
                    }
                    param->playback = net_buf_pull_u8(buf);
                    need_report     = 1;
                    break;
                case BT_PRF_AVRCP_NOTIFICATION_TRACK_CHANGE:
                    if (buf->len < sizeof(param->elm_id)) {
                        return;
                    }
                    memcpy(param->elm_id, net_buf_pull_mem(buf, sizeof(param->elm_id)), sizeof(param->elm_id));
                    need_report = 1;
                    break;
                case BT_PRF_AVRCP_NOTIFICATION_PLAY_POS_CHANGED:
                    if (buf->len < 4) {
                        return;
                    }
                    param->play_pos = net_buf_pull_be32(buf);
                    need_report     = 1;
                    break;
                case BT_PRF_AVRCP_NOTIFICATION_BATTERY_STATUS_CHANGE:
                    if (buf->len < 1) {
                        return;
                    }
                    param->batt = net_buf_pull_u8(buf);
                    need_report = 1;
                    break;
                case BT_PRF_AVRCP_NOTIFICATION_VOLUME_CHANGE:
                    if (buf->len < 1) {
                        return;
                    }
                    param->volume = net_buf_pull_u8(buf) & 0x7F;
                    need_report   = 1;
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }

    if (need_report) {
        AVRCP_CT_EVENT_REPORT(BT_PRF_AVRCP_CT_CHANGE_NOTIFY_EVT, &resp);
    }
}

/** AVRCP Interface callbacks */
static const int avrcp_ct_data_handle(struct bt_avrcp *session, uint8_t tl, struct net_buf *buf)
{
    int                                  event = -1;
    int                                  i;
    u8_t                                 cap_id;
    u16_t                                param_len;
    u8_t                                 attrnum;
    struct bt_avc_hdr *                  avc_hdr;
    struct bt_avrcp_hdr *                avrcp_hdr;
    struct bt_avrcp_pass_througt_packet *pt;
    bt_prf_avrcp_ct_cb_param_t           resp;

    if (buf->len < sizeof(*avc_hdr)) {
        BT_ERR("Err: len");
        return -EINVAL;
    }

    avc_hdr = net_buf_pull_mem(buf, sizeof(*avc_hdr));

    BT_DBG("opcode 0x%02x", avc_hdr->opcode);

    if (avc_hdr->cr == BT_AVRCP_RESPONSE_REJECTED) {
        BT_ERR("Err: remote reject");
        return -EINVAL;
    }

    switch (avc_hdr->opcode) {
        /** ignore UNIT_INFO SUBUNIT_INFO opcode */
        case BT_AVCTP_OPCODE_UNIT_INFO:
        case BT_AVCTP_OPCODE_SUBUNIT_INFO:
            break;
        case BT_AVCTP_OPCODE_PASS_THROUGH:
            pt                            = net_buf_pull_mem(buf, sizeof(*pt));
            resp.psth_rsp.operation_id    = pt->operation_id;
            resp.psth_rsp.operation_state = pt->operation_state;
            event                         = BT_PRF_AVRCP_CT_PASSTHROUGH_RSP_EVT;
            break;
        case BT_AVCTP_OPCODE_VENDOR_DEPENDENT:
            if (buf->len < sizeof(*avrcp_hdr) + 2) {
                return -EINVAL;
            }
            avrcp_hdr = net_buf_pull_mem(buf, sizeof(*avrcp_hdr));
            param_len = net_buf_pull_be16(buf);

            if (param_len != buf->len) {
                BT_ERR("Err: param_len");
                return -EINVAL;
            }

            BT_DBG("pdu id 0x%02x, param_len %d", avrcp_hdr->pdu_id, param_len);

            switch (avrcp_hdr->pdu_id) {
                case BT_AVRCP_PDU_ID_GET_CAPABILITIES:
                    if (buf->len < AVRCP_PDU_ID_GET_CAPABILITIES_PACKET_LEN) {
                        return -EINVAL;
                    }
                    cap_id                         = net_buf_pull_u8(buf);
                    resp.get_rn_caps_rsp.cap_count = net_buf_pull_u8(buf);
                    if (cap_id == BT_AVRCP_CAPABLITITY_EVENT_ID) {
                        resp.get_rn_caps_rsp.evt_set.bits = 0;
                        for (i = 0; i < resp.get_rn_caps_rsp.cap_count; i++) {
                            resp.get_rn_caps_rsp.evt_set.bits |= (1 << net_buf_pull_u8(buf));
                        }
                        event = BT_PRF_AVRCP_CT_GET_RN_CAPABILITIES_RSP_EVT;
                    }
                    break;
                case BT_AVRCP_PDU_ID_GET_PLAY_STATUS:
                    if (buf->len < AVRCP_PDU_ID_GET_PLAY_STATUS_PACKET_LEN) {
                        return -EINVAL;
                    }
                    resp.get_rn_play_status_rsp.song_len    = net_buf_pull_be32(buf);
                    resp.get_rn_play_status_rsp.song_pos    = net_buf_pull_be32(buf);
                    resp.get_rn_play_status_rsp.play_status = net_buf_pull_u8(buf);
                    event                                   = BT_PRF_AVRCP_CT_PLAY_STATUS_RSP_EVT;
                    break;
                case BT_AVRCP_PDU_ID_REGISTER_NOTIFICATION:
                    avrcp_ct_handle_rn(avc_hdr->cr, buf, param_len);
                    break;
                case BT_AVRCP_PDU_ID_SET_ABSOLUTE_VOLUME:
                    if (buf->len < AVRCP_PDU_ID_SET_ABSOLUTE_VOLUME_PACKET_LEN) {
                        return -EINVAL;
                    }
                    resp.set_volume_rsp.volume = net_buf_pull_u8(buf) & 0x7F;
                    event                      = BT_PRF_AVRCP_CT_SET_ABSOLUTE_VOLUME_RSP_EVT;
                    break;
                case BT_AVRCP_PDU_ID_GET_ELEMENT_ATTRIBUTES:
                    if (buf->len < 1) {
                        return -EINVAL;
                    }
                    attrnum = net_buf_pull_u8(buf);

                    for (i = 0; i < attrnum; i ++) {
                        if (buf->len < 6) {
                            return -EINVAL;
                        }

                        resp.meta_rsp.attr_id = net_buf_pull_be32(buf);
                        /* do not need CharacterSetID */
                        net_buf_pull_be16(buf);
                        resp.meta_rsp.attr_length = net_buf_pull_be16(buf);
                        resp.meta_rsp.attr_text = buf->data;
                        net_buf_pull_mem(buf, resp.meta_rsp.attr_length);
                        AVRCP_CT_EVENT_REPORT(BT_PRF_AVRCP_CT_METADATA_RSP_EVT, &resp);
                    }
                    break;
                /** not support for now */
                case BT_AVRCP_PDU_ID_REQUEST_CONTINUING_RESPONSE:
                case BT_AVRCP_PDU_ID_REQUEST_ABORT_CONTINUING_RESPONSE:
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
                    BT_ERR("Err: pdu id not supported");
                    return -ENOENT;
            }
            break;
        default:
            break;
    }

    /** need report to upper layer, if the event has set up */
    if (event >= 0) {
        AVRCP_CT_EVENT_REPORT(event, &resp);
    }

    return 0;
}

static void avrcp_report_connection(uint8_t connected, uint8_t *addr)
{
    bt_prf_avrcp_ct_cb_param_t resp;

    resp.conn_stat.connected = connected;
    memcpy(resp.conn_stat.peer_addr.val, addr, BT_STACK_BD_ADDR_LEN);

    AVRCP_CT_EVENT_REPORT(BT_PRF_AVRCP_CT_CONNECTION_STATE_EVT, &resp);
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

/** AVRCP controller event callback */
static const struct bt_avrcp_event_cb avrcp_event_ct_cb = {
    .ind  = &cb_ind,
    .data = avrcp_ct_data_handle,
};

/** AVRCP controller SDP attribute table */
static struct bt_sdp_attribute avrcp_ct_attrs[] = {
    BT_SDP_NEW_SERVICE,
    BT_SDP_LIST(BT_SDP_ATTR_SVCLASS_ID_LIST, BT_SDP_TYPE_SIZE_VAR(BT_SDP_SEQ8, 6),
                BT_SDP_DATA_ELEM_LIST(
                    { BT_SDP_TYPE_SIZE(BT_SDP_UUID16), BT_SDP_ARRAY_16(BT_SDP_AV_REMOTE_SVCLASS) },
                    { BT_SDP_TYPE_SIZE(BT_SDP_UUID16), BT_SDP_ARRAY_16(BT_SDP_AV_REMOTE_CONTROLLER_SVCLASS) }, ) ),
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
            }, ) ),
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
            }, ) ),
    BT_SDP_SUPPORTED_FEATURES(0x0001),
};

static struct bt_sdp_record avrcp_ct_rec = BT_SDP_RECORD(avrcp_ct_attrs);

/** AVRCP controller init function */
bt_stack_status_t bt_prf_avrcp_ct_init(void)
{
    int err;

    err = bt_avrcp_init();

    if (err) {
        BT_ERR("Err: %d", err);
        return err;
    }

    bt_avrcp_ct_register(&avrcp_event_ct_cb);

    err = bt_sdp_register_service(&avrcp_ct_rec);

    if (err) {
        BT_ERR("Err: %d", err);
    }

    return err;
}

/** AVRCP controller init function */
void bt_prf_avrcp_ct_register_callback(bt_prf_avrcp_ct_cb_t callback)
{
    avrcp_ct_cb = callback;
}

/** AVRCP controller send register notifucation command */
bt_stack_status_t bt_prf_avrcp_ct_cmd_register_notification(bt_prf_avrcp_rn_event_ids_t event_id)
{
    struct net_buf *                             buf;
    struct bt_avrcp_register_notification_packet rn;

    rn.event_id = event_id;

    /** fixed playback interval 2 second */
    sys_put_be32(CONFIG_AVRCP_PLAYBACK_SECOND, (uint8_t *)&rn.interval);

    buf = bt_avrcp_create_vendor_depenent_pdu(BT_AVRCP_PDU_ID_REGISTER_NOTIFICATION, &rn, sizeof(rn), avrcp_get_tl(),
                                              avrcp_get_ctype_from_pdu_id(BT_AVRCP_PDU_ID_REGISTER_NOTIFICATION));

    if (buf == NULL) {
        return -ENOMEM;
    }

    return bt_avrcp_send(buf);
}

/** AVRCP controller send get register notifucation capalitity command */
bt_stack_status_t bt_prf_avrcp_ct_cmd_get_rn_capabilities(void)
{
    struct net_buf *                           buf;
    struct bt_avrcp_get_capablitity_req_packet cap;

    cap.capablitity_id = BT_AVRCP_CAPABLITITY_EVENT_ID;

    buf = bt_avrcp_create_vendor_depenent_pdu(BT_AVRCP_PDU_ID_GET_CAPABILITIES, &cap, sizeof(cap), avrcp_get_tl(),
                                              avrcp_get_ctype_from_pdu_id(BT_AVRCP_PDU_ID_GET_CAPABILITIES));

    if (buf == NULL) {
        return -ENOMEM;
    }

    return bt_avrcp_send(buf);
}

/** AVRCP controller send pass through command */
bt_stack_status_t bt_prf_avrcp_ct_cmd_passthrough(bt_prf_avrcp_op_id_t   operation_id,
                                                  bt_prf_avrcp_op_stat_t operation_state)
{
    struct net_buf *buf;

    buf = bt_avrcp_create_pass_through_pdu(operation_id, operation_state, avrcp_get_tl(), BT_AVRCP_CTYPE_CONTROL);

    if (buf == NULL) {
        return -ENOMEM;
    }

    return bt_avrcp_send(buf);
}

/** AVRCP controller send get play status command */
bt_stack_status_t bt_prf_avrcp_ct_cmd_get_play_status(void)
{
    struct net_buf *buf;

    buf = bt_avrcp_create_vendor_depenent_pdu(BT_AVRCP_PDU_ID_GET_PLAY_STATUS, NULL, 0, avrcp_get_tl(),
                                              avrcp_get_ctype_from_pdu_id(BT_AVRCP_PDU_ID_GET_PLAY_STATUS));

    if (buf == NULL) {
        return -ENOMEM;
    }

    return bt_avrcp_send(buf);
}

/** AVRCP controller send set abslute volume command */
bt_stack_status_t bt_prf_avrcp_ct_cmd_set_absolute_volume(uint8_t volume)
{
    struct net_buf *                       buf;
    struct bt_avrcp_absolute_volume_packet vol;

    vol.abs_volume = volume & 0x7F;

    buf = bt_avrcp_create_vendor_depenent_pdu(BT_AVRCP_PDU_ID_SET_ABSOLUTE_VOLUME, &vol, sizeof(vol), avrcp_get_tl(),
                                              avrcp_get_ctype_from_pdu_id(BT_AVRCP_PDU_ID_SET_ABSOLUTE_VOLUME));

    if (buf == NULL) {
        return -ENOMEM;
    }

    return bt_avrcp_send(buf);
}

/** AVRCP controller send get metadata command */
bt_stack_status_t bt_prf_avrcp_ct_cmd_get_metadata(uint32_t attr_bit_map)
{
    int i;
    struct net_buf *                    buf;
    struct bt_avrcp_metadata_req_packet meda;

    meda.identifier = 0;
    meda.attrnum    = 0;

    for (i = 0; i < BT_AVRCP_MAX_METADETA_ATTR_SIZE; i ++) {
        if (attr_bit_map & (1 << i)) {
            meda.attr[meda.attrnum ++] = sys_cpu_to_be32(i + 1);
        }
    }

    buf = bt_avrcp_create_vendor_depenent_pdu(BT_AVRCP_PDU_ID_GET_ELEMENT_ATTRIBUTES, &meda, 9 + 4 * meda.attrnum, avrcp_get_tl(),
                                              avrcp_get_ctype_from_pdu_id(BT_AVRCP_PDU_ID_GET_ELEMENT_ATTRIBUTES));

    if (buf == NULL) {
        return -ENOMEM;
    }

    return bt_avrcp_send(buf);
}
