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

#ifndef HAL_CAMERA_IMPL_H
#define HAL_CAMERA_IMPL_H

#include <stdint.h>

#include <devices/camera.h>
#include <devices/driver.h>


#ifdef __cplusplus
extern "C" {
#endif

typedef struct camera_drv {
    driver_t drv;
    int (*channel_open)(rvm_dev_t *dev, rvm_hal_camera_channel_cfg_t *cfg);
    int (*channel_close)(rvm_dev_t *dev, rvm_hal_camera_channel_id_e chn_id);
    int (*channel_query)(rvm_dev_t *dev, rvm_hal_camera_channel_cfg_t *cfg);
    int (*channel_start)(rvm_dev_t *dev, rvm_hal_camera_channel_id_e chn_id);
    int (*channel_stop)(rvm_dev_t *dev, rvm_hal_camera_channel_id_e chn_id);
    int (*create_event)(rvm_dev_t *dev);
    int (*destory_event)(rvm_dev_t *dev);
    int (*subscribe_event)(rvm_dev_t *dev, rvm_hal_camera_event_subscription_t *subscribe);
    int (*unsubscribe_event)(rvm_dev_t *dev, rvm_hal_camera_event_subscription_t *subscribe);
    int (*get_event)(rvm_dev_t *dev, rvm_hal_camera_event_t *event, int timeout_ms);
    int (*get_frame_count)(rvm_dev_t *dev, rvm_hal_camera_channel_id_e chn_id);
    int (*get_frame)(rvm_dev_t *dev, rvm_hal_camera_channel_id_e chn_id, rvm_hal_frame_ex_t *frame, int timeout_ms);
} camera_drv_t;

#ifdef __cplusplus
}
#endif

#endif
