/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <rtc_test.h>

typedef int (*test_func)(char *args);

typedef struct {
    char *test_func_name;
    test_func test_func_p;
} test_func_map_t;


test_func_map_t rtc_test_funcs_map[] = {
    {"RTC_INTERFACE", test_rtc_interface},
    {"RTC_ALARM", test_rtc_alarm_set},
    {"RTC_TIME", test_rtc_time},
};


int test_rtc_main(char *args)
{
    uint8_t i;

    for (i = 0; i < sizeof(rtc_test_funcs_map) / sizeof(test_func_map_t); i++) {
        if (!strcmp((void *)_mc_name, rtc_test_funcs_map[i].test_func_name)) {
            (*(rtc_test_funcs_map[i].test_func_p))(args);
            return 0;
        }
    }

    TEST_CASE_TIPS("RTC module don't support this command.");
    return -1;
}


