/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

/******************************************************************************
* @file     ck_dmac.h
* @brief    header file for DMAC Driver
* @version  V1.0
* @date     02. June 2017
******************************************************************************/
#ifndef __CK_DMA_H
#define __CK_DMA_H
#include <stdio.h>
#include "soc.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CK_DMA_MAXCHANNEL  2
#define CK_DMA_INT_EN      1
#define CK_DMA_CH_EN       1
#define CK_DMA_TFR   0x0002
#define CK_DMA_ERR   0x0001
#define CK_DMA_INTC     0x03
#define CK_DMA_MASK     0x03

typedef enum {
    DMA_ADDR_INCREMENT = 0,
    DMA_ADDR_DECREMENT = 1,
    DMA_ADDR_NOCHANGE  = 2
} enum_addr_state_e;

typedef enum {
    DMA_DATAWIDTH_SIZE8  = 1,
    DMA_DATAWIDTH_SIZE16 = 2,
    DMA_DATAWIDTH_SIZE32 = 4
} dma_datawidth_e;

typedef enum {
    DMA_HANDSHAKING_HARDWARE = 0,
    DMA_HANDSHAKING_SOFTWARE = 1,
} dma_handshaking_select_e;

typedef enum {
    DMA_PRIORITY0 = 0,
    DMA_PRIORITY1 = 1,
    DMA_PRIOTITY2 = 2,
    DMA_PRIOTITY3 = 3
} dma_priority_t;

typedef struct {
    __IOM uint32_t   SAR;       /* offset: 0x00 (R/W) Channel Source Address Register        */
    __IOM uint32_t   DAR;       /* offset: 0x04 (R/W) Channel Destination Address Register   */
    __IOM uint32_t   CHCTRLA;   /* offset: 0x08 (R/W) Channel Control Register A             */
    __IOM uint32_t   CHCTRLB;   /* offset: 0x0C (R/W) Channel Control Register B             */
    __IOM uint8_t   CHINTM: 2;   /* offset: 0x10 (R/W) Channel Interrupt Mask Register        */
    uint8_t  RESERVED0[3];
    __IM  uint8_t   CHINTS: 2;   /* offset: 0x14 (R/ ) Channel Interrupt Status Register      */
    uint8_t  RESERVED1[3];
    __IOM uint8_t   CHINTC: 2;   /* offset: 0x18 (R/W) Channel Interrupt Clear Register       */
    uint8_t  RESERVED2[3];
    __IOM uint8_t   CHSREQ: 1;   /* offset: 0x1C (R/W) Channel Software Request Register      */
    uint8_t  RESERVED3[3];
    __IOM uint8_t   CHEN: 1;     /* offset: 0x20 (R/W) Channel Enable Register                */
    uint8_t  RESERVED4[3];
} ck_dma_reg_t;
#ifdef __cplusplus
}
#endif

#endif /* __CK_DMA_H */
