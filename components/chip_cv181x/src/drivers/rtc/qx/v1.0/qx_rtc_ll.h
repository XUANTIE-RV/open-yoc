/*
 *	Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/*******************************************************
 * @file       qx_rtc_ll.h
 * @brief      header file for rtc ll driver
 * @version    V1.0
 * @date       23. Sep 2020
 * ******************************************************/

#ifndef _QX_RTC_LL_H_
#define _QX_RTC_LL_H_

#include <stdio.h>
#include <soc.h>

#ifdef	__cplusplus
extern "C" {
#endif

#define QX_RTC_CAL_WEEK_TO_SECOND                        ((7U * 24U) * 3600U)

#define QX_RTC_WAIT_WRITED_10S                           (10000U)

#define QX_RTC_WAIT_IS_TIMEOUT(_time_ms_, _result_)      {        \
        do {                                                      \
            if (_time_ms_ >= QX_RTC_WAIT_WRITED_10S) {            \
                _result_ = -1;                                    \
            }                                                     \
            mdelay(1U);                                           \
        } while(0);                                               \
    }

/* RTC control Register, 	offset: 0x00 */
#define RTC_RESET_Pos                               (2U)
#define RTC_RESET_Msk                               (0x1U << RTC_RESET_Pos)
#define RTC_RESET_EN                                RTC_RESET_Msk

#define RTC_LATCH_Pos                               (1U)
#define RTC_LATCH_Msk                               (0x1U << RTC_LATCH_Pos)
#define RTC_LATCH_EN                                RTC_LATCH_Msk

/* RTC update state Register, 	offset: 0x04 */
#define RTC_WEEK_UPDATE_Pos                         (3U)
#define RTC_WEEK_UPDATE_Msk                         (0x1U << RTC_WEEK_UPDATE_Pos)
#define RTC_WEEK_UPDATE_STA                          RTC_WEEK_UPDATE_Msk

#define RTC_SECOND_UPDATE_Pos                       (2U)
#define RTC_SECOND_UPDATE_Msk                       (0x1U << RTC_SECOND_UPDATE_Pos)
#define RTC_SECOND_UPDATE_STA                       RTC_SECOND_UPDATE_Msk

#define RTC_SUBSECOND_UPDATE_Pos                    (1U)
#define RTC_SUBSECOND_UPDATE_Msk                    (0x1U << RTC_SUBSECOND_UPDATE_Pos)
#define RTC_SUBSECOND_UPDATE_STA                    RTC_SUBSECOND_UPDATE_Msk

#define RTC_ADJUST_UPDATE_Pos                       (0U)
#define RTC_ADJUST_UPDATE_Msk                       (0x1U << RTC_ADJUST_UPDATE_Pos)
#define RTC_ADJUST_UPDATE_STA                       RTC_ADJUST_UPDATE_Msk

/* RTC int enable Register, 	offset: 0x34 */

#define RTC_INT_ENABLE_Pos                          (0U)
#define RTC_INT_ENABLE_Msk                          (0x1U << RTC_INT_ENABLE_Pos)
#define RTC_INT_ENABLE_STA                          RTC_INT_ENABLE_Msk

#define RTC_INT_FLAG_Pos                            (0U)
#define RTC_INT_FLAG_Msk                            (0x1U << RTC_INT_FLAG_Pos)
#define RTC_INT_FLAG_CLEAN                          RTC_INT_FLAG_Msk

