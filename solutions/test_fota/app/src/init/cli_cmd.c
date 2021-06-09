/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */


#include <aos/cli.h>
//#include <aos/cli_cmd.h>

void board_cli_init()
{
    aos_cli_init();

    extern void cli_reg_cmd_ping(void);
    cli_reg_cmd_ping();

    extern void cli_reg_cmd_ifconfig(void);
    cli_reg_cmd_ifconfig();

    extern void cli_reg_cmd_ps(void);
    cli_reg_cmd_ps();

    extern void cli_reg_cmd_free(void);
    cli_reg_cmd_free();

    extern void cli_reg_cmd_factory(void);
    cli_reg_cmd_factory();

    extern void cli_reg_cmd_sysinfo(void);
    cli_reg_cmd_sysinfo();

    extern void cli_reg_cmd_kvtool(void);
    cli_reg_cmd_kvtool();

    extern void test_yunit_test_register_cmd(void);
    test_yunit_test_register_cmd();

    extern void cli_reg_cmd_fotatest(void);
    cli_reg_cmd_fotatest();
}
