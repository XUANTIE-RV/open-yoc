/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     wj_usi_ll.c
 * @brief    CSI Source File for USI Driver
 * @version  V1.0
 * @date     19. Feb 2020
 ******************************************************************************/

#include <drv/uart.h>
#include <drv/irq.h>
#include <wj_usi_ll.h>
uint32_t  wj_usi_uart_wait_timeout(ck_usi_regs_t *uart_base)
{
    return 0U;
}

int32_t wj_usi_uart_config_baudrate(ck_usi_regs_t *uart_base, uint8_t index, uint32_t baud, uint32_t uart_freq)
{
    /* baudrate=(seriak clock freq)/(16* (divisor + 1); algorithm :rounding*/
    uint32_t freq = soc_get_usi_freq((uint32_t)((uint32_t)index>>1U)); 
    uint32_t divisor = (freq / (16U * baud)) - 1U;

    uart_base->USI_CLK_DIV0 = divisor;

    return 0;
}

int32_t  wj_usi_uart_config_stop_bits(ck_usi_regs_t *uart_base, uint32_t stop_bits)
{
    int32_t ret = 0;;
    switch (stop_bits) {
        case UART_STOP_BITS_1:
            uart_base->USI_UART_CTRL |= WJ_USI_UART_CTRL_PBIT_1;
            break;

        case UART_STOP_BITS_2:
            uart_base->USI_UART_CTRL |= WJ_USI_UART_CTRL_PBIT_2;
            break;

        case UART_STOP_BITS_1_5:
            uart_base->USI_UART_CTRL |= WJ_USI_UART_CTRL_PBIT_1P5;
            break;

        default:
            ret = -1;
    }

    return ret;
}

int32_t wj_usi_uart_config_parity_none(ck_usi_regs_t *uart_base)
{
    uart_base->USI_UART_CTRL &= (~WJ_USI_UART_CTRL_PEN_EN);
    return 0;
}

int32_t wj_usi_uart_config_parity_odd(ck_usi_regs_t *uart_base)
{
    uart_base->USI_UART_CTRL |= WJ_USI_UART_CTRL_PEN_EN;
    uart_base->USI_UART_CTRL |= WJ_USI_UART_CTRL_EPS_EN;
    return 0;
}

int32_t wj_usi_uart_config_parity_even(ck_usi_regs_t *uart_base)
{
    uart_base->USI_UART_CTRL |= WJ_USI_UART_CTRL_PEN_EN;
    uart_base->USI_UART_CTRL &= (~WJ_USI_UART_CTRL_EPS_EN);
    return 0;
}

int32_t wj_usi_uart_config_data_bits(ck_usi_regs_t *uart_base, uint32_t data_bits)
{
    switch (data_bits) {
        case 5:
            wj_usi_set_uart_ctrl_dbit(uart_base, WJ_USI_UART_CTRL_DBIT_5);
            break;

        case 6:
            wj_usi_set_uart_ctrl_dbit(uart_base, WJ_USI_UART_CTRL_DBIT_6);
            break;

        case 7:
            wj_usi_set_uart_ctrl_dbit(uart_base, WJ_USI_UART_CTRL_DBIT_7);
            break;

        case 8:
            wj_usi_set_uart_ctrl_dbit(uart_base, WJ_USI_UART_CTRL_DBIT_8);
            break;
    }

    return 0;

}

uint32_t wj_usi_get_recvfifo_waiting_num(ck_usi_regs_t *addr)
{
    return ((addr->USI_FIFO_STA & WJ_USI_FIFO_STA_RX_NUM_Msk) >> WJ_USI_FIFO_STA_RX_NUM_Pos);
}
void wj_usi_rst_regs(ck_usi_regs_t *addr)
{
    addr->USI_MODE_SEL      = 0; 
    addr->USI_SPI_MODE      = 0;      
    addr->USI_SPI_CTRL      = 0;      
    addr->USI_INTR_CTRL     = 0;     
    addr->USI_INTR_EN       = 0;       
    addr->USI_INTR_UNMASK   = 0;   
    addr->USI_INTR_CLR      = 0xFFFFU;
    addr->USI_DMA_CTRL      = 0;      
    addr->USI_DMA_THRESHOLD = 0; 
    addr->USI_CTRL          = 0;          
}
