/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

/******************************************************************************
* @file     dw_dmac_mux.c
* @brief    CSI Source File for DMAC MUX Driver
* @version  V1.0
* @date     16. September 2010
******************************************************************************/
#include <stdbool.h>
#include <string.h>
#include "dw_dmac_mux.h"

typedef enum {
    DMA_FIXED_FUNC,
    DMA_VARIABLE_FUNC,
} dw_dma_mux_ch_type_e;

typedef enum {
    DMA_MUX_CH_IDLE,
    DMA_MUX_CH_BUSY,
} dw_dma_mux_ch_sta_e;

typedef struct {
    int32_t channle_idx;
    int32_t func;
    dw_dma_mux_ch_type_e type;
    dw_dma_mux_ch_sta_e sta;
} dw_dma_mux_info;

typedef enum {
    DMA0_I2S0_TX_REQ,
    DMA0_I2S0_RX_REQ,
    DMA0_I2S1_TX_REQ,
    DMA0_I2S1_RX_REQ,
    DMA0_USI0_TX_REQ,
    DMA0_USI0_RX_REQ,
    DMA0_USI1_TX_REQ,
    DMA0_USI1_RX_REQ,
    DMA0_UART0_TX_REQ,
    DMA0_UART0_RX_REQ,
    DMA0_UART1_TX_REQ,
    DMA0_UART1_RX_REQ,
    DMA0_REQ_NUM,
} DMA0_CHANNEL_MUX;

typedef enum {
    //fixed
    DMA1_CODEC_REQ_0,   //0
    DMA1_CODEC_REQ_1,
    DMA1_CODEC_REQ_4,
    DMA1_CODEC_REQ_5,
    DMA1_UART2_TX_REQ,
    DMA1_UART2_RX_REQ,  //5
    DMA1_USI2_TX_REQ,
    DMA1_USI2_RX_REQ,   //7
    //variable
    DMA1_I2S2_TX_REQ,   //8
    DMA1_I2S2_RX_REQ,
    DMA1_CODEC_REQ_6,   //10
    DMA1_CODEC_REQ_7,   //11
    DMA1_CODEC_REQ_8,   //12
    DMA1_RESERVED,
    DMA1_I2S3_TX_REQ,
    DMA1_I2S3_RX_REQ,   //15
    DMA1_USI3_TX_REQ,
    DMA1_USI3_RX_REQ,
    DMA1_UART3_TX_REQ,
    DMA1_UART3_RX_REQ,
    DMA1_RESERVED1,     //20
    DMA1_RESERVED2,
    DMA1_SPDIF_TX_REQ,
    DMA1_SPDIF_RX_REQ,
    DMA1_RESERVED3,
    DMA1_RESERVED4,     //25
    DMA1_TDM_REQ_0,
    DMA1_TDM_REQ_1,
    DMA1_TDM_REQ_2,
    DMA1_TDM_REQ_3,
    DMA1_TDM_REQ_4,     //30
    DMA1_TDM_REQ_5,
    DMA1_TDM_REQ_6,
    DMA1_TDM_REQ_7,
    DMA1_PDM_REQ_0,
    DMA1_PDM_REQ_1,     //35
    DMA1_PDM_REQ_2,
    DMA1_PDM_REQ_3,
    DMA1_CODEC_REQ_2,   //38
    DMA1_CODEC_REQ_3,   //39
} dw_dma1_mux_e;

