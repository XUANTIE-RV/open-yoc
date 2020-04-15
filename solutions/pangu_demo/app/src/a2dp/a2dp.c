/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <aos/aos.h>
#include <yoc/uservice.h>
#include <yoc/eventid.h>

#include "yoc_gap_bt_api.h"
#include "yoc_gatts_api.h"
#include "yoc_bt_defs.h"
#include "yoc_gatt_common_api.h"

#include "bt_app_core.h"
#include "bt_app_av.h"

#include "yoc_bt_main.h"
#include "yoc_bt_device.h"
#include "yoc_gap_bt_api.h"
#include "yoc_a2dp_api.h"
#include "yoc_avrc_api.h"

#include <player.h>
#include <pin_name.h>

#include <devices/devicelist.h>
#include <devices/rtl8723ds.h>
#include <alsa/mixer.h>
#include <avutil/named_fifo.h>

#include <aos/cli.h>

#include <pin.h>

#include <avutil/named_straightfifo.h>
#define TAG "A2DP_DEMO"

static player_t *g_player;
static nsfifo_t *fifo = NULL;
static char url[128];
#define URL_PATTEN "fifo://tts/1?avformat=rawaudio&avcodec=pcm_s%dle&channel=%d&rate=%d"

static void _player_event(player_t *player, uint8_t type, const void *data, uint32_t len)
{
    UNUSED(len);
    UNUSED(data);
    UNUSED(handle);
    LOGI(TAG, "=====%s, %d, type = %d", __FUNCTION__, __LINE__, type);

    switch (type) {
    case PLAYER_EVENT_ERROR:
        player_stop(player);
        break;

    case PLAYER_EVENT_START:
        break;

    case PLAYER_EVENT_FINISH:
        player_stop(player);
        break;

    default:
        break;
    }
}

/* event for handler "bt_av_hdl_stack_up */
enum {
    BT_APP_EVT_STACK_UP = 0,
};

void bt_app_gap_cb(yoc_bt_gap_cb_event_t event, yoc_bt_gap_cb_param_t *param)
{
    switch (event) {
    case YOC_BT_GAP_AUTH_CMPL_EVT: {
        if (param->auth_cmpl.stat == YOC_BT_STATUS_SUCCESS) {
            LOGI(TAG, "authentication success: %s", param->auth_cmpl.device_name);
            //LOG_buffer_hex(BT_AV_TAG, param->auth_cmpl.bda, yoc_BD_ADDR_LEN);
        } else {
            LOGE(TAG, "authentication failed, status:%d", param->auth_cmpl.stat);
        }
        break;
    }
#if 0
    case yoc_BT_GAP_CFM_REQ_EVT:
        LOGI(BT_AV_TAG, "yoc_BT_GAP_CFM_REQ_EVT Please compare the numeric value: %d", param->cfm_req.num_val);
        yoc_bt_gap_ssp_confirm_reply(param->cfm_req.bda, true);
        break;
    case yoc_BT_GAP_KEY_NOTIF_EVT:
        LOGI(BT_AV_TAG, "yoc_BT_GAP_KEY_NOTIF_EVT passkey:%d", param->key_notif.passkey);
        break;
    case yoc_BT_GAP_KEY_REQ_EVT:
        LOGI(BT_AV_TAG, "yoc_BT_GAP_KEY_REQ_EVT Please enter passkey!");
        break;
#endif
    default: {
        LOGI(BT_AV_TAG, "event: %d", event);
        break;
    }
    }
    return;
}

static void bt_av_hdl_stack_evt(uint16_t event, void *p_param)
{
    LOGD(BT_AV_TAG, "%s evt %d", __func__, event);
    switch (event) {
    case BT_APP_EVT_STACK_UP: {
        /* set up device name */
        char *dev_name = "YOC_SPEAKER";
        yoc_bt_dev_set_device_name(dev_name);

        yoc_bt_gap_register_callback(bt_app_gap_cb);
        /* initialize A2DP sink */
        yoc_a2d_register_callback(&bt_app_a2d_cb);
        yoc_a2d_sink_register_data_callback(bt_app_a2d_data_cb);
        yoc_a2d_sink_init();

        /* initialize AVRCP controller */
        yoc_avrc_ct_init();
        yoc_avrc_ct_register_callback(bt_app_rc_ct_cb);

        /* set discoverable and connectable mode, wait to be connected */
        yoc_bt_gap_set_scan_mode(YOC_BT_CONNECTABLE, YOC_BT_GENERAL_DISCOVERABLE);
        break;
    }
    default:
        LOGE(TAG, "%s unhandled evt %d", __func__, event);
        break;
    }
}

