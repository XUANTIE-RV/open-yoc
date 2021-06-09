
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
#include <test_common.h>


typedef struct{
    uint32_t idx;
    struct tm tm;
    time_t offset_time;
} test_rtc_args;

//default test case

extern int test_rtc_main(char *args);
extern int test_rtc_interface(char *args);
extern int test_rtc_alarm_set(char *args);
extern int rtc_time_assert(struct tm *ret_time, struct tm *expect);
extern int calc_expect_rtc(struct tm *test_time, struct tm *expect, int sec_delta);
extern int test_rtc_time(char *args);

#endif