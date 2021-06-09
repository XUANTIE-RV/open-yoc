/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */


#include <aos/cli.h>

void cli_reg_cmd_ps(void);
void cli_reg_cmd_free(void);
void cli_reg_cmd_sysinfo(void);

void board_cli_init()
{
    aos_cli_init();

    cli_reg_cmd_ps();
    cli_reg_cmd_free();
    cli_reg_cmd_sysinfo();
}
