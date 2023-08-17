/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef HAL_CIR_IMPL_H
#define HAL_CIR_IMPL_H

#include <stdint.h>

#include <devices/driver.h>
#include <devices/cir.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct cir_driver {
    driver_t drv;
    int (*config)(rvm_dev_t *dev, rvm_hal_cir_config_t *config);
    int (*config_get)(rvm_dev_t *dev, rvm_hal_cir_config_t *config);
    int (*send)(rvm_dev_t *dev, uint32_t scancode);
    int (*recv_start)(rvm_dev_t *dev);
    int (*set_rx_event)(rvm_dev_t *dev, rvm_hal_cir_rx_callback callback, void *arg);
    int (*get_recv_scancode)(rvm_dev_t *dev, uint32_t *scancode);
} cir_driver_t;

#ifdef __cplusplus
}
#endif

#endif