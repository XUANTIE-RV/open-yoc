/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#include <board.h>

#include <stdbool.h>
#include <uservice/uservice.h>
#include <aos/cli.h>
#include <aos/console_uart.h>
#include <drv/uart.h>
#include <drv_amp.h>

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

#if defined(BOARD_WIFI_SUPPORT) && BOARD_WIFI_SUPPORT > 0
    board_wifi_init();
#endif

#if defined(BOARD_BT_SUPPORT) && BOARD_BT_SUPPORT > 0
    board_bt_init();
#endif

#if defined(BOARD_AUDIO_SUPPORT) && BOARD_AUDIO_SUPPORT > 0
    board_audio_init();
#endif

    event_service_init(NULL);

    ulog_init();
    aos_set_log_level(AOS_LL_DEBUG);

#ifdef AOS_COMP_DEBUG
    aos_debug_init();
#endif

#if defined(CONFIG_BOARD_VENDOR) && CONFIG_BOARD_VENDOR > 0
    board_vendor_init();
#endif
}
