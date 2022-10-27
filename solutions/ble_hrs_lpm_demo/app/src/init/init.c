/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */

#include <stdbool.h>
#include <aos/aos.h>
#include <aos/kv.h>
#include <yoc/yoc.h>
#include <yoc/partition.h>
#include <devices/devicelist.h>
#include <board_config.h>

#include "pin_name.h"
#include "app_init.h"

const char *TAG = "INIT";

#ifndef CONSOLE_IDX
#define CONSOLE_IDX 0
#endif

extern void csi_pinmux_prepare_sleep_action();
extern void csi_gpio_prepare_sleep_action();
extern void csi_pinmux_wakeup_sleep_action();
extern void csi_gpio_wakeup_sleep_action();
extern void csi_usart_prepare_sleep_action();
extern void csi_usart_wakeup_sleep_action();

__attribute__((section(".__sram.code"))) int pm_prepare_sleep_action()
{
    csi_pinmux_prepare_sleep_action();
    csi_gpio_prepare_sleep_action();
    csi_usart_prepare_sleep_action();
    return 0;
}

__attribute__((section(".__sram.code"))) int pm_after_sleep_action()
{
    csi_pinmux_wakeup_sleep_action();
    csi_gpio_wakeup_sleep_action();
    csi_usart_wakeup_sleep_action();
    return 0;
}

void board_yoc_init()
{
    int ret;

    board_init();

    console_init(CONSOLE_UART_IDX, 115200, 128);

    ulog_init();
    aos_set_log_level(AOS_LL_DEBUG);

    /* load partition */
    ret = partition_init();

    if (ret <= 0) {
        LOGE(TAG, "partition init failed");
    } else {
        LOGI(TAG, "find %d partitions", ret);
    }

    /* kvfs init */
    aos_kv_init("kv");

#if defined(AOS_COMP_CLI) || defined(USE_CLI)
    board_cli_init();
#endif
    LOGI(TAG, "Build:%s,%s", __DATE__, __TIME__);
}
