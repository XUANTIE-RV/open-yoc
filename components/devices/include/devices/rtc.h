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

#ifndef _DEVICE_RTC_H_
#define _DEVICE_RTC_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <devices/device.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*rvm_hal_rtc_callback)(rvm_dev_t *dev, void *arg);

#define rvm_hal_rtc_open(name)        rvm_hal_device_open(name)
#define rvm_hal_rtc_close(dev)        rvm_hal_device_close(dev)

/**
  \brief       Set system date
  \param[in]   dev      Pointer to device object.
  \param[in]   time     Pointer to RTC time
  \return      0 on success, else on fail.
*/
int rvm_hal_rtc_set_time(rvm_dev_t *dev, const struct tm *time);

/**
  \brief       Get system date
  \param[in]   dev      Pointer to device object.
  \param[in]   time     Pointer to RTC time
  \return      0 on success, else on fail.
*/
int rvm_hal_rtc_get_time(rvm_dev_t *dev, struct tm *time);

/**
  \brief       Get alarm remaining time
  \param[in]   dev      Pointer to device object.
  \return      The remaining time(s)
*/
uint32_t rvm_hal_rtc_get_alarm_remaining_time(rvm_dev_t *dev);

/**
  \brief       Config RTC alarm timer
  \param[in]   dev         Pointer to device object.
  \param[in]   time        Time to wake up
  \param[in]   callback    Callback function
  \param[in]   arg         Callback's param
  \return      0 on success, else on fail.
*/
int rvm_hal_rtc_set_alarm(rvm_dev_t *dev, const struct tm *time, rvm_hal_rtc_callback callback, void *arg);

/**
  \brief       Cancel the RTC alarm
  \param[in]   dev      Pointer to device object.
  \return      0 on success, else on fail.
*/
int rvm_hal_rtc_cancel_alarm(rvm_dev_t *dev);


#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <devices/vfs_rtc.h>
#endif

#ifdef __cplusplus
}
#endif

#endif
