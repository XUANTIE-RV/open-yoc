/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */


#include <aos/cli.h>
#include <aos/cli_cmd.h>

void board_cli_init(utask_t *task)
{
    cli_service_init(task);

    cli_reg_cmd_help();

    cli_reg_cmd_ping();
    cli_reg_cmd_ifconfig();
    // cli_reg_cmd_ntp();
    cli_reg_cmd_ps();
    cli_reg_cmd_free();
    cli_reg_cmd_factory();
    cli_reg_cmd_sysinfo();
    cli_reg_cmd_kvtool();
}
