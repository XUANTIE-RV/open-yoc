/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <stdio.h>
#include <pin.h>
#include <pinmux.h>
#include <drv/gpio.h>
#include "wm_psram.h"
#include "hci_hal_h4.h"
#include <stdbool.h>
#include <aos/aos.h>
#include <devices/devicelist.h>
#include <devices/uart.h>
#include <k_api.h>

extern void board_base_init(void);

gpio_pin_handle_t bt_reset_pin = NULL;
extern size_t __psram_data_start__;
extern size_t __psram_data_end__;
extern k_mm_head *g_kmm_head;
k_mm_region_t mm_region[] = {
    {(uint8_t *)&__psram_data_end__, (size_t)1024*6000},
};

void board_mm_init()
{
    memset((void *)&__psram_data_start__, 0, (uint32_t)&__psram_data_end__ - (uint32_t)&__psram_data_start__);
    krhino_add_mm_region(g_kmm_head, mm_region[0].start, mm_region[0].len);
}

static void board_pinmux_config(void)
{
    drv_pinmux_config(CONSOLE_TXD, CONSOLE_TXD_FUNC);
    drv_pinmux_config(CONSOLE_RXD, CONSOLE_RXD_FUNC);

    drv_pinmux_config(AT_UART_TXD, AT_UART_TXD_FUNC);
    drv_pinmux_config(AT_UART_RXD, AT_UART_RXD_FUNC);

    drv_pinmux_config(HCI_UART_TXD, HCI_UART_TXD_FUNC);
    drv_pinmux_config(HCI_UART_RXD, HCI_UART_RXD_FUNC);

    drv_pinmux_config(BT_RESET_PIN, PIN_FUNC_GPIO);

    drv_pinmux_config(PSRAM_CK_IO, PSRAM_CK_FUNC);
	drv_pinmux_config(PSRAM_CS_IO, PSRAM_CS_FUNC);
	drv_pinmux_config(PSRAM_DAT0_IO, PSRAM_DAT0_FUNC);
	drv_pinmux_config(PSRAM_DAT1_IO, PSRAM_DAT1_FUNC);
#ifdef SUPPORT_4BIT_PSRAM    
	drv_pinmux_config(PSRAM_DAT2_IO, PSRAM_DAT2_FUNC);
	drv_pinmux_config(PSRAM_DAT3_IO, PSRAM_DAT3_FUNC);
#endif    
}

static void bt_reset_pin_init()
{
    bt_reset_pin = csi_gpio_pin_initialize(BT_RESET_PIN, NULL);

    csi_gpio_pin_config(bt_reset_pin, GPIO_MODE_PULLUP, GPIO_DIRECTION_OUTPUT);

    csi_gpio_pin_write(bt_reset_pin, 0);
}

void board_bt_reset()
{
    csi_gpio_pin_write(bt_reset_pin, 0);

    extern void mdelay(uint32_t ms);
    mdelay(10);

    csi_gpio_pin_write(bt_reset_pin, 1);
}

void board_init(void)
{
    board_pinmux_config();

    bt_reset_pin_init();
    
    psram_init(0);

    board_mm_init();

    board_base_init();

    board_bt_reset();

    uart_config_t uart_config;

    uart_config_default(&uart_config);
    uart_config.baud_rate = 1000000;

    extern void bt_hci_uart_h5_register(int uart_idx, uart_config_t config);
    bt_hci_uart_h5_register(CONFIG_HCI_UART_IDX, uart_config);

    extern int hci_h5_driver_init();
    hci_h5_driver_init();
}

