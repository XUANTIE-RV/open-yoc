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
#include <devices/impl/adc_impl.h>

#define ADC_DRIVER(dev)  ((adc_driver_t*)(dev->drv))
#define ADC_VAILD(dev) do { \
    if (device_valid(dev, "adc") != 0) \
        return -1; \
} while(0)

void rvm_hal_adc_config_default(rvm_hal_adc_config_t *config)
{
    config->mode          = RVM_ADC_CONTINUOUS;
    config->trigger       = 0;
    config->intrp_mode    = 0;
    config->sampling_time = 2;
    config->freq          = 128;
    config->offset        = 0;
}

int rvm_hal_adc_config(rvm_dev_t *dev, rvm_hal_adc_config_t *config)
{
    int ret;

    ADC_VAILD(dev);

    device_lock(dev);
    ret = ADC_DRIVER(dev)->config(dev, config);
    device_unlock(dev);

    return ret;
}

int rvm_hal_adc_pin2channel(rvm_dev_t *dev, int pin)
{
    int ret;

    ADC_VAILD(dev);

    device_lock(dev);
    ret = ADC_DRIVER(dev)->pin2channel(dev, pin);
    device_unlock(dev);

    return ret;
}

int rvm_hal_adc_read(rvm_dev_t *dev, uint8_t ch, void *output, uint32_t timeout)
{
    if (output == 0) {
        return -EINVAL;
    }

    int ret;

    ADC_VAILD(dev);

    device_lock(dev);
    ret = ADC_DRIVER(dev)->read(dev, ch, output, 1, timeout);
    device_unlock(dev);

    return ret;
}

int rvm_hal_adc_read_multiple(rvm_dev_t *dev, uint8_t ch, void *output, size_t num, uint32_t timeout)
{
    if (output == 0 && num < 1) {
        return -EINVAL;
    }

    int ret;

    ADC_VAILD(dev);

    device_lock(dev);
    ret = ADC_DRIVER(dev)->read(dev, ch, output, num, timeout);
    device_unlock(dev);

    return ret;
}

int rvm_hal_adc_trans_dma_enable(rvm_dev_t *dev, bool enable)
{
    int ret;

    ADC_VAILD(dev);

    device_lock(dev);
    ret = ADC_DRIVER(dev)->trans_dma_enable(dev, enable);
    device_unlock(dev);

    return ret;
}
