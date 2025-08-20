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
#include <drv/pwm.h>
#include <drv/clk.h>
#include <devices/impl/pwm_impl.h>

#define TAG "pwm_drv"

#ifdef CONFIG_PWM_MAX_CHANNEL_COUNT
#define MAX_CHANNEL_COUNT CONFIG_PWM_MAX_CHANNEL_COUNT
#else
#define MAX_CHANNEL_COUNT 6
#endif

typedef struct {
    rvm_dev_t device;
    csi_pwm_t handle;
    rvm_hal_pwm_config_t config[MAX_CHANNEL_COUNT];
} pwm_dev_t;

#define PWM(dev) ((pwm_dev_t *)dev)

static rvm_dev_t *_pwm_init(driver_t *drv, void *config, int id)
{
    pwm_dev_t *pwm = (pwm_dev_t *)rvm_hal_device_new(drv, sizeof(pwm_dev_t), id);

    return (rvm_dev_t *)pwm;
}

#define _pwm_uninit rvm_hal_device_free

static int _pwm_open(rvm_dev_t *dev)
{
    csi_error_t ret;

    ret = csi_pwm_init(&PWM(dev)->handle, dev->id);
    if (ret != CSI_OK) {
        LOGE(TAG, "csi_pwm_init error, dev->id:%d", dev->id);
        return -1;
    }
    return 0;
}

static int _pwm_close(rvm_dev_t *dev)
{
    csi_pwm_uninit(&PWM(dev)->handle);
    return 0;
}

static int _pwm_clock(rvm_dev_t *dev, bool enable)
{
    if (enable) {
        csi_clk_enable(&PWM(dev)->handle.dev);
    } else {
        csi_clk_disable(&PWM(dev)->handle.dev);
    }
    return 0;
}

static int _pwm_lpm(rvm_dev_t *dev, int state)
{
#ifdef CONFIG_PM
    if (state == 0) {
        csi_pwm_disable_pm(&PWM(dev)->handle);
    } else {
        csi_pwm_enable_pm(&PWM(dev)->handle);
    }
#endif
    return 0;
}

static int _pwm_config(rvm_dev_t *dev, rvm_hal_pwm_config_t *config, uint8_t channel)
{
    csi_error_t ret;
    uint32_t period_us, pulse_width, polarity;

    if (!config || channel > MAX_CHANNEL_COUNT - 1) {
        return -EINVAL;
    }
    period_us = 1000000 / config->freq;
    pulse_width = period_us * config->duty_cycle;
    polarity = config->polarity;
    ret = csi_pwm_out_config(&PWM(dev)->handle, channel, period_us, pulse_width, polarity);
    if (ret != CSI_OK) {
        LOGE(TAG, "csi_pwm_out_config error");
        return -1;
    }
    memcpy(&PWM(dev)->config[channel], config, sizeof(rvm_hal_pwm_config_t));
    return 0;
}

static int _pwm_config_get(rvm_dev_t *dev, rvm_hal_pwm_config_t *config, uint8_t channel)
{
    if (!config || channel > MAX_CHANNEL_COUNT - 1) {
        return -EINVAL;
    }
    memcpy(config, &PWM(dev)->config[channel], sizeof(rvm_hal_pwm_config_t));
    return 0;
}

static int _pwm_start(rvm_dev_t *dev, uint8_t channel)
{
    csi_error_t ret;

    if (channel > MAX_CHANNEL_COUNT - 1)
        return -EINVAL;

    ret = csi_pwm_out_start(&PWM(dev)->handle, channel);
    if (ret != CSI_OK) {
        LOGE(TAG, "csi_pwm_out_start error");
        return -1;
    }
    return 0;
}

static int _pwm_stop(rvm_dev_t *dev, uint8_t channel)
{
    if (channel > MAX_CHANNEL_COUNT - 1)
        return -EINVAL;
    csi_pwm_out_stop(&PWM(dev)->handle, channel);
    return 0;
}

static pwm_driver_t pwm_driver = {
    .drv = {
        .name   = "pwm",
        .init   = _pwm_init,
        .uninit = _pwm_uninit,
        .open   = _pwm_open,
        .close  = _pwm_close,
        .clk_en = _pwm_clock,
        .lpm    = _pwm_lpm
    },
    .config          = _pwm_config,
    .config_get      = _pwm_config_get,
    .start           = _pwm_start,
    .stop            = _pwm_stop
};

void rvm_pwm_drv_register(int idx)
{
    rvm_driver_register(&pwm_driver.drv, NULL, idx);
}
