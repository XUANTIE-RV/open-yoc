/*
 *	Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/*******************************************************
 * @file 	wj_rtc_ll.h
 * @brief	header file for rtc ll driver
 * @version V1.0
 * @date	23. Sep 2020
 * ******************************************************/

#ifndef _WJ_RTC_LL_H_
#define _WJ_RTC_LL_H_

#include <stdio.h>
#include <soc.h>

#ifdef	__cplusplus
extern "C" {
#endif


/*! CCR, offset: 0xC */
#define WJ_RTC_CCR_INTREN_Pos                       (0U)
#define WJ_RTC_CCR_INTREN_Msk                       (0x1U << WJ_RTC_CCR_INTREN_Pos)
#define WJ_RTC_CCR_INTREN_EN                        WJ_RTC_CCR_INTREN_Msk

#define WJ_RTC_CCR_INTRMASK_Pos                     (1U)
#define WJ_RTC_CCR_INTRMASK_Msk                     (0x1U << WJ_RTC_CCR_INTRMASK_Pos)
#define WJ_RTC_CCR_INTRMASK_EN                      WJ_RTC_CCR_INTRMASK_Msk

#define WJ_RTC_CCR_COUTEN_Pos                       (2U)
#define WJ_RTC_CCR_COUTEN_Msk                       (0x1U << WJ_RTC_CCR_COUTEN_Pos)
#define WJ_RTC_CCR_COUTEN_EN                        WJ_RTC_CCR_COUTEN_Msk

#define WJ_RTC_CCR_WRAPEN_Pos                       (3U)
#define WJ_RTC_CCR_WRAPEN_Msk                       (0x1U << WJ_RTC_CCR_WRAPEN_Pos)
#define WJ_RTC_CCR_WRAPEN_EN                        WJ_RTC_CCR_WRAPEN_Msk

/*! STAT, offset: 0x10 */
#define WJ_RTC_STAT_INTSTA_Pos                      (0U)
#define WJ_RTC_STAT_INTSTA_Msk                      (0x1U << WJ_RTC_STAT_INTSTA_Pos)
#define WJ_RTC_STAT_INTSTA_EN                       WJ_RTC_STAT_INTSTA_Msk

/*! RSTAT, offset: 0x14 */
#define WJ_RTC_RSTAT_INTRSTA_Pos                    (0U)
#define WJ_RTC_RSTAT_INTRSTA_Msk                    (0x1U << WJ_RTC_RSTAT_INTRSTA_Pos)
#define WJ_RTC_RSTAT_INTRSTA_EN                     WJ_RTC_RSTAT_INTRSTA_Msk

/*! EOI, offset: 0x18 */
#define WJ_RTC_EOI_RDCLEAR_Pos                      (0U)
#define WJ_RTC_EOI_RDCLEAR_Msk                      (0x1U << WJ_RTC_EOI_RDCLEAR_Pos)
#define WJ_RTC_EOI_RDCLEAR_EN                       WJ_RTC_EOI_RDCLEAR_Msk

typedef struct {
    __IM  uint32_t    RTC_CCVR_L;                   /* Offset: 0x000 (R/ )  low current count value register */
    __IOM uint32_t    RTC_CMR_L;                    /* Offset: 0x004 (R/W)  low count match register */
    __IOM uint32_t    RTC_CLR_L;                    /* Offset: 0x008 (R/W)  low count load register */
    __IOM uint32_t    RTC_CCR;                      /* Offset: 0x00c (R/W)  count control register */
    __IM  uint32_t    RTC_STAT;                     /* Offset: 0x010 (R/ )  interrupt status register */
    __IM  uint32_t    RTC_RSTAT;                    /* Offset: 0x014 (R/ )  interrupt raw status register */
    __IM  uint32_t    RTC_EOI;                      /* Offset: 0x018 (R/ )  end of interrupt register */
    __IM  uint32_t    RTC_COMP_VERSION;             /* Offset: 0x01c (R/ )  component version register */
    __IOM  uint32_t   RTC_CCVR_H;                   /* Offset: 0x020 (R/W)  high current count value register */
    __IOM uint32_t    RTC_CMR_H;                    /* Offset: 0x024 (R/W)  high count match register */
    __IOM uint32_t    RTC_CLR_H;                    /* Offset: 0x028 (R/W)  high count load register */
} wj_rtc_regs_t;


static inline uint32_t wj_rtc_get_currcount_l(wj_rtc_regs_t *rtc_base)
{
    return (rtc_base->RTC_CCVR_L);
}

static inline uint32_t wj_rtc_get_matchcount_l(wj_rtc_regs_t *rtc_base)
{
    return (rtc_base->RTC_CMR_L);
}
static inline void wj_rtc_set_matchcount_l(wj_rtc_regs_t *rtc_base, uint32_t value)
{
    rtc_base->RTC_CMR_L = value;
}

static inline uint32_t wj_rtc_get_loadcount_l(wj_rtc_regs_t *rtc_base)
{
    return (rtc_base->RTC_CLR_L);
}
static inline void wj_rtc_set_loadcount_l(wj_rtc_regs_t *rtc_base, uint32_t value)
{
    rtc_base->RTC_CLR_L = value;
}

static inline uint32_t wj_rtc_get_intr_en_sta(wj_rtc_regs_t *rtc_base)
{
    return ((rtc_base->RTC_CCR & WJ_RTC_CCR_INTREN_EN) ? (uint32_t)1 : (uint32_t)0);
}

static inline void wj_rtc_intr_enable(wj_rtc_regs_t *rtc_base)
{
    rtc_base->RTC_CCR |= WJ_RTC_CCR_INTREN_EN;
}

static inline void wj_rtc_intr_disable(wj_rtc_regs_t *rtc_base)
{
    rtc_base->RTC_CCR &= ~WJ_RTC_CCR_INTREN_EN;
}

static inline uint32_t wj_rtc_get_mask_sta(wj_rtc_regs_t *rtc_base)
{
    return ((rtc_base->RTC_CCR & WJ_RTC_CCR_INTRMASK_EN) ? (uint32_t)1 : (uint32_t)0);
}

static inline void wj_rtc_mask_enable(wj_rtc_regs_t *rtc_base)
{
    rtc_base->RTC_CCR |= WJ_RTC_CCR_INTRMASK_EN;
}

static inline void wj_rtc_mask_disable(wj_rtc_regs_t *rtc_base)
{
    rtc_base->RTC_CCR &= ~WJ_RTC_CCR_INTRMASK_EN;
}
static inline uint32_t wj_rtc_get_counter_sta(wj_rtc_regs_t *rtc_base)
{
    return ((rtc_base->RTC_CCR & WJ_RTC_CCR_COUTEN_EN) ? (uint32_t)1 : (uint32_t)0);
}

static inline void wj_rtc_counter_enable(wj_rtc_regs_t *rtc_base)
{
    rtc_base->RTC_CCR |= WJ_RTC_CCR_COUTEN_EN;
}

static inline void wj_rtc_counter_disable(wj_rtc_regs_t *rtc_base)
{
    rtc_base->RTC_CCR &= ~WJ_RTC_CCR_COUTEN_EN;
}

static inline uint32_t wj_rtc_get_wrap_sta(wj_rtc_regs_t *rtc_base)
{
    return ((rtc_base->RTC_CCR & WJ_RTC_CCR_WRAPEN_EN) ? (uint32_t)1 : (uint32_t)0);
}

static inline void wj_rtc_wrap_enable(wj_rtc_regs_t *rtc_base)
{
    rtc_base->RTC_CCR |= WJ_RTC_CCR_WRAPEN_EN;
}

static inline void wj_rtc_wrap_disable(wj_rtc_regs_t *rtc_base)
{
    rtc_base->RTC_CCR &= ~WJ_RTC_CCR_WRAPEN_EN;
}

static inline uint32_t wj_rtc_get_intr_sta(wj_rtc_regs_t *rtc_base)
{
    return ((rtc_base->RTC_STAT & WJ_RTC_STAT_INTSTA_EN) ? (uint32_t)1 : (uint32_t)0);
}

static inline uint32_t wj_rtc_get_intr_rsta(wj_rtc_regs_t *rtc_base)
{
    return ((rtc_base->RTC_RSTAT & WJ_RTC_RSTAT_INTRSTA_EN) ? (uint32_t)1 : (uint32_t)0);
}

static inline uint32_t wj_rtc_clear_intr_sta(wj_rtc_regs_t *rtc_base)
{
    return ((rtc_base->RTC_EOI & WJ_RTC_EOI_RDCLEAR_EN) ? (uint32_t)1 : (uint32_t)0);
}

static inline uint32_t wj_rtc_get_comp_vers(wj_rtc_regs_t *rtc_base)
{
    return (rtc_base->RTC_COMP_VERSION);
}

static inline uint32_t wj_rtc_get_currcount_h(wj_rtc_regs_t *rtc_base)
{
    return (rtc_base->RTC_CCVR_H);
}

static inline uint32_t wj_rtc_get_matchcount_h(wj_rtc_regs_t *rtc_base)
{
    return (rtc_base->RTC_CMR_H);
}
static inline void wj_rtc_set_matchcount_h(wj_rtc_regs_t *rtc_base, uint32_t value)
{
    rtc_base->RTC_CMR_H = value;
}
static inline uint32_t wj_rtc_get_loadcount_h(wj_rtc_regs_t *rtc_base)
{
    return (rtc_base->RTC_CLR_H);
}
static inline void wj_rtc_set_loadcount_h(wj_rtc_regs_t *rtc_base, uint32_t value)
{
    rtc_base->RTC_CLR_H = value;
}

static inline void wj_rtc_reset_register(wj_rtc_regs_t *rtc_base)
{
    rtc_base->RTC_CMR_L = 0U;
    rtc_base->RTC_CLR_L = 0U;
    rtc_base->RTC_CCR = 0U;
    rtc_base->RTC_CCVR_H = 0U;
    rtc_base->RTC_CMR_H = 0U;
    rtc_base->RTC_CMR_L = 0U;
}

uint64_t    wj_rtc_read_current_count(wj_rtc_regs_t *rtc_base);

uint64_t    wj_rtc_read_load_count(wj_rtc_regs_t *rtc_base);

uint64_t    wj_rtc_read_match_count(wj_rtc_regs_t *rtc_base);

void        wj_rtc_write_load_count(wj_rtc_regs_t *rtc_base, uint64_t count);

void        wj_rtc_write_match_count(wj_rtc_regs_t *rtc_base, uint64_t count);

uint64_t    wj_rtc_read_runningtime(wj_rtc_regs_t *rtc_base);

uint64_t    wj_rtc_read_remaincount(wj_rtc_regs_t *rtc_base);

uint64_t    wj_rtc_read_loadtime(wj_rtc_regs_t *rtc_base);

uint64_t    wj_rtc_read_matchtime(wj_rtc_regs_t *rtc_base);

int32_t     wj_rtc_write_loadtime(wj_rtc_regs_t *rtc_base, uint64_t settime);

void        wj_rtc_write_matchtime(wj_rtc_regs_t *rtc_base, uint64_t settime);

#ifdef __cplusplus
}
#endif

#endif /* _WJ_RTC_LL_H_ */
