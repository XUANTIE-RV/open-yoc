/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef _RTC_TEST_H_
#define _RTC_TEST_H_
#include <stdint.h>
#include <drv/rtc.h>
#include <soc.h>
#include <string.h>
#include <stdlib.h>
#include <autotest.h>
#include <test_log.h>
#include <test_config.h>

typedef struct{
    uint32_t idx;
    csi_rtc_time_t *tm;
} test_rtc_args;

//default test case

extern int test_rtc_main(char *args);
extern int test_rtc_interface(csi_rtc_t *rtc_handler);
extern int test_rtc_time(csi_rtc_t *rtc_handler, test_rtc_args *test_args);
extern int test_rtc_alarm_set(csi_rtc_t *rtc_handler, test_rtc_args *test_args);
extern int test_rtc_alarm_cancel(csi_rtc_t *rtc_handler, test_rtc_args *test_args);
extern int rtc_time_assert(csi_rtc_time_t *ret_time, csi_rtc_time_t *expect);
extern int calc_expect_rtc(csi_rtc_time_t *test_time, csi_rtc_time_t *expect, int sec_delta);
#endif