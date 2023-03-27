/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <stdio.h>

#include <board.h>
#include <devices/devicelist.h>
#include <pinmux.h>

void board_uart_init(void)
{
    // uart0 pinmux
    drv_pinmux_config(BOARD_UART0_TX_PIN, PB19_UART0_TX);
    drv_pinmux_config(BOARD_UART0_RX_PIN, PB20_UART0_RX);
    rvm_uart_drv_register(BOARD_UART0_IDX);

    // uart4 pinmux
    drv_pinmux_config(BOARD_UART1_TX_PIN, PB4_UART4_TX);
    drv_pinmux_config(BOARD_UART1_RX_PIN, PB5_UART4_RX);
    rvm_uart_drv_register(BOARD_UART1_IDX);
}
