/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef HAL_CAN_IMPL_H
#define HAL_CAN_IMPL_H

#include <stdint.h>

#include <devices/driver.h>
#include <devices/can.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct can_driver {
    driver_t drv;
    int (*config)(rvm_dev_t *dev, rvm_hal_can_config_t *config);
    int (*config_get)(rvm_dev_t *dev, rvm_hal_can_config_t *config);
    int (*filter_init)(rvm_dev_t *dev, rvm_hal_can_filter_config_t *filter_config);
    int (*send)(rvm_dev_t *dev, rvm_hal_can_msg_t *msg, uint32_t timeout);
    int (*recv)(rvm_dev_t *dev, rvm_hal_can_msg_t *msg, uint32_t timeout);
    int (*set_event)(rvm_dev_t *dev, rvm_hal_can_callback callback, void *arg);
} can_driver_t;

#ifdef __cplusplus
}
#endif

#endif