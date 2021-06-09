/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <trng_test.h>

test_func_map_t trng_test_funcs_map[] = {
    {"TRNG_MULTI", test_trng_multi},
    {"TRNG_SINGLE", test_trng_single},
    {"TRNG_MULTI_PERFORMANCE", test_trng_multi_performance},
    {"TRNG_SINGLE_PERFORMANCE", test_trng_single_performance},
    {"TRNG_INTERFACE", test_trng_interface},
};


int test_trng_main(char *args)
{
    uint8_t i;

    for (i = 0; i < sizeof(trng_test_funcs_map) / sizeof(test_func_map_t); i++) {
        if (!strcmp((void *)_mc_name, trng_test_funcs_map[i].test_func_name)) {
            (*(trng_test_funcs_map[i].test_func_p))(args);
            return 0;
        }
    }

    TEST_CASE_TIPS("TRNG module don't support this command.");
    return -1;
}

