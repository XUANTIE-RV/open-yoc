 /*
 * Copyright (C) 2017-2024 Alibaba Group Holding Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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
