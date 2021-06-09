/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */


#include <aos/cli.h>

void cli_reg_cmd_iperf(void);
void cli_reg_cmd_ping(void);
void cli_reg_cmd_ifconfig(void);
void cli_reg_cmd_kvtool(void);
void cli_reg_cmd_ntp(void);
void cli_reg_cmd_ps(void);
void cli_reg_cmd_free(void);
void cli_reg_cmd_factory(void);
void cli_reg_cmd_sysinfo(void);

void board_cli_init()
{
    aos_cli_init();

    cli_reg_cmd_ping();
    cli_reg_cmd_ifconfig();
    cli_reg_cmd_kvtool();
    cli_reg_cmd_iperf();
    cli_reg_cmd_ntp();
    cli_reg_cmd_ps();
    cli_reg_cmd_free();
    cli_reg_cmd_factory();
    cli_reg_cmd_sysinfo();
}
