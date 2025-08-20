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

#ifndef DEVICE_ADC_H
#define DEVICE_ADC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <devices/device.h>

/****** ADC conversion mode *****/
typedef enum {
    RVM_ADC_SINGLE = 0,       ///< Single conversion mode --- select one channel to convert at a time.
    RVM_ADC_CONTINUOUS,       ///< Continuous conversion mode --- select a channel to convert in a specific times.
} rvm_hal_adc_mode_e;

typedef struct {
    uint32_t    clk_prescaler;          ///< select ADC clock  prescaler.
    rvm_hal_adc_mode_e  mode;           ///< \ref rvm_hal_adc_mode_e
    uint32_t    trigger;                ///< 0 -- software adc start or 1 -- external event trigger to start adc.
    uint32_t    intrp_mode;             ///< specifies whether the ADC is configured is interrupt mode (1)or in polling mode (0).
    uint32_t    sampling_time;          ///< sampling time value to be set for the selected channel. Unit:ADC clock cycles.
    uint32_t    freq;                   ///< ADC frequency division factor
    uint32_t    offset;                 ///< reserved for future use, can be set to 0.
} rvm_hal_adc_config_t;


#define rvm_hal_adc_open(name) rvm_hal_device_open(name)
#define rvm_hal_adc_close(dev) rvm_hal_device_close(dev)

int rvm_hal_adc_config(rvm_dev_t *dev, rvm_hal_adc_config_t *config);
int rvm_hal_adc_pin2channel(rvm_dev_t *dev, int pin);
int rvm_hal_adc_read(rvm_dev_t *dev, uint8_t ch, void *output, uint32_t timeout);
int rvm_hal_adc_read_multiple(rvm_dev_t *dev, uint8_t ch, void *output, size_t num, uint32_t timeout);
void rvm_hal_adc_config_default(rvm_hal_adc_config_t *config);
int rvm_hal_adc_trans_dma_enable(rvm_dev_t *dev, bool enable);


#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <devices/vfs_adc.h>
#endif

#ifdef __cplusplus
}
#endif

#endif
