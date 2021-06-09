/*
 *    Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/*******************************************************
 * @file     wj_rtc_ll.c
 * @brief    source file for rtc ll driver
 * @version  V2.0
 * @date     30. July 2020
 * ******************************************************/
#include <drv/rtc.h>
#include "rtc_alg.h"
#include "wj_rtc_ll.h"

/* ########################## Chip IP Core Selection ############################## */
/**
  * This is the list of ip core to be used different clocks in the csi driver
  *
  * ----------------FAMILY-A----------------··
  * CH2201 / PANGU
  * ----------------FAMILY-B----------------
  * PHOBOS
  * ----------------FAMILY-C----------------
  * CH6101
  * ----------------FAMILY-D----------------
  * DANICA / MERAK  / DUBHE  /
  * MIZAR  / ALKAID / ALKAID
  * ----------------FAMILY-E----------------
  * OTHER
  * --------------------------------------
  */
#ifdef CONFIG_RTC_FAMILY_A
#define WJ_RTC_CONVERT_CALENDAR2TIME(_val_, _rtc_base_)  (((uint64_t)_val_ * (uint64_t)soc_get_rtc_freq(0U)) / 1000ULL)
#define WJ_RTC_CONVERT_TIME2CALENDAR(_val_, _rtc_base_)  (((uint64_t)_val_ * 1000ULL) / (uint64_t)soc_get_rtc_freq(0U))
#endif

#ifdef CONFIG_RTC_FAMILY_B
#define WJ_RTC_CONVERT_CALENDAR2TIME(_val_, _rtc_base_)  (((uint64_t)_val_ * ((uint64_t)soc_get_rtc_freq(0U) / 1000ULL)) / 16384ULL)
#define WJ_RTC_CONVERT_TIME2CALENDAR(_val_, _rtc_base_)  (((uint64_t)_val_ * 16384ULL) / ((uint64_t)soc_get_rtc_freq(0U) / 1000ULL))
#endif

#ifdef CONFIG_RTC_FAMILY_C
#define WJ_RTC_GET_DIVIDER_VALUE(_rtc_base_)             ((wj_rtc_get_divider(_rtc_base_) + (uint32_t)1U) << 1)
#define WJ_RTC_CONVERT_CALENDAR2TIME(_val_, _rtc_base_)  (((uint64_t)_val_ * ((uint64_t)27373ULL << 1)) / ((uint64_t)WJ_RTC_GET_DIVIDER_VALUE(_rtc_base_) * 1000ULL))
#define WJ_RTC_CONVERT_TIME2CALENDAR(_val_, _rtc_base_)  (((uint64_t)_val_ * ((uint64_t)WJ_RTC_GET_DIVIDER_VALUE(_rtc_base_) * 1000ULL)) / ((uint64_t)27373ULL << 1))
#endif

#ifdef CONFIG_RTC_FAMILY_D
#define WJ_RTC_GET_DIVIDER_VALUE(_rtc_base_)             ((wj_rtc_get_divider(_rtc_base_) + (uint32_t)1U) << 1)
#define WJ_RTC_CONVERT_CALENDAR2TIME(_val_, _rtc_base_)  (((uint64_t)_val_ * (((uint64_t)soc_get_rtc_freq(0U) << 1) / (uint64_t)WJ_RTC_GET_DIVIDER_VALUE(_rtc_base_))) / 1000ULL)
#define WJ_RTC_CONVERT_TIME2CALENDAR(_val_, _rtc_base_)  (((uint64_t)_val_ * 1000ULL) / (((uint64_t)soc_get_rtc_freq(0U) << 1) / (uint64_t)WJ_RTC_GET_DIVIDER_VALUE(_rtc_base_)))
#endif

#ifdef CONFIG_RTC_FAMILY_E
#define WJ_RTC_GET_DIVIDER_VALUE(_rtc_base_)             ((wj_rtc_get_divider(_rtc_base_) + (uint32_t)1U) << 1)
#define WJ_RTC_CONVERT_CALENDAR2TIME(_val_, _rtc_base_)  (((uint64_t)_val_ * ((uint64_t)soc_get_rtc_freq(0U) / (uint64_t)WJ_RTC_GET_DIVIDER_VALUE(_rtc_base_))) / 1000ULL)
#define WJ_RTC_CONVERT_TIME2CALENDAR(_val_, _rtc_base_)  (((uint64_t)_val_ * 1000ULL) / ((uint64_t)soc_get_rtc_freq(0U) / (uint64_t)WJ_RTC_GET_DIVIDER_VALUE(_rtc_base_)))
#endif

