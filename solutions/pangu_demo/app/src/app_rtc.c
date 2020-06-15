/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <app_config.h>

#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <pin_name.h>
#include <drv/rtc.h>
#include <aos/log.h>
#include <yoc/uservice.h>

#include "app_main.h"

#define TAG "RTC"

static rtc_handle_t g_rtc_hd = NULL;
void app_rtc_init(void)
{
    if (g_rtc_hd != NULL) {
        return;
    }

    g_rtc_hd = csi_rtc_initialize(0, NULL);

    if (g_rtc_hd == NULL) {
        LOGE(TAG, "rtc init");
    }
}

void app_rtc_from_system(void)
{
    app_rtc_init();

    struct tm *tm_now;
    time_t time_t_now = time(NULL);

    tm_now = gmtime(&time_t_now);

    if (g_rtc_hd == NULL) {
        LOGE(TAG, "rtc init");
        return;
    }

    if (csi_rtc_set_time(g_rtc_hd, tm_now) < 0) {
        LOGE(TAG, "rtc set");
    }
}

void app_rtc_to_system(void)
{
    app_rtc_init();

    struct tm tm_now;
    time_t time_rtc_now;
    time_t time_sys_now;

    if (g_rtc_hd == NULL) {
        LOGE(TAG, "rtc init");
        return;
    }

    if (csi_rtc_get_time(g_rtc_hd, &tm_now) < 0) {
        LOGE(TAG, "rtc get");
        return; 
    }

    time_sys_now = time(NULL);
    time_rtc_now = mktime(&tm_now);

    struct timeval tv;
    tv.tv_sec = time_rtc_now;
    tv.tv_usec = 0;

    if (abs(time_sys_now - time_rtc_now) > 5) {
        settimeofday(&tv, NULL);
        event_publish_delay(EVENT_NTP_RETRY_TIMER, NULL, 6000);
        LOGE(TAG, "ntp time");
    }
}
