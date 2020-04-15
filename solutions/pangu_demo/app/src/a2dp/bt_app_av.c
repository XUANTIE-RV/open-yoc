
/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "bt_app_core.h"
#include "bt_app_av.h"
#include "yoc_bt_main.h"
#include "yoc_bt_device.h"
#include "yoc_gap_bt_api.h"
#include "yoc_a2dp_api.h"
#include "yoc_avrc_api.h"

#include <aos/log.h>


/* a2dp event handler */
static void bt_av_hdl_a2d_evt(uint16_t event, void *p_param);
/* avrc event handler */
static void bt_av_hdl_avrc_evt(uint16_t event, void *p_param);

static uint32_t s_pkt_cnt = 0;
static yoc_a2d_audio_state_t s_audio_state = YOC_A2D_AUDIO_STATE_STOPPED;
static const char *s_a2d_conn_state_str[] = {"Disconnected", "Connecting", "Connected", "Disconnecting"};
static const char *s_a2d_audio_state_str[] = {"Suspended", "Stopped", "Started"};


/* callback for A2DP sink */
void bt_app_a2d_cb(yoc_a2d_cb_event_t event, yoc_a2d_cb_param_t *param)
{
    switch (event) {
    case YOC_A2D_CONNECTION_STATE_EVT:
    case YOC_A2D_AUDIO_STATE_EVT:
    case YOC_A2D_AUDIO_CFG_EVT: {
        bt_app_work_dispatch(bt_av_hdl_a2d_evt, event, param, sizeof(yoc_a2d_cb_param_t), NULL);
        break;
    }
    default:
        LOGE(BT_AV_TAG, "Invalid A2DP event: %d", event);
        break;
    }
}

void bt_app_a2d_data_cb(const uint8_t *data, uint32_t len)
{

    audio_dev_proc(data, len);
    if (++s_pkt_cnt % 100 == 0) {
        LOGI(BT_AV_TAG, "Audio packet count %u", s_pkt_cnt);
    }
}

void bt_app_alloc_meta_buffer(yoc_avrc_ct_cb_param_t *param)
{
    yoc_avrc_ct_cb_param_t *rc = (yoc_avrc_ct_cb_param_t *)(param);
    uint8_t *attr_text = (uint8_t *) malloc (rc->meta_rsp.attr_length + 1);
    memcpy(attr_text, rc->meta_rsp.attr_text, rc->meta_rsp.attr_length);
    attr_text[rc->meta_rsp.attr_length] = 0;

    rc->meta_rsp.attr_text = attr_text;
}

void bt_app_rc_ct_cb(yoc_avrc_ct_cb_event_t event, yoc_avrc_ct_cb_param_t *param)
{
    switch (event) {
    case YOC_AVRC_CT_METADATA_RSP_EVT:
        bt_app_alloc_meta_buffer(param);
        /* fall through */
    case YOC_AVRC_CT_CONNECTION_STATE_EVT:
    case YOC_AVRC_CT_PASSTHROUGH_RSP_EVT:
    case YOC_AVRC_CT_CHANGE_NOTIFY_EVT:
    case YOC_AVRC_CT_REMOTE_FEATURES_EVT: {
        bt_app_work_dispatch(bt_av_hdl_avrc_evt, event, param, sizeof(yoc_avrc_ct_cb_param_t), NULL);
        break;
    }
    default:
        LOGE(BT_AV_TAG, "Invalid AVRC event: %d", event);
        break;
    }
}

