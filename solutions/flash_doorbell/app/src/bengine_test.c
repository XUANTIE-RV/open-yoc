/*
 * Copyright (C) 2018-2022 Alibaba Group Holding Limited
 */

#if defined(CONFIG_BENGINE_ENABLE) && CONFIG_BENGINE_ENABLE
#include <stdio.h>
#include "ulog/ulog.h"
#include "aos/cli.h"
#include <fcntl.h>
#include <aos/errno.h>
#include <aos/kernel.h>
void dump_bengine_stat(int32_t argc, char **argv)
{
    extern void bengine_dload_disp();
    bengine_dload_disp();
}

ALIOS_CLI_CMD_REGISTER(dump_bengine_stat, dump_bengine_stat, dump bengine stat);
#endif