typedef struct {
    volatile  uint32_t    RTC_CTRL;                      /* Offset: 0x000 ( /W)  control register  */
    volatile  uint32_t    RTC_UDATE_STATE;               /* Offset: 0x004 (R/ )  current update state register */
    volatile  uint32_t    RTC_CURRENT_SUBSECOND;         /* Offset: 0x008 (R/W)  current subsecond register    */
    volatile  uint32_t    RTC_CURRENT_SECOND;            /* Offset: 0x00c (R/W)  current second register       */
    volatile  uint32_t    RTC_CURRENT_WEEK;              /* Offset: 0x010 (R/W)  current week register         */
    volatile  uint32_t    RTC_FREQ_ADJUST;               /* Offset: 0x014 (R/W)  frequecy adjust register      */
    volatile  uint32_t    RTC_SUBSECOND_ADJUST;          /* Offset: 0x018 ( /W)  subsecond adjust register     */
    volatile  uint32_t    RTC_SUBSECOND_LATCH_CPU;       /* Offset: 0x01c (R/ )  subsecond latch register      */
    volatile  uint32_t    RTC_SECOND_LATCH_CPU;          /* Offset: 0x020 (R/ )  second latch register         */
    volatile  uint32_t    RTC_WEEK_LATCH_CPU;            /* Offset: 0x024 (R/ )  week latch register           */
    volatile  uint32_t    RTC_SUBSECOND_LATCH_EXT;       /* Offset: 0x028 (R/ )  subsecond latch ext register  */
    volatile  uint32_t    RTC_SECOND_LATCH_EXT;          /* Offset: 0x02c (R/ )  second latch ext register     */
    volatile  uint32_t    RTC_WEEK_LATCH_EXT;            /* Offset: 0x030 (R/ )  week latch ext register       */
    volatile  uint32_t    RTC_INT_ENABLE;                /* Offset: 0x034 (R/W)  int enable register           */
    volatile  uint32_t    RTC_INT_FLAG;                  /* Offset: 0x038 (R/W)  int flag register             */
} qx_rtc_regs_t;


static inline void rtc_reset(qx_rtc_regs_t *rtc_base)
{
    rtc_base->RTC_CTRL |= RTC_RESET_EN;
}

static inline void rtc_dereset(qx_rtc_regs_t *rtc_base)
{
    rtc_base->RTC_CTRL &= ~RTC_RESET_EN;
}

static inline void rtc_latch(qx_rtc_regs_t *rtc_base)
{
    rtc_base->RTC_CTRL |= RTC_LATCH_EN;
}

static inline uint32_t rtc_get_weekupdate_sta(qx_rtc_regs_t *rtc_base)
{
    return ((rtc_base->RTC_UDATE_STATE & RTC_WEEK_UPDATE_STA) ? (uint32_t)1 : (uint32_t)0);
}

static inline uint32_t rtc_get_secondupdate_sta(qx_rtc_regs_t *rtc_base)
{
    return ((rtc_base->RTC_UDATE_STATE & RTC_SECOND_UPDATE_STA) ? (uint32_t)1 : (uint32_t)0);
}

static inline uint32_t rtc_get_subsecondupdate_sta(qx_rtc_regs_t *rtc_base)
{
    return ((rtc_base->RTC_UDATE_STATE & RTC_SUBSECOND_UPDATE_STA) ? (uint32_t)1 : (uint32_t)0);
}

static inline uint32_t rtc_get_adjustupdate_sta(qx_rtc_regs_t *rtc_base)
{
    return ((rtc_base->RTC_UDATE_STATE & RTC_ADJUST_UPDATE_STA) ? (uint32_t)1 : (uint32_t)0);
}

static inline uint32_t rtc_get_current_subsecond(qx_rtc_regs_t *rtc_base)
{
    return rtc_base->RTC_CURRENT_SUBSECOND;
}

static inline uint32_t rtc_get_current_second(qx_rtc_regs_t *rtc_base)
{
    return rtc_base->RTC_CURRENT_SECOND;
}

static inline uint32_t rtc_get_current_week(qx_rtc_regs_t *rtc_base)
{
    return rtc_base->RTC_CURRENT_WEEK;
}

static inline void rtc_set_current_subsecond(qx_rtc_regs_t *rtc_base, uint32_t set_time)
{
    rtc_base->RTC_CURRENT_SUBSECOND = set_time;
}

static inline void rtc_set_current_second(qx_rtc_regs_t *rtc_base, uint32_t set_time)
{
    rtc_base->RTC_CURRENT_SECOND = set_time;
}

static inline void rtc_set_current_week(qx_rtc_regs_t *rtc_base, uint32_t set_time)
{
    rtc_base->RTC_CURRENT_WEEK = set_time;
}

static inline void rtc_set_freq_adjust(qx_rtc_regs_t *rtc_base, uint32_t set_time)
{
    rtc_base->RTC_FREQ_ADJUST = set_time;
}

static inline uint32_t rtc_get_freq_adjust(qx_rtc_regs_t *rtc_base)
{
    return rtc_base->RTC_FREQ_ADJUST;
}

