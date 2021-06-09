/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include "i2s_test.h"

test_func_map_t i2s_test_funcs_map[] = {
    {"I2S_INTERFACE", test_i2s_interface},
    {"I2S_STATUS", test_i2s_sync_status},
    {"I2S_SYNC_SEND", test_i2s_sync_send},
    {"I2S_SYNC_RECEIVE", test_i2s_sync_receive},
    {"I2S_ASYNC_SEND", test_i2s_async_send},
    {"I2S_ASYNC_RECEIVE", test_i2s_async_receive},
};

int test_i2s_main(char *args)
{
    uint8_t i;

    for (i = 0; i < sizeof(i2s_test_funcs_map) / sizeof(test_func_map_t); i++) {
        if (!strcmp((void *)_mc_name, i2s_test_funcs_map[i].test_func_name)) {
            (*(i2s_test_funcs_map[i].test_func_p))(args);
            return 0;
        }
    }

    TEST_CASE_TIPS("i2s module don't support this command.");
    return -1;
}
