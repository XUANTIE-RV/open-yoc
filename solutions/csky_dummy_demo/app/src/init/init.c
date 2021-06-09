/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <stdbool.h>
#include <aos/aos.h>
#include <yoc/yoc.h>
#include "board.h"
#include "app_init.h"

const char *TAG = "INIT";

void board_yoc_init()
{
    board_init();
    console_init(CONSOLE_UART_IDX, 115200, 128);
    ulog_init();
    aos_set_log_level(AOS_LL_DEBUG);

    LOGI(TAG, "Build:%s,%s",__DATE__, __TIME__);

    board_cli_init();
}
