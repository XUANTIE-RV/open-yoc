/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <stdio.h>

#include <board.h>

void board_uart_init(void)
{
    drv_pinmux_config(BOARD_UART0_TX_PIN, FMUX_UART0_TX);
    drv_pinmux_config(BOARD_UART0_RX_PIN, FMUX_UART0_RX);
}
