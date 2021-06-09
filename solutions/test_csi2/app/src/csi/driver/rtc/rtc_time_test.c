/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include "rtc_test.h"
#include <time.h>

typedef int (*set_time_func)(csi_rtc_t *rtc, const csi_rtc_time_t *rtctime);
uint32_t time_ms = 0;

static int test_rtc_time_func(csi_rtc_t *rtc_handler, test_rtc_args rtc_args, set_time_func func)
{
    bool rtc_runing;
    csi_error_t rtc_ret;
    csi_rtc_time_t cur_time;
    uint64_t diff_time = 0;
    time_t rtc;
    time_t cur;
    TEST_CASE_TIPS("set RTC time: %d-%d-%d %d:%d:%d", rtc_args.tm->tm_year + 1900, rtc_args.tm->tm_mon + 1, rtc_args.tm->tm_mday, rtc_args.tm->tm_hour, rtc_args.tm->tm_min, rtc_args.tm->tm_sec);

    rtc_ret = func(rtc_handler, rtc_args.tm);

    if (rtc_args.tm->tm_year < 70 || rtc_args.tm->tm_year > 199 || \
        rtc_args.tm->tm_mon < 0 || rtc_args.tm->tm_mon > 11     || \
        rtc_args.tm->tm_mday < 1 || rtc_args.tm->tm_mday > 31   || \
        rtc_args.tm->tm_hour < 0 || rtc_args.tm->tm_hour > 23   || \
        rtc_args.tm->tm_min < 0 || rtc_args.tm->tm_min > 59   || \
        rtc_args.tm->tm_sec < 0 || rtc_args.tm->tm_sec > 59
       ) {
        TEST_CASE_ASSERT_QUIT(rtc_ret == CSI_ERROR, "RTC time set invalid,should return %d,but return %d", CSI_ERROR, rtc_ret);
    } else {
        TEST_CASE_ASSERT_QUIT(rtc_ret == CSI_OK, "RTC time set error");
        rtc_runing = csi_rtc_is_running(rtc_handler);
        TEST_CASE_ASSERT(rtc_runing == true, "the status of RTC should be runing,but get a wrong RTC state");
        tst_mdelay(time_ms);
        rtc_ret = csi_rtc_get_time(rtc_handler, &cur_time);
        TEST_CASE_TIPS("get RTC time: %d-%d-%d %d:%d:%d", cur_time.tm_year + 1900, cur_time.tm_mon + 1, cur_time.tm_mday, cur_time.tm_hour, cur_time.tm_min, cur_time.tm_sec);

        rtc = mktime(&(rtc_args.tm));
        cur = mktime(&cur_time);

        diff_time = difftime(rtc, cur);
        TEST_CASE_TIPS("diff_time is %u", diff_time);
        TEST_CASE_ASSERT(diff_time == time_ms - 1, "RTC test error,current time is not equal to time set.");
    }

    return 0;
}

int test_rtc_time(char *args)
{
    csi_rtc_time_t tm_test = {.tm_year = 120,
                              .tm_mon = 11,
                              .tm_mday = 31,
                              .tm_hour = 23,
                              .tm_min = 59,
                              .tm_sec = 59
                             };
    csi_rtc_t rtc_handler;
    test_rtc_args rtc_args;
    csi_error_t rtc_ret;
    rtc_args.tm = &tm_test;
    TEST_CASE_READY();
    sscanf(args, "%d,%d,%d,%d,%d,%d,%d,%d", &rtc_args.idx, &time_ms, &tm_test.tm_year, &tm_test.tm_mon, &tm_test.tm_mday, &tm_test.tm_hour, &tm_test.tm_min, &tm_test.tm_sec);

    rtc_ret = csi_rtc_init(&rtc_handler, rtc_args.idx);
    TEST_CASE_ASSERT_QUIT(rtc_ret == CSI_OK, "RTC init error, idx:%d", rtc_args.idx);

    test_rtc_time_func(&rtc_handler, rtc_args, csi_rtc_set_time);

    test_rtc_time_func(&rtc_handler, rtc_args, csi_rtc_set_time_no_wait);

    csi_rtc_uninit(&rtc_handler);
    return 0;
}

