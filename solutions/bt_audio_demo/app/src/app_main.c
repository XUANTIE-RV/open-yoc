/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#include <aos/bt.h>
#include <ulog/ulog.h>
#include <uservice/eventid.h>
#include <uservice/uservice.h>
#include <yoc/partition.h>

#include "player/app_player.h"
#include "sys/app_sys.h"
#include "app_main.h"

#define TAG "APP"

#if defined (CONFIG_BT_A2DP) && CONFIG_BT_A2DP
static void avrcp_notify_handle(bt_prf_avrcp_rn_event_ids_t event_id, bt_prf_avrcp_rn_param_t *event_parameter)
{
    switch (event_id) {
        case BT_PRF_AVRCP_NOTIFICATION_PLAY_STATUS_CHANGE:
            LOGD(TAG, "PLAY_STATUS_CHANGE: %d", event_parameter->playback);
            break;
        case BT_PRF_AVRCP_NOTIFICATION_PLAY_POS_CHANGED:
            LOGD(TAG, "PLAY_POS_CHANGE: %d", event_parameter->play_pos);
            break;
        case BT_PRF_AVRCP_NOTIFICATION_BATTERY_STATUS_CHANGE:
            LOGD(TAG, "BATTERY_STATUS_CHANGE: %d", event_parameter->batt);
            break;
        case BT_PRF_AVRCP_NOTIFICATION_VOLUME_CHANGE:
            LOGD(TAG, "VOLUME_CHANGE: %d", event_parameter->volume);
            break;
        case BT_PRF_AVRCP_NOTIFICATION_TRACK_CHANGE:
            LOGD(TAG, "TRACK_CHANG");
            break;
        default:
            break;
    }
}

static void avrcp_ct_handle(bt_prf_avrcp_ct_cb_event_t event, bt_prf_avrcp_ct_cb_param_t *param)
{
    uint8_t *bt_addr;
    uint16_t bits;

    switch (event) {
        case BT_PRF_AVRCP_CT_CONNECTION_STATE_EVT:
            bt_addr = param->conn_stat.peer_addr.val;
            LOGD(TAG, "AVRCP_CT_CONNECTION_STATE_EVT state: %d, mac %02x:%02x:%02x:%02x:%02x:%02x", param->conn_stat.connected,
                 bt_addr[5], bt_addr[4], bt_addr[3], bt_addr[2], bt_addr[1], bt_addr[0]);
#if !(defined(CONFIG_APP_NO_AVRCP_TG) && CONFIG_APP_NO_AVRCP_TG)
            if (param->conn_stat.connected) {
                bt_prf_avrcp_ct_cmd_get_rn_capabilities();
            }
#endif
            break;
        case BT_PRF_AVRCP_CT_PASSTHROUGH_RSP_EVT:
            LOGD(TAG, "AVRCP_CT_PASSTHROUGH_RSP_EV operation_id: %d, operation_state %d", param->psth_rsp.operation_id,
                 param->psth_rsp.operation_state);

            break;
        case BT_PRF_AVRCP_CT_METADATA_RSP_EVT:
            break;
        case BT_PRF_AVRCP_CT_PLAY_STATUS_RSP_EVT:
            LOGD(TAG, "AVRCP_CT_PLAY_STATUS_RSP_EVT songlen: %d, song_pso: %d, play_status: %d",
                 param->get_rn_play_status_rsp.song_len, param->get_rn_play_status_rsp.song_pos,
                 param->get_rn_play_status_rsp.play_status);
            break;
        case BT_PRF_AVRCP_CT_CHANGE_NOTIFY_EVT:
            LOGD(TAG, "AVRCP_CT_CHANGE_NOTIFY_EVT event_id: %d", param->change_ntf.event_id);
            avrcp_notify_handle(param->change_ntf.event_id, &param->change_ntf.event_parameter);
            bt_prf_avrcp_ct_cmd_register_notification(param->change_ntf.event_id);
            break;
        case BT_PRF_AVRCP_CT_SET_ABSOLUTE_VOLUME_RSP_EVT:
            LOGD(TAG, "AVRCPCT_SET_ABSOLUTE_VOLUME_RSP_EVT volume: %d", param->set_volume_rsp.volume);
            break;
        case BT_PRF_AVRCP_CT_REMOTE_FEATURES_EVT:
            break;
        case BT_PRF_AVRCP_CT_GET_RN_CAPABILITIES_RSP_EVT:
            bits = param->get_rn_caps_rsp.evt_set.bits;
            LOGD(TAG, "AVRCP_CT_GET_RN_CAPABILITIES_RSP_EVT evt_bitmap: %08x", bits);
            if (bits & (1 << BT_PRF_AVRCP_NOTIFICATION_PLAY_STATUS_CHANGE)) {
                bt_prf_avrcp_ct_cmd_register_notification(BT_PRF_AVRCP_NOTIFICATION_PLAY_STATUS_CHANGE);
            }
            break;
        default:
            break;
    }
}

