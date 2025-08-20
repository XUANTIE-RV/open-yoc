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

#ifndef HAL_I2S_IMPL_H
#define HAL_I2S_IMPL_H

#include <stdint.h>

#include <devices/driver.h>
#include <devices/i2s.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct i2s_driver {
    driver_t drv;
    uint32_t timeout;
    int (*config)(rvm_dev_t *dev, rvm_hal_i2s_config_t *config);
    int (*config_get)(rvm_dev_t *dev, rvm_hal_i2s_config_t *config);
    int (*send)(rvm_dev_t *dev, const void *data, size_t size, uint32_t timeout);
    int (*recv)(rvm_dev_t *dev, void *data, size_t size, uint32_t timeout);
    int (*pause)(rvm_dev_t *dev);
    int (*resume)(rvm_dev_t *dev);
    int (*stop)(rvm_dev_t *dev);
} i2s_driver_t;

#ifdef __cplusplus
}
#endif

#endif