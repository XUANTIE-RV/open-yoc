/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include <aos/cli.h>

extern void cli_reg_cmd_ps(void);
extern void cli_reg_cmd_free(void);

#if defined(CONFIG_GW_SMARTLIVING_SUPPORT) && CONFIG_GW_SMARTLIVING_SUPPORT
extern void cli_reg_cmd_triples();
#endif

void board_cli_init()
{
    aos_cli_init();

    cli_reg_cmd_ps();

    cli_reg_cmd_free();
#if defined(CONFIG_GW_SMARTLIVING_SUPPORT) && CONFIG_GW_SMARTLIVING_SUPPORT
    cli_reg_cmd_triples();
#endif
}