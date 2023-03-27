/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef _DEVICE_RTC_IMPL_H_
#define _DEVICE_RTC_IMPL_H_

#include <stdio.h>
#include <stdlib.h>

#include <devices/driver.h>
#include <devices/rtc.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @brief rtc dirver ops */
typedef struct rtc_driver {
    driver_t drv;
    int (*set_time)(rvm_dev_t *dev, const struct tm *time);
    int (*get_time)(rvm_dev_t *dev, struct tm *time);
    uint32_t (*get_alarm_remaining_time)(rvm_dev_t *dev);
    int (*set_alarm)(rvm_dev_t *dev, const struct tm *time, rvm_hal_rtc_callback callback, void *arg);
    int (*cancel_alarm)(rvm_dev_t *dev);
} rtc_driver_t;

#ifdef __cplusplus
}
#endif

#endif
