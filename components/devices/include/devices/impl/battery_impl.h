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

#ifndef HAL_BATTERY_H
#define HAL_BATTERY_H

#include <stdint.h>

#include <devices/driver.h>
#include <devices/battery.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef struct battery_driver {
    driver_t    drv;
    int         (*fetch)(rvm_dev_t *dev, rvm_hal_battery_attr_t attr);
    int         (*getvalue)(rvm_dev_t *dev, rvm_hal_battery_attr_t attr, void *value, size_t size);
    int         (*event_cb)(rvm_dev_t *dev, rvm_hal_battery_event_t event);
} battery_driver_t;

#ifdef __cplusplus
}
#endif

#endif
