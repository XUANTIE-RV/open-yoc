/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */

#include <aos/cli.h>
#include "app_main.h"

extern void cli_reg_cmd_free(void);

void app_cli_init(void)
{
    cli_reg_cmd_free();
}
