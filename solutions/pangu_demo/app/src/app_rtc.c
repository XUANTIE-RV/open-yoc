/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <app_config.h>

#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#ifdef CONFIG_CSI_V2
#else
#include <pin_name.h>
#endif
#include <drv/rtc.h>
#include <uservice/uservice.h>

#include "app_main.h"

#define TAG "RTC"
#ifdef CONFIG_CSI_V2
static csi_rtc_t  g_rtc ;
static csi_rtc_t  *g_rtc_hd = NULL;
#else
static rtc_handle_t g_rtc_hd = NULL;
#endif
void app_rtc_init(void)
{
    if (g_rtc_hd != NULL) {
        return;
    }

#ifdef CONFIG_CSI_V2
    csi_rtc_init(&g_rtc, 0U);
    g_rtc_hd = &g_rtc;
#else
    g_rtc_hd = csi_rtc_initialize(0, NULL);
#endif

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
#ifdef CONFIG_CSI_V2
    if (csi_rtc_set_time_no_wait(g_rtc_hd, (const csi_rtc_time_t *)tm_now) < 0) {
        LOGE(TAG, "rtc set");
    }
#else
    if (csi_rtc_set_time(g_rtc_hd, tm_now) < 0) {
        LOGE(TAG, "rtc set");
    }
#endif
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

#ifdef CONFIG_CSI_V2
    if (csi_rtc_get_time(g_rtc_hd, (csi_rtc_time_t *) &tm_now) < 0) {
        LOGE(TAG, "rtc get");
        return;
    }
#else
    if (csi_rtc_get_time(g_rtc_hd, &tm_now) < 0) {
        LOGE(TAG, "rtc get");
        return;
    }
#endif
    time_sys_now = time(NULL);
    time_rtc_now = mktime(&tm_now);
    if (time_rtc_now < 0) {
        LOGE(TAG, "mktime error");
        return;
    }

    struct timeval tv;
    tv.tv_sec = time_rtc_now;
    tv.tv_usec = 0;

    if (abs(time_sys_now - time_rtc_now) > 5) {
        settimeofday(&tv, NULL);
        event_publish_delay(EVENT_NTP_RETRY_TIMER, NULL, 6000);
        LOGE(TAG, "ntp time");
    }
}
