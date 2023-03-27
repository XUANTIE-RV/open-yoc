/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef _DEVICE_CLK_IMPL_H_
#define _DEVICE_CLK_IMPL_H_

#include <stdio.h>
#include <stdlib.h>

#include <devices/driver.h>
#include <devices/clk.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @brief clk dirver ops */
typedef struct clk_driver {
    driver_t drv;
    int (*enable)(rvm_dev_t *dev, const char *dev_name);
    int (*disable)(rvm_dev_t *dev, const char *dev_name);
    int (*get_freq)(rvm_dev_t *dev, rvm_hal_clk_id_t clk_id, uint32_t idx, uint32_t *freq);
    int (*set_freq)(rvm_dev_t *dev, rvm_hal_clk_id_t clk_id, uint32_t idx, uint32_t freq);
} clk_driver_t;

#ifdef __cplusplus
}
#endif

#endif