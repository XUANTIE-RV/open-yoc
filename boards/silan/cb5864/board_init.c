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
#include <drv/usart.h>
#include <soc.h>
#include <csi_config.h>
#include <csi_core.h>
#include <pinmux.h>
#include "board_config.h"
#include "silan_uart_regs.h"

#define USART2_TX_FUNC       PA4_UART2_TX
#define USART2_RX_FUNC       PA5_UART2_RX

#define __sREG32(addr, offset)    (*(volatile uint32_t*)((addr) + offset))

usart_handle_t console_handle;

void board_init(void)
{
    drv_pinmux_config(UART_TXD2, USART2_TX_FUNC);
    drv_pinmux_config(UART_RXD2, USART2_RX_FUNC);

    /* init the console */
    console_handle = csi_usart_initialize(CONSOLE_IDX, NULL);
    /* config the UART */
    csi_usart_config(console_handle, 115200, USART_MODE_ASYNCHRONOUS, USART_PARITY_NONE, USART_STOP_BITS_1, USART_DATA_BITS_8);
}

int fputc(int ch, FILE *stream)
{
    if (console_handle == NULL) {
        return -1;
    }

    if (ch == '\n') {
        csi_usart_putchar(console_handle, '\r');
    }

    csi_usart_putchar(console_handle, ch);

    return 0;
}

int fgetc(FILE *stream)
{
    uint8_t ch;

    if (console_handle == NULL) {
        return -1;
    }

    csi_usart_getchar(console_handle, &ch);

    return ch;
}

int shell_fputc(int ch)
{
    return fputc(ch, (void *)-1);
}

int shell_fgetc(char *ch)
{
    int sys_uart = CONSOLE_UART;
    if((__sREG32(SILAN_UART_BASE(sys_uart), UART_FR) & UART_FR_RXFE) == 0) {
        *ch = __sREG32(SILAN_UART_BASE(sys_uart), UART_DR) & 0xff;
        return 1;
    }
    else {
        return 0;
    }
}