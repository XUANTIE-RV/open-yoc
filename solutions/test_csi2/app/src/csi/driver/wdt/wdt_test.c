/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <wdt_test.h>

test_func_map_t wdt_test_funcs_map[] = {
    {"WDT_INTERFACE", test_wdt_interface},
    {"WDT_QUERY_FEED", test_wdt_queryFeed},
    {"WDT_INTERRUPT_FEED", test_wdt_interruptFeed},
    {"WDT_TIMEOUT_RESET", test_wdt_timeoutReset},
    {"WDT_START_STOP", test_wdt_startStop},
    {"WDT_GET_INFO", test_wdt_getInfo},
};


int test_wdt_main(char *args)
{
    uint8_t i;

    for (i = 0; i < sizeof(wdt_test_funcs_map) / sizeof(test_func_map_t); i++) {
        if (!strcmp((void *)_mc_name, wdt_test_funcs_map[i].test_func_name)) {
            (*(wdt_test_funcs_map[i].test_func_p))(args);
            return 0;
        }
    }

    TEST_CASE_TIPS("WDT module don't support this command.");
    return -1;
}


