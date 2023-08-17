/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef HAL_PWM_IMPL_H
#define HAL_PWM_IMPL_H

#include <stdint.h>

#include <devices/driver.h>
#include <devices/pwm.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct pwm_driver {
    driver_t drv;
    int (*config)(rvm_dev_t *dev, rvm_hal_pwm_config_t *config, uint8_t channel);
    int (*config_get)(rvm_dev_t *dev, rvm_hal_pwm_config_t *config, uint8_t channel);
    int (*start)(rvm_dev_t *dev, uint8_t channel);
    int (*stop)(rvm_dev_t *dev, uint8_t channel);
} pwm_driver_t;

#ifdef __cplusplus
}
#endif

#endif