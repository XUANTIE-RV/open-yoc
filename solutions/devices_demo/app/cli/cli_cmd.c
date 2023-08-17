/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include <aos/cli.h>

void board_cli_init()
{
    aos_cli_init();

    extern void cli_reg_cmd_kvtool(void);
    cli_reg_cmd_kvtool();
}

