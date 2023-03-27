/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */

#include <ulog/ulog.h>

#include <uservice/eventid.h>
#include <uservice/uservice.h>
#include <yoc/mic.h>
#include <aos/kernel.h>
#include <ulog/ulog.h>

#include "player/app_player.h"
#include "display/app_disp.h"
#include "bt/app_bt.h"
#include "app_event.h"

#define TAG "STATUS"

status_event_t g_status_event_list[] = {
    /*系统*/
    { EVENT_STATUS_STARTING, "starting" },
    { EVENT_KEY_PRESSED, "keypress" },
    { EVENT_PLAYER_CHANGE, "player status change"},

    /*配网*/
    { EVENT_STATUS_WIFI_PROV_START, "wifiprov" },
    { EVENT_STATUS_WIFI_PROV_ALREADY_START, "wifiprov runing" },
    { EVENT_STATUS_WIFI_PROV_TIMEOUT, "wifiprov timeout" },
    { EVENT_STATUS_WIFI_PROV_FAILED, "wifiprov failed" },
    { EVENT_STATUS_WIFI_PROV_RECVED, "wifiprov recved" },

    /*网络连接*/
    { EVENT_STATUS_WIFI_CONN_SUCCESS, "wifi conn" },
    { EVENT_STATUS_WIFI_CONN_FAILED, "wifi failed" },
    { EVENT_STATUS_NTP_SUCCESS, "ntp succ" },

    /*蓝牙*/
    { EVENT_STATUS_BT_CONNECTED, "bt conn" },
    { EVENT_STATUS_BT_DISCONNECTED, "bt disconn" },

    /*唤醒*/
    { EVENT_STATUS_SESSION_START, "session start" },
    { EVENT_STATUS_SESSION_STOP, "session stop" },

    /*交互*/
    { EVENT_STATUS_NLP_NOTHING, "nlp nothing" },
    { EVENT_STATUS_NLP_UNKNOWN, "nlp unknown" },

    /* 播放器事件 */
    { EVENT_MEDIA_START, "audio start" },
    { EVENT_MEDIA_SYSTEM_START, "tone start"},
    { EVENT_MEDIA_MUSIC_FINISH, "audio finish" },
    { EVENT_MEDIA_SYSTEM_FINISH, "tone finish" },
    { EVENT_MEDIA_MUSIC_ERROR, "audio error" },
    { EVENT_MEDIA_SYSTEM_ERROR, "tone error" },

    /* 调试 */
    { EVENT_DEBUG_EVENT_BLOCK, "block 60 second" },
};

int g_status_event_list_count = sizeof(g_status_event_list) / sizeof(status_event_t);

static long long g_notify_play_status = 0;

static void app_event_mgr(uint32_t event_id, const void *data, void *context)
{
    LOGI(TAG, "Event ID = %x", event_id);

    switch (event_id) {
            /*系统*/
        case EVENT_STATUS_STARTING:
            local_audio_play("sys_starting.mp3");
            break;
        case EVENT_KEY_PRESSED:
            app_display_show(DISP_SHOW_KEYPRESS);
            break;
        case EVENT_PLAYER_CHANGE:
            break;

            /*配网*/
        case EVENT_STATUS_WIFI_PROV_START:
            local_audio_play("wifiprov_start.mp3");
            break;
        case EVENT_STATUS_WIFI_PROV_ALREADY_START:
            local_audio_play("wifiprov_already_start.mp3");
            break;
        case EVENT_STATUS_WIFI_PROV_TIMEOUT:
            local_audio_play("wifiprov_timeout.mp3");
            break;
        case EVENT_STATUS_WIFI_PROV_FAILED:
            local_audio_play("wifiprov_fail.mp3");
            break;
        case EVENT_STATUS_WIFI_PROV_RECVED:
            local_audio_play("wifiprov_recved.mp3");
            break;

            /*网络连接*/
        case EVENT_STATUS_WIFI_CONN_SUCCESS:
            break;
        case EVENT_STATUS_WIFI_CONN_FAILED:
            local_audio_play("wifi_conn_fail.mp3");
            break;
        case EVENT_STATUS_NTP_SUCCESS:
            local_audio_play("wifi_conn_succ.mp3");
            break;

            /*蓝牙*/
        case EVENT_STATUS_BT_CONNECTED:
            local_audio_play("bt_connected.mp3");
            // ad2p断开是否启动广播
            //app_bt_adv_enable(1);
            break;
        case EVENT_STATUS_BT_DISCONNECTED:
            local_audio_play("bt_disconnected.mp3");
            break;

            /*唤醒*/
        case EVENT_STATUS_SESSION_START:
            g_notify_play_status = aos_now_ms();
            aui_mic_control(MIC_CTRL_NOTIFY_PLAYER_STATUS, 1, 200);
            local_wakeup_audio_play("wakeup.wav");
            break;
        case EVENT_STATUS_SESSION_STOP:
            break;

            /*交互*/
        case EVENT_STATUS_NLP_NOTHING:
            local_audio_play("npl_nothing.mp3");
            break;
        case EVENT_STATUS_NLP_UNKNOWN:
            local_audio_play("npl_unknown.mp3");
            break;

            /*播放器*/
        case EVENT_MEDIA_START:
            break;
        case EVENT_MEDIA_SYSTEM_START:
            break;
        case EVENT_MEDIA_MUSIC_FINISH:
            event_publish_delay(EVENT_PLAYER_CHANGE, NULL, 500);
            break;
        case EVENT_MEDIA_SYSTEM_FINISH:
            if (g_notify_play_status) {
                LOGD(TAG, "play time %d ms", (int)(aos_now_ms() - g_notify_play_status));
                aui_mic_control(MIC_CTRL_NOTIFY_PLAYER_STATUS, 0, 200);
                g_notify_play_status = 0;
            }
            event_publish_delay(EVENT_PLAYER_CHANGE, NULL, 500);
            break;
        case EVENT_MEDIA_MUSIC_ERROR:
            break;
        case EVENT_MEDIA_SYSTEM_ERROR:
            break;
        case EVENT_DEBUG_EVENT_BLOCK:
            LOGI(TAG, "block 60 second");
            aos_msleep(60000);
            LOGI(TAG, "block finished");
            break;
        default:
            break;
    }

    /* 显示状态更新 */
    app_display_update();
}

void app_event_update(int eventid)
{
    event_publish(eventid, NULL);
}

void app_event_init()
{
    for (int i = 0; i < g_status_event_list_count; i++) {
        event_subscribe(g_status_event_list[i].eventid, app_event_mgr, NULL);
    }
}
