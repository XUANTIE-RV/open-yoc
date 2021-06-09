/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */


#include <aos/debug.h>
#include <aos/cli.h>

void board_cli_init()
{
    
    aos_cli_init();

    extern void cli_reg_cmd_ps(void);
    cli_reg_cmd_ps();

    extern void cli_reg_cmd_free(void);
    cli_reg_cmd_free();

    extern void cli_reg_cmd_kvtool(void);
    cli_reg_cmd_kvtool();

    // extern void cli_reg_cmd_ifconfig_wifi(void);
    // cli_reg_cmd_ifconfig_wifi();

    extern void cli_reg_cmd_factory(void);
    cli_reg_cmd_factory();

    extern void cli_reg_cmd_sysinfo(void);
    cli_reg_cmd_sysinfo();

    // extern void cli_reg_cmd_ping(void);
    // cli_reg_cmd_ping();

}
