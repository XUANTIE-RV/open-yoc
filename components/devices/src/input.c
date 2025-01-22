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
#include <devices/impl/input_impl.h>

#define INPUT_DRIVER(dev)  ((input_driver_t*)(dev->drv))
#define INPUT_VAILD(dev) do { \
    if (device_valid(dev, "input") != 0) \
        return -1; \
} while(0)

int rvm_hal_input_set_event(rvm_dev_t *dev, rvm_hal_input_event event_cb, void *priv)
{
    int ret;

    INPUT_VAILD(dev);

    device_lock(dev);
    ret = INPUT_DRIVER(dev)->set_event(dev, event_cb, priv);
    device_unlock(dev);

    return ret;
}

int rvm_hal_input_unset_event(rvm_dev_t *dev, rvm_hal_input_event event_cb)
{
    int ret;

    INPUT_VAILD(dev);

    device_lock(dev);
    ret = INPUT_DRIVER(dev)->unset_event(dev, event_cb);
    device_unlock(dev);

    return ret;
}

int rvm_hal_input_read(rvm_dev_t *dev, void *data, uint32_t size, unsigned int timeout_ms)
{
    int ret;

    INPUT_VAILD(dev);

    device_lock(dev);
    ret = INPUT_DRIVER(dev)->read(dev, data, size, timeout_ms);
    device_unlock(dev);

    return ret;
}
