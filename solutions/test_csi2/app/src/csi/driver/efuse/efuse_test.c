/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include "efuse_test.h"

test_func_map_t efuse_test_funcs_map[] = {
    {"EFUSE_INTERFACE", test_efuse_interface},
    {"EFUSE_READPROGRAM", test_efuse_readprogram},
};

int test_efuse_main(char *args)
{
    uint8_t i;

    for (i = 0; i < sizeof(efuse_test_funcs_map) / sizeof(test_func_map_t); i++) {
        if (!strcmp((void *)_mc_name, efuse_test_funcs_map[i].test_func_name)) {
            (*(efuse_test_funcs_map[i].test_func_p))(args);
            return 0;
        }
    }

    TEST_CASE_TIPS("EFUSE module don't support this command.");
    return -1;
}

