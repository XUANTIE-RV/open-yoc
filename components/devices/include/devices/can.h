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

#ifndef _DEVICE_CAN_H_
#define _DEVICE_CAN_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <devices/device.h>

typedef enum {
    RVM_HAL_CAN_EVENT_RX_IND         = 0x01,    /* Rx indication */
    RVM_HAL_CAN_EVENT_TX_DONE        = 0x02,    /* Tx complete   */
    RVM_HAL_CAN_EVENT_TX_FAIL        = 0x03,    /* Tx fail   */
    RVM_HAL_CAN_EVENT_RX_TIMEOUT     = 0x05,    /* Rx timeout    */
    RVM_HAL_CAN_EVENT_RXOF_IND       = 0x06,    /* Rx overflow */
} rvm_hal_can_event_t;

typedef enum {
    RVM_HAL_CAN_NORMAL_MODE = 0,
    RVM_HAL_CAN_LISTEN_MODE,
    RVM_HAL_CAN_LOOPBACK_MODE,
    RVM_HAL_CAN_LOOPBACKANLISTEN_MODE,
    RVM_HAL_CAN_SLEEP_MODE
} rvm_hal_can_mode_t;

typedef struct {
    uint32_t    id;         /* CAN ID, 标志格式 11 位，扩展格式 29 位 */
#define RVM_HAL_CAN_DTR   0
#define RVM_HAL_CAN_RTR   1
    uint8_t     rtr;        /* 远程帧标识位 */
#define RVM_HAL_CAN_STDID 0
#define RVM_HAL_CAN_EXTID 1
    uint8_t     ide;        /* 扩展帧标识位 */
    uint8_t     dlc;        /* 数据长度代码（Data Length Code）*/
    uint8_t     data[8];    /* 数据段 */
} rvm_hal_can_msg_t;

typedef enum {
    RVM_HAL_CAN_FILTER_CLOSE        = 0,
    RVM_HAL_CAN_SINGLE_FILTER_MODE  = 1,
    RVM_HAL_CAN_DUAL_FILTER_MODE    = 2,
} rvm_hal_can_filter_mode_t;

typedef union {
    struct single_filter_std {
        uint16_t    id_filter;
        uint8_t     rtr_filter;
        uint8_t     data0_filter;
        uint8_t     data1_filter;
    } sfs;

    struct single_filter_ext {
        uint32_t    id_filter;
        uint8_t     rtr_filter;
    } sfe;

    struct dual_filter_std {
        uint16_t    id_filter0;
        uint8_t     rtr_filter0;
        uint8_t     data0_filter0;
        uint16_t    id_filter1;
        uint8_t     rtr_filter1;
    } dfs;

    struct dual_filter_ext {
        uint16_t    id_filter0;
        uint16_t    id_filter1;
    } dfe;
} rvm_hal_can_filter_t;

typedef struct can_filter_config {
    rvm_hal_can_filter_mode_t   filter_mode;
    /* is_eff indicates whether the filter is used to filter extended frame */
    uint8_t                     is_eff;
    rvm_hal_can_filter_t        rxcode;
    rvm_hal_can_filter_t        rxmask;
} rvm_hal_can_filter_config_t;

typedef enum {
    RVM_HAL_CAN_BR_1M       = 1000000,
    RVM_HAL_CAN_BR_900K     = 900000,
    RVM_HAL_CAN_BR_800K     = 800000,
    RVM_HAL_CAN_BR_725K     = 725000,
    RVM_HAL_CAN_BR_600K     = 600000,
    RVM_HAL_CAN_BR_500K     = 500000,
    RVM_HAL_CAN_BR_400K     = 400000,
    RVM_HAL_CAN_BR_300K     = 300000,
    RVM_HAL_CAN_BR_200K     = 200000,
    RVM_HAL_CAN_BR_100K     = 100000,
    RVM_HAL_CAN_BR_50K      = 50000,
    RVM_HAL_CAN_BR_20K      = 20000,
} rvm_hal_can_baudrate_t;

typedef struct {
    rvm_hal_can_baudrate_t baud_rate;       /**< baud rate of can */
    rvm_hal_can_mode_t mode;                /**< mode of can */
} rvm_hal_can_config_t;

typedef void (*rvm_hal_can_callback)(rvm_dev_t *dev, rvm_hal_can_event_t event, void *arg);

#define rvm_hal_can_open(name) rvm_hal_device_open(name)
#define rvm_hal_can_close(dev) rvm_hal_device_close(dev)

/**
  \brief       Set config for can device
  \param[in]   dev    Pointer to device object.
  \param[in]   config Pointer to the configuration
  \return      0 : on success,  otherwise is error
*/
int rvm_hal_can_config(rvm_dev_t *dev, rvm_hal_can_config_t *config);

/**
  \brief       Get config for can device
  \param[in]   dev    Pointer to device object.
  \param[out]  config Pointer to the configuration
  \return      0 : on success,  otherwise is error
*/
int rvm_hal_can_config_get(rvm_dev_t *dev, rvm_hal_can_config_t *config);

/**
  \brief config a can fliter
  \param[in]  dev           Pointer to device object.
  \param[in]  filter_config point to a filter config
  \return  0 : on success,  otherwise is error
 */
int rvm_hal_can_filter_init(rvm_dev_t *dev, rvm_hal_can_filter_config_t *filter_config);

/**
  \brief can send message
  \param[in]  dev      Pointer to device object.
  \param[in]  msg      The message data to send
  \param[in]  timeout  timeout in milisecond, set this value to AOS_WAIT_FOREVER if you want to wait forever
  \return  0 : on success,  otherwise is error
 */
int rvm_hal_can_send(rvm_dev_t *dev, rvm_hal_can_msg_t *msg, uint32_t timeout);

/**
  \brief can receive message
  \param[in]  dev      Pointer to device object.
  \param[out] msg      The message data to receive
  \param[in]  timeout  timeout in milisecond, set this value to AOS_WAIT_FOREVER if you want to wait forever
  \return  0 : on success,  otherwise is error
 */
int rvm_hal_can_recv(rvm_dev_t *dev, rvm_hal_can_msg_t *msg, uint32_t timeout);

/**
  \brief can set callback event
  \param[in]  dev      Pointer to device object.
  \param[in]  callback The callback function
  \param[in]  arg      The argument for callback function
  \return  0 : on success,  otherwise is error
 */
int rvm_hal_can_set_event(rvm_dev_t *dev, rvm_hal_can_callback callback, void *arg);


#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <devices/vfs_can.h>
#endif

#ifdef __cplusplus
}
#endif

#endif
