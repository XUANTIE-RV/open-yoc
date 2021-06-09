/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */


#include <aos/debug.h>
#include <aos/cli.h>

extern void cli_reg_cmd_ping(void);
extern void cli_reg_cmd_ifconfig(void);
extern void cli_reg_cmd_kvtool(void);

void board_cli_init()
{
    aos_cli_init();

    cli_reg_cmd_ping();
    cli_reg_cmd_ifconfig();

    // extern void cli_reg_cmd_ps(void);
    // cli_reg_cmd_ps();

    // extern void cli_reg_cmd_free(void);
    // cli_reg_cmd_free();

    cli_reg_cmd_kvtool();
}
