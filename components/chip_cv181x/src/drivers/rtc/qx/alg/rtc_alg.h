/*
 *    Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/*******************************************************
 * @file     rtc_alg.h
 * @brief    header file for rtc driver
 * @version  V1.0
 * @date     13. April 2020
 * ******************************************************/

#ifndef _RTC_ALG_H_
#define _RTC_ALG_H_

#include <time.h>
#include <drv/rtc.h>
#include <csi_config.h>

#ifdef    __cplusplus
extern "C" {
#endif

/* ########################## Date Clock Default value ############################## */

#define RTC_TIME_BASE_YEAR                          (1900)                                 //< Year,      Effective range[1970,2099]

#define RTC_TIME_MAX_VAL_YEAR                       (199)                                  ///< Year,     Maximum value
#define RTC_TIME_MAX_VAL_MON                        (11)                                   ///< Month,    Maximum value
#define RTC_TIME_MAX_VAL_DAY                        (31)                                   ///< Day,      Maximum value
#define RTC_TIME_MAX_VAL_HOUR                       (23)                                   ///< Hour,     Maximum value
#define RTC_TIME_MAX_VAL_MIN                        (59)                                   ///< Minute,   Maximum value
#define RTC_TIME_MAX_VAL_SEC                        (59)                                   ///< Second,   Maximum value

#define RTC_TIME_MIN_VAL_YEAR                       (70)                                   ///< Year,     Minimum value
#define RTC_TIME_MIN_VAL_MON                        (0)                                    ///< Month,    Minimum value    
#define RTC_TIME_MIN_VAL_DAY                        (1)                                    ///< Day,      Minimum value
#define RTC_TIME_MIN_VAL_HOUR                       (0)                                    ///< Hour,     Minimum value
#define RTC_TIME_MIN_VAL_MIN                        (0)                                    ///< Minute,   Minimum value    
#define RTC_TIME_MIN_VAL_SEC                        (0)                                    ///< Second,   Minimum value  

#define RTC_TIME_DEFAULT_VAL_YEAR                   (70)                                   ///< Year,     Dafault min value
#define RTC_TIME_DEFAULT_VAL_MON                    (0)                                    ///< Month,    Dafault min value
#define RTC_TIME_DEFAULT_VAL_DAY                    (1)                                    ///< Day,      Dafault min value
#define RTC_TIME_DEFAULT_VAL_HOUR                   (8)                                    ///< Hour,     Dafault min value
#define RTC_TIME_DEFAULT_VAL_MIN                    (0)                                    ///< Minute,   Dafault min value
#define RTC_TIME_DEFAULT_VAL_SEC                    (0)                                    ///< Second,   Dafault min value

#define RTC_TIME_TEMP_LEAP_YEAR                     (72)                                   ///< Year,     Dafault min value
#define RTC_TIME_TEMP_NONLEAP_YEAR                  (70)                                   ///< Year,     Dafault min value

/**
 * \brief Determine if it is a leap year
 * 0 - this is not leapyear 
 * 1  - this is leapyear 
*/
#define WJ_RTC_IS_LEAPYEAR(_year_)                  (((_year_) % 400) ? (((_year_) % 100) ? (((_year_) % 4) ? (int)0 : (int)1) : (int)0) : (int)1)

#define WJ_RTC_SEC_PER_MIN                          ((time_t)60)
#define WJ_RTC_SEC_PER_HOUR                         ((time_t)60 * WJ_RTC_SEC_PER_MIN)
#define WJ_RTC_SEC_PER_DAY                          ((time_t)24 * WJ_RTC_SEC_PER_HOUR)

#define WJ_RTC_PARA_UNSUPPORT                       (0)

/**
 * week:                 tm_wday:
 * 0 is Monday           0 is Sunday
 * 1 is Tuesday          1 is Monday
 * 2 is Wednesday        2 is Tuesday
 * 3 is Thursday         3 is Wednesday
 * 4 is Friday           4 is Thursday
 * 5 is Saturday         5 is Friday
 * 6 is Sunday           6 is Saturday
*/
#define WJ_RTC_CALCULATE_WEEK(_week_)               ((_week_ == 6) ? 0 : (_week_ + 1))



time_t   mktime(struct tm *tp);

struct tm *gmtime_r(const time_t *timer, struct tm *result);

int      clock_daysbeforemonth(int month, int leapyear);

time_t   clock_calendar2utc(int year, int month, int day);

void     clock_utc2calendar(time_t days, int *year, int *month, int *day);

int32_t  clock_check_tm_ok(const struct tm *rtctime);

int32_t clock_update_basetime(const struct tm *rtctime);

#ifdef __cplusplus
}
#endif

#endif /* _RTC_ALG_H_ */
