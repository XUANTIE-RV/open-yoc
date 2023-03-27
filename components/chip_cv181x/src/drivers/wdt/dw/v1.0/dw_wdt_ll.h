/*
 *	Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/*******************************************************
 * @file 	dw_wdt_ll.h
 * @brief	header file for wdt ll driver
 * @version V1.0
 * @date	23. Sep 2020
 * ******************************************************/

#ifndef _DW_WDT_LL_H_
#define _DW_WDT_LL_H_

#include <stdio.h>
#include <soc.h>
#include <csi_core.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*! WDT control register, 	        offset: 0x00 */
#define DW_WDT_CR_ENABLE_Pos                                    (0U)
#define DW_WDT_CR_ENABLE_Msk                                    (0x1U << DW_WDT_CR_ENABLE_Pos)
#define DW_WDT_CR_ENABLE_EN                                     DW_WDT_CR_ENABLE_Msk

#define DW_WDT_CR_RMOD_Pos                                      (1U)
#define DW_WDT_CR_RMOD_Msk                                      (0x1U << DW_WDT_CR_RMOD_Pos)
#define DW_WDT_CR_RMOD_EN                                       DW_WDT_CR_RMOD_Msk

#define DW_WDT_CR_RPL_Pos                                       (2U)
#define DW_WDT_CR_RPL_Msk                                       (0x7U << DW_WDT_CR_RPL_Pos)
#define DW_WDT_CR_RPL                                           DW_WDT_CR_RPL_Msk
#define DW_WDT_CR_RPL_0                                         (0x0U << DW_WDT_CR_RPL_Pos)
#define DW_WDT_CR_RPL_1                                         (0x1U << DW_WDT_CR_RPL_Pos)
#define DW_WDT_CR_RPL_2                                         (0x2U << DW_WDT_CR_RPL_Pos)
#define DW_WDT_CR_RPL_3                                         (0x3U << DW_WDT_CR_RPL_Pos)
#define DW_WDT_CR_RPL_4                                         (0x4U << DW_WDT_CR_RPL_Pos)
#define DW_WDT_CR_RPL_5                                         (0x5U << DW_WDT_CR_RPL_Pos)
#define DW_WDT_CR_RPL_6                                         (0x6U << DW_WDT_CR_RPL_Pos)
#define DW_WDT_CR_RPL_7                                         (0x7U << DW_WDT_CR_RPL_Pos)

/*! WDT timeout range register, 	offset: 0x04 */
#define DW_WDT_TORR_TOP_Pos                                     (0U)
#define DW_WDT_TORR_TOP_Msk                                     (0x4U << DW_WDT_TORR_TOP_Pos)
#define DW_WDT_TORR_TOP                                         DW_WDT_TORR_TOP_Msk

#define DW_WDT_TORR_TOPINIT_Pos                                 (4U)
#define DW_WDT_TORR_TOPINIT_Msk                                 (0x4U << DW_WDT_TORR_TOPINIT_Pos)
#define DW_WDT_TORR_TOPINIT                                     DW_WDT_TORR_TOPINIT_Msk

/*! WDT count restart register, 	offset: 0x0C */
#define DW_WDT_CNT_RESTART_REG                                  (0x76U)

/*! WDT interrupt status register, 	offset: 0x10 */
#define DW_WDT_STAT_INTR_Pos                                    (0U)
#define DW_WDT_STAT_INTR_Msk                                    (0x1U << DW_WDT_STAT_INTR_Pos)
#define DW_WDT_STAT_INTR_EN                                     DW_WDT_STAT_INTR_Msk

/*! WDT interrupt clear register, 	offset: 0x14 */
#define DW_WDT_EOI_INTR_Pos                                     (0U)
#define DW_WDT_EOI_INTR_Msk                                     (0x1U << DW_WDT_EOI_INTR_Pos)
#define DW_WDT_EOI_INTR_EN                                      DW_WDT_EOI_INTR_Msk

typedef struct {
    __IOM uint32_t WDT_CR;                                      /* Offset: 0x000 (R/W)  WDT control register                */
    __IOM uint32_t WDT_TORR;                                    /* Offset: 0x004 (R/W)  WDT timeout range register          */
    __IM  uint32_t WDT_CCVR;                                    /* Offset: 0x008 (R/ )  WDT current counter value register  */
    __OM  uint32_t WDT_CRR;                                     /* Offset: 0x00C ( /W)  WDT count restart register          */
    __IM  uint32_t WDT_STAT;                                    /* Offset: 0x010 (R/ )  WDT interrupt status register       */
    __IM  uint32_t WDT_EOI;                                     /* Offset: 0x014 (R/ )  WDT interrupt clear register        */
} dw_wdt_regs_t;

