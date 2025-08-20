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
#include <devices/impl/cir_impl.h>

#define CIR_DRIVER(dev)  ((cir_driver_t*)(dev->drv))
#define CIR_VAILD(dev) do { \
    if (device_valid(dev, "cir") != 0) \
        return -1; \
} while(0)

int rvm_hal_cir_config(rvm_dev_t *dev, rvm_hal_cir_config_t *config)
{
    int ret;

    CIR_VAILD(dev);

    device_lock(dev);
    ret = CIR_DRIVER(dev)->config(dev, config);
    device_unlock(dev);

    return ret;
}

int rvm_hal_cir_config_get(rvm_dev_t *dev, rvm_hal_cir_config_t *config)
{
    int ret;

    CIR_VAILD(dev);

    device_lock(dev);
    ret = CIR_DRIVER(dev)->config_get(dev, config);
    device_unlock(dev);

    return ret;
}

int rvm_hal_cir_send(rvm_dev_t *dev, uint32_t scancode)
{
    int ret;

    CIR_VAILD(dev);

    device_lock(dev);
    ret = CIR_DRIVER(dev)->send(dev, scancode);
    device_unlock(dev);

    return ret;  
}

int rvm_hal_cir_recv_start(rvm_dev_t *dev)
{
    int ret;

    CIR_VAILD(dev);

    device_lock(dev);
    ret = CIR_DRIVER(dev)->recv_start(dev);
    device_unlock(dev);

    return ret;  
}

int rvm_hal_cir_set_rx_event(rvm_dev_t *dev, rvm_hal_cir_rx_callback callback, void *arg)
{
    int ret;

    CIR_VAILD(dev);

    device_lock(dev);
    ret = CIR_DRIVER(dev)->set_rx_event(dev, callback, arg);
    device_unlock(dev);

    return ret; 
}

int rvm_hal_cir_get_recv_scancode(rvm_dev_t *dev, uint32_t *scancode)
{
    int ret;

    CIR_VAILD(dev);

    device_lock(dev);
    ret = CIR_DRIVER(dev)->get_recv_scancode(dev, scancode);
    device_unlock(dev);

    return ret;  
}
