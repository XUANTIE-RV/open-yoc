/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */
/******************************************************************************
 * @file     ck_trng.h
 * @brief    header file for trng driver
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/
#ifndef _CK_TRNG_H_
#define _CK_TRNG_H_

#include "drv/trng.h"
#include "soc.h"

/*
 *  define the bits for TCR
 */
#define TRNG_EN              (1UL << 1)
#define TRNG_LOWPER_MODE     (1UL << 2)
#define TRNG_DATA_READY      1

typedef struct {
    __IOM uint32_t TCR;                     /* Offset: 0x000 (W/R)  TRNG control register */
    __IM  uint32_t TDR;                     /* Offset: 0x004 (R)    TRNG Data register */

} ck_trng_reg_t;
#endif
