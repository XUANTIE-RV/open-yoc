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
#include <soc.h>
#include <csi_core.h>
#include <pinmux.h>
#include <drv/usart.h>
#include <drv/spiflash.h>
#include "board.h"

static usart_handle_t g_console_handle = NULL;

static void board_pinmux_config(void)
{
    //console
    drv_pinmux_config(SC5654_USART2_TX, SC5654_USART2_TX_FUNC);
    drv_pinmux_config(SC5654_USART2_RX, SC5654_USART2_RX_FUNC);
}

void board_yoc_init(void)
{
    board_pinmux_config();

    /* init the console */
    g_console_handle = csi_usart_initialize(CONSOLE_IDX, NULL);
    /* config the UART */
    csi_usart_config(g_console_handle, 115200, USART_MODE_ASYNCHRONOUS, USART_PARITY_NONE, USART_STOP_BITS_1, USART_DATA_BITS_8);

    extern int boot_flash_init(void);
    boot_flash_init();
}

int fputc(int ch, FILE *stream)
{
    if (g_console_handle == NULL) {
        return -1;
    }

    if (ch == '\n') {
        csi_usart_putchar(g_console_handle, '\r');
    }

    csi_usart_putchar(g_console_handle, ch);

    return 0;
}