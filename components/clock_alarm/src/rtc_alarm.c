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

    rtc_init();

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

    printf("\tUTC:%s %d\n", ctime(&time_sys_now), time_sys_now );
    printf("\tRTC:%s %d\n", ctime(&time_rtc_now), time_rtc_now );
}

void rtc_irq_handler(int32_t idx, rtc_event_e event)
{
    //LOGD(TAG, "-----rtc_irqhandler: Time is up!-----");
    //rtc_debug();
    event_publish(EVENT_CLOCK_ALARM, NULL);
}

void rtc_init(void)
{
    if (g_rtc_hd != NULL) {
        return;
    }

    g_rtc_hd = csi_rtc_initialize(0, rtc_irq_handler);
}

void rtc_from_system(void)
{
    rtc_init();

    struct tm *tm_now;
    time_t time_t_now = time(NULL);
    tm_now = gmtime(&time_t_now);

    if (g_rtc_hd == NULL) {
        LOGE(TAG, "rtc init,from sys");
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
    struct tm tm_now = {0,};
    time_t time_rtc_now;

    rtc_init();

    if (g_rtc_hd == NULL) {
        LOGE(TAG, "rtc init,to sys");
        return 0;
    }

    if (csi_rtc_get_time(g_rtc_hd, &tm_now) < 0) {
        LOGE(TAG, "rtc get");
        return 0;
    }
    LOGD(TAG, "rtc time [%02d:%02d:%02d]", tm_now.tm_hour, tm_now.tm_min, tm_now.tm_sec);

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

    rtc_init();

    if (g_rtc_hd == NULL) {
        LOGE(TAG, "rtc init,get time");
        return 0;
    }

    if (csi_rtc_get_time(g_rtc_hd, &tm_now) < 0) {
        LOGE(TAG, "rtc get");
        return 0;
    }

    return mktime(&tm_now);
}


void rtc_set_alarm(int32_t week, int day, int32_t hour, int32_t min, int32_t sec)
{
    struct tm tm_time;
    tm_time.tm_wday = week;
    tm_time.tm_mday = day;
    tm_time.tm_hour = hour;
    tm_time.tm_min  = min;
    tm_time.tm_sec  = sec;

    rtc_init();

    if (g_rtc_hd == NULL) {
        LOGE(TAG, "rtc init,set alarm");
        return;
    }

    if (csi_rtc_set_alarm(g_rtc_hd, &tm_time) < 0) {
        LOGE(TAG, "set alarm");
        return;
    }

    if (csi_rtc_enable_alarm(g_rtc_hd, 1) < 0) {
        LOGE(TAG, "enable alarm");
        return;
    }
}
