/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <aos/aos.h>
#include <yoc/uservice.h>
#include <yoc/eventid.h>
#include <yoc/lpm.h>
#include <sys_freq.h>
#include "app_main.h"

#define TAG  "app"

void user_local_event_cb(uint32_t event_id, const void *param, void *context)
{
    if (event_id == EVENT_NETMGR_GOT_IP) {
        LOGI(TAG, "Net Ready");
    } else if (event_id == EVENT_NETMGR_NET_DISCON) {
        LOGI(TAG, "Net disconnect");
    } else if (event_id == EVENT_IOT_CONNECT_SUCCESS) {
        LOGI(TAG, "Iot connect succ");
    } else if (event_id == EVENT_IOT_DISCONNECTED) {
        LOGI(TAG, "Iot disconnect");
    } else if (event_id == EVENT_IOT_PUSH_SUCCESS) {
        LOGI(TAG, "Iot push succ");
    } else if (event_id == EVENT_IOT_PUSH_FAILED) {
        LOGI(TAG, "Iot push failed");
    } else if (event_id == EVENT_IOT_CONNECT_FAILED) {
        LOGI(TAG, "Iot connect failed");
    }
}

void lpm_event_cb(pm_policy_t policy, lpm_event_e event)
{
    if (event == LPM_EVENT_SUSPEND) {
        LOGD(TAG, "goto sleep");
    } else if (event == LPM_EVENT_WAKUP) {
        LOGD(TAG, "wakeup");
    } else {
        LOGE(TAG, "lpm error");
    }
}

void main()
{
    board_yoc_init();
    LOGI(TAG, "hello world!\n");
    // drv_rtcclk_config(ELS_CLK);
    pm_init(lpm_event_cb);
    pm_config_policy(LPM_POLICY_DEEP_SLEEP);
    //pm_agree_halt(0);
    event_subscribe(EVENT_NETMGR_GOT_IP, user_local_event_cb, NULL);
    event_subscribe(EVENT_NETMGR_NET_DISCON, user_local_event_cb, NULL);

    event_subscribe(EVENT_IOT_CONNECT_SUCCESS, user_local_event_cb, NULL);
    event_subscribe(EVENT_IOT_DISCONNECTED, user_local_event_cb, NULL);
    event_subscribe(EVENT_IOT_PUSH_SUCCESS, user_local_event_cb, NULL);
    event_subscribe(EVENT_IOT_PUSH_FAILED, user_local_event_cb, NULL);
    event_subscribe(EVENT_IOT_CONNECT_FAILED, user_local_event_cb, NULL);
}
