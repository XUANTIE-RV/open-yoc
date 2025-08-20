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

#ifndef DEVICE_INPUT_H
#define DEVICE_INPUT_H

#include <stdio.h>
#include <stdlib.h>

#include <aos/aos.h>
#include <devices/device.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @brief display event id */
#define INPUT_EVENT_READ     0
#define INPUT_EVENT_OVERFLOW 1

/** @brief sync event */
#define EV_SYN              0
/** @brief sync event code */
#define SYN_REPORT          0
#define SYN_CONFIG          1
#define SYN_MT_REPORT       2

/** @brief key event */
#define EV_KEY              1
/** @brief sync event code */
#define BTN_TOUCH           330

/** @brief absolute coordinate event */
#define EV_ABS              2
/** @brief absolute coordinate event code */
#define ABS_X               0
#define ABS_Y               1
#define ABS_Z               2
#define ABS_RX              3
#define ABS_RY              4
#define ABS_RZ              5
#define ABS_MT_SLOT         47
#define ABS_MT_TOUCH_MAJOR  48

/** @brief input event structure */
typedef struct {
    uint16_t type;
    uint16_t code;
    int32_t  value;
    uint64_t timestamp_ms;
} rvm_hal_input_event_t;

/** @brief display event callback */
typedef void (*rvm_hal_input_event)(rvm_dev_t *dev, int event_id, void *priv);

#define rvm_hal_input_open(name)        rvm_hal_device_open(name)
#define rvm_hal_input_close(dev)        rvm_hal_device_close(dev)

/**
  \brief       set event callback, allow to set mutiple event_cb
  \param[in]   dev      Pointer to device object.
  \param[in]   event_cb event callback
  \param[in]   priv     private data for user
  \return      0 on success, else on fail.
*/
int rvm_hal_input_set_event(rvm_dev_t *dev, rvm_hal_input_event event_cb, void *priv);

/**
  \brief       unset event callback
  \param[in]   dev      Pointer to device object.
  \param[in]   event_cb event callback
  \return      0 on success, else on fail.
*/
int rvm_hal_input_unset_event(rvm_dev_t *dev, rvm_hal_input_event event_cb);

/**
  \brief       read data
  \param[in]   dev        Pointer to device object.
  \param[out]  data       data pointer
  \param[in]   size       data size
  \param[in]   timeout_ms read timeout ms
  \return      0 on success, else on fail.
*/
int rvm_hal_input_read(rvm_dev_t *dev, void *data, uint32_t size, unsigned int timeout_ms);


#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <devices/vfs_input.h>
#endif

#ifdef __cplusplus
}
#endif

#endif
