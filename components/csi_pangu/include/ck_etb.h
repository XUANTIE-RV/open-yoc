/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     ck_etb.h
 * @brief    header file for event trigger block driver
 * @version  V1.0
 * @date     28. octorber 2017
 ******************************************************************************/
#ifndef _CK_ETB_H_
#define _CK_ETB_H_

#include <soc.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ETB_CHANNEL_FUNC_ERROR     1
#define ETB_NO_SUCH_TRIGGER        2
#define ETB_NO_SUCH_CHANNEL_AVAIL  3
#define ETB_MODE_ERROR             4

typedef enum {
    CK_ETB_CH0    = 0,
    CK_ETB_CH1    = 1,
    CK_ETB_CH2    = 2,
    CK_ETB_CH3    = 3,
    CK_ETB_CH4    = 4,
    CK_ETB_CH5    = 5,
    CK_ETB_CH6    = 6,
    CK_ETB_CH7    = 7,
    CK_ETB_CH8    = 8,
    CK_ETB_CH9    = 9,
    CK_ETB_CH10   = 10,
    CK_ETB_CH11   = 11,
    CK_ETB_CH12   = 12,
    CK_ETB_CH13   = 13,
    CK_ETB_CH14   = 14,
    CK_ETB_CH15   = 15,
    CK_ETB_CH16   = 16,
    CK_ETB_CH17   = 17,
    CK_ETB_CH18   = 18,
    CK_ETB_CH19   = 19,
    CK_ETB_CH20   = 20,
    CK_ETB_CH21   = 22,
    CK_ETB_CH23   = 23,
    CK_ETB_CH24   = 24,
    CK_ETB_CH25   = 25,
    CK_ETB_CH26   = 26,
    CK_ETB_CH27   = 27,
    CK_ETB_CH28   = 28,
    CK_ETB_CH29   = 29,
    CK_ETB_CH30   = 30,
    CK_ETB_CH31   = 31
} CKENUM_ETB_CHANNEL;

#define CK_ETB_EN          CSKY_ETB_BASE
#define CK_ETB_SOFTTRIG    (CSKY_ETB_BASE + 0x4)
#define CK_ETB_CFG0_CH0    (CSKY_ETB_BASE + 0x8)
#define CK_ETB_CFG1_CH0    (CSKY_ETB_BASE + 0xc)
#define CK_ETB_CFG0_CH1    (CSKY_ETB_BASE + 0x10)
#define CK_ETB_CFG1_CH1    (CSKY_ETB_BASE + 0x14)
#define CK_ETB_CFG0_CH2    (CSKY_ETB_BASE + 0x18)
#define CK_ETB_CFG1_CH2    (CSKY_ETB_BASE + 0x1c)
#define CK_ETB_CFG_CH3     (CSKY_ETB_BASE + 0x30)
#define CK_ETB_CFG_CH4     (CSKY_ETB_BASE + 0x34)
#define CK_ETB_CFG_CH5     (CSKY_ETB_BASE + 0x38)
#define CK_ETB_CFG_CH6     (CSKY_ETB_BASE + 0x3c)
#define CK_ETB_CFG_CH7     (CSKY_ETB_BASE + 0x40)
#define CK_ETB_CFG_CH8     (CSKY_ETB_BASE + 0x44)
#define CK_ETB_CFG_CH9     (CSKY_ETB_BASE + 0x48)
#define CK_ETB_CFG_CH10    (CSKY_ETB_BASE + 0x4c)
#define CK_ETB_CFG_CH11    (CSKY_ETB_BASE + 0x50)
#define CK_ETB_CFG_CH12    (CSKY_ETB_BASE + 0x54)
#define CK_ETB_CFG_CH13    (CSKY_ETB_BASE + 0x58)
#define CK_ETB_CFG_CH14    (CSKY_ETB_BASE + 0x5c)
#define CK_ETB_CFG_CH15    (CSKY_ETB_BASE + 0x60)
#define CK_ETB_CFG_CH16    (CSKY_ETB_BASE + 0x64)
#define CK_ETB_CFG_CH17    (CSKY_ETB_BASE + 0x68)
#define CK_ETB_CFG_CH18    (CSKY_ETB_BASE + 0x6c)
#define CK_ETB_CFG_CH19    (CSKY_ETB_BASE + 0x70)
#define CK_ETB_CFG_CH20    (CSKY_ETB_BASE + 0x74)
#define CK_ETB_CFG_CH21    (CSKY_ETB_BASE + 0x78)
#define CK_ETB_CFG_CH22    (CSKY_ETB_BASE + 0x7c)
#define CK_ETB_CFG_CH23    (CSKY_ETB_BASE + 0x80)
#define CK_ETB_CFG_CH24    (CSKY_ETB_BASE + 0x84)
#define CK_ETB_CFG_CH25    (CSKY_ETB_BASE + 0x88)
#define CK_ETB_CFG_CH26    (CSKY_ETB_BASE + 0x8c)
#define CK_ETB_CFG_CH27    (CSKY_ETB_BASE + 0x90)
#define CK_ETB_CFG_CH28    (CSKY_ETB_BASE + 0x94)
#define CK_ETB_CFG_CH29    (CSKY_ETB_BASE + 0x98)
#define CK_ETB_CFG_CH30    (CSKY_ETB_BASE + 0x9c)
#define CK_ETB_CFG_CH31    (CSKY_ETB_BASE + 0xa0)

#define CK_ETB_TRIGGER_MODE_BIT   1
#define CK_ETB_SRC0_EN_CH0_BIT    0
#define CK_ETB_SRC1_EN_CH0_BIT    10
#define CK_ETB_SRC2_EN_CH0_BIT    20

#define CK_ETB_CH0_SOURCE_TRIGGER_MAX          123
#define CK_ETB_CH0_SOURCE_TRIGGER_BITS_WIDTH   7
#define CK_ETB_CH0_DEST_TRIGGER_MAX            255
#define CK_ETB_CH0_DEST_TRIGGER_BIT_BASE       24
#define CK_ETB_CH0_DEST_TRIGGER_BITS_WIDTH     8

#define CK_ETB_DEST0_EN_CH1_BIT    0
#define CK_ETB_DEST1_EN_CH1_BIT    10
#define CK_ETB_DEST2_EN_CH1_BIT    20

#define CK_ETB_CH1_DEST_TRIGGER_MAX              255
#define CK_ETB_CH1_DEST_TRIGGER_BITS_WIDTH       8
#define CK_ETB_CH1_SOURCE_TRIGGER_MAX            123
#define CK_ETB_CH1_SOURCE_TRIGGER_BIT_BASE       25
#define CK_ETB_CH1_SOURCE_TRIGGER_BITS_WIDTH     7

#define CK_ETB_DEST0_EN_CH2_BIT    0
#define CK_ETB_DEST1_EN_CH2_BIT    10
#define CK_ETB_DEST2_EN_CH2_BIT    20

#define CK_ETB_CH2_DEST_TRIGGER_MAX              255
#define CK_ETB_CH2_DEST_TRIGGER_BITS_WIDTH       8
#define CK_ETB_CH2_SOURCE_TRIGGER_MAX            123
#define CK_ETB_CH2_SOURCE_TRIGGER_BIT_BASE       25
#define CK_ETB_CH2_SOURCE_TRIGGER_BITS_WIDTH     7

#define CK_ETB_CH3_31_SOURCE_TRIGGER_MAX            123
#define CK_ETB_CH3_31_SOURCE_TRIGGER_BITS_WIDTH     7
#define CK_ETB_CH3_31_DEST_TRIGGER_MAX              255
#define CK_ETB_CH3_31_DEST_TRIGGER_BIT_BASE         24
#define CK_ETB_CH3_31_DEST_TRIGGER_BITS_WIDTH       8

#define CK_ETB_CH3_31_SOURCE_BASE_BIT    12
#define CK_ETB_CH3_31_DEST_BASE_BIT    24

#define CHANNEL_ENABLE_COMMAND    1
#define CHANNEL_DISABLE_COMMAND   0

//source location and its' number
typedef enum {
    RTC = 0
} CKENUM_ETB_SOURCE_LOCALTION;

//dest location and its' number
typedef enum {
    DMA_CH0 = 0
} CKENUM_ETB_DEST_LOCALTION;


