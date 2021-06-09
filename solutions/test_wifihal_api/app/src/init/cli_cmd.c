/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <aos/cli.h>

void cli_reg_cmds()
{
    extern void test_yunit_test_register_cmd(void);
    test_yunit_test_register_cmd();
}
