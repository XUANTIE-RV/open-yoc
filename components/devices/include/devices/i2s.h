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

#ifndef _DEVICE_I2S_H_
#define _DEVICE_I2S_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <devices/device.h>

/*
 * I2S mode
 */
typedef enum {
    RVM_HAL_I2S_MODE_SLAVE_TX,
    RVM_HAL_I2S_MODE_SLAVE_RX,
    RVM_HAL_I2S_MODE_MASTER_TX,
    RVM_HAL_I2S_MODE_MASTER_RX
} rvm_hal_i2s_mode_t;

/*
 * I2S standard
 */
typedef enum {
    RVM_HAL_I2S_STANDARD_PHILIPS,   /**< Philips standard */
    RVM_HAL_I2S_STANDARD_MSB,       /**< MSB align standard */
    RVM_HAL_I2S_STANDARD_LSB,       /**< LSB align standard */
    RVM_HAL_I2S_STANDARD_PCM_SHORT, /**< PCM short frame standard */
    RVM_HAL_I2S_STANDARD_PCM_LONG   /**< PCM long frame standard */
} rvm_hal_i2s_std_t;

/*
 * I2S data format
 */
typedef enum {
    RVM_HAL_I2S_DATAFORMAT_16B,             /**< 16 bit dataformat */
    RVM_HAL_I2S_DATAFORMAT_16B_EXTENDED,    /**< 16 bit externded dataformat, 32 bit frame */
    RVM_HAL_I2S_DATAFORMAT_24B,             /**< 24 bit dataformat */
    RVM_HAL_I2S_DATAFORMAT_32B              /**< 32 bit dataformat */
} rvm_hal_i2s_data_format_t;

/*
 * I2S configuration
 */
typedef struct {
    uint32_t                    freq;         /**< I2S communication frequency */
    rvm_hal_i2s_mode_t          mode;         /**< I2S operating mode */
    rvm_hal_i2s_std_t           standard;     /**< I2S communication standard */
    rvm_hal_i2s_data_format_t   data_format;  /**< I2S communication data format */
    bool                        tx_dma_enable;
    bool                        rx_dma_enable;
} rvm_hal_i2s_config_t;


#define rvm_hal_i2s_open(name) rvm_hal_device_open(name)
#define rvm_hal_i2s_close(dev) rvm_hal_device_close(dev)

/**
  \brief       Set config for i2s device
  \param[in]   dev    Pointer to device object.
  \param[in]   config Pointer to the configuration
  \return      0 : on success,  otherwise is error
*/
int rvm_hal_i2s_config(rvm_dev_t *dev, rvm_hal_i2s_config_t *config);

/**
  \brief       Get the config of i2s device
  \param[in]   dev    Pointer to device object.
  \param[out]  config Pointer to the configuration
  \return      0 : on success,  otherwise is error
*/
int rvm_hal_i2s_config_get(rvm_dev_t *dev, rvm_hal_i2s_config_t *config);

/**
 \brief      Transmit data on a I2S dev
 \param[in]  dev      Pointer to device object.
 \param[in]  data     pointer to the start of data
 \param[in]  size     number of bytes to transmit
 \param[in]  timeout  timeout in milisecond, set this value to AOS_WAIT_FOREVER if you want to wait forever
 \return     0 : on success,  otherwise is error
 */
int rvm_hal_i2s_send(rvm_dev_t *dev, const void *data, size_t size, uint32_t timeout);

/**
 \brief       Receive data on a I2S dev
 \param[in]   dev      Pointer to device object.
 \param[out]  data     pointer to the buffer which will store incoming data
 \param[in]   size     number of bytes to receive
 \param[in]   timeout  timeout in milisecond, set this value to AOS_WAIT_FOREVER if you want to wait forever
 \return      0 : on success,  otherwise is error
 */
int rvm_hal_i2s_recv(rvm_dev_t *dev, void *data, size_t size, uint32_t timeout);

/**
 \brief      Pause a I2S dev
 \param[in]  dev  Pointer to device object.
 \return     0 : on success,  otherwise is error
 */
int rvm_hal_i2s_pause(rvm_dev_t *dev);

/**
 \brief      Resume a I2S dev
 \param[in]  dev  Pointer to device object.
 \return     0 : on success,  otherwise is error
 */
int rvm_hal_i2s_resume(rvm_dev_t *dev);

/**
 \brief      Stop a I2S dev
 \param[in]  dev  Pointer to device object.
 \return     0 : on success,  otherwise is error
 */
int rvm_hal_i2s_stop(rvm_dev_t *dev);


#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <devices/vfs_i2s.h>
#endif

#ifdef __cplusplus
}
#endif

#endif
