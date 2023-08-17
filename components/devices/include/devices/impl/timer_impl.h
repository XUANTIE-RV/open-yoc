/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef HAL_TIMER_IMPL_H
#define HAL_TIMER_IMPL_H

#include <stdint.h>

#include <devices/timer.h>
#include <devices/driver.h>


#ifdef __cplusplus
extern "C" {
#endif

typedef struct timer_driver {
    driver_t drv;
    int (*start)(rvm_dev_t *dev, uint32_t timeout_us);
    int (*oneshot_start)(rvm_dev_t *dev, uint32_t timeout_us);
    int (*stop)(rvm_dev_t *dev);
    int (*attach_callback)(rvm_dev_t *dev, rvm_hal_timer_callback callback, void *arg);
    int (*detach_callback)(rvm_dev_t *dev);
    uint32_t (*get_remaining_value)(rvm_dev_t *dev);
    uint32_t (*get_load_value)(rvm_dev_t *dev);
} timer_driver_t;

#ifdef __cplusplus
}
#endif

#endif