void audio_dev_set_rate(uint32_t rate, uint32_t bitwith, uint32_t channel)
{
    snprintf(url, sizeof(url), URL_PATTEN, bitwith, channel, rate);
}

void audio_player(uint8_t action)
{
    int ret;
    static int player_status = 0;

    LOGI(BT_AV_TAG, "%s , %d", __func__, action);

    switch (action){
        case 2:
        if (g_player) {
            if (player_status == 1) {
                ret = player_pause(g_player);
                player_status = 2;
            }
        }
        break;
        case 1:
        if (g_player) {
            if(player_status == 0) {
                fifo = nsfifo_open(url, O_CREAT, 102400);
                aos_check(fifo, ERR_MEM);
                ret = player_play(g_player, url);
                aos_check(!ret, ERR_MEM);
                player_status = 1;
            } else if (player_status == 2) {
                ret = player_resume(g_player);
                player_status = 1;
            }
        }
        break;
        case 0:
        if (g_player) {
            if (player_status) {
                player_stop(g_player);
                nsfifo_close(fifo);
                fifo = NULL;
                player_status = 0;
            }
        }
        break;
        default:
        break;
    }
}

void audio_dev_proc(const uint8_t *data, uint32_t len)
{
    int available_len = 0;
    int remaind_len = len;
    char *pos = NULL;

    if (fifo == NULL) {
        return;
    }

again:
    available_len = nsfifo_get_wpos(fifo, &pos, 200);

    if (available_len < 0) {
        LOGE(TAG, "nsfifo_get_wpos error");
        return;
    }

    if (available_len < remaind_len) {
        /* If available space is too small, wait player eat the buffer */
        memcpy(pos, data, available_len);
        nsfifo_set_wpos(fifo, available_len);
        remaind_len -= available_len;
        data += remaind_len;
        // aos_msleep(10);
        LOGE(TAG, "audio_dev_proc no room %d %d", available_len, len);
        goto again;
    }

    memcpy(pos, data, remaind_len);
    nsfifo_set_wpos(fifo, remaind_len);
}

void a2dp_start(char *wbuf, int wbuf_len, int argc, char **argv)
{
    bt_err_t ret;

    plyh_t plyh;

    memset(&plyh, 0, sizeof(plyh_t));
    plyh.ao_name       = "alsa";
    // plyh.eq_segments   = EQ_SEGMENT;
    //plyh.resample_rate = 48000;
    plyh.rcv_timeout   =  -1;
    // plyh.get_dec_cb    = _get_decrypt;
    plyh.event_cb      = _player_event;

    g_player = player_new(&plyh);

        // stream_register_mem();
        // stream_register_file();
        // stream_register_http();
        // stream_register_fifo();

        // demux_register_wav();
        // demux_register_mp3();
        // demux_register_mp4();
        // demux_register_adts();
        // demux_register_rawaudio();
        // ad_register_pcm();


    uart_csky_register(0);

    rtl8723ds_bt_config config = {
        .uart_id = BT_UART_IDX,
        .bt_dis_pin = BT_DIS_PIN,
    };

    bt_rtl8723ds_register(&config);

    ret = yoc_bluedroid_init();

    if (ret) {
        LOGE(TAG, "%s init bluetooth failed: %d\n", __func__, ret);
        return;
    }

    ret = yoc_bluedroid_enable();

    if (ret) {
        LOGE(TAG, "%s enable bluetooth failed: %d\n", __func__, ret);
        return;
    }

    /* create application task */
    bt_app_task_start_up();

    /* Bluetooth device name, connection mode and profile set up */

    bt_app_work_dispatch(bt_av_hdl_stack_evt, BT_APP_EVT_STACK_UP, NULL, 0, NULL);

    yoc_bt_sp_param_t param_type = YOC_BT_SP_IOCAP_MODE;
    yoc_bt_io_cap_t   iocap    = YOC_BT_IO_CAP_NONE;
    yoc_bt_gap_set_security_param(param_type, &iocap, sizeof(uint8_t));
}

void cli_reg_cmd_a2dp(void)
{
    static const struct cli_command cmd_info = 
    {
        "a2dp",
        "as2p sink",
        a2dp_start
    };

    aos_cli_register_command(&cmd_info);
}
