/*
 * Copyright (C) 2019-2030 Alibaba Group Holding Limited
 */

#include <aos/cli.h>

void board_cli_init()
{
    aos_cli_init();

    extern void cli_reg_cmd_ping(void);
    cli_reg_cmd_ping();

    extern void cli_reg_cmd_ifconfig(void);
    cli_reg_cmd_ifconfig();

    extern void cli_reg_cmd_iperf(void);
    cli_reg_cmd_iperf();

    extern void cli_reg_cmd_ps(void);
    cli_reg_cmd_ps();

    extern void cli_reg_cmd_free(void);
    cli_reg_cmd_free();

    extern void cli_reg_cmd_sysinfo(void);
    cli_reg_cmd_sysinfo();

    extern void cli_reg_cmd_kvtool(void);
    cli_reg_cmd_kvtool();
    
    extern void cli_reg_cmd_yunit_test(void);
    cli_reg_cmd_yunit_test();
}
