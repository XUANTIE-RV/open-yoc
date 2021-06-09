/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <pin_test.h>

int test_pin_main(char *args)
{
    char *case_name[] = {
        "PIN_CONFIG",
    };

    int (*case_func[])(char *args) = {
        test_pin_config,
    };

    uint8_t i = 0;

    for (i = 0; i < sizeof(case_name) / sizeof(char *); i++) {
        if (!strcmp((void *)_mc_name, case_name[i])) {
            case_func[i](args);
            return 0;
        }
    }

    TEST_CASE_TIPS("CORETIM don't supported this command");
    return -1;

}
