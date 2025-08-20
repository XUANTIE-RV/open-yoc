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

#ifndef _DEVICE_CLK_IMPL_H_
#define _DEVICE_CLK_IMPL_H_

#include <stdio.h>
#include <stdlib.h>

#include <devices/driver.h>
#include <devices/clk.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @brief clk dirver ops */
typedef struct clk_driver {
    driver_t drv;
    int (*enable)(rvm_dev_t *dev, const char *dev_name);
    int (*disable)(rvm_dev_t *dev, const char *dev_name);
    int (*get_freq)(rvm_dev_t *dev, rvm_hal_clk_id_t clk_id, uint32_t idx, uint32_t *freq);
    int (*set_freq)(rvm_dev_t *dev, rvm_hal_clk_id_t clk_id, uint32_t idx, uint32_t freq);
} clk_driver_t;

#ifdef __cplusplus
}
#endif

#endif