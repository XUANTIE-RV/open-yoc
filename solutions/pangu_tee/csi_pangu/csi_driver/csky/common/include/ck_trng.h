/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#ifndef _CK_TRNG_H_
#define _CK_TRNG_H_

#include "drv_trng.h"
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
