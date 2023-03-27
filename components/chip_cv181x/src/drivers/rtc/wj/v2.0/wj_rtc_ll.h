/*
 *	Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/*******************************************************
 * @file       wj_rtc_ll.h
 * @brief      Header File for RTC ll Driver
 * @version    V2.0
 * @date       12. Oct 2020
 * @model      rtc
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
    __IOM uint32_t    RTC_DIV;                      /* Offset: 0x020 (R/W)  frequency division register */
} wj_rtc_regs_t;


static inline uint32_t wj_rtc_get_currcount(wj_rtc_regs_t *rtc_base)
{
    return (rtc_base->RTC_CCVR_L);
}

static inline uint32_t wj_rtc_get_matchcount(wj_rtc_regs_t *rtc_base)
{
    return (rtc_base->RTC_CMR_L);
}
static inline void wj_rtc_set_matchcount(wj_rtc_regs_t *rtc_base, uint32_t value)
{
    rtc_base->RTC_CMR_L = value;
}

static inline uint32_t wj_rtc_get_loadcount(wj_rtc_regs_t *rtc_base)
{
    return (rtc_base->RTC_CLR_L);
}
static inline void wj_rtc_set_loadcount(wj_rtc_regs_t *rtc_base, uint32_t value)
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

static inline uint32_t wj_rtc_get_divider(wj_rtc_regs_t *rtc_base)
{
    return (rtc_base->RTC_DIV);
}

static inline void wj_rtc_reset_register(wj_rtc_regs_t *rtc_base)
{
    rtc_base->RTC_CMR_L = 0U;
    rtc_base->RTC_CLR_L = 0U;
    rtc_base->RTC_CCR = 0U;
    rtc_base->RTC_DIV = 0U;
}

uint32_t    wj_rtc_read_current_count(wj_rtc_regs_t *rtc_base);

uint32_t    wj_rtc_read_load_count(wj_rtc_regs_t *rtc_base);

uint32_t    wj_rtc_read_match_count(wj_rtc_regs_t *rtc_base);

void        wj_rtc_write_load_count(wj_rtc_regs_t *rtc_base, uint32_t count);

void        wj_rtc_write_match_count(wj_rtc_regs_t *rtc_base, uint32_t count);

uint32_t    wj_rtc_read_runningtime(wj_rtc_regs_t *rtc_base);

uint32_t    wj_rtc_read_alarmcount(wj_rtc_regs_t *rtc_base);

uint32_t    wj_rtc_read_loadtime(wj_rtc_regs_t *rtc_base);

uint32_t    wj_rtc_read_matchtime(wj_rtc_regs_t *rtc_base);

int32_t     wj_rtc_write_loadtime(wj_rtc_regs_t *rtc_base, uint32_t settime);

void        wj_rtc_write_matchtime(wj_rtc_regs_t *rtc_base, uint32_t settime);

#ifdef __cplusplus
}
#endif

#endif /* _WJ_RTC_LL_H_ */
