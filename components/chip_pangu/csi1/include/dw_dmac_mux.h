/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

/******************************************************************************
* @file     dw_dmac_mux.h
* @brief    CSI Source File for DMAC MUX Driver
* @version  V1.0
* @date     16. September 2010
* @vendor   csky
* @chip     pangu
******************************************************************************/

#ifndef PG1_CPR_PMU_H_
#define PG1_CPR_PMU_H_

#include "soc.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PANGU_CPR1_DMA1_MUX_BASE (CSKY_CPR1_BASE+ 0x44)

typedef struct {
    __IOM uint32_t CPR1_DMA_CH_SEL[8];
} pg1_cpr1_reg_t;

#define DMAC1_CFG(addr)  (*(volatile uint32_t *)((DMAC1_BASE + (addr))))

#ifdef __cplusplus
}
#endif

#endif /* PG1_CPR_PMU_H_ */

