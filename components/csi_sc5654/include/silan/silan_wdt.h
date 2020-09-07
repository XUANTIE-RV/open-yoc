/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
/******************************************************************************
 * @file     silan_wdt.h
 * @brief    header file for wdt driver
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/
#ifndef __SILAN_WDT_H
#define __SILAN_WDT_H

#include <stdio.h>
#include "soc.h"


#define WDT_CONTROL_RESERN          (1 << 1)
#define WDT_CONTROL_INTEN           (1 << 0)

#define WDT_INTCLR_EN               (1 << 0)

typedef struct {
    __IOM uint32_t WDT_LOAD;         /* Offset: 0x000 (R/W)  WDT load register        */
    __IM  uint32_t WDT_VALUE;        /* Offset: 0x004 (R/)   WDT current counter value register  */
    __IOM uint32_t WDT_CONTROL;      /* Offset: 0x008 (R/W ) WDT control register  */
    __OM  uint32_t WDT_INTCLR;       /* Offset: 0x00C ( /W)  WDT interrupt clear register */
    __IM  uint32_t WDT_RIS;          /* Offset: 0x010 (R/ )  WDT raw interrupt status register */
    __IM  uint32_t WDT_MIS;          /* Offset: 0x014 (R/ )  WDT mask interrupt register */
} silan_wdt_reg_t;

#endif /* __SILAN_WDT_H */

