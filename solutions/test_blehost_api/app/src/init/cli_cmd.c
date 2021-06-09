/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */


#include <aos/debug.h>
#include <uservice/uservice.h>
#include <aos/cli.h>


void cli_cmd_reg()
{
    // extern void cli_reg_cmd_help(void);
    // cli_reg_cmd_help();

    extern void cli_reg_cmd_ps(void);
    cli_reg_cmd_ps();

    extern void cli_reg_cmd_free(void);
    cli_reg_cmd_free();

    extern void test_yunit_test_register_cmd(void);
    test_yunit_test_register_cmd();

}
