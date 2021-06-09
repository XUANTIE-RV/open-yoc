/*
 * Copyright (C) 2019-2030 Alibaba Group Holding Limited
 */

#include <aos/cli.h>


void board_cli_init()
{
    aos_cli_init();
#if 0
    extern void cli_reg_cmd_help(void);
    cli_reg_cmd_help();

    extern void cli_reg_cmd_ps(void);
    cli_reg_cmd_ps();

    extern void cli_reg_cmd_kvtool(void);
    cli_reg_cmd_kvtool();

    extern void cli_reg_cmd_free(void);
    cli_reg_cmd_free();

    //extern void cli_reg_cmd_ifconfig_wifi(void);
    //cli_reg_cmd_ifconfig_wifi();

    extern void cli_reg_cmd_ping(void);
    cli_reg_cmd_ping();
#endif
    extern void cli_reg_cmd_wifi(void);
    cli_reg_cmd_wifi();

    extern void test_yunit_test_register_cmd(void);
    test_yunit_test_register_cmd();
}