static void  bt_av_hdl_a2d_evt(uint16_t event, void *p_param)
{
    LOGD(BT_AV_TAG, "%s evt %d", __func__, event);
    yoc_a2d_cb_param_t *a2d = NULL;
    switch (event) {
    case YOC_A2D_CONNECTION_STATE_EVT: {
        a2d = (yoc_a2d_cb_param_t *)(p_param);
        uint8_t *bda = a2d->conn_stat.remote_bda;
        LOGI(BT_AV_TAG, "A2DP connection state: %s, [%02x:%02x:%02x:%02x:%02x:%02x]",
             s_a2d_conn_state_str[a2d->conn_stat.state], bda[0], bda[1], bda[2], bda[3], bda[4], bda[5]);
        if (a2d->conn_stat.state == YOC_A2D_CONNECTION_STATE_DISCONNECTED) {
            yoc_bt_gap_set_scan_mode(YOC_BT_CONNECTABLE, YOC_BT_GENERAL_DISCOVERABLE);

        } else if (a2d->conn_stat.state == YOC_A2D_CONNECTION_STATE_CONNECTED){
            yoc_bt_gap_set_scan_mode(YOC_BT_NON_CONNECTABLE, YOC_BT_NON_DISCOVERABLE);

        }
        break;
    }
    case YOC_A2D_AUDIO_STATE_EVT: {
        a2d = (yoc_a2d_cb_param_t *)(p_param);
        LOGI(BT_AV_TAG, "A2DP audio state: %s", s_a2d_audio_state_str[a2d->audio_stat.state]);
        s_audio_state = a2d->audio_stat.state;
        if (YOC_A2D_AUDIO_STATE_STARTED == a2d->audio_stat.state) {

            audio_player(1);
            s_pkt_cnt = 0;
        } else if (a2d->audio_stat.state == YOC_A2D_AUDIO_STATE_STOPPED ||
                   a2d->audio_stat.state ==  YOC_A2D_AUDIO_STATE_REMOTE_SUSPEND) {
            audio_player(0);
        }
        break;
    }
    case YOC_A2D_AUDIO_CFG_EVT: {
        a2d = (yoc_a2d_cb_param_t *)(p_param);
        LOGI(BT_AV_TAG, "A2DP audio stream configuration, codec type %d", a2d->audio_cfg.mcc.type);
        // for now only SBC stream is supported
        if (a2d->audio_cfg.mcc.type == YOC_A2D_MCT_SBC) {
            int sample_rate = 16000;
            char oct0 = a2d->audio_cfg.mcc.cie.sbc[0];
            if (oct0 & (0x01 << 6)) {
                sample_rate = 32000;
            } else if (oct0 & (0x01 << 5)) {
                sample_rate = 44100;
            } else if (oct0 & (0x01 << 4)) {
                sample_rate = 48000;
            }
          //  i2s_set_clk(0, sample_rate, 16, 2);
          audio_dev_set_rate(sample_rate, 16, 2);

            LOGI(BT_AV_TAG, "Configure audio player %x-%x-%x-%x",
                     a2d->audio_cfg.mcc.cie.sbc[0],
                     a2d->audio_cfg.mcc.cie.sbc[1],
                     a2d->audio_cfg.mcc.cie.sbc[2],
                     a2d->audio_cfg.mcc.cie.sbc[3]);
            LOGI(BT_AV_TAG, "Audio player configured, sample rate=%d", sample_rate);
        }
        break;
    }
    default:
        LOGE(BT_AV_TAG, "%s unhandled evt %d", __func__, event);
        break;
    }
}

static void bt_av_new_track()
{
    //Register notifications and request metadata
    yoc_avrc_ct_send_metadata_cmd(0, YOC_AVRC_MD_ATTR_TITLE | YOC_AVRC_MD_ATTR_ARTIST | YOC_AVRC_MD_ATTR_ALBUM | YOC_AVRC_MD_ATTR_GENRE);
    yoc_avrc_ct_send_register_notification_cmd(1, YOC_AVRC_RN_TRACK_CHANGE, 0);
}

void bt_av_notify_evt_handler(uint8_t event_id, yoc_avrc_rn_param_t *event_parameter)
{
    switch (event_id) {
    case YOC_AVRC_RN_TRACK_CHANGE:
        bt_av_new_track();
        break;
    }
}

static void bt_av_hdl_avrc_evt(uint16_t event, void *p_param)
{
    LOGD(BT_AV_TAG, "%s evt %d", __func__, event);
    yoc_avrc_ct_cb_param_t *rc = (yoc_avrc_ct_cb_param_t *)(p_param);
    switch (event) {
    case YOC_AVRC_CT_CONNECTION_STATE_EVT: {
        uint8_t *bda = rc->conn_stat.remote_bda;
        LOGI(BT_AV_TAG, "AVRC conn_state evt: state %d, [%02x:%02x:%02x:%02x:%02x:%02x]",
                 rc->conn_stat.connected, bda[0], bda[1], bda[2], bda[3], bda[4], bda[5]);

        if (rc->conn_stat.connected) {
            bt_av_new_track();
        }
        break;
    }
    case YOC_AVRC_CT_PASSTHROUGH_RSP_EVT: {
        LOGI(BT_AV_TAG, "AVRC passthrough rsp: key_code 0x%x, key_state %d", rc->psth_rsp.key_code, rc->psth_rsp.key_state);
        break;
    }
    case YOC_AVRC_CT_METADATA_RSP_EVT: {
        LOGI(BT_AV_TAG, "AVRC metadata rsp: attribute id 0x%x, %s", rc->meta_rsp.attr_id, rc->meta_rsp.attr_text);
        free(rc->meta_rsp.attr_text);
        break;
    }
    case YOC_AVRC_CT_CHANGE_NOTIFY_EVT: {
        LOGI(BT_AV_TAG, "AVRC event notification: %d, param: %d", rc->change_ntf.event_id, rc->change_ntf.event_parameter);
        bt_av_notify_evt_handler(rc->change_ntf.event_id, &rc->change_ntf.event_parameter);
        break;
    }
    case YOC_AVRC_CT_REMOTE_FEATURES_EVT: {
        LOGI(BT_AV_TAG, "AVRC remote features %x", rc->rmt_feats.feat_mask);
        break;
    }
    default:
        LOGE(BT_AV_TAG, "%s unhandled evt %d", __func__, event);
        break;
    }
}
