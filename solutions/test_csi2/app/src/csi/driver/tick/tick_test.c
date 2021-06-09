/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <tick_test.h>

test_func_map_t tick_test_funcs_map[] = {
    {"TICK_FUNC", test_tick_func},
};


int test_tick_main(char *args)
{
    uint8_t i;

    for (i = 0; i < sizeof(tick_test_funcs_map) / sizeof(test_func_map_t); i++) {
        if (!strcmp((void *)_mc_name, tick_test_funcs_map[i].test_func_name)) {
            (*(tick_test_funcs_map[i].test_func_p))(args);
            return 0;
        }
    }

    TEST_CASE_TIPS("TICK module don't support this command.");
    return -1;
}