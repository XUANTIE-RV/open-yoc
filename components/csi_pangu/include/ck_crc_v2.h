/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     ck_crc_v2.h
 * @brief    header file for crc_v2 driver
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/
#ifndef _CK_CRC_v2_H_
#define _CK_CRC_v2_H_

#include "stdint.h"
#include "soc.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    __IOM uint32_t CONFIG_REG;                     /* Offset: 0x000 (W/R) config register */
    __IOM uint32_t INIT_VALUE;                     /* Offset: 0x004 (W/R) initial value register */
    __IOM  uint32_t XOR_OUT;                       /* Offset: 0x008 (W/R) xor this value to crc_v2 result as final crc_v2 result  */
    __IM  uint32_t CRC_RESULT;                     /* Offset: 0x00c (R) crc_v2 result result register */
    uint32_t RESERVED1[12];
    __IOM  uint32_t NEW_DATA;                      /* Offset: 0x040 (W/R) new data register */

} ck_crc_reg_t;
#ifdef __cplusplus
}
#endif

#endif
