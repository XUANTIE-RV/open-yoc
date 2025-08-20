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

#ifndef HAL_ADC_IMPL_H
#define HAL_ADC_IMPL_H

#include <stdint.h>

#include <devices/adc.h>
#include <devices/driver.h>


#ifdef __cplusplus
extern "C" {
#endif

typedef struct adc_driver {
    driver_t drv;
    int (*config)(rvm_dev_t *dev, rvm_hal_adc_config_t *config);
    int (*pin2channel)(rvm_dev_t *dev, int pin);
    int (*read)(rvm_dev_t *dev, uint8_t ch, void *output, size_t num, uint32_t timeout);
    int (*trans_dma_enable)(rvm_dev_t *dev, bool enable);
} adc_driver_t;

#ifdef __cplusplus
}
#endif

#endif
