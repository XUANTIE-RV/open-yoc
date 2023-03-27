
/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
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

#if defined(CONFIG_TB_KP) && CONFIG_TB_KP
static int _app_init_mac_address(void)
{
    uint8_t    addr[6];
    key_handle key_addr;
    uint32_t   len;

    int ret = km_get_key(KEY_ID_MAC, &key_addr, &len);

    if (ret != KM_OK) {
        LOGE(TAG, "Config mac address from KP fail, ret %d", ret);
        return -1;
    }

    memcpy(addr, (uint8_t *)key_addr, 6);
    extern int dut_hal_mac_store(uint8_t addr[6]);
    return dut_hal_mac_store(addr);
}
#endif

void board_yoc_init(void)
{
    board_init();
	stduart_init();


#if defined(BOARD_PWM_NUM) && BOARD_PWM_NUM > 0
    board_pwm_init();
#endif

#if defined(BOARD_BUTTON_NUM) && BOARD_BUTTON_NUM > 0
    board_button_init();
#endif

#if defined(BOARD_LED_NUM) && BOARD_LED_NUM > 0
    board_led_init();
#endif

#if defined(BOARD_WIFI_SUPPORT) && BOARD_WIFI_SUPPORT > 0
    board_wifi_init();
#endif

#if defined(BOARD_BT_SUPPORT) && BOARD_BT_SUPPORT > 0
    board_bt_init();
#endif

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

#if defined(CONFIG_TB_KP) && CONFIG_TB_KP
    ret = km_init();
    if (ret) {
        LOGE(TAG, "KP init fail, pls check kp file!");
    }
    _app_init_mac_address();
#endif

    board_cli_init();
}