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

#ifndef DEVICE_INPUT_IMPL_H
#define DEVICE_INPUT_IMPL_H

#include <stdio.h>
#include <stdlib.h>

#include <devices/driver.h>
#include <devices/device.h>
#include <devices/input.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @brief input dirver ops */
typedef struct input_driver {
    driver_t drv;
    /** set event callback, should support mutil event_cb set */
    int (*set_event)(rvm_dev_t *dev, rvm_hal_input_event event_cb, void *priv);
    /** unset event callback */
    int (*unset_event)(rvm_dev_t *dev, rvm_hal_input_event event_cb);
    /** read input data */
    int (*read)(rvm_dev_t *dev, void *data, uint32_t size, unsigned int timeout_ms);
} input_driver_t;

#ifdef __cplusplus
}
#endif

#endif