static dw_dma_mux_info dma0_channel_mux[DMA0_REQ_NUM] = {
    {0, DMA0_I2S0_TX_REQ, DMA_FIXED_FUNC, DMA_MUX_CH_IDLE},
    {1, DMA0_I2S0_RX_REQ, DMA_FIXED_FUNC, DMA_MUX_CH_IDLE},
    {2, DMA0_I2S1_TX_REQ, DMA_FIXED_FUNC, DMA_MUX_CH_IDLE},
    {3, DMA0_I2S1_RX_REQ, DMA_FIXED_FUNC, DMA_MUX_CH_IDLE},
    {4, DMA0_USI0_TX_REQ, DMA_FIXED_FUNC, DMA_MUX_CH_IDLE},
    {5, DMA0_USI0_RX_REQ, DMA_FIXED_FUNC, DMA_MUX_CH_IDLE},
    {6, DMA0_USI1_TX_REQ, DMA_FIXED_FUNC, DMA_MUX_CH_IDLE},
    {7, DMA0_USI1_RX_REQ, DMA_FIXED_FUNC, DMA_MUX_CH_IDLE},
    {8, DMA0_UART0_TX_REQ, DMA_FIXED_FUNC, DMA_MUX_CH_IDLE},
    {9, DMA0_UART0_RX_REQ, DMA_FIXED_FUNC, DMA_MUX_CH_IDLE},
    {10, DMA0_UART1_TX_REQ, DMA_FIXED_FUNC, DMA_MUX_CH_IDLE},
    {11, DMA0_UART1_RX_REQ, DMA_FIXED_FUNC, DMA_MUX_CH_IDLE},
};

static dw_dma_mux_info dma1_channel_mux[16] = {
    {0, DMA1_CODEC_REQ_0, DMA_FIXED_FUNC, DMA_MUX_CH_IDLE},
    {1, DMA1_CODEC_REQ_1, DMA_FIXED_FUNC, DMA_MUX_CH_IDLE},
    {2, DMA1_CODEC_REQ_4, DMA_FIXED_FUNC, DMA_MUX_CH_IDLE},
    {3, DMA1_CODEC_REQ_5, DMA_FIXED_FUNC, DMA_MUX_CH_IDLE},
    {4, DMA1_UART2_TX_REQ, DMA_FIXED_FUNC, DMA_MUX_CH_IDLE},
    {5, DMA1_UART2_RX_REQ, DMA_FIXED_FUNC, DMA_MUX_CH_IDLE},
    {6, DMA1_USI2_TX_REQ, DMA_FIXED_FUNC, DMA_MUX_CH_IDLE},
    {7, DMA1_USI2_RX_REQ, DMA_FIXED_FUNC, DMA_MUX_CH_IDLE},
    {8, DMA1_I2S2_TX_REQ, DMA_VARIABLE_FUNC, DMA_MUX_CH_IDLE},
    {9, DMA1_I2S2_RX_REQ, DMA_VARIABLE_FUNC, DMA_MUX_CH_IDLE},
    {10, DMA1_I2S3_TX_REQ, DMA_VARIABLE_FUNC, DMA_MUX_CH_IDLE},
    {11, DMA1_I2S3_RX_REQ, DMA_VARIABLE_FUNC, DMA_MUX_CH_IDLE},
    {12, DMA1_USI3_TX_REQ, DMA_VARIABLE_FUNC, DMA_MUX_CH_IDLE},
    {13, DMA1_USI3_RX_REQ, DMA_VARIABLE_FUNC, DMA_MUX_CH_IDLE},
    {14, DMA1_UART3_TX_REQ, DMA_VARIABLE_FUNC, DMA_MUX_CH_IDLE},
    {15, DMA1_UART3_RX_REQ, DMA_VARIABLE_FUNC, DMA_MUX_CH_IDLE},
};

static dw_dma_mux_info *dma_find_mux_channel(dw_dma_mux_info *buf, int buf_len, int32_t func)
{
    int i = 0;

    for (i = 0; i < buf_len; i++) {
        if (buf[i].func == func) {
            return &buf[i];
        }
    }

    return NULL;
}

/*find a  channle that variable and idle*/
static dw_dma_mux_info *dam_find_idle_mux_channel(dw_dma_mux_info *buf, int buf_len)
{
    int i = 0;

    for (i = 0; i < buf_len; i++) {
        if (buf[i].sta == DMA_MUX_CH_IDLE && buf[i].type == DMA_VARIABLE_FUNC) {
            return &buf[i];
        }
    }

    return NULL;
}

