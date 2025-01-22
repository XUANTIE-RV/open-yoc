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

#ifndef DEVICE_DISPLAY_IMPL_H
#define DEVICE_DISPLAY_IMPL_H

#include <stdio.h>
#include <stdlib.h>

#include <devices/driver.h>
#include <devices/device.h>
#include <devices/display.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @brief display dirver ops */
typedef struct display_driver {
    driver_t drv;
    /** set event callback */
    int (*set_event)(rvm_dev_t *dev, rvm_hal_display_event event_cb, void *priv);
    /** get display infomation */
    int (*get_info)(rvm_dev_t *dev, rvm_hal_display_info_t *info);
    /** get framebuffer */
    int (*get_framebuffer)(rvm_dev_t *dev, void ***smem_start, size_t *smem_len);
    /** set backlight brightness */
    int (*set_brightness)(rvm_dev_t *dev, uint8_t brightness);
    /** get backlight brightness */
    int (*get_brightness)(rvm_dev_t *dev, uint8_t *brightness);
    /** write data from framebuffer in specific area in synchronous mode */
    int (*write_area)(rvm_dev_t *dev, rvm_hal_display_area_t *area, void *data);
    /**
     * write data from framebuffer in specific area in asynchronous mode
     * will call event_cb with DISPLAY_EVENT_WRITE_DONE after done
     */
    int (*write_area_async)(rvm_dev_t *dev, rvm_hal_display_area_t *area, void *data);
    /** read data from framebuffer in specific area */
    int (*read_area)(rvm_dev_t *dev, rvm_hal_display_area_t *area, void *data);
    /**
     * display farmebuffer data to pan,
     * will call event_cb with DISPLAY_EVENT_FLUSH_DONE after done
     */
    int (*pan_display)(rvm_dev_t *dev);
    /** display control on off */
    int (*blank_on_off)(rvm_dev_t *dev, uint8_t on_off);
} display_driver_t;

#ifdef __cplusplus
}
#endif

#endif
