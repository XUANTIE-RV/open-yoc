/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include "board.h"
#include <aos/kernel.h>
#include <stdio.h>
#include <k_api.h>
#include <ulog/ulog.h>

#include "cx_init.h"
#include "app_main.h"

extern void  cxx_system_init(void);
extern void board_yoc_init(void);

int main(int argc, char *argv[])
{
    cxx_system_init();
    board_yoc_init();

    LOGI("c906", "build time: %s, %s\r\n", __DATE__, __TIME__);

    app_cli_init();

    cx_init();
}

