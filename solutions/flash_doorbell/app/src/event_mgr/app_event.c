/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */

#include <ulog/ulog.h>

#include <uservice/eventid.h>
#include <uservice/uservice.h>
#include <aos/kernel.h>
#include <ulog/ulog.h>

#include <cx/cloud_lv_service.h>
#include <app_cx_record.h>

#include "app_event.h"
#include "app_main.h"

#define TAG "STATUS"

status_event_t g_status_event_list[] = {
    /*系统*/
    { EVENT_STATUS_STARTING, "starting" },

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

};

int g_status_event_list_count = sizeof(g_status_event_list) / sizeof(status_event_t);

//static long long g_notify_play_status = 0;

static void app_event_mgr(uint32_t event_id, const void *data, void *context)
{
    LOGI(TAG, "Event ID = %x", event_id);

    switch (event_id) {
            /*系统*/
        case EVENT_STATUS_STARTING:
            /* Do things on sys bootup beginning*/
            break;
#if 0
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
            /* (配网超时)您输入的密码好像不太对，请检查后再试一次吧  */
            local_audio_play("wifiprov_fail.mp3");
            break;
        case EVENT_STATUS_WIFI_PROV_RECVED:
            local_audio_play("wifiprov_recved.mp3");
            break;
#endif
            /*网络连接*/
        case EVENT_STATUS_WIFI_CONN_SUCCESS:
            app_linkvisual_init();
            break;
        case EVENT_STATUS_WIFI_CONN_FAILED:
            
            break;
        case EVENT_STATUS_NTP_SUCCESS:
            app_record_init();
            break;

        default:
            break;
    }

    /* 显示状态更新 */
    //app_display_update();
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
