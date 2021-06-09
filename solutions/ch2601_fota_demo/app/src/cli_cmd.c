/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */


#include <aos/cli.h>

extern void cli_reg_cmd_kvtool(void);

void board_cli_init()
{
    aos_cli_init();
    // cli_service_init();

    // cli_reg_cmd_help();

    // cli_reg_cmd_ping();
    // cli_reg_cmd_ifconfig();
    // // cli_reg_cmd_ntp();
    // cli_reg_cmd_ps();
    // cli_reg_cmd_free();
    // cli_reg_cmd_factory();
    // cli_reg_cmd_sysinfo();
    cli_reg_cmd_kvtool();
}
