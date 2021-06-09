/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <intnest_test.h>

test_func_map_t intnest_test_funcs_map[] = {
    {"INTNEST_TIMER", test_intnest_timer},
};

int test_intnest_main(char *args)
{
    uint8_t i;

    for (i = 0; i < sizeof(intnest_test_funcs_map) / sizeof(test_func_map_t); i++) {
        if (!strcmp((void *)_mc_name, intnest_test_funcs_map[i].test_func_name)) {
            (*(intnest_test_funcs_map[i].test_func_p))(args);
            return 0;
        }
    }

    TEST_CASE_TIPS("INTNEST module don't support this command.");
    return -1;
}
