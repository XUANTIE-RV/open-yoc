/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdbool.h>
#include <aos/aos.h>
#include <yoc/yoc.h>
#include <devices/devicelist.h>

#include "board.h"
#include "app_init.h"
#include "drv/pin.h"

const char *TAG = "INIT";

#ifndef CONSOLE_UART_IDX
#define CONSOLE_UART_IDX 0
#endif

void board_yoc_init()
{
    board_init();
    console_init(CONSOLE_UART_IDX, 115200, 128);

    ulog_init();
    aos_set_log_level(AOS_LL_DEBUG);
    
    LOGI(TAG, "Build:%s,%s",__DATE__, __TIME__);
}
