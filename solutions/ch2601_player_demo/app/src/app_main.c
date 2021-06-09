/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <app_config.h>
#include <aos/aos.h>
#include <aos/cli.h>
#include <aos/kernel.h>
#include "app_main.h"
#include "app_init.h"
#include "board.h"
#include "player_demo.h"
#include "oled.h"

#define TAG "app"

extern void cxx_system_init(void);

int main(void)
{
    // must call this func when use c++
    cxx_system_init();

    board_yoc_init();

    oled_init();

    player_init();

    cli_reg_cmd_player();

    return 0;
}



