/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#if defined(AOS_COMP_CLI) || defined(USE_CLI)
#include <aos/cli.h>

extern void cli_reg_cmd_ps(void);
extern void cli_reg_cmd_sysinfo(void);
extern void cli_reg_cmd_sysinfo(void);
extern void cli_reg_cmd_free(void);
extern void cli_reg_cmd_kvtool(void);

void board_cli_init()
{
    aos_cli_init();

    cli_reg_cmd_ps();

    cli_reg_cmd_sysinfo();

    cli_reg_cmd_free();

    cli_reg_cmd_kvtool();
}
#endif