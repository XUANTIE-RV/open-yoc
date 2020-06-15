/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     ck_aes.h
 * @brief    header file for aes driver
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/
#ifndef _CK_AES_H_
#define _CK_AES_H_

#include <stdio.h>
#include "drv/aes.h"
#include "soc.h"

#define AES_LITTLE_ENDIAN   0x00000100

#define AES_MAX_KEY_LENGTH  32
#define AES_IT_DATAINT      0x4
#define AES_IT_KEYINT       0x2
#define AES_IT_BUSY         0x1
#define AES_IT_ALL          0x7
#define AES_CRYPTO_KEYEXP   0x2

#define AES_WORK_ENABLE_OFFSET  0
#define AES_INT_ENABLE_OFFSET   2
#define AES_MODE_OFFSET         3
#define AES_KEY_LEN_OFFSET      4
#define AES_OPCODE_OFFSET       6

typedef struct {
    __IOM uint32_t DATAIN[4];                 /* Offset: 0x000 (R/W) Data input 0~127 */
    __IOM uint32_t KEY[8];                    /* Offset: 0x010 (R/W) Key 0~255 */
    __IOM uint32_t IV[4];                     /* Offset: 0x030 (R/W) Initial Vector: 0~127 */
    __IOM uint32_t CTRL;                      /* Offset: 0x040 (R/W) AES Control Register */
    __IOM uint32_t STATE;                     /* Offset: 0x044 (R/W) AES State Register */
    __IOM uint32_t DATAOUT[4];                /* Offset: 0x048 (R/W) Data Output 0~31 */
} ck_aes_reg_t;

#endif
