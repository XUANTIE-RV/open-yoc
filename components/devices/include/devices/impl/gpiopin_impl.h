/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef _DEVICE_GPIOPIN_IMPL_H_
#define _DEVICE_GPIOPIN_IMPL_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <devices/driver.h>
#include <devices/gpiopin.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @brief gpiopin dirver ops */
typedef struct gpio_pin_driver {
    driver_t drv;
    int (*set_direction)(rvm_dev_t *dev, rvm_hal_gpio_dir_t dir);
    int (*set_mode)(rvm_dev_t *dev, rvm_hal_gpio_mode_t mode);
    int (*attach_callback)(rvm_dev_t *dev, rvm_hal_gpio_pin_callback callback, void *arg);
    int (*set_irq_mode)(rvm_dev_t *dev, rvm_hal_gpio_irq_mode_t irq_mode);
    int (*irq_enable)(rvm_dev_t *dev, bool enable);
    int (*set_debounce)(rvm_dev_t *dev, bool enable);
    int (*pin_write)(rvm_dev_t *dev, rvm_hal_gpio_pin_data_t value);
    int (*pin_read)(rvm_dev_t *dev, rvm_hal_gpio_pin_data_t *value);
    int (*output_toggle)(rvm_dev_t *dev);
} gpio_pin_driver_t;

#ifdef __cplusplus
}
#endif

#endif
