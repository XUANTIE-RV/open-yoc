/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include <aos/cli.h>

extern void cli_reg_cmd_kvtool(void);
extern void cli_reg_cmd_iperf(void);
extern void cli_reg_cmd_ping(void);
extern void cli_reg_cmd_ifconfig(void);
extern void debug_cli_cmd_init(void);

void app_cli_init()
{
    aos_cli_init();
    cli_reg_cmd_kvtool();
    cli_reg_cmd_ping();
    cli_reg_cmd_ifconfig();
    cli_reg_cmd_iperf();
    debug_cli_cmd_init();
}