#define WJ_RTC_WAIT_WRITED_10S                           (10000U)

#define WJ_RTC_WAIT_IS_TIMEOUT(_time_ms_, _result_)      {        \
        do {                                                      \
            if (_time_ms_ >= WJ_RTC_WAIT_WRITED_10S) {            \
                _result_ = -1;                                    \
            }                                                     \
            mdelay(1U);                                           \
        } while(0);                                               \
    }

uint32_t wj_rtc_read_current_count(wj_rtc_regs_t *rtc_base)
{
    return wj_rtc_get_currcount(rtc_base);
}
uint32_t wj_rtc_read_load_count(wj_rtc_regs_t *rtc_base)
{
    return wj_rtc_get_loadcount(rtc_base);
}
uint32_t wj_rtc_read_match_count(wj_rtc_regs_t *rtc_base)
{
    return wj_rtc_get_matchcount(rtc_base);
}

void wj_rtc_write_load_count(wj_rtc_regs_t *rtc_base, uint32_t count)
{
    wj_rtc_set_loadcount(rtc_base, count);
}
void wj_rtc_write_match_count(wj_rtc_regs_t *rtc_base, uint32_t count)
{
    wj_rtc_set_matchcount(rtc_base, count);
}

uint32_t wj_rtc_read_runningtime(wj_rtc_regs_t *rtc_base)
{
    uint32_t currecount;
    uint32_t loadcount;
    uint64_t retime = 0U;

    currecount = wj_rtc_read_current_count(rtc_base);
    loadcount = wj_rtc_read_load_count(rtc_base);

    if (currecount > loadcount) {
        currecount -= loadcount;
        retime = WJ_RTC_CONVERT_TIME2CALENDAR((uint64_t)currecount, (wj_rtc_regs_t *)rtc_base);
    }

    return (uint32_t)retime;
}

uint32_t wj_rtc_read_alarmcount(wj_rtc_regs_t *rtc_base)
{
    uint32_t matchcount;
    uint32_t currecount;
    uint64_t retime = 0U;

    matchcount = wj_rtc_read_match_count(rtc_base);
    currecount = wj_rtc_read_current_count(rtc_base);

    if (matchcount > currecount) {
        matchcount -= currecount;
        retime = WJ_RTC_CONVERT_TIME2CALENDAR((uint64_t)matchcount, (wj_rtc_regs_t *)rtc_base);
    }

    return (uint32_t)retime;
}

int32_t wj_rtc_write_loadtime(wj_rtc_regs_t *rtc_base, uint32_t settime)
{
    int32_t ret = 0;
    uint32_t loopcount = 0U;
    uint32_t timecount = 0U;
    uint32_t currcount = 0U;

    wj_rtc_write_load_count(rtc_base, settime);

    do {
        loopcount++;

        if (loopcount > 10U) {
            loopcount = 0U;
            wj_rtc_write_load_count(rtc_base, settime + 1U);     ///< STEP 1: Change the setting value and write again
            udelay(500U);                                        ///< STEP 2: Delay 500us waiting for hardware writing
            wj_rtc_write_load_count(rtc_base, settime);          ///< STEP 3: Rewrite correct settings
        }

        currcount = wj_rtc_read_current_count(rtc_base);

        WJ_RTC_WAIT_IS_TIMEOUT(++timecount, ret);

        if (ret < 0) {
            break;
        }
    } while (currcount != settime);

    return ret;
}

void wj_rtc_write_loadtime_no_wait(wj_rtc_regs_t *rtc_base, uint32_t settime)
{
    wj_rtc_write_load_count(rtc_base, settime);
}

void wj_rtc_write_matchtime(wj_rtc_regs_t *rtc_base, uint32_t settime)
{
    uint64_t matchcount;

    matchcount  = WJ_RTC_CONVERT_CALENDAR2TIME((uint64_t)settime, (wj_rtc_regs_t *)rtc_base);

    matchcount += (uint64_t)wj_rtc_read_current_count(rtc_base);

    wj_rtc_write_match_count(rtc_base, (uint32_t)matchcount);
}
