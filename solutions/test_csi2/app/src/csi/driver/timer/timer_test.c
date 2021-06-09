/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <timer_test.h>

test_func_map_t timer_test_funcs_map[] = {
    {"TIMER_INTERFACE", test_timer_interface},
    {"TIMER_PRECISION", test_timer_precision},
    {"TIMER_STATE", test_timer_state},
    {"TIMER_SETTING", test_timer_setting},
    {"TIMER_GETLOADVALUE", test_timer_getLoadvalue},
    {"TIMER_GETREMAININGVALUE", test_timer_getRemainingvalue},
    {"TIMER_CALLBACK", test_timer_callback},
};

int test_timer_main(char *args)
{
    uint8_t i;

    for (i = 0; i < sizeof(timer_test_funcs_map) / sizeof(test_func_map_t); i++) {
        if (!strcmp((void *)_mc_name, timer_test_funcs_map[i].test_func_name)) {
            (*(timer_test_funcs_map[i].test_func_p))(args);
            return 0;
        }
    }

    TEST_CASE_TIPS("TIMER module don't support this command.");
    return -1;
}
