/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */
#include <stdbool.h>
#include <board.h>
#include <board_config.h>
#include <yoc/partition.h>
#include <yoc/init.h>
#include <uservice/uservice.h>
#include <cvi_misc.h>
#include "app_main.h"

#define TAG "init"

static void stduart_init(void)
{
    extern void console_init(int idx, uint32_t baud, uint16_t buf_size);
    console_init(CONSOLE_UART_IDX, CONFIG_CLI_USART_BAUD, CONFIG_CONSOLE_UART_BUFSIZE);
}

void board_yoc_init(void)
{
    int ret;

    board_init();
    stduart_init();

    extern void  cxx_system_init(void);
    cxx_system_init();

    ulog_init();

#ifdef CONFIG_DEBUG
    aos_set_log_level(AOS_LL_DEBUG);
#else
    aos_set_log_level(AOS_LL_WARN);
#endif

    ret = partition_init();
    if (ret <= 0) {
        LOGE(TAG, "partition_init failed(%d).\n", ret);
        aos_assert(false);
        return;
    }
#if defined(CONFIG_OTA_AB) && (CONFIG_OTA_AB > 0)
    extern int bootab_init(void);
    if (bootab_init() < 0) {
        LOGE(TAG, "bootab init failed.");
        aos_assert(false);
    }
#endif
    event_service_init(NULL);
    return;
}
