/*
 * Copyright (C) 2019-2021 Alibaba Group Holding Limited
 */

#ifndef ___JQUICK_BASE_TIME_H__
#define ___JQUICK_BASE_TIME_H__

#ifdef __cplusplus
extern "C" {
#endif

/** get the millisecond from the system boot
 *
 */
long long jquick_get_current_time();

long long jquick_get_current_time_ns();

void jquick_sleep(int millisecond);

long long jquick_get_real_time();

#ifdef __cplusplus
}
#endif

#endif  // ___JQUICK_BASE_TIME_H__
