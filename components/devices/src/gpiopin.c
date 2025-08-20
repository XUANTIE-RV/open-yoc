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

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dev_internal.h>
#include <devices/impl/gpiopin_impl.h>

#define GPIOPIN_DRIVER(dev)  ((gpio_pin_driver_t*)(dev->drv))
#define GPIOPIN_VAILD(dev) do { \
    if (device_valid(dev, "gpio_pin") != 0) \
        return -1; \
} while(0)

rvm_dev_t *rvm_hal_gpio_pin_open_by_pin_name(const char *name, int pin_name)
{
    if (!name) {
        return NULL;
    }
    return device_open_id(name, pin_name);
}

int rvm_hal_gpio_pin_attach_callback(rvm_dev_t *dev, rvm_hal_gpio_pin_callback callback, void *arg)
{
    int ret;

    GPIOPIN_VAILD(dev);

    device_lock(dev);
    ret = GPIOPIN_DRIVER(dev)->attach_callback(dev, callback, arg);
    device_unlock(dev);

    return ret;
}

int rvm_hal_gpio_pin_set_direction(rvm_dev_t *dev, rvm_hal_gpio_dir_t dir)
{
    int ret;

    GPIOPIN_VAILD(dev);

    device_lock(dev);
    ret = GPIOPIN_DRIVER(dev)->set_direction(dev, dir);
    device_unlock(dev);

    return ret;
}

int rvm_hal_gpio_pin_set_mode(rvm_dev_t *dev, rvm_hal_gpio_mode_t mode)
{
    int ret;

    GPIOPIN_VAILD(dev);

    device_lock(dev);
    ret = GPIOPIN_DRIVER(dev)->set_mode(dev, mode);
    device_unlock(dev);

    return ret;
}

int rvm_hal_gpio_pin_set_irq_mode(rvm_dev_t *dev, rvm_hal_gpio_irq_mode_t irq_mode)
{
    int ret;

    GPIOPIN_VAILD(dev);

    device_lock(dev);
    ret = GPIOPIN_DRIVER(dev)->set_irq_mode(dev, irq_mode);
    device_unlock(dev);

    return ret;
}

int rvm_hal_gpio_pin_irq_enable(rvm_dev_t *dev, bool enable)
{
    int ret;

    GPIOPIN_VAILD(dev);

    device_lock(dev);
    ret = GPIOPIN_DRIVER(dev)->irq_enable(dev, enable);
    device_unlock(dev);

    return ret;
}

int rvm_hal_gpio_pin_set_debounce(rvm_dev_t *dev, bool enable)
{
    int ret;

    GPIOPIN_VAILD(dev);

    device_lock(dev);
    ret = GPIOPIN_DRIVER(dev)->set_debounce(dev, enable);
    device_unlock(dev);

    return ret;
}

int rvm_hal_gpio_pin_write(rvm_dev_t *dev, rvm_hal_gpio_pin_data_t value)
{
    int ret;

    GPIOPIN_VAILD(dev);

    device_lock(dev);
    ret = GPIOPIN_DRIVER(dev)->pin_write(dev, value);
    device_unlock(dev);

    return ret;
}

int rvm_hal_gpio_pin_output_toggle(rvm_dev_t *dev)
{
    int ret;

    GPIOPIN_VAILD(dev);

    device_lock(dev);
    ret = GPIOPIN_DRIVER(dev)->output_toggle(dev);
    device_unlock(dev);

    return ret;
}

int rvm_hal_gpio_pin_read(rvm_dev_t *dev, rvm_hal_gpio_pin_data_t *value)
{
    int ret;

    GPIOPIN_VAILD(dev);

    device_lock(dev);
    ret = GPIOPIN_DRIVER(dev)->pin_read(dev, value);
    device_unlock(dev);

    return ret;
}
