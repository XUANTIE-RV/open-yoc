/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <aos/debug.h>
#include <aos/cli.h>
#include <aos/cli_cmd.h>
#include <stdio.h>

void board_cli_init(utask_t *task)
{
    if (task == NULL) {
        task = utask_new("cli", 8 * 1024, QUEUE_MSG_COUNT, 32);
    }

    aos_check(task, EIO);

    cli_service_init(task);

    cli_reg_cmd_help();

    cli_reg_cmd_ps();

    cli_reg_cmd_free();

    cli_reg_cmd_addr();
    cli_reg_cmd_kvtool();
    cli_reg_cmd_factory();
    cli_reg_cmd_ifconfig();
    // cli_reg_cmd_ping();

    // cli_reg_cmd_sysinfo();

    cli_reg_cmd_ls();
    cli_reg_cmd_rm();
    cli_reg_cmd_cat();
    cli_reg_cmd_mkdir();
    cli_reg_cmd_mv();

    // cli_reg_cmd_gpstest();

    // cli_reg_cmd_hostfstest();

    // cli_reg_cmd_flashtest();

    cli_reg_cmd_ping();
    
}