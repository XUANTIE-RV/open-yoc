/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */


/******************************************************************************
 * @file     devices.c
 * @brief    source file for the devices
 * @version  V1.0
 * @date     26. Dec 2019
 ******************************************************************************/

#include <stdio.h>
#include <csi_config.h>
#include <soc.h>
#include <drv/uart.h>
#include <drv/timer.h>
#include <drv/rtc.h>
#include <drv/crc.h>
#include <drv/aes.h>
#include <drv/rsa.h>
#include <drv/eflash.h>
#include <drv/spi.h>
#include <drv/adc.h>
#include <drv/gpio.h>
#include <drv/irq.h>
#include <drv/pin.h>


const csi_perip_info_t g_soc_info[] = {
    {DW_UART0_BASE, DW_UART0_HS_IRQn, 0, DEV_DW_UART_TAG},
    {DW_UART1_BASE, DW_UART1_HS_IRQn, 1, DEV_DW_UART_TAG},
    {DW_UART2_BASE, DW_UART2_IRQn, 2, DEV_DW_UART_TAG},
    {DW_UART3_BASE, DW_UART3_IRQn, 3, DEV_DW_UART_TAG},
    {WJ_USI0_BASE, WJ_USI0_IRQn, 0, DEV_WJ_USI_TAG},
    {WJ_USI1_BASE, WJ_USI1_IRQn, 1, DEV_WJ_USI_TAG},
    {WJ_USI2_BASE, WJ_USI2_IRQn, 2, DEV_WJ_USI_TAG},
    {WJ_USI3_BASE, WJ_USI3_IRQn, 3, DEV_WJ_USI_TAG},
    {WJ_MCC1_BASE,  WJ_WCOM_IRQn,     0, DEV_WJ_MBOX_TAG},
    {DW_TIM0_BASE,  DW_TIMER0_INT0_IRQn, 0, DEV_DW_TIMER_TAG},
    {DW_TIM1_BASE,  DW_TIMER0_INT1_IRQn, 1, DEV_DW_TIMER_TAG},
    {WJ_LPTIM6_BASE,  WJ_LPTIMER_INT0_IRQn, 2, DEV_DW_TIMER_TAG},
    {WJ_LPTIM7_BASE,  WJ_LPTIMER_INT1_IRQn, 3, DEV_DW_TIMER_TAG},
    {DW_TIM2_BASE,  DW_TIMER1_INT0_IRQn, 4, DEV_DW_TIMER_TAG},
    {DW_TIM3_BASE,  DW_TIMER1_INT1_IRQn, 5, DEV_DW_TIMER_TAG},
    {DW_TIM4_BASE,  DW_TIMER2_INT0_IRQn, 6, DEV_DW_TIMER_TAG},
    {DW_TIM5_BASE,  DW_TIMER2_INT1_IRQn, 7, DEV_DW_TIMER_TAG},
    {0, 0, 0, 0}
};

const uint8_t g_dma_chnum[] = {4, 8};

/* DMA handshake number */
/* The member of uart_tx_hs_num is the handshake number for ETB */
const uint16_t uart_tx_hs_num[2] = {8, 10};
const uint16_t uart_rx_hs_num[2] = {9, 11};
const uint16_t usi_tx_hs_num[3]  = {4, 6, 4};
const uint16_t usi_rx_hs_num[3]  = {5, 7, 5};
const uint16_t i2s_tx_hs_num[3]  = {0, 2, 0};
const uint16_t i2s_rx_hs_num[3]  = {1, 3, 2};

const csi_dma_ch_desc_t uart0_dma_ch_list[] = {
    {0, 2}, {0, 3},
    {0xff, 0xff}
};
const csi_dma_ch_desc_t uart1_dma_ch_list[] = {
    {0, 2}, {0, 3},
    {0xff, 0xff}
};
const csi_dma_ch_desc_t uart2_dma_ch_list[] = {
    {0xff, 0xff}
};

const csi_dma_ch_desc_t usi0_dma_ch_list[] = {
    {0, 0}, {0, 1}, {0, 2}, {0, 3},
    {0xff, 0xff}
};
const csi_dma_ch_desc_t usi1_dma_ch_list[] = {
    {0, 2}, {0, 3},
    {0xff, 0xff}
};
const csi_dma_ch_desc_t usi2_dma_ch_list[] = {
    {0xff, 0xff}
};
const csi_dma_ch_desc_t i2s0_dma_ch_list[] = {
    {0, 2}, {0, 3},
    {0xff, 0xff}
};
const csi_dma_ch_desc_t i2s1_dma_ch_list[] = {
    {0, 2}, {0, 3},
    {0xff, 0xff}
};
const csi_dma_ch_desc_t i2s2_dma_ch_list[] = {
    {0xff, 0xff}
};

const csi_dma_ch_spt_list_t dma_spt_list[] = {
    {DEV_DW_UART_TAG, 0, uart0_dma_ch_list},
    {DEV_DW_UART_TAG, 1, uart1_dma_ch_list},
    {DEV_DW_UART_TAG, 2, uart2_dma_ch_list},
    {DEV_WJ_USI_TAG,  0, usi0_dma_ch_list},
    {DEV_WJ_USI_TAG,  1, usi1_dma_ch_list},
    {DEV_WJ_USI_TAG,  2, usi2_dma_ch_list},
    {DEV_WJ_I2S_TAG,  0, i2s0_dma_ch_list},
    {DEV_WJ_I2S_TAG,  0, i2s0_dma_ch_list},
    {DEV_WJ_I2S_TAG,  1, i2s1_dma_ch_list},
    {DEV_WJ_I2S_TAG,  2, i2s2_dma_ch_list},
    {0xFFFFU,         0xFFU,         NULL},
};



const csi_pinmap_t adc_pinmap[] = {

    {0xFFFFFFFFU,   0xFFU, 0xFFU,       0xFFFFFFFFU },
};

const csi_pinmap_t pwm_pinmap[] = {

    {0xFFFFFFFFU,   0xFFU, 0xFFU,       0xFFFFFFFFU },
};

const csi_pinmap_t gpio_pinmap[] = {

    {0xFFFFFFFFU,   0xFFU, 0xFFU,       0xFFFFFFFFU   },
};


const csi_pinmap_t uart_pinmap[] = {

    {0xFFFFFFFFU, 0xFFU, 0xFFU,      0xFFFFFFFFU  },
};

const csi_pinmap_t iic_pinmap[] = {

    {0xFFFFFFFFU, 0xFFU,       0xFFU,   0xFFFFFFFFU},
};


const csi_pinmap_t spi_pinmap[] = {

    {0xFFFFFFFFU, 0xFFU,        0xFFU,    0xFFFFFFFFU},
};


const csi_pinmap_t i2s_pinmap[] = {

    {0xFFFFFFFFU, 0xFFU, 0xFFU, 0xFFFFFFFFU   },
};

const csi_clkmap_t clk_map[] = {
    {0xFFFFFFFFU, 0xFFFFU,          0xFFU}
};
