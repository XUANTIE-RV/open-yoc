/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */
#include <board.h>

#include <stdbool.h>
#include <uservice/uservice.h>
#include <aos/cli.h>
#include <aos/console_uart.h>
#include <drv/uart.h>
#ifdef AOS_COMP_DEBUG
#include <debug/dbg.h>
#endif

#define TAG "init"

static void stduart_init(void)
{
    console_init(CONSOLE_UART_IDX, CONFIG_CLI_USART_BAUD, CONFIG_CONSOLE_UART_BUFSIZE);
}

void board_yoc_init(void)
{
    board_init();

    stduart_init();
    aos_cli_init();

    event_service_init(NULL);
    ulog_init();
    aos_set_log_level(AOS_LL_DEBUG);

#if defined(CONFIG_BOARD_DISPLAY) && CONFIG_BOARD_DISPLAY
    board_display_init();
#endif

#ifdef AOS_COMP_DEBUG
    aos_debug_init();
#endif

#if defined(CONFIG_BOARD_VENDOR) && CONFIG_BOARD_VENDOR > 0
    board_vendor_init();
#endif

}
