/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <sys/time.h>
#include <drv/rtc.h>
#include <yoc/uservice.h>
#include <yoc/eventid.h>
#include <aos/log.h>
#include <rtc_alarm.h>

#define TAG "RTC"

static rtc_handle_t g_rtc_hd = NULL;

void rtc_debug(void)
{
    struct tm tm_now = {0,};
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

    LOGI(TAG, "RTC=%d,SYS=%d",time_rtc_now, time_sys_now);
    LOGI(TAG, "app_rtc_debug %d:%d week%d", tm_now.tm_hour, tm_now.tm_min, tm_now.tm_wday);
}

void rtc_irq_handler(int32_t idx, rtc_event_e event)
{
    LOGD(TAG, "-----rtc_irqhandler: Time is up!-----");
    //rtc_debug();
    event_publish(EVENT_CLOCK_ALARM, NULL);
}

void rtc_init(void)
{
    if (g_rtc_hd != NULL) {
        return;
    }

    g_rtc_hd = csi_rtc_initialize(0, rtc_irq_handler);
    if (g_rtc_hd == NULL) {
        LOGE(TAG, "rtc init");
    }
}

void rtc_from_system(void)
{
    rtc_init();

    struct tm *tm_now;
    time_t time_t_now = time(NULL);
    tm_now = gmtime(&time_t_now);

    if (g_rtc_hd == NULL) {
        LOGE(TAG, "rtc init");
        return;
    }

    if (tm_now == NULL) {
        LOGE(TAG, "gmtime");
        return;
    }

    if (csi_rtc_set_time(g_rtc_hd, tm_now) < 0) {
        LOGE(TAG, "rtc set");
    }
}

time_t rtc_to_system(void)
{
    rtc_init();

    struct tm tm_now = {0,};
    time_t time_rtc_now;

    if (g_rtc_hd == NULL) {
        LOGE(TAG, "rtc init");
        return 0;
    }

    if (csi_rtc_get_time(g_rtc_hd, &tm_now) < 0) {
        LOGE(TAG, "rtc get");
        return 0;
    }

    time_rtc_now = mktime(&tm_now);

    struct timeval tv;
    tv.tv_sec = time_rtc_now;
    tv.tv_usec = 0;

    settimeofday(&tv, NULL);

    return time_rtc_now;
}

time_t rtc_get_time(void)
{
    struct tm tm_now = {0,};

    if (g_rtc_hd == NULL) {
        LOGE(TAG, "rtc init");
        return 0;
    }

    if (csi_rtc_get_time(g_rtc_hd, &tm_now) < 0) {
        LOGE(TAG, "rtc get");
        return 0;
    }

    return mktime(&tm_now);
}


void rtc_set_alarm(int32_t week, int32_t hour, int32_t min, int32_t sec)
{
    struct tm tm_time;
    tm_time.tm_wday = week;
    tm_time.tm_hour = hour;
    tm_time.tm_min  = min;
    tm_time.tm_sec  = sec;

    if (g_rtc_hd == NULL) {
        LOGE(TAG, "rtc init");
        return;
    }

    if (csi_rtc_set_alarm(g_rtc_hd, &tm_time) < 0) {
        LOGE(TAG, "config alarm");
        return;
    }

    if (csi_rtc_enable_alarm(g_rtc_hd, 1) < 0) {
        LOGE(TAG, "enable alarm");
        return;
    }
}
