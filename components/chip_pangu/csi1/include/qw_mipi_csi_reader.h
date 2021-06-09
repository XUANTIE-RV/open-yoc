/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     qw_mipi_csi_reader.h
 * @brief    header file for mipi csi reader Driver
 * @version  V1.2
 * @date     24. May 2019
 ******************************************************************************/
#ifndef _QW_MIPI_CSI_READER_H_
#define _QW_MIPI_CSI_READER_H_

#include "soc.h"

typedef struct {
    __IOM uint32_t MIPI2DMA_CTRL;   //= 0x0,
    __IOM uint32_t SRC_WIDTH;       //= 0x4,
    __IOM uint32_t SRC_HIGH;        //= 0x8,
    __IOM uint32_t SRC_PIX_NUM;     //= 0xc,
    __IOM uint32_t MIPI_ADDR0;      //= 0x10,
    __IOM uint32_t MIPI_ADDR1;      //= 0x14,
    __IOM uint32_t MIPI_ADDR2;      //= 0x18,
    __IOM uint32_t MIPI_ADDR3;      //= 0x1c,
    __IOM uint32_t MIPI_ADDR4;      //= 0x20,
    __IOM uint32_t MIPI_ADDR5;      //= 0x24,
    __IOM uint32_t MIPI2DMA_EN;     //= 0x28,
    __IOM uint32_t MIPI2DMA_BUSY;   //= 0x2c,
    __IOM uint32_t MIPI2DMA_INT_STAT;       //= 0x30,
    __IOM uint32_t MIPI2DMA_INT_RAW_STAT;   //= 0x34,
    __IOM uint32_t MIPI2DMA_INT_EN;         //= 0x38
    __IOM uint32_t MIPI2DMA_INT_CLR;        //= 0x3c,
    __IOM uint32_t MIPI_SEL;                //= 0x40,
} qw_mcr_reg_t;

enum rgb_store {
    STORE_RGB = 0,
    STORE_RBG,
    STORE_GRB,
    STORE_GBR,
    STORE_BRG,
    STORE_BGR
};
#endif /* _QW_MIPI_CSI_READER_H_ */
