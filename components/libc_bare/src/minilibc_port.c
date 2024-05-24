/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <csi_config.h>
#include <drv/uart.h>

extern csi_uart_t g_console_handle;

__attribute__((weak)) int write(int __fd, __const void *__buf, int __n)
{
    return 0;
}

int fputc(int ch, FILE *stream)
{
    if (ch == '\n') {
        csi_uart_putc(&g_console_handle, '\r');
    }

    csi_uart_putc(&g_console_handle, ch);
    return 0;
}

int uart_putc(int ch)
{
    if (ch == '\n') {
        csi_uart_putc(&g_console_handle, '\r');
    }

    csi_uart_putc(&g_console_handle, ch);
    return 0;
}

int fgetc(FILE *stream)
{
    (void)stream;

    return csi_uart_getc(&g_console_handle);
}

