/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <sha_test.h>

test_func_map_t sha_test_funcs_map[] = {
    {"SHA_SYNC", test_sha_sync},
    {"SHA_ASYNC", test_sha_async},
    {"SHA_SET_SHA_MODE", test_set_sha_mode},
    {"SHA_SYNC_PERFORMANCE", test_sha_sync_performance},
    {"SHA_ASYNC_PERFORMANCE", test_sha_async_performance},
    {"SHA_SYNC_STABILITY", test_sha_sync_stability},
    {"SHA_ASYNC_STABILITY", test_sha_async_stability}
};


int test_sha_main(char *args)
{
    uint8_t i;

    for (i = 0; i < sizeof(sha_test_funcs_map) / sizeof(test_func_map_t); i++) {
        if (!strcmp((void *)_mc_name, sha_test_funcs_map[i].test_func_name)) {
            (*(sha_test_funcs_map[i].test_func_p))(args);
            return 0;
        }
    }

    TEST_CASE_TIPS("SHA module don't support this command.");
    return -1;
}
