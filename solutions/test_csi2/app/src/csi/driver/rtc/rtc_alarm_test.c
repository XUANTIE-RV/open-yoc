/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include "rtc_test.h"

uint32_t timeout_ms = 0;
uint32_t alarm_ms = 0;

static void rtc_callback(csi_rtc_t *rtc, void *arg)
{
    *((uint8_t *)arg) = 1;
}

int rtc_set_alarm_manual(csi_rtc_time_t *test_time, csi_rtc_time_t *alarm_time, int sec_delta)
{
    if (test_time->tm_mon == 11) {
        alarm_time->tm_year = test_time->tm_year + 1;
        alarm_time->tm_mon = 0;
    } else {
        alarm_time->tm_year = test_time->tm_year;
        alarm_time->tm_mon = test_time->tm_mon + 1;
    }

    alarm_time->tm_mday = 1;
    alarm_time->tm_hour = 0;
    alarm_time->tm_min = 0;
    alarm_time->tm_sec = sec_delta - 1;

    return 0;
}

int test_rtc_alarm_func(csi_rtc_t *rtc_handler, test_rtc_args *rtc_args, int cancel_flag)
{
    csi_error_t rtc_ret;
    csi_rtc_time_t cur_time;
    csi_rtc_time_t alarm_time;
    uint32_t remain_time = 0;
    volatile uint8_t rtc_flag;

    TEST_CASE_TIPS("set RTC time: %d-%d-%d %d:%d:%d", rtc_args->tm->tm_year + 1900, rtc_args->tm->tm_mon + 1, rtc_args->tm->tm_mday, rtc_args->tm->tm_hour, rtc_args->tm->tm_min, rtc_args->tm->tm_sec);

    rtc_set_alarm_manual(rtc_args->tm, &alarm_time, alarm_ms);
    TEST_CASE_TIPS("set RTC alarm: %d-%d-%d %d:%d:%d", alarm_time.tm_year + 1900, alarm_time.tm_mon + 1, alarm_time.tm_mday, alarm_time.tm_hour, alarm_time.tm_min, alarm_time.tm_sec);

    rtc_ret = csi_rtc_init(rtc_handler, rtc_args->idx);
    TEST_CASE_ASSERT_QUIT(rtc_ret == CSI_OK, "RTC init error, idx:%d", rtc_args->idx);

    rtc_ret = csi_rtc_set_time(rtc_handler, rtc_args->tm);
    TEST_CASE_ASSERT_QUIT(rtc_ret == CSI_OK, "RTC set time error, idx:%d", rtc_args->idx);

    rtc_flag = 0;
    rtc_ret = csi_rtc_set_alarm(rtc_handler, &alarm_time, rtc_callback, &rtc_flag);

    tst_mdelay(1000);
    remain_time = csi_rtc_get_alarm_remaining_time(rtc_handler);
    TEST_CASE_ASSERT(remain_time > 0 && remain_time < 5, "RTC get wrong remaining time: %d", remain_time);

    if (cancel_flag == 1) {
        rtc_ret =  csi_rtc_cancel_alarm(rtc_handler);
        TEST_CASE_TIPS("RTC alarm cancel");
        TEST_CASE_ASSERT(rtc_ret == CSI_OK, "RTC alarm cancel error,should return CSI_OK,but return %d ", rtc_ret);
    }

    tst_mdelay(timeout_ms);
    TEST_CASE_ASSERT(rtc_flag == 0, "Alarm work before expected time");

    tst_mdelay(timeout_ms * 2);

    if (cancel_flag == 0) {
        TEST_CASE_ASSERT(rtc_flag == 1, "Alarm doesn't work as expected ");
    } else {
        TEST_CASE_ASSERT(rtc_flag == 0, "Alarm work after canceling the alarm");
    }

    csi_rtc_uninit(rtc_handler);
    return 0;
}

int test_rtc_alarm_set(char *args)
{
    csi_rtc_time_t tm_test = {.tm_year = 120,
                              .tm_mon = 11,
                              .tm_mday = 31,
                              .tm_hour = 23,
                              .tm_min = 59,
                              .tm_sec = 59
                             };
    csi_rtc_t *rtc_handler;
    test_rtc_args *rtc_args;
    rtc_args->tm = &tm_test;
    csi_rtc_t *test_rtc_handler;
    TEST_CASE_READY();
    sscanf(args, "%d,%d,%d,%d,%d,%d,%d,%d", &rtc_args->idx, &alarm_ms, &timeout_ms, &tm_test.tm_year, &tm_test.tm_mon, &tm_test.tm_mday, &tm_test.tm_hour, &tm_test.tm_min, &tm_test.tm_sec);

    test_rtc_alarm_func(rtc_handler, rtc_args, 0);     ///<test for no alarm cancelling

    test_rtc_alarm_func(rtc_handler, rtc_args, 1);     //<test for alarm cancelling
    return 0;
}