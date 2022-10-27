/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <aos/kernel.h>
#include <aos/debug.h>
#include <devices/hal/uart_impl.h>
#include <devices/apdu_uart.h>

aos_dev_t *g_apdu_handle = NULL;
static uint16_t g_apdu_buf_size = 128;

const char *apdu_get_devname(void)
{
    static char apdu_devname[32] = {0};

    if (g_apdu_handle) {
        snprintf(apdu_devname, sizeof(apdu_devname), "%s%d",
                 ((driver_t *)g_apdu_handle->drv)->name, g_apdu_handle->id);
    }

    return apdu_devname;
}

uint16_t apdu_get_buffer_size(void)
{
    return g_apdu_buf_size;
}

void apdu_init(int idx, uint32_t baud, uint16_t buf_size)
{
    uart_config_t config;
    g_apdu_handle = uart_open_id("uart", idx);

    aos_check(g_apdu_handle, EIO);

    if (g_apdu_handle != NULL) {
        uart_config_default(&config);
        config.baud_rate = baud;
        uart_config(g_apdu_handle, &config);
        uart_set_type(g_apdu_handle, UART_TYPE_CONSOLE);

        g_apdu_buf_size = buf_size;
    }
}

void apdu_deinit()
{
    if (g_apdu_handle) {
        uart_close(g_apdu_handle);
        g_apdu_handle = NULL;
    }
}

int apdu_uart_putc(int ch)
{
//    int data;

    if (g_apdu_handle == NULL) {
        return -1;
    }
#if 0
    if (ch == '\n') {
        //csi_usart_putchar(dev_get_handler(g_apdu_handle), '\r');
        data = '\r';
        uart_send(g_apdu_handle, &data, 1);
    }
#endif
    //csi_usart_putchar(dev_get_handler(g_apdu_handle), ch);
    uart_send(g_apdu_handle, &ch, 1);

    return 0;
}

int apdu_uart_getc(void)
{
    if (g_apdu_handle != NULL) {
        char ch = 0;
        uart_recv(g_apdu_handle, &ch, 1, -1);

        return (int)ch;
    }

    return 0;
}


