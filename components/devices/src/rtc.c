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

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dev_internal.h>
#include <devices/impl/rtc_impl.h>

#define RTC_DRIVER(dev)  ((rtc_driver_t*)(dev->drv))
#define RTC_VAILD(dev) do { \
    if (device_valid(dev, "rtc") != 0) \
        return -1; \
} while(0)

int rvm_hal_rtc_set_time(rvm_dev_t *dev, const struct tm *time)
{
    int ret;

    RTC_VAILD(dev);
    if (!time) {
        return -EINVAL;
    }
    device_lock(dev);
    ret = RTC_DRIVER(dev)->set_time(dev, time);
    device_unlock(dev);

    return ret;
}

int rvm_hal_rtc_get_time(rvm_dev_t *dev, struct tm *time)
{
    int ret;

    RTC_VAILD(dev);
    if (!time) {
        return -EINVAL;
    }

    device_lock(dev);
    ret = RTC_DRIVER(dev)->get_time(dev, time);
    device_unlock(dev);

    return ret;
}

uint32_t rvm_hal_rtc_get_alarm_remaining_time(rvm_dev_t *dev)
{
    int ret;

    if (device_valid(dev, "rtc")) {
        return 0;
    }

    device_lock(dev);
    ret = RTC_DRIVER(dev)->get_alarm_remaining_time(dev);
    device_unlock(dev);

    return ret;
}

int rvm_hal_rtc_set_alarm(rvm_dev_t *dev, const struct tm *time, rvm_hal_rtc_callback callback, void *arg)
{
    int ret;

    RTC_VAILD(dev);
    if (!time) {
        return -EINVAL;
    }
    device_lock(dev);
    ret = RTC_DRIVER(dev)->set_alarm(dev, time, callback, arg);
    device_unlock(dev);

    return ret;
}

int rvm_hal_rtc_cancel_alarm(rvm_dev_t *dev)
{
    int ret;

    RTC_VAILD(dev);

    device_lock(dev);
    ret = RTC_DRIVER(dev)->cancel_alarm(dev);
    device_unlock(dev);

    return ret;
}
