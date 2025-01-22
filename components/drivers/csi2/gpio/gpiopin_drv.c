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
#include <aos/kernel.h>
#include <drv/gpio_pin.h>
#include <devices/impl/gpiopin_impl.h>

#define TAG "gpiopin_drv"

typedef struct {
    rvm_dev_t device;
    csi_gpio_pin_t handle;
    void *arg;
    rvm_hal_gpio_pin_callback callback;
} gpio_pin_dev_t;

#define GPIOPINDEV(dev) ((gpio_pin_dev_t *)dev)

static rvm_dev_t *_gpio_pin_init(driver_t *drv, void *config, int id)
{
    gpio_pin_dev_t *gpio_pin = (gpio_pin_dev_t *)rvm_hal_device_new(drv, sizeof(gpio_pin_dev_t), id);

    return (rvm_dev_t *)gpio_pin;
}

#define _gpio_pin_uninit rvm_hal_device_free

static int _gpio_pin_open(rvm_dev_t *dev)
{
    csi_error_t ret = csi_gpio_pin_init(&GPIOPINDEV(dev)->handle, dev->id);

    if (ret != CSI_OK) {
        LOGE(TAG, "csi_gpio_pin_init error");
        return -1;
    }
    return 0;
}

static int _gpio_pin_close(rvm_dev_t *dev)
{
    csi_gpio_pin_uninit(&GPIOPINDEV(dev)->handle);
    return 0;
}

static int _gpio_pin_clock(rvm_dev_t *dev, bool enable)
{
    if (enable) {
        csi_clk_enable(&GPIOPINDEV(dev)->handle.gpio->dev);
    } else {
        csi_clk_disable(&GPIOPINDEV(dev)->handle.gpio->dev);
    }
    return 0;
}

static void _gpio_pin_callback(csi_gpio_pin_t *gpio_pin, void *arg)
{
    rvm_dev_t *dev = (rvm_dev_t *)arg;

    if (GPIOPINDEV(dev)->callback) {
        GPIOPINDEV(dev)->callback(dev, GPIOPINDEV(dev)->arg);
    }
}

static int _gpio_pin_attach_callback(rvm_dev_t *dev, rvm_hal_gpio_pin_callback callback, void *arg)
{
    GPIOPINDEV(dev)->arg = arg;
    GPIOPINDEV(dev)->callback = callback;
    csi_error_t ret = csi_gpio_pin_attach_callback(&GPIOPINDEV(dev)->handle, _gpio_pin_callback, dev);
    if (ret != CSI_OK) {
        LOGE(TAG, "csi_gpio_pin_attach_callback error");
        return -1;
    }
    return 0;
}

static int _gpio_pin_set_direction(rvm_dev_t *dev, rvm_hal_gpio_dir_t dir)
{
    csi_error_t ret = csi_gpio_pin_dir(&GPIOPINDEV(dev)->handle, (csi_gpio_dir_t)dir);
    if (ret != CSI_OK) {
        LOGE(TAG, "csi_gpio_pin_dir error");
        return -1;
    }
    return 0;
}

static int _gpio_pin_set_mode(rvm_dev_t *dev, rvm_hal_gpio_mode_t mode)
{
    csi_error_t ret = csi_gpio_pin_mode(&GPIOPINDEV(dev)->handle, (csi_gpio_mode_t)mode);
    if (ret != CSI_OK) {
        LOGE(TAG, "csi_gpio_pin_mode error");
        return -1;
    }
    return 0;
}

static int _gpio_pin_set_irq_mode(rvm_dev_t *dev, rvm_hal_gpio_irq_mode_t irq_mode)
{
    csi_error_t ret = csi_gpio_pin_irq_mode(&GPIOPINDEV(dev)->handle, (csi_gpio_irq_mode_t)irq_mode);
    if (ret != CSI_OK) {
        LOGE(TAG, "csi_gpio_pin_irq_mode error");
        return -1;
    }
    return 0;
}

static int _gpio_pin_irq_enable(rvm_dev_t *dev, bool enable)
{
    csi_error_t ret = csi_gpio_pin_irq_enable(&GPIOPINDEV(dev)->handle, enable);
    if (ret != CSI_OK) {
        LOGE(TAG, "csi_gpio_pin_irq_enable error");
        return -1;
    }
    return 0;
}

static int _gpio_pin_set_debounce(rvm_dev_t *dev, bool enable)
{
    csi_error_t ret = csi_gpio_pin_debounce(&GPIOPINDEV(dev)->handle, enable);
    if (ret != CSI_OK) {
        LOGE(TAG, "csi_gpio_pin_debounce error");
        return -1;
    }
    return 0;
}

static int _gpio_pin_write(rvm_dev_t *dev, rvm_hal_gpio_pin_data_t value)
{
    csi_gpio_pin_write(&GPIOPINDEV(dev)->handle, (csi_gpio_pin_state_t)value);
    return 0;
}

static int _gpio_pin_read(rvm_dev_t *dev, rvm_hal_gpio_pin_data_t *value)
{
    rvm_hal_gpio_pin_data_t val = (rvm_hal_gpio_pin_data_t)csi_gpio_pin_read(&GPIOPINDEV(dev)->handle);
    *value = val;
    return 0;
}

static int _gpio_pin_output_toggle(rvm_dev_t *dev)
{
    csi_gpio_pin_toggle(&GPIOPINDEV(dev)->handle);
    return 0;
}

static gpio_pin_driver_t gpio_pin_driver = {
    .drv = {
        .name   = "gpio_pin",
        .init   = _gpio_pin_init,
        .uninit = _gpio_pin_uninit,
        .open   = _gpio_pin_open,
        .close  = _gpio_pin_close,
        .clk_en = _gpio_pin_clock,
    },
    .attach_callback = _gpio_pin_attach_callback,
    .set_direction   = _gpio_pin_set_direction,
    .set_mode        = _gpio_pin_set_mode,
    .set_irq_mode    = _gpio_pin_set_irq_mode,
    .irq_enable      = _gpio_pin_irq_enable,
    .set_debounce    = _gpio_pin_set_debounce,
    .pin_write       = _gpio_pin_write,
    .pin_read        = _gpio_pin_read,
    .output_toggle   = _gpio_pin_output_toggle
};

void rvm_gpio_pin_drv_register(int pin_name)
{
    rvm_driver_register(&gpio_pin_driver.drv, NULL, pin_name);
}