/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <aos/kernel.h>
#include <aos/yloop.h>
#include "app_main.h"

extern void cxx_system_init(void);
extern int cli_reg_cmd_xplayer(void);

int main(int argc, char *argv[])
{
    cxx_system_init();
    board_yoc_init();
    cli_reg_cmd_xplayer();
#ifdef CONFIG_WIFI_DRIVER_BL606P
    aos_loop_run();
#endif
    for (;;) {
        aos_msleep(2000);
    };
}

