/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     ck_crc_v1.h
 * @brief    header file for crc driver
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/
#ifndef _CK_CRC_v1_H_
#define _CK_CRC_v1_H_

#include "stdint.h"
#include "soc.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    __IOM uint32_t CRC_DATA;                     /* Offset: 0x000 (W/R)  data register */
    __IOM uint32_t CRC_SEL;                      /* Offset: 0x004 (W/R)  mode select register for CRC */
    __OM  uint32_t CRC_INIT;                     /* Offset: 0x008 (W)    initial value register */

} ck_crc_reg_t;
#ifdef __cplusplus
}
#endif

#endif
