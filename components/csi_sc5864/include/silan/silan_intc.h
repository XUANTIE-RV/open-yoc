/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     silan_intc.h
 * @brief    CSI CK802 Core Peripheral Access Layer Header File
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/

#ifndef __SILAN_INTC_H
#define __SILAN_INTC_H

#include <stdint.h>
#include <soc.h>

#if CONFIG_CKCPU_MMU
#define INTC_BASE  SILAN_INTL_BASE
#else
#define INTC_BASE  SILAN_INTL_PHY_BASE
#endif

#define INTC    ((silan_intc_reg_t *)INTC_BASE)
typedef struct {
    __IOM uint32_t IRQ_INTEN;           /* Offset: 0x000 (R/W) Enable the Interrupt Register */
    uint32_t RESERVED1;
    __IOM uint32_t IRQ_INTMASK;         /* Offset: 0x008 (R/W) Mask Interrupt Register */
    uint32_t RESERVED2;
    __IOM uint32_t INTFORCE;            /* Offset: 0x010 (R/W) Enable  the Soft Interrupt Register */
    uint32_t RESERVED3;
    __IM uint32_t IRG_RAWSTATUS;        /* Offset: 0x018 (R) Raw Interrupt Status Register */
    uint32_t RESERVED4;
    __IM uint32_t IRQ_STATUS;           /* Offset: 0x020 (R) Interrupt Status Register */
    uint32_t RESERVED5;
    __IM uint32_t IRQ_MASKSTATUS;       /* Offset: 0x028 (R) Mask Interrupt Status Register */
    uint32_t RESERVED6;
    __IOM uint32_t IRQ_FINALSTATUS;     /* Offset: 0x030 (R/W) Final Interrupt Status Register */
} silan_intc_reg_t;

#endif /* __SILAN_INTC_H */
