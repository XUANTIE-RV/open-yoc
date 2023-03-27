/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef _DEVICE_WDT_IMPL_H_
#define _DEVICE_WDT_IMPL_H_

#include <stdio.h>
#include <stdlib.h>

#include <devices/driver.h>
#include <devices/wdt.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @brief wdt dirver ops */
typedef struct wdt_driver {
    driver_t drv;
    int (*set_timeout)(rvm_dev_t *dev, uint32_t ms);
    int (*start)(rvm_dev_t *dev);
    int (*stop)(rvm_dev_t *dev);
    int (*feed)(rvm_dev_t *dev);
    uint32_t (*get_remaining_time)(rvm_dev_t *dev);
    int (*attach_callback)(rvm_dev_t *dev, rvm_hal_wdt_callback callback, void *arg);
    int (*detach_callback)(rvm_dev_t *dev);
} wdt_driver_t;

#ifdef __cplusplus
}
#endif

#endif