static inline uint32_t dw_wdt_cr_status_get(dw_wdt_regs_t *wdt_base)
{
    return ((wdt_base->WDT_CR & DW_WDT_CR_ENABLE_EN) ? (uint32_t)1 : (uint32_t)0);
}
static inline void dw_wdt_cr_enable(dw_wdt_regs_t *wdt_base)
{
    wdt_base->WDT_CR |= DW_WDT_CR_ENABLE_EN;
}
static inline void dw_wdt_cr_disable(dw_wdt_regs_t *wdt_base)
{
    wdt_base->WDT_CR &= ~DW_WDT_CR_ENABLE_EN;
}

static inline uint32_t dw_wdt_cr_rmod_get(dw_wdt_regs_t *wdt_base)
{
    return ((wdt_base->WDT_CR & DW_WDT_CR_RMOD_EN) ? (uint32_t)1 : (uint32_t)0);
}
static inline void dw_wdt_cr_rmod_set(dw_wdt_regs_t *wdt_base)
{
    wdt_base->WDT_CR |= DW_WDT_CR_RMOD_EN;
}
static inline void dw_wdt_cr_rmod_clr(dw_wdt_regs_t *wdt_base)
{
    wdt_base->WDT_CR &= ~DW_WDT_CR_RMOD_EN;
}

static inline uint32_t dw_wdt_cr_rpl_get(dw_wdt_regs_t *wdt_base)
{
    return ((wdt_base->WDT_CR & DW_WDT_CR_RPL_Msk) >> DW_WDT_CR_RPL_Pos);
}
static inline void dw_wdt_cr_rpl_set(dw_wdt_regs_t *wdt_base, uint32_t value)
{
    wdt_base->WDT_CR &= ~DW_WDT_CR_RPL_Msk;
    wdt_base->WDT_CR += (value << DW_WDT_CR_RPL_Pos);
}

static inline uint32_t dw_wdt_torr_top_get(dw_wdt_regs_t *wdt_base)
{
    return ((wdt_base->WDT_TORR & DW_WDT_TORR_TOP) >> DW_WDT_TORR_TOP_Pos);
}
static inline void dw_wdt_torr_top_set(dw_wdt_regs_t *wdt_base, uint32_t value)
{
    wdt_base->WDT_TORR &= ~DW_WDT_TORR_TOP_Msk;
    wdt_base->WDT_TORR += (value << DW_WDT_TORR_TOP_Pos);
}

static inline uint32_t dw_wdt_torr_topinit_get(dw_wdt_regs_t *wdt_base)
{
    return ((wdt_base->WDT_TORR & DW_WDT_TORR_TOPINIT) >> DW_WDT_TORR_TOPINIT_Pos);
}
static inline void dw_wdt_torr_topinit_set(dw_wdt_regs_t *wdt_base, uint32_t value)
{
    wdt_base->WDT_TORR &= ~DW_WDT_TORR_TOPINIT_Msk;
    wdt_base->WDT_TORR += (value << DW_WDT_TORR_TOPINIT_Pos);
}

static inline uint32_t dw_wdt_ccvr_get(dw_wdt_regs_t *wdt_base)
{
    return (wdt_base->WDT_CCVR);
}

static inline void dw_wdt_crr_safety_restart(dw_wdt_regs_t *wdt_base)
{
    wdt_base->WDT_CRR = DW_WDT_CNT_RESTART_REG;
}

static inline uint32_t dw_wdt_stat_status_get(dw_wdt_regs_t *wdt_base)
{
    return ((wdt_base->WDT_STAT & DW_WDT_STAT_INTR_EN) ? (uint32_t)1 : (uint32_t)0);
}

static inline void dw_wdt_eoi_intr_clear(dw_wdt_regs_t *wdt_base)
{
    wdt_base->WDT_EOI;
}

static inline void dw_wdt_reset_register(dw_wdt_regs_t *wdt_base)
{
    wdt_base->WDT_CR = 0U;
    wdt_base->WDT_TORR = 0U;
    wdt_base->WDT_CRR = 0U;
}

#ifdef __cplusplus
}
#endif

#endif /* _DW_WDT_LL_H_ */