#define DMA_MUX_DONT_CONTAIN -1
#define DMA_MUX_CHANNEL_BUSY -2

static int32_t dma_alloc_mux_channel(dw_dma_mux_info *buf, int buf_len, int32_t func)
{
    dw_dma_mux_info *p = dma_find_mux_channel(buf, buf_len, func);

    if (p == NULL) {
        return DMA_MUX_DONT_CONTAIN;
    }

    if (p->sta == DMA_MUX_CH_BUSY) {
        return DMA_MUX_CHANNEL_BUSY;
    }

    p->sta = DMA_MUX_CH_BUSY;
    return p->channle_idx;
}

static int32_t dma_release_mux_channel(dw_dma_mux_info *buf, int buf_len, int32_t func)
{
    dw_dma_mux_info *p = dma_find_mux_channel(buf, buf_len, func);

    if (p != NULL) {
        p->sta = DMA_MUX_CH_IDLE;
        return 0;
    }

    return -1;
}

static int32_t dam0_alloc_mux_channel(int32_t func)
{
    int ret = dma_alloc_mux_channel(dma0_channel_mux, DMA0_REQ_NUM, func);

    return ret < 0 ? -1 : ret;
}

static int32_t dam0_release_mux_channel(int32_t func)
{
    return dma_release_mux_channel(dma0_channel_mux, DMA0_REQ_NUM, func);
}

static int32_t dma1_config_mux(int mux_channel, int func)
{
    if (func > DMA1_USI2_RX_REQ) {
        /*func convert to mux register mask val*/
        func -= DMA1_I2S2_TX_REQ;
    }

    pg1_cpr1_reg_t  *cpr1_base = (pg1_cpr1_reg_t *)PANGU_CPR1_DMA1_MUX_BASE;
    cpr1_base->CPR1_DMA_CH_SEL[mux_channel - 8] = func;

    return 0;
}

#define DMA1_FUNC_START_IDX DWENUM_DMA_CODEC_ADC_0

static int32_t dma1_alloc_mux_channel(int32_t func)
{
    dw_dma_mux_info *p = NULL;

    /*func convert to dma1 mux id*/
    func -= DMA1_FUNC_START_IDX;

    uint32_t dma1_channel_mux_num = sizeof(dma1_channel_mux) / sizeof(dma1_channel_mux[0]);
    int ret = dma_alloc_mux_channel(dma1_channel_mux, dma1_channel_mux_num, func);

    if (ret == DMA_MUX_CHANNEL_BUSY) {
        return -1;
    } else if (ret >= 0) {
        return ret;
    }

    p = dam_find_idle_mux_channel(dma1_channel_mux, dma1_channel_mux_num);

    if (p == NULL) {
        return -1;
    }

    p->func = func;
    p->sta = DMA_MUX_CH_BUSY;

    dma1_config_mux(p->channle_idx, func);

    return p->channle_idx;
}

static int32_t dma1_release_mux_channel(int32_t func)
{
    uint32_t dma1_channel_mux_num = sizeof(dma1_channel_mux) / sizeof(dma1_channel_mux[0]);
    func -= DMA1_FUNC_START_IDX;
    return dma_release_mux_channel(dma1_channel_mux, dma1_channel_mux_num, func);
}

/*success return mux channel, else return -1*/
int32_t dma_mux_channel_alloc(dwenum_dma_device_e func)
{
    if (func < 0 || func > DWENUM_DMA_CODEC_ADC_3) {
        return -1;
    }

    if (func < DWENUM_DMA_UART1_RX) {
        return dam0_alloc_mux_channel(func);
    }

    return dma1_alloc_mux_channel(func);
}

/*success return 0, else return -1*/
int32_t dma_mux_channel_release(dwenum_dma_device_e func)
{
    if (func < 0 || func > DWENUM_DMA_CODEC_ADC_3) {
        return -1;
    }

    if (func < DWENUM_DMA_UART1_RX) {
        return dam0_release_mux_channel(func);
    }

    return dma1_release_mux_channel(func);
}
