/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <sys/time.h>
#include <drv/rtc.h>
#include <uservice/uservice.h>
#include <uservice/eventid.h>
#include <rtc_alarm.h>

#define TAG "RTC"

#ifdef CONFIG_CSI_V2
static csi_rtc_t g_rtc_handle;
#else
static rtc_handle_t g_rtc_hd = NULL;
#endif
static uint8_t g_rtc_start = 0;

void rtc_debug(void)
{
    struct tm tm_now = {0,};
    time_t time_rtc_now;
    time_t time_sys_now;
    rtc_init();
#ifdef CONFIG_CSI_V2
    int32_t state;
    csi_rtc_time_t last_time;

    state = csi_rtc_get_time(&g_rtc_handle, &last_time);

    if (state < 0) {
        LOGE(TAG, "rtc get");
        return;
    }

    tm_now.tm_year = last_time.tm_year;
    tm_now.tm_mon = last_time.tm_mon ;
    tm_now.tm_mday = last_time.tm_mday;
    tm_now.tm_hour = last_time.tm_hour;
    tm_now.tm_min = last_time.tm_min ;
    tm_now.tm_sec = last_time.tm_sec ;
    tm_now.tm_wday = last_time.tm_wday;
    tm_now.tm_yday = last_time.tm_yday;
#else

    if (g_rtc_hd == NULL) {
        LOGE(TAG, "rtc init");
        return;
    }

    if (csi_rtc_get_time(g_rtc_hd, &tm_now) < 0) {
        LOGE(TAG, "rtc get");
        return;
    }

#endif
    time_sys_now = time(NULL);
    time_rtc_now = mktime(&tm_now);

    printf("\tUTC:%s %d\n", ctime(&time_sys_now), time_sys_now);
    printf("\tRTC:%s %d\n", ctime(&time_rtc_now), time_rtc_now);
}
#ifdef CONFIG_CSI_V2
void rtc_irq_handler(csi_rtc_t *rtc_handle, void *arg)
#else
void rtc_irq_handler(int32_t idx, rtc_event_e event)
#endif
{
    //LOGD(TAG, "-----rtc_irqhandler: Time is up!-----");
    //rtc_debug();
    event_publish(EVENT_CLOCK_ALARM, NULL);
}

void rtc_init(void)
{
#ifdef CONFIG_CSI_V2
    int32_t state;
    if (0 == g_rtc_start) {
        state = csi_rtc_init(&g_rtc_handle, 0U);

        if (state < 0) {
            LOGE(TAG, "rtc init");
            return;
        }

        g_rtc_start = 1;
    }

#else
    int ret;
    if (g_rtc_hd != NULL) {
        return;
    }

    g_rtc_hd = csi_rtc_initialize(0, rtc_irq_handler);


    if (0 == g_rtc_start) {
        ret = csi_rtc_start(g_rtc_hd);

        if (0 != ret) {
            LOGE(TAG, "rtc start failed %d", ret);
            return;
        }

        g_rtc_start = 1;
    }

#endif
}

void rtc_from_system(void)
{
    rtc_init();

    struct tm *tm_now;
    time_t time_t_now = time(NULL);
    tm_now = gmtime(&time_t_now);

#ifdef  CONFIG_CSI_V2

    if (tm_now == NULL) {
        LOGE(TAG, "gmtime");
        return;
    }

    int32_t state;
    csi_rtc_time_t initime;
    initime.tm_year    = tm_now->tm_year;
    initime.tm_mon     = tm_now->tm_mon ;
    initime.tm_mday    = tm_now->tm_mday ;
    initime.tm_hour    = tm_now->tm_hour;
    initime.tm_min     = tm_now->tm_min ;
    initime.tm_sec     = tm_now->tm_sec ;
    initime.tm_wday    = tm_now->tm_wday;
    initime.tm_yday    = tm_now->tm_yday;
    state = csi_rtc_set_time_no_wait(&g_rtc_handle, &initime);

    if (state < 0) {
        LOGE(TAG, "rtc set");
    }

#else

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

#endif
}

time_t rtc_to_system(void)
{
    struct tm tm_now = {0,};
    time_t time_rtc_now;

    rtc_init();

#ifdef  CONFIG_CSI_V2
    int32_t state;
    csi_rtc_time_t last_time;
    state = csi_rtc_get_time(&g_rtc_handle, &last_time);

    if (state < 0) {
        LOGE(TAG, "rtc get");
        return 0;
    }

    tm_now.tm_year = last_time.tm_year ;
    tm_now.tm_mon = last_time.tm_mon  ;
    tm_now.tm_mday = last_time.tm_mday ;
    tm_now.tm_hour = last_time.tm_hour ;
    tm_now.tm_min = last_time.tm_min  ;
    tm_now.tm_sec = last_time.tm_sec  ;
    tm_now.tm_wday = last_time.tm_wday;
    tm_now.tm_yday = last_time.tm_yday;

#else

    if (g_rtc_hd == NULL) {
        LOGE(TAG, "rtc init,to sys");
        return 0;
    }

    if (csi_rtc_get_time(g_rtc_hd, &tm_now) < 0) {
        LOGE(TAG, "rtc get");
        return 0;
    }

#endif
    LOGD(TAG, "rtc time [%02d:%02d:%02d]", tm_now.tm_hour + TIME_ZONE, tm_now.tm_min, tm_now.tm_sec);

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
#ifdef  CONFIG_CSI_V2
    int32_t state;
    csi_rtc_time_t last_time;
    state = csi_rtc_get_time(&g_rtc_handle, &last_time);

    if (state < 0) {
        LOGE(TAG, "rtc get");
        return 0;
    }

    tm_now.tm_year = last_time.tm_year ;
    tm_now.tm_mon = last_time.tm_mon  ;
    tm_now.tm_mday = last_time.tm_mday ;
    tm_now.tm_hour = last_time.tm_hour ;
    tm_now.tm_min = last_time.tm_min  ;
    tm_now.tm_sec = last_time.tm_sec  ;
    tm_now.tm_wday = last_time.tm_wday;
    tm_now.tm_yday = last_time.tm_yday;
#else

    if (g_rtc_hd == NULL) {
        LOGE(TAG, "rtc init,get time");
        return 0;
    }

    if (csi_rtc_get_time(g_rtc_hd, &tm_now) < 0) {
        LOGE(TAG, "rtc get");
        return 0;
    }

#endif
    return mktime(&tm_now);
}


void rtc_set_alarm(struct tm *tm_set)
{
    rtc_init();
#ifdef CONFIG_CSI_V2
    int32_t state;
    csi_rtc_time_t alarm_time = {0};

    alarm_time.tm_wday = tm_set->tm_wday;
    alarm_time.tm_mday = tm_set->tm_mday;
    alarm_time.tm_hour = tm_set->tm_hour;
    alarm_time.tm_min  = tm_set->tm_min;
    alarm_time.tm_sec  = tm_set->tm_sec;
    alarm_time.tm_year = tm_set->tm_year;

    state = csi_rtc_set_alarm(&g_rtc_handle, &alarm_time, rtc_irq_handler, NULL);

    if (state < 0) {
        LOGE(TAG, "set alarm");
        return;
    }

#else

    if (g_rtc_hd == NULL) {
        LOGE(TAG, "rtc init,set alarm");
        return;
    }

    if (csi_rtc_set_alarm(g_rtc_hd, tm_set) < 0) {
        LOGE(TAG, "set alarm");
        return;
    }

    if (csi_rtc_enable_alarm(g_rtc_hd, 1) < 0) {
        LOGE(TAG, "enable alarm");
        return;
    }

#endif
}
