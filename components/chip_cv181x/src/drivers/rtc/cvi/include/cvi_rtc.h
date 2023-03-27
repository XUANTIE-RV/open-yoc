/*
* Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
*/
#ifndef __CVI_RTC_LL_H__
#define __CVI_RTC_LL_H__

#include <hal_rtc.h>
//#define DEBUG

#define CVI_PARAM_CHK(para, err)                        \
    do                                                  \
    {                                                   \
        if ((unsigned long)para == (unsigned long)NULL) \
        {                                               \
            return (err);                               \
        }                                               \
    } while (0)

#define CVI_PARAM_CHK_NORETVAL(para)                    \
    do                                                  \
    {                                                   \
        if ((unsigned long)para == (unsigned long)NULL) \
        {                                               \
            return;                                     \
        }                                               \
    } while (0)

struct cvi_rtc {
	uint32_t irq_num;
	uintptr_t reg_base;
    void (*callback)(struct cvi_rtc *);
    void *arg;
};

typedef struct {
    int tm_sec;             ///< Second.      [0-59]
    int tm_min;             ///< Minute.      [0-59]
    int tm_hour;            ///< Hour.        [0-23]
    int tm_mday;            ///< Day.         [1-31]
    int tm_mon;             ///< Month.       [0-11]
    int tm_year;            ///< Year-1900.   [70- ]      !NOTE:Set 100 mean 2000
    int tm_wday;            ///< Day of week. [0-6 ]      !NOTE:Set 0 mean Sunday
    int tm_yday;            ///< Days in year.[0-365]     !NOTE:Set 0 mean January 1st
} cvi_rtc_time_t;

typedef int64_t time64_t;

static inline int is_leap_year(unsigned int year)
{
	return (!(year % 4) && (year % 100)) || !(year % 400);
}

time64_t mktime64(const unsigned int year0, const unsigned int mon0,
		const unsigned int day, const unsigned int hour,
		const unsigned int min, const unsigned int sec);

time64_t rtc_tm_to_time64(const cvi_rtc_time_t *tm);
void rtc_time64_to_tm(time64_t time, cvi_rtc_time_t *tm);
int rtc_valid_tm(const cvi_rtc_time_t *tm);
int cvi_rtc_init(struct cvi_rtc *rtc, uintptr_t reg_base, uint32_t irq_num);
void cvi_rtc_uninit(void);
int cvi_rtc_set_time(struct cvi_rtc *rtc, const cvi_rtc_time_t *tm);
int cvi_rtc_set_time_no_wait(struct cvi_rtc*rtc, const cvi_rtc_time_t *tm);
int cvi_rtc_get_time(struct cvi_rtc *rtc, cvi_rtc_time_t *tm);
uint32_t cvi_rtc_get_alarm_remaining_time(struct cvi_rtc *rtc);
int cvi_rtc_set_alarm(struct cvi_rtc *rtc, const cvi_rtc_time_t *tm, void *callback, void *arg);
int cvi_rtc_cancel_alarm(struct cvi_rtc *rtc);

int cvi_rtc_get_chipsn(uint64_t *sn);


#endif
