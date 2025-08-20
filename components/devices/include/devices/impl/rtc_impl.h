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

#ifndef _DEVICE_RTC_IMPL_H_
#define _DEVICE_RTC_IMPL_H_

#include <stdio.h>
#include <stdlib.h>

#include <devices/driver.h>
#include <devices/rtc.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @brief rtc dirver ops */
typedef struct rtc_driver {
    driver_t drv;
    int (*set_time)(rvm_dev_t *dev, const struct tm *time);
    int (*get_time)(rvm_dev_t *dev, struct tm *time);
    uint32_t (*get_alarm_remaining_time)(rvm_dev_t *dev);
    int (*set_alarm)(rvm_dev_t *dev, const struct tm *time, rvm_hal_rtc_callback callback, void *arg);
    int (*cancel_alarm)(rvm_dev_t *dev);
} rtc_driver_t;

#ifdef __cplusplus
}
#endif

#endif
