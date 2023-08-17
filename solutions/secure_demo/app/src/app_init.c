/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#include <stdbool.h>
#include <aos/kv.h>
#include <uservice/uservice.h>
#include <yoc/init.h>
#include <ulog/ulog.h>
#include "board.h"
#include "app_main.h"
#include <aos/aos.h>
#include <yoc/yoc.h>
#include <devices/devicelist.h>
#include <drv/spiflash.h>



#define CONSOLE_IDX 0
#define TAG "init"

static void stduart_init(void)
{
    extern void console_init(int idx, uint32_t baud, uint16_t buf_size);
    console_init(CONSOLE_UART_IDX, CONFIG_CLI_USART_BAUD, CONFIG_CONSOLE_UART_BUFSIZE);
}

void board_yoc_init(void)
{
    board_init();
    stduart_init();
    printf("\n###Welcome to YoC###\n[%s,%s]\n", __DATE__, __TIME__);
    event_service_init(NULL);

    ulog_init();

    board_cli_init();
}