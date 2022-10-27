/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     devices.c
 * @brief    source file for the devices
 * @version  V1.0
 * @date     2019-12-18
******************************************************************************/

#include <stdio.h>
#include <csi_config.h>
#include <soc.h>
#include <drv/uart.h>
#include <drv/timer.h>
#include <drv/dma.h>
#include <drv/iic.h>
#include <drv/rtc.h>
#include <drv/spi.h>
#include <drv/adc.h>
#include <drv/gpio.h>
#include <drv/irq.h>
#include <drv/pin.h>

/*
#### csi_perip_info_t

| 成员      | 说明                              |
| ----------| --------------------------------- |
| 第一个成员 | 外设基址 |
| 第二个成员 | 外设对应的中断号 |
| 第三个成员 | 外设设备号，即同类型的外设的第几个，用该成员寻找将要使用的外设的基址和对应外设的中断号 |
| 第四个成员 | 外设设备类型，用该成员寻找将要使用的外设的基址和对应外设的中断号 |

const csi_perip_info_t g_soc_info[] = {
    {///< TODO：外设基址, ///< TODO：外设对应的中断号, ///< TODO：外设设备号, ///< TODO：外设设备类型}
};
*/
const csi_perip_info_t g_soc_info[] = {
    {DW_UART0_BASE,            DW_UART0_IRQn,            0,    DEV_DW_UART_TAG},
    {0, 0, 0, 0}
};

///< TODO：定义DMA的通道的数量
const uint8_t g_dma_chnum[] = {};

/* DMA handshake number */
/* The member of uart_tx_hs_num is the handshake number for ETB */
///< TODO：DMA的硬件握手号
const uint16_t uart_tx_hs_num[1] = {};
const uint16_t uart_rx_hs_num[1] = {};

const csi_dma_ch_desc_t uart0_dma_ch_list[] = {
    {0xff, 0xff}
};

/*
#### csi_dma_ch_spt_list_t

| 成员      | 说明        |
| ----------| ------------|
| 第一个成员 | 外设设备类型 |
| 第二个成员 | 外设设备号 |
| 第三个成员 | 外设对应的csi_dma_ch_desc_t变量 |

const csi_dma_ch_spt_list_t dma_spt_list[] = {
    {///< TODO：外设设备类型, ///< TODO：外设设备号, ///< TODO：外设对应的csi_dma_ch_desc_t变量}
};
*/
const csi_dma_ch_spt_list_t dma_spt_list[] = {
    {0xFFFFU,         0xFFU,         NULL},
};

/*
#### csi_pinmap_t

| 成员      | 说明        |
| ----------| ------------|
| 第一个成员 | GPIO引脚名 |
| 第二个成员 | 引脚号 |
| 第三个成员 | 引脚通道 |
| 第四个成员 | 引脚功能 |

const csi_pinmap_t gpio_pinmap[] = {
    {///< TODO：GPIO引脚名, ///< TODO：引脚号, ///< TODO：引脚通道, ///< TODO：引脚功能 },
};
*/
const csi_pinmap_t gpio_pinmap[] = {
    {0xFFFFFFFFU,   0xFFU, 0xFFU,       0xFFFFFFFFU   },
};

const csi_pinmap_t uart_pinmap[] = {
    {0xFFFFFFFFU, 0xFFU, 0xFFU,      0xFFFFFFFFU  },
};

/*
#### csi_clkmap_t

| 成员      | 说明        |
| ----------| ------------|
| 第一个成员 | 外设 |
| 第二个成员 | 外设类型 |
| 第三个成员 | 外设设备号 |

const csi_clkmap_t clk_map[] = {
    {///< TODO：外设, ///< TODO：外设类型, ///< TODO：外设设备号}
};
*/
const csi_clkmap_t clk_map[] = {
    {0xFFFFFFFFU, 0xFFFFU,          0xFFU}
};