static inline void rtc_set_subsecond_adjust(qx_rtc_regs_t *rtc_base, uint32_t set_time)
{
    rtc_base->RTC_SUBSECOND_ADJUST = set_time;
}

static inline uint32_t rtc_get_subsecond_adjust(qx_rtc_regs_t *rtc_base)
{
    return rtc_base->RTC_SUBSECOND_ADJUST;
}

static inline uint32_t rtc_get_latch_subsecond(qx_rtc_regs_t *rtc_base)
{
    return rtc_base->RTC_SUBSECOND_LATCH_CPU;
}

static inline uint32_t rtc_get_latch_second(qx_rtc_regs_t *rtc_base)
{
    return rtc_base->RTC_SECOND_LATCH_CPU;
}

static inline uint32_t rtc_get_latch_week(qx_rtc_regs_t *rtc_base)
{
    return rtc_base->RTC_WEEK_LATCH_CPU;
}

static inline uint32_t rtc_get_subsecond_ext(qx_rtc_regs_t *rtc_base)
{
    return rtc_base->RTC_SUBSECOND_LATCH_EXT;
}

static inline uint32_t rtc_get_second_ext(qx_rtc_regs_t *rtc_base)
{
    return rtc_base->RTC_SECOND_LATCH_EXT;
}

static inline uint32_t rtc_get_week_ext(qx_rtc_regs_t *rtc_base)
{
    return rtc_base->RTC_WEEK_LATCH_EXT;
}

static inline void qx_rtc_intr_enable(qx_rtc_regs_t *rtc_base)
{
    rtc_base->RTC_INT_ENABLE |= RTC_INT_ENABLE_STA;
}

static inline void qx_rtc_intr_disable(qx_rtc_regs_t *rtc_base)
{
    rtc_base->RTC_INT_ENABLE &= ~RTC_INT_ENABLE_STA;
}

static inline uint32_t qx_rtc_get_intr_sta(qx_rtc_regs_t *rtc_base)
{
    return rtc_base->RTC_INT_FLAG;
}

static inline void qx_rtc_clear_intr_sta(qx_rtc_regs_t *rtc_base)
{
    rtc_base->RTC_INT_FLAG |= RTC_INT_FLAG_CLEAN;
}

static inline void qx_rtc_counter_enable(qx_rtc_regs_t *rtc_base)
{
    ;
}

static inline uint32_t qx_rtc_get_counter_sta(qx_rtc_regs_t *rtc_base)
{
    return 1U;
}

static inline void qx_rtc_reset_register(qx_rtc_regs_t *rtc_base)
{
    rtc_base->RTC_CTRL = 0U;
    rtc_base->RTC_CURRENT_SUBSECOND = 0U;
    rtc_base->RTC_CURRENT_SECOND = 0U;
    rtc_base->RTC_CURRENT_WEEK = 0U;
    rtc_base->RTC_FREQ_ADJUST = 0U;
    rtc_base->RTC_SUBSECOND_ADJUST = 0U;
    rtc_base->RTC_INT_ENABLE = 0U;
    rtc_base->RTC_INT_FLAG = 0U;
}

uint32_t    qx_rtc_read_current_count(qx_rtc_regs_t *rtc_base);

uint32_t    qx_rtc_read_load_count(qx_rtc_regs_t *rtc_base);

uint32_t    qx_rtc_read_match_count(qx_rtc_regs_t *rtc_base);

void        qx_rtc_write_load_count(qx_rtc_regs_t *rtc_base, uint32_t count);

void        qx_rtc_write_match_count(qx_rtc_regs_t *rtc_base, uint32_t count);

uint32_t    qx_rtc_read_runningtime(qx_rtc_regs_t *rtc_base);

uint32_t    qx_rtc_read_alarmcount(qx_rtc_regs_t *rtc_base);

uint32_t    qx_rtc_read_loadtime(qx_rtc_regs_t *rtc_base);

uint32_t    qx_rtc_read_matchtime(qx_rtc_regs_t *rtc_base);

int32_t     qx_rtc_write_loadtime(qx_rtc_regs_t *rtc_base, uint32_t settime);

void        qx_rtc_write_matchtime(qx_rtc_regs_t *rtc_base, uint32_t settime);

#ifdef __cplusplus
}
#endif

#endif /* _QX_RTC_LL_H_ */
