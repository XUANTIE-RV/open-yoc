/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef HAL_I2S_IMPL_H
#define HAL_I2S_IMPL_H

#include <stdint.h>

#include <devices/driver.h>
#include <devices/i2s.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct i2s_driver {
    driver_t drv;
    int (*config)(rvm_dev_t *dev, rvm_hal_i2s_config_t *config);
    int (*config_get)(rvm_dev_t *dev, rvm_hal_i2s_config_t *config);
    int (*send)(rvm_dev_t *dev, const void *data, size_t size, uint32_t timeout);
    int (*recv)(rvm_dev_t *dev, void *data, size_t size, uint32_t timeout);
    int (*pause)(rvm_dev_t *dev);
    int (*resume)(rvm_dev_t *dev);
    int (*stop)(rvm_dev_t *dev);
} i2s_driver_t;

#ifdef __cplusplus
}
#endif

#endif