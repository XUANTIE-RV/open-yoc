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

#ifndef _DEVICE_CIR_H_
#define _DEVICE_CIR_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <devices/device.h>

typedef enum cir_protocol {
    RVM_HAL_CIR_PROTOCOL_NEC = 0U,
    RVM_HAL_CIR_PROTOCOL_RC5 = 1U
} rvm_hal_cir_protocol_t;

typedef struct cir_config {
    rvm_hal_cir_protocol_t protocol;
    uint32_t tx_duty;
    uint32_t rx_level; /* Indicates the idle level of RX */
} rvm_hal_cir_config_t;

typedef enum {
    RVM_HAL_CIR_EVENT_RECEIVE_COMPLETE,
    RVM_HAL_CIR_EVENT_ERROR,
} rvm_hal_cir_event_t;

typedef void (*rvm_hal_cir_rx_callback)(rvm_dev_t *dev, rvm_hal_cir_event_t event, void *arg);

#define rvm_hal_cir_open(name) rvm_hal_device_open(name)
#define rvm_hal_cir_close(dev) rvm_hal_device_close(dev)

/**
  \brief       Set config for cir device
  \param[in]   dev    Pointer to device object.
  \param[in]   config Pointer to the configuration
  \return      0 : on success,  otherwise is error
*/
int rvm_hal_cir_config(rvm_dev_t *dev, rvm_hal_cir_config_t *config);

/**
  \brief       Get config for cir device
  \param[in]   dev    Pointer to device object.
  \param[out]  config Pointer to the configuration
  \return      0 : on success,  otherwise is error
*/
int rvm_hal_cir_config_get(rvm_dev_t *dev, rvm_hal_cir_config_t *config);

/**
  \brief cir send code
  \param[in]  dev      Pointer to device object.
  \param[in]  scancode The scancode
  \return  0 : on success,  otherwise is error
 */
int rvm_hal_cir_send(rvm_dev_t *dev, uint32_t scancode);

/**
  \brief cir start to receive data
  \param[in]  dev      Pointer to device object.
  \return  0 : on success,  otherwise is error
 */
int rvm_hal_cir_recv_start(rvm_dev_t *dev);

/**
  \brief cir set receive data callback event
  \param[in]  dev      Pointer to device object.
  \param[in]  callback The callback function
  \param[in]  arg      The argument for callback function
  \return  0 : on success,  otherwise is error
 */
int rvm_hal_cir_set_rx_event(rvm_dev_t *dev, rvm_hal_cir_rx_callback callback, void *arg);

/**
  \brief cir get rx scancode
  \param[in]  dev      Pointer to device object.
  \param[in]  scancode The received scancode
  \return  0 : on success,  otherwise is error
 */
int rvm_hal_cir_get_recv_scancode(rvm_dev_t *dev, uint32_t *scancode);


#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <devices/vfs_cir.h>
#endif

#ifdef __cplusplus
}
#endif

#endif