#if !(defined(CONFIG_APP_NO_AVRCP_TG) && CONFIG_APP_NO_AVRCP_TG)
static void avrcp_tg_handle(bt_prf_avrcp_tg_cb_event_t event, bt_prf_avrcp_tg_cb_param_t *param)
{
    uint8_t *bt_addr;

    switch (event) {
        case BT_PRF_AVRCP_TG_CONNECTION_STATE_EVT:
            bt_addr = param->conn_stat.peer_addr.val;
            LOGD(TAG, "AVRCP_TG_CONNECTION_STATE_EVT state: %d, mac %02x:%02x:%02x:%02x:%02x:%02x", param->conn_stat.connected,
                 bt_addr[5], bt_addr[4], bt_addr[3], bt_addr[2], bt_addr[1], bt_addr[0]);
            break;
        case BT_PRF_AVRCP_TG_PASSTHROUGH_CMD_EVT:
            LOGD(TAG, "AVRCP_TG_PASSTHROUGH_RSP_EV operation_id: %d, operation_state %d", param->psth_cmd.operation_id,
                 param->psth_cmd.operation_state);

            break;
        case BT_PRF_AVRCP_TG_REGISTER_NOTIFICATION_EVT:
            LOGD(TAG, "AVRCP_TG_CHANGE_NOTIFY_EVT event_id: %d", param->reg_ntf.event_id);
            bt_prf_avrcp_rn_param_t p;
            p.volume = 0x7F;
            bt_prf_avrcp_tg_send_rn_rsp(param->reg_ntf.event_id, BT_AVRCP_RESPONSE_INTERIM, &p);
            break;
        case BT_PRF_AVRCP_TG_SET_ABSOLUTE_VOLUME_CMD_EVT:
            LOGD(TAG, "AVRCP_TG_SET_ABSOLUTE_VOLUME_RSP_EVT volume: %d", param->set_abs_vol.volume);
            break;
        case BT_PRF_AVRCP_TG_REMOTE_FEATURES_EVT:
            break;
        default:
            break;
    }
}
#endif

static void a2dp_handle(bt_prf_a2dp_cb_event_t event, bt_prf_a2dp_cb_param_t *param)
{
    uint8_t *bt_addr;
    uint8_t *conf;

    switch (event) {
        case BT_PRF_A2DP_CONNECTION_STATE_EVT:
            bt_addr = param->conn_stat.peer_addr.val;
            LOGD(TAG, "A2DP_CONNECTION_STATE_EVT %d, mac %02x:%02x:%02x:%02x:%02x:%02x", param->conn_stat.state, bt_addr[5],
                 bt_addr[4], bt_addr[3], bt_addr[2], bt_addr[1], bt_addr[0]);

            if (param->conn_stat.state == BT_PRF_A2DP_CONNECTION_STATE_DISCONNECTED_ABNORMAL) {
                bt_prf_a2dp_sink_connect(&param->conn_stat.peer_addr);
            } else if (param->conn_stat.state == BT_PRF_A2DP_CONNECTION_STATE_DISCONNECTED_NORMAL) {
                bt_stack_set_connectable(1);
            }
            break;
        case BT_PRF_A2DP_AUDIO_STATE_EVT:
            bt_addr = param->conn_stat.peer_addr.val;
            LOGD(TAG, "A2DP_AUDIO_STATE_EVT %d, mac %02x:%02x:%02x:%02x:%02x:%02x", param->audio_stat.state, bt_addr[5],
                 bt_addr[4], bt_addr[3], bt_addr[2], bt_addr[1], bt_addr[0]);
            break;
        case BT_PRF_A2DP_AUDIO_SBC_CFG_EVT:
            bt_addr = param->audio_cfg.peer_addr.val;
            conf    = param->audio_cfg.sbc.config;
            LOGD(TAG, "A2DP_AUDIO_SBC_CFG_EVT sbc config %02x%02x, mac %02x:%02x:%02x:%02x:%02x:%02x", conf[0], conf[1],
                 bt_addr[5], bt_addr[4], bt_addr[3], bt_addr[2], bt_addr[1], bt_addr[0]);
            break;
    }
}
#endif

