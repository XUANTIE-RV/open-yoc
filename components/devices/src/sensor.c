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
#include <errno.h>
#include <dev_internal.h>
#include <devices/impl/sensor_impl.h>

#define SENSOR_DRIVER(dev)  ((sensor_driver_t*)(dev->drv))
#define SENSOR_VAILD(dev) do { \
    if (device_valid(dev, "sensor") != 0) \
        return -1; \
} while(0)

int rvm_hal_sensor_fetch(rvm_dev_t *dev)
{
    int ret;

    SENSOR_VAILD(dev);

    device_lock(dev);
    ret = SENSOR_DRIVER(dev)->fetch(dev);
    device_unlock(dev);

    return ret < 0 ? -EIO : 0;
}

int rvm_hal_sensor_getvalue(rvm_dev_t *dev, void *value, size_t size)
{
    int ret;

    SENSOR_VAILD(dev);

    device_lock(dev);
    ret = SENSOR_DRIVER(dev)->getvalue(dev, value, size);
    device_unlock(dev);

    return ret < 0 ? -EIO : 0;
}
