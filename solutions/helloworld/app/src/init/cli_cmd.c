/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <yoc_config.h>

#include <aos/debug.h>
#include <aos/cli.h>

void board_cli_init(utask_t *task)
{
    if (task == NULL) {
        task = utask_new("cli", 8 * 1024, QUEUE_MSG_COUNT, 32);
    }

    aos_check(task, EIO);

    cli_service_init(task);

    extern void cli_reg_cmd_help(void);
    cli_reg_cmd_help();

    extern void cli_reg_cmd_ps(void);
    cli_reg_cmd_ps();

    extern void cli_reg_cmd_free(void);
    cli_reg_cmd_free();
}
