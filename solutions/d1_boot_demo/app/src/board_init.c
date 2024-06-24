/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     board_init.c
 * @brief    CSI Source File for board init
 * @version  V1.0
 * @date     31. June 2018
 ******************************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <drv/uart.h>
#include <drv/pin.h>
#include <board.h>

static int g_uart_init;
static csi_uart_t g_console_handle;

static void uart_init(void)
{
    /* init the console */
    if (csi_uart_init(&g_console_handle, CONSOLE_UART_IDX)) {

    }

    /* config the UART */
    csi_uart_baud(&g_console_handle, CONFIG_CLI_USART_BAUD);
    csi_uart_format(&g_console_handle, UART_DATA_BITS_8, UART_PARITY_NONE, UART_STOP_BITS_1);
    g_uart_init = 1;
}

void board_yoc_init(void)
{
    board_init();
    uart_init();

    extern int boot_flash_init(void);
    boot_flash_init();
}

int fputc(int ch, FILE *stream)
{
    if (!g_uart_init)
        return -1;

    if (ch == '\n') {
        csi_uart_putc(&g_console_handle, '\r');
    }

    csi_uart_putc(&g_console_handle, ch);
    return 0;
}

int uart_putc(int ch)
{
    if (!g_uart_init)
        return -1;

    if (ch == '\n') {
        csi_uart_putc(&g_console_handle, '\r');
    }

    csi_uart_putc(&g_console_handle, ch);
    return 0;
}