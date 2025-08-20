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

#ifndef HAL_CAN_IMPL_H
#define HAL_CAN_IMPL_H

#include <stdint.h>

#include <devices/driver.h>
#include <devices/can.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct can_driver {
    driver_t drv;
    int (*config)(rvm_dev_t *dev, rvm_hal_can_config_t *config);
    int (*config_get)(rvm_dev_t *dev, rvm_hal_can_config_t *config);
    int (*filter_init)(rvm_dev_t *dev, rvm_hal_can_filter_config_t *filter_config);
    int (*send)(rvm_dev_t *dev, rvm_hal_can_msg_t *msg, uint32_t timeout);
    int (*recv)(rvm_dev_t *dev, rvm_hal_can_msg_t *msg, uint32_t timeout);
    int (*set_event)(rvm_dev_t *dev, rvm_hal_can_callback callback, void *arg);
} can_driver_t;

#ifdef __cplusplus
}
#endif

#endif