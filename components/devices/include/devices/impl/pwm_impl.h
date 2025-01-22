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

#ifndef HAL_PWM_IMPL_H
#define HAL_PWM_IMPL_H

#include <stdint.h>

#include <devices/driver.h>
#include <devices/pwm.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct pwm_driver {
    driver_t drv;
    int (*config)(rvm_dev_t *dev, rvm_hal_pwm_config_t *config, uint8_t channel);
    int (*config_get)(rvm_dev_t *dev, rvm_hal_pwm_config_t *config, uint8_t channel);
    int (*start)(rvm_dev_t *dev, uint8_t channel);
    int (*stop)(rvm_dev_t *dev, uint8_t channel);
} pwm_driver_t;

#ifdef __cplusplus
}
#endif

#endif