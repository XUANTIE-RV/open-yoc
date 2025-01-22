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

#include <devices/led.h>
#include <dev_internal.h>
#include <devices/impl/led_impl.h>

#define LED_DRIVER(dev)  ((led_driver_t*)(dev->drv))
#define LED_VAILD(dev) do { \
    if (device_valid(dev, "led") != 0) \
        return -1; \
} while(0)

int rvm_hal_led_control(rvm_dev_t *dev, int color, int on_time, int off_time)
{
    int ret;

    LED_VAILD(dev);

    device_lock(dev);
    ret = LED_DRIVER(dev)->control(dev, color, on_time, off_time);
    device_unlock(dev);

    return ret < 0 ? -EIO : 0;
}
