/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */
#include <aos/aos.h>
#include <aos/cli.h>

extern void cli_reg_cmd_triples(void);

void board_cli_init()
{
    aos_cli_init();

    cli_reg_cmd_triples();
}