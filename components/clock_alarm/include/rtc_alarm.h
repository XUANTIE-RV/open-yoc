/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#ifndef _RTC_ALARM_H_
#define _RTC_ALARM_H_

#include <stdbool.h>
#include <stdint.h>

/**
 * 系统时间同步到rtc
 *
 * @param[in]  无
 *
 * @return  无.
 */
void rtc_from_system(void);

/**
 * rtc时间同步到系统
 *
 * @param[in]  无
 *
 * @return  rtc绝对时间.
 */
time_t rtc_to_system(void);

/**
 * 获取rtc的绝对时间
 *
 * @param[in]  无
 *
 * @return  rtc绝对时间.
 */
time_t rtc_get_time(void);

/**
 * 设置及使能rtc
 *
 * @param[in]  week
 * 
 *
 * @return  无.
 */
void rtc_set_alarm(int32_t week, int32_t hour, int32_t min, int32_t sec);

#endif