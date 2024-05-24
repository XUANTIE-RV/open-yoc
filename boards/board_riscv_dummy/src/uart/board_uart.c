/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <stdio.h>
#include <board.h>
#if CONFIG_DEVICES_RVM_HAL
#include <devices/devicelist.h>
#else
#include <drv/uart.h>
#endif

#if CONFIG_DEVICES_RVM_HAL
void board_uart_init(void)
{
    rvm_uart_drv_register(0);
}
#else
__attribute__((weak)) csi_uart_t g_console_handle;

void board_uart_init(void)
{
    /* init the console */
    csi_uart_init(&g_console_handle, CONSOLE_UART_IDX);

    /* config the UART */
    csi_uart_baud(&g_console_handle, CONFIG_CLI_USART_BAUD);
    csi_uart_format(&g_console_handle, UART_DATA_BITS_8, UART_PARITY_NONE, UART_STOP_BITS_1);
}
#endif
