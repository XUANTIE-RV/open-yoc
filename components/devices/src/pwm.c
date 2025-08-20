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
#include <devices/impl/pwm_impl.h>

#define PWM_DRIVER(dev)  ((pwm_driver_t*)(dev->drv))
#define PWM_VAILD(dev) do { \
    if (device_valid(dev, "pwm") != 0) \
        return -1; \
} while(0)

int rvm_hal_pwm_config(rvm_dev_t *dev, rvm_hal_pwm_config_t *config, uint8_t channel)
{
    int ret;

    PWM_VAILD(dev);

    device_lock(dev);
    ret = PWM_DRIVER(dev)->config(dev, config, channel);
    device_unlock(dev);

    return ret;
}

int rvm_hal_pwm_config_get(rvm_dev_t *dev, rvm_hal_pwm_config_t *config, uint8_t channel)
{
    int ret;

    PWM_VAILD(dev);

    device_lock(dev);
    ret = PWM_DRIVER(dev)->config_get(dev, config, channel);
    device_unlock(dev);

    return ret;
}

int rvm_hal_pwm_start(rvm_dev_t *dev, uint8_t channel)
{
    int ret;

    PWM_VAILD(dev);

    device_lock(dev);
    ret = PWM_DRIVER(dev)->start(dev, channel);
    device_unlock(dev);

    return ret;
}

int rvm_hal_pwm_stop(rvm_dev_t *dev, uint8_t channel)
{
    int ret;

    PWM_VAILD(dev);

    device_lock(dev);
    ret = PWM_DRIVER(dev)->stop(dev, channel);
    device_unlock(dev);

    return ret;
}