typedef struct {
    __IOM uint32_t ETB_EN;         /* offset: 0x00 (R/W) ETB enable Register        */
    __IOM uint32_t SOFTTRIG;       /* offset: 0x04 (R/W) Software trigger Configure Register        */
    __IOM uint32_t CFG0_CH0;       /* offset: 0x08 (R/W) ETB Channel0 Configure0 Register        */
    __IOM uint32_t CFG1_CH0;       /* offset: 0x0c (R/W) ETB Channel0 Configure1 Register        */
    __IOM uint32_t CFG0_CH1;       /* offset: 0x10 (R/W) ETB Channel1 Configure0 Register        */
    __IOM uint32_t CFG1_CH1;       /* offset: 0x14 (R/W) ETB Channel1 Configure1 Register        */
    __IOM uint32_t CFG0_CH2;       /* offset: 0x18 (R/W) ETB Channel2 Configure0 Register        */
    __IOM uint32_t CFG1_CH2;       /* offset: 0x1c (R/W) ETB Channel2 Configure1 Register        */
    uint32_t RESERVED0[4];
    __IOM uint32_t CFG_CH3;        /* offset: 0x30 (R/W) ETB Channel3 Configure Register        */
    __IOM uint32_t CFG_CH4;        /* offset: 0x34 (R/W) ETB Channel4 Configure Register        */
    __IOM uint32_t CFG_CH5;        /* offset: 0x38 (R/W) ETB Channel5 Configure Register        */
    __IOM uint32_t CFG_CH6;        /* offset: 0x3c (R/W) ETB Channel6 Configure Register        */
    __IOM uint32_t CFG_CH7;        /* offset: 0x40 (R/W) ETB Channel7 Configure Register        */
    __IOM uint32_t CFG_CH8;        /* offset: 0x44 (R/W) ETB Channel8 Configure Register        */
    __IOM uint32_t CFG_CH9;        /* offset: 0x48 (R/W) ETB Channel9 Configure Register        */
    __IOM uint32_t CFG_CH10;       /* offset: 0x4c (R/W) ETB Channel10 Configure Register        */
    __IOM uint32_t CFG_CH11;       /* offset: 0x50 (R/W) ETB Channel11 Configure Register        */
    __IOM uint32_t CFG_CH12;       /* offset: 0x54 (R/W) ETB Channel12 Configure Register        */
    __IOM uint32_t CFG_CH13;       /* offset: 0x58 (R/W) ETB Channel13 Configure Register        */
    __IOM uint32_t CFG_CH14;       /* offset: 0x5c (R/W) ETB Channel14 Configure Register        */
    __IOM uint32_t CFG_CH15;       /* offset: 0x60 (R/W) ETB Channel15 Configure Register        */
    __IOM uint32_t CFG_CH16;       /* offset: 0x64 (R/W) ETB Channel16 Configure Register        */
    __IOM uint32_t CFG_CH17;       /* offset: 0x68 (R/W) ETB Channel17 Configure Register        */
    __IOM uint32_t CFG_CH18;       /* offset: 0x6c (R/W) ETB Channel18 Configure Register        */
    __IOM uint32_t CFG_CH19;       /* offset: 0x70 (R/W) ETB Channel19 Configure Register        */
    __IOM uint32_t CFG_CH20;       /* offset: 0x74 (R/W) ETB Channel20 Configure Register        */
    __IOM uint32_t CFG_CH21;       /* offset: 0x78 (R/W) ETB Channel21 Configure Register        */
    __IOM uint32_t CFG_CH22;       /* offset: 0x7c (R/W) ETB Channel22 Configure Register        */
    __IOM uint32_t CFG_CH23;       /* offset: 0x80 (R/W) ETB Channel23 Configure Register        */
    __IOM uint32_t CFG_CH24;       /* offset: 0x84 (R/W) ETB Channel24 Configure Register        */
    __IOM uint32_t CFG_CH25;       /* offset: 0x88 (R/W) ETB Channel25 Configure Register        */
    __IOM uint32_t CFG_CH26;       /* offset: 0x8c (R/W) ETB Channel26 Configure Register        */
    __IOM uint32_t CFG_CH27;       /* offset: 0x90 (R/W) ETB Channel27 Configure Register        */
    __IOM uint32_t CFG_CH28;       /* offset: 0x94 (R/W) ETB Channel28 Configure Register        */
    __IOM uint32_t CFG_CH29;       /* offset: 0x98 (R/W) ETB Channel29 Configure Register        */
    __IOM uint32_t CFG_CH30;       /* offset: 0x9c (R/W) ETB Channel30 Configure Register        */
    __IOM uint32_t CFG_CH31;       /* offset: 0xa0 (R/W) ETB Channel31 Configure Register        */
} ck_etb_reg_t;

#ifdef __cplusplus
}
#endif

#endif /* __CK_ETB_H */

