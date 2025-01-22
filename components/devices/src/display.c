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
#include <devices/impl/display_impl.h>

#define DISP_DRIVER(dev)  ((display_driver_t*)(dev->drv))
#define DISP_VAILD(dev) do { \
    if (device_valid(dev, "disp") != 0) \
        return -1; \
} while(0)


int rvm_hal_display_set_event(rvm_dev_t *dev, rvm_hal_display_event event_cb, void *priv)
{
    int ret;

    DISP_VAILD(dev);

    device_lock(dev);
    ret = DISP_DRIVER(dev)->set_event(dev, event_cb, priv);
    device_unlock(dev);

    return ret;
}

int rvm_hal_display_get_info(rvm_dev_t *dev, rvm_hal_display_info_t *info)
{
    int ret;

    DISP_VAILD(dev);

    device_lock(dev);
    ret = DISP_DRIVER(dev)->get_info(dev, info);
    device_unlock(dev);

    return ret;
}

int rvm_hal_display_get_framebuffer(rvm_dev_t *dev, void ***smem_start, size_t *smem_len)
{
    int ret;

    DISP_VAILD(dev);

    device_lock(dev);
    ret = DISP_DRIVER(dev)->get_framebuffer(dev, smem_start, smem_len);
    device_unlock(dev);

    return ret;
}

int rvm_hal_display_set_brightness(rvm_dev_t *dev, uint8_t brightness)
{
    int ret;

    DISP_VAILD(dev);

    device_lock(dev);
    ret = DISP_DRIVER(dev)->set_brightness(dev, brightness);
    device_unlock(dev);

    return ret;
}

int rvm_hal_display_get_brightness(rvm_dev_t *dev, uint8_t *brightness)
{
    int ret;

    DISP_VAILD(dev);

    device_lock(dev);
    ret = DISP_DRIVER(dev)->get_brightness(dev, brightness);
    device_unlock(dev);

    return ret;
}

int rvm_hal_display_write_area(rvm_dev_t *dev, rvm_hal_display_area_t *area, void *data)
{
    int ret;

    DISP_VAILD(dev);

    device_lock(dev);
    ret = DISP_DRIVER(dev)->write_area(dev, area, data);
    device_unlock(dev);

    return ret;
}

int rvm_hal_display_write_area_async(rvm_dev_t *dev, rvm_hal_display_area_t *area, void *data)
{
    int ret;

    DISP_VAILD(dev);

    device_lock(dev);
    ret = DISP_DRIVER(dev)->write_area_async(dev, area, data);
    device_unlock(dev);

    return ret;
}

int rvm_hal_display_read_area(rvm_dev_t *dev, rvm_hal_display_area_t *area, void *data)
{
    int ret;

    DISP_VAILD(dev);

    device_lock(dev);
    ret = DISP_DRIVER(dev)->read_area(dev, area, data);
    device_unlock(dev);

    return ret;
}

int rvm_hal_display_pan_display(rvm_dev_t *dev)
{
    int ret;

    DISP_VAILD(dev);

    device_lock(dev);
    ret = DISP_DRIVER(dev)->pan_display(dev);
    device_unlock(dev);

    return ret;
}

int rvm_hal_display_blank_on_off(rvm_dev_t *dev, uint8_t on_off)
{
    int ret;

    DISP_VAILD(dev);

    device_lock(dev);
    ret = DISP_DRIVER(dev)->blank_on_off(dev, on_off);
    device_unlock(dev);

    return ret;
}
