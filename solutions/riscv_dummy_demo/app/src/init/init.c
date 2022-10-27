/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <stdbool.h>
#include <aos/aos.h>
#include <yoc/yoc.h>
#include "board.h"
#include "app_init.h"

const char *TAG = "INIT";

static void stduart_init(void)
{
    extern void console_init(int idx, uint32_t baud, uint16_t buf_size);
    console_init(CONSOLE_UART_IDX, CONFIG_CLI_USART_BAUD, CONFIG_CONSOLE_UART_BUFSIZE);
}

void board_yoc_init()
{
    board_init();
    stduart_init();
    printf("\n###Welcom to YoC###\n[%s,%s]\n", __DATE__, __TIME__);
    ulog_init();
    aos_set_log_level(AOS_LL_DEBUG);
    LOGI(TAG, "Build:%s,%s",__DATE__, __TIME__);
    board_cli_init();
}
