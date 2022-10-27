/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <aos/cli.h>

extern void debug_cli_cmd_init(void);
void board_cli_init()
{
    aos_cli_init();
    debug_cli_cmd_init();
}
