/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <stdio.h>

#include <board.h>
#include <drv/gpio.h>
#include <devices/devicelist.h>
#include <devices/uart.h>
#include <pinmux.h>

#define BT_RESET_PIN         PB11
#define HCI_UART_TXD         PB6
#define HCI_UART_RXD         PB7
#define HCI_UART_TXD_FUNC    PB6_UART1_TX
#define HCI_UART_RXD_FUNC    PB7_UART1_RX
#define CONFIG_HCI_UART_IDX  1
#define CONFIG_HCI_UART_BAUD 1000000

static gpio_pin_handle_t bt_reset_pin = NULL;

static void bt_reset_pin_init()
{
    drv_pinmux_config(BT_RESET_PIN, PIN_FUNC_GPIO);

    bt_reset_pin = csi_gpio_pin_initialize(BT_RESET_PIN, NULL);

    csi_gpio_pin_config(bt_reset_pin, GPIO_MODE_PULLUP, GPIO_DIRECTION_OUTPUT);

    csi_gpio_pin_write(bt_reset_pin, 0);

    extern void mdelay(uint32_t ms);
    mdelay(10);

    csi_gpio_pin_write(bt_reset_pin, 1);
}

void board_bt_init(void)
{
    bt_reset_pin_init();

    drv_pinmux_config(HCI_UART_TXD, HCI_UART_TXD_FUNC);
    drv_pinmux_config(HCI_UART_RXD, HCI_UART_RXD_FUNC);

    rvm_uart_drv_register(CONFIG_HCI_UART_IDX);

    rvm_hal_uart_config_t uart_config;

    rvm_hal_uart_config_default(&uart_config);
    uart_config.baud_rate = CONFIG_HCI_UART_BAUD;

#if defined(CONFIG_BT) && CONFIG_BT
    extern void bt_hci_uart_h5_register(int uart_idx, rvm_hal_uart_config_t config);
    bt_hci_uart_h5_register(CONFIG_HCI_UART_IDX, uart_config);

    extern int hci_h5_driver_init();
    hci_h5_driver_init();
#endif
}
