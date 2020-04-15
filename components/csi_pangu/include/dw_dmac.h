/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

/******************************************************************************
* @file     dw_dmac.h
* @brief    header file for DMAC Driver
* @version  V1.0
* @date     02. June 2017
******************************************************************************/
#ifndef __CK_DMA_H
#define __CK_DMA_H

#include <stdio.h>
#include "soc.h"
#include "drv/dmac.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CK_DMA_MAXCHANNEL  2
#define CK_DMA_INT_EN      1
#define CK_DMA_CH_EN       1
#define CK_DMA_TFR   0x0002
#define CK_DMA_ERR   0x0001
#define CK_DMA_INTC     0x1f
#define CK_DMA_MASK     0x1f00

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

#define DMA_REG_SARx            0x0
#define DMA_REG_DARx            0x8
#define DMA_REG_LLPX            0x10
#define DMA_REG_CTRLax          0x18
#define DMA_REG_CTRLbx          0x1c
#define DMA_REG_CFGax           0x40
#define DMA_REG_CFGbx           0x44

#define DMA_REG_RawTfr          0x2c0
#define DMA_REG_RawBlock        0x2c8
#define DMA_REG_RawSrcTran      0x2d0
#define DMA_REG_RawDstTran      0x2d8
#define DMA_REG_RawErr          0x2e0

#define DMA_REG_StatusTfr       0x2e8
#define DMA_REG_StatusBlock     0x2f0
#define DMA_REG_StatusSrcTran   0x2f8
#define DMA_REG_StatusDstTran   0x300
#define DMA_REG_StatusErr       0x308

#define DMA_REG_MaskTfr         0x310
#define DMA_REG_MaskBlock       0x318
#define DMA_REG_MaskSrcTran     0x320
#define DMA_REG_MaskDstTran     0x328
#define DMA_REG_MaskErr         0x330

#define DMA_REG_ClearTfr        0x338
#define DMA_REG_ClearBlock      0x340
#define DMA_REG_ClearSrcTran    0x348
#define DMA_REG_ClearDstTran    0x350
#define DMA_REG_ClearErr        0x358
#define DMA_REG_StatusInt       0x360

#define DMA_REG_ReqSrc          0x368
#define DMA_REG_ReqDst          0x370
#define DMA_REG_SglReqSrc       0x378
#define DMA_REG_SglReqDst       0x380
#define DMA_REG_LstReqSrc       0x388
#define DMA_REG_LstReqDst       0x390

#define DMA_REG_Cfg             0x398
#define DMA_REG_ChEn            0x3a0

//typedef struct {
//    __IOM uint32_t   SAR;       /* offset: 0x00 (R/W) Channel Source Address Register        */
//    __IOM uint32_t   DAR;       /* offset: 0x04 (R/W) Channel Destination Address Register   */
//    __IOM uint32_t   CHCTRLA;   /* offset: 0x08 (R/W) Channel Control Register A             */
//    __IOM uint32_t   CHCTRLB;   /* offset: 0x0C (R/W) Channel Control Register B             */
//    __IOM uint8_t   CHINTM:2;    /* offset: 0x10 (R/W) Channel Interrupt Mask Register        */
//    uint8_t  RESERVED0[3];
//    __IM  uint8_t   CHINTS:2;    /* offset: 0x14 (R/ ) Channel Interrupt Status Register      */
//    uint8_t  RESERVED1[3];
//    __IOM uint8_t   CHINTC:2;    /* offset: 0x18 (R/W) Channel Interrupt Clear Register       */
//    uint8_t  RESERVED2[3];
//    __IOM uint8_t   CHSREQ:1;    /* offset: 0x1C (R/W) Channel Software Request Register      */
//    uint8_t  RESERVED3[3];
//    __IOM uint8_t   CHEN:1;      /* offset: 0x20 (R/W) Channel Enable Register                */
//    uint8_t  RESERVED4[3];
//} dw_dma_reg_t;

typedef struct {
#ifdef CONFIG_LPM
    uint8_t dma_power_status;
#endif
    uint32_t base;
    uint32_t control_base;
    uint32_t irq;
    dma_event_cb_t cb_event;
    void *cb_arg;
    uint8_t ch_num;
    uint8_t ch_opened;
    dma_status_e status;
    uint32_t src_tw;
    uint32_t dst_tw;
    uint32_t prer_dmac_ch_num;
    dwenum_dma_device_e ch_mux_func;
} dw_dma_priv_t;

#ifdef __cplusplus
}
#endif

#endif /* __CK_DMA_H */