#if defined (CONFIG_BT_HFP_HF) && CONFIG_BT_HFP_HF
static void hfp_handle(bt_prf_hfp_hf_cb_event_t event, bt_prf_hfp_hf_cb_param_t *param)
{
    uint8_t *bt_addr;

    switch (event) {
        case BT_PRF_HFP_HF_CONNECTION_STATE_EVT:
            bt_addr = param->conn_stat.peer_addr.val;
            LOGD(TAG, "HF_CONNECTION_STATE_EVT %d, peer_feat: %x, chld_feat %x, mac %02x:%02x:%02x:%02x:%02x:%02x",
                 param->conn_stat.state, param->conn_stat.peer_feat, param->conn_stat.chld_feat, bt_addr[5], bt_addr[4],
                 bt_addr[3], bt_addr[2], bt_addr[1], bt_addr[0]);
            break;
        case BT_PRF_HFP_HF_AUDIO_STATE_EVT:
            bt_addr = param->audio_stat.peer_addr.val;
            LOGD(TAG, "HF_AUDIO_STATE_EVT %d, mac %02x:%02x:%02x:%02x:%02x:%02x",
                 param->audio_stat.state, bt_addr[5], bt_addr[4],
                 bt_addr[3], bt_addr[2], bt_addr[1], bt_addr[0]);
            break;
        case BT_PRF_HFP_HF_CIND_CALL_EVT:
            LOGD(TAG, "HF_CIND_CALL_EVT %d", param->call.status);
            break;
        case BT_PRF_HFP_HF_CIND_CALL_SETUP_EVT:
            LOGD(TAG, "HF_CIND_CALL_SETUP_EVT %d", param->call_setup.status);
            break;
        case BT_PRF_HFP_HF_CIND_CALL_HELD_EVT:
            LOGD(TAG, "HF_CIND_CALL_HELD_EVT %d", param->call_held.status);
            break;
        case BT_PRF_HFP_HF_CIND_SERVICE_AVAILABILITY_EVT:
            LOGD(TAG, "HF_CIND_SERVICE_AVAILABILITY_EVT %d", param->service_availability.status);
            break;
        case BT_PRF_HFP_HF_CLIP_EVT:
            LOGD(TAG, "HF_CLIP_EVT %s", param->clip.number);
            break;
        case BT_PRF_HFP_HF_CCWA_EVT:
            LOGD(TAG, "HF_CCWA_EVT %s", param->ccwa.number);
            break;
        case BT_PRF_HFP_HF_CLCC_EVT:
            LOGD(TAG, "HF_CLCC_EVT %s", param->clcc.number);
            break;
        case BT_PRF_HFP_HF_BSIR_EVT:
            LOGD(TAG, "HF_BSIR_EVT %d", param->bsir.state);
            break;
        case BT_PRF_HFP_HF_CIND_BATTERY_LEVEL_EVT:
            LOGD(TAG, "HF_CIND_BATTERY_LEVEL_EVT %d", param->battery_level.value);
            break;
        case BT_PRF_HFP_HF_CIND_ROAMING_STATUS_EVT:
            LOGD(TAG, "HF_CIND_ROAMING_STATUS_EVT %d", param->roaming.status);
            break;
        case BT_PRF_HFP_HF_CIND_SIGNAL_STRENGTH_EVT:
            LOGD(TAG, "HF_CIND_SIGNAL_STRENGTH_EVT %d", param->signal_strength.value);
            break;
        case BT_PRF_HFP_HF_RING_IND_EVT:
            LOGD(TAG, "HF_RING_IND_EVT");
        case BT_PRF_HFP_HF_VOLUME_CONTROL_EVT:
        case BT_PRF_HFP_HF_AT_RESPONSE_EVT:
        case BT_PRF_HFP_HF_CNUM_EVT:
        case BT_PRF_HFP_HF_BINP_EVT:
        case BT_PRF_HFP_HF_COPS_CURRENT_OPERATOR_EVT:
        case BT_PRF_HFP_HF_BTRH_EVT:
        case BT_PRF_HFP_HF_BVRA_EVT:
            break;
    }
}
#endif

int main(int argc, char *argv[])
{
    board_yoc_init();
    LOGI(TAG, "bt_audio_demo");

    app_sys_init();

#if defined(CONFIG_BOARD_AUDIO) && CONFIG_BOARD_AUDIO > 0
    app_speaker_init();

    app_player_init();
#endif

    app_cli_init();

    /** bt stack init */
    bt_stack_init();

    ble_stack_setting_load();

#if defined (CONFIG_BT_A2DP) && CONFIG_BT_A2DP
    /** a2dp sink init */
    bt_prf_a2dp_sink_init();
    bt_prf_a2dp_register_callback(a2dp_handle);

    /** avrcp controller init */
    bt_prf_avrcp_ct_init();
    bt_prf_avrcp_ct_register_callback(avrcp_ct_handle);

#if !(defined(CONFIG_APP_NO_AVRCP_TG) && CONFIG_APP_NO_AVRCP_TG)
    /** avrcp target init */
    bt_prf_avrcp_tg_init();
    bt_prf_avrcp_tg_register_callback(avrcp_tg_handle);

    /** avrcp target set local register notification capability */
    bt_prf_avrcp_tg_set_rn_evt_cap((1 << BT_PRF_AVRCP_NOTIFICATION_VOLUME_CHANGE));
#endif

#endif

#if defined (CONFIG_BT_HFP_HF) && CONFIG_BT_HFP_HF
    /** hfp hf init */
    bt_prf_hfp_hf_init();
    bt_prf_hfp_hf_register_callback(hfp_handle);
#endif

    bt_stack_set_name("BT Audio Demo");

    /** set device to connectable discoverable mode */
    bt_stack_set_connectable(1);
    bt_stack_set_discoverable(1);

    return 0;
}
