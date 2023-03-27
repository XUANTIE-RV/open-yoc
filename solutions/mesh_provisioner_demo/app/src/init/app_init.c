
/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */
#include <stdbool.h>
#include <aos/aos.h>
#include <aos/kv.h>
#include <board.h>
#include <key_mgr.h>
#include <yoc/yoc.h>
#include <yoc/partition.h>
#include <debug/dbg.h>

#include "app_init.h"

const char *TAG = "INIT";

#ifndef CONSOLE_UART_IDX
#define CONSOLE_UART_IDX 0
#endif

static void stduart_init(void)
{
    console_init(CONSOLE_UART_IDX, CONFIG_CLI_USART_BAUD, CONFIG_CONSOLE_UART_BUFSIZE);
}

void board_yoc_init(void)
{
    board_init();

#if defined(BOARD_BT_SUPPORT) && BOARD_BT_SUPPORT > 0
    board_bt_init();
#endif

    stduart_init();

    ulog_init();
    aos_set_log_level(AOS_LL_DEBUG);

#ifdef AOS_COMP_DEBUG
    aos_debug_init();
#endif

    /* load partition */
    int ret = partition_init();
    if (ret <= 0) {
        LOGE(TAG, "partition init failed");
    } else {
        LOGI(TAG, "find %d partitions", ret);
    }

    aos_kv_init("kv");

    board_cli_init();
}
