/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include "soc.h"
#include "pin_name.h"
#include "pinmux.h"
#include "drv_usart.h"

#define CONSOLE_TXD                     PA19
#define CONSOLE_RXD                     PA20
#define CONSOLE_TXD_FUNC                PA19_UART1_TX
#define CONSOLE_RXD_FUNC                PA20_UART1_RX
#define CONSOLE_IDX                     1

static usart_handle_t console_handle = NULL;
extern int32_t dw_usart_putchar(usart_handle_t handle, uint8_t ch);

int putchar(int ch)
{
    if (ch == '\n') {
        csi_usart_putchar(console_handle, '\r');
    }

    csi_usart_putchar(console_handle, ch);

    return 0;
}

void usart_event_cb(usart_event_e event, void *cb_arg)
{
    //do nothing
}

void console_init()
{
    drv_pinmux_config(CONSOLE_TXD, CONSOLE_TXD_FUNC);
    drv_pinmux_config(CONSOLE_RXD, CONSOLE_RXD_FUNC);

    console_handle = csi_usart_initialize(CONSOLE_IDX, NULL);

    csi_usart_config(console_handle, 115200, USART_MODE_ASYNCHRONOUS, USART_PARITY_NONE, USART_STOP_BITS_1, USART_DATA_BITS_8);
}
