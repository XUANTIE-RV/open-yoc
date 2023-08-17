/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */

#include <ulog/ulog.h>

#include <uservice/eventid.h>
#include <uservice/uservice.h>
#include <yoc/mic.h>
#include <aos/kernel.h>
#include <ulog/ulog.h>
#include <devices/wifi.h>

#include "player/app_player.h"
#include "display/app_disp.h"
#include "bt/app_bt.h"
#include "event_mgr/app_event.h"
#include "wifi/app_wifi.h"
#include "ui/ui_entry.h"

#define TAG "UI"

//extern wifi_ap_record_t g_app_wifi_user_scan_table[];

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

    /* GUI用户事件*/
    { EVENT_GUI_USER_WIFI_SCAN_START, "gui user wifi scan start"},
    { EVENT_GUI_USER_WIFI_PROV_START, "gui user wifi provision start"},
    { EVENT_GUI_USER_MESH_SCAN_START, "gui user mesh scan start"},
    { EVENT_GUI_USER_MESH_PROV_START, "gui user mesh provision start"},
    { EVENT_GUI_USER_MESH_UNPROV_START, "gui user mesh un-provision start"},
    { EVENT_GUI_USER_WIFI_SCAN_FINISH, "gui user wifi scan finish"},
    { EVENT_GUI_USER_WIFI_PROV_FINISH, "gui user wifi provision finish"},
    { EVENT_GUI_USER_MESH_SCAN_FINISH, "gui user mesh scan finish"},
    { EVENT_GUI_USER_MESH_PROV_FINISH, "gui user mesh provision finish"},
    { EVENT_GUI_USER_MESH_UNPROV_FINISH, "gui user mesh un-provision finish"},
    { EVENT_GUI_USER_WIFI_SCAN_INFO_UPDATE, "gui user wifi scan info finish"},
    { EVENT_GUI_USER_MESH_SCAN_INFO_UPDATE, "gui user mesh scan info finish"},
    { EVENT_GUI_USER_MESH_ADD_INFO_UPDATE, "gui user mesh add index update"},
    { EVENT_GUI_USER_VOICEASRRESULT_UPDATE, "gui user voice asr result update"},
    { EVENT_GUI_USER_MESH_STATUS_ONOFF_UPDATE, "gui user mesh subdev status changed"},
    { EVENT_GUI_USER_FOTA_START_UPDATE, "gui user fota start"},
    { EVENT_GUI_USER_WIFI_NOEXIST, "gui wifi noexist"},
    { EVENT_GUI_USER_WIFI_PSK_ERR, "gui wifi psk error"},
    { EVENT_GUI_USER_FOTA_PROGRESS, "gui fota progress"},
    { EVENT_GUI_USER_MUSIC_NEXT, "gui music next"},
    { EVENT_GUI_USER_MUSIC_PREV, "gui music prev"},
    { EVENT_GUI_USER_MUSIC_PAUSE, "gui music pause"},
    { EVENT_GUI_USER_MUSIC_PLAY, "gui music play local"},
    { EVENT_GUI_USER_PLAY_OVER, "gui music play over"},

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
            //local_audio_play("sys_starting.mp3");
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
            local_wakeup_audio_play("wakeup_im_here.mp3");
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
#if 1
        /* GUI 用户触发的事件　*/
        case EVENT_GUI_USER_WIFI_SCAN_START:
            LOGI(TAG, "gui trigger wifi scan");
            app_user_scan_wifi_network_list();
            break;
        case EVENT_GUI_USER_WIFI_PROV_START:
            LOGI(TAG, "gui trigger wifi provision");
            break;
        case EVENT_GUI_USER_MESH_SCAN_START:
            LOGI(TAG, "gui trigger mesh scan");
            break;
        case EVENT_GUI_USER_MESH_PROV_START:
            LOGI(TAG, "gui trigger mesh provision");
            break;
        case EVENT_GUI_USER_MESH_UNPROV_START:
            LOGI(TAG, "gui trigger mesh un-provision");
            break;
#endif

#if 1
        /* 系统反馈给GUI的状态事件　*/
        case EVENT_GUI_USER_WIFI_SCAN_FINISH:
            //LOGI(TAG, "gui trigger wifi scan finish - ap list is :");
            // Debug
            //int i = 0;
            // while(g_app_wifi_user_scan_table[i].rssi !=0)
            // {
            //     LOGI(TAG,"%s : %d", g_app_wifi_user_scan_table[i].ssid, g_app_wifi_user_scan_table[i].rssi);
            //     i++;
            // }
            //app_event_update(EVENT_GUI_USER_WIFI_SCAN_INFO_UPDATE);
            //local_audio_play("wifiscanover.mp3");
            break;
        case EVENT_GUI_USER_WIFI_PROV_FINISH:
            LOGI(TAG, "gui trigger wifi provision finish");
            break;
        case EVENT_GUI_USER_MESH_SCAN_FINISH:
            LOGI(TAG, "gui trigger mesh scan finish");
            break;
        case EVENT_GUI_USER_MESH_PROV_FINISH:
            LOGI(TAG, "gui trigger mesh provision finish");
            break;
        case EVENT_GUI_USER_MESH_UNPROV_FINISH:
            LOGI(TAG, "gui trigger mesh un-provision finish");
            break;
#endif
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
