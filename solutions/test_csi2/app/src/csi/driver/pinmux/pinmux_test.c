/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <pinmux_test.h>


test_pinmux_args_t pinmux_config;

int test_pinmux_main(char *args)
{
    test_pinmux_args_t td;
    int ret;

    uint32_t get_data[3];

    ret = args_parsing(args, get_data, 3);
    TEST_CASE_ASSERT_QUIT(ret == 0, "num_of_parameter error");

    td.dev_idx = (uint8_t)get_data[0];
    td.pin = (uint8_t)get_data[1];
    td.pin_func = (uint8_t)get_data[2];

    TEST_CASE_TIPS("dev idx is %d", td.dev_idx);
    TEST_CASE_TIPS("pin idx is %d", td.pin);
    TEST_CASE_TIPS("pin_func idx is %d", td.pin_func);



    if ((!strcmp((const char *)_mc_name, "PINMUX_CONFIG")) || (!strcmp((const char *)_mc_name, "PINMUX_CONFIG_ASSIST"))) {
        test_pinmux_config(&td);
        return td.dev_idx;
    }

    TEST_CASE_ASSERT(1 == 0, "PINMUX don't supported this command");
    return -1;

}


