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

#ifndef _DEVICE_QSPI_H_
#define _DEVICE_QSPI_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <devices/device.h>

#define DEFAULT_QSPI_FREQ 400000

typedef enum
{
    RVM_HAL_QSPI_WORK_MODE_0 = 0,              // CPOL = 0; CPHA = 0
    RVM_HAL_QSPI_WORK_MODE_1,                  // CPOL = 0; CPHA = 1
    RVM_HAL_QSPI_WORK_MODE_2,                  // CPOL = 1; CPHA = 0
    RVM_HAL_QSPI_WORK_MODE_3,                  // CPOL = 1; CPHA = 1
} rvm_hal_qspi_work_mode_e;

/**
 * \enum   rvm_hal_qspi_bus_width_t
 * \brief  QSPI bus width
 */
typedef enum {
    RVM_HAL_QSPI_CFG_BUS_SINGLE = 0,  ///< Single line
    RVM_HAL_QSPI_CFG_BUS_DUAL,        ///< Two line
    RVM_HAL_QSPI_CFG_BUS_QUAD,        ///< Four line
} rvm_hal_qspi_bus_width_t;

/**
 * \enum   rvm_hal_qspi_address_size_t
 * \brief  Address size in bits
 */
typedef enum {
    RVM_HAL_QSPI_ADDRESS_8_BITS = 0,
    RVM_HAL_QSPI_ADDRESS_16_BITS,
    RVM_HAL_QSPI_ADDRESS_24_BITS,
    RVM_HAL_QSPI_ADDRESS_32_BITS,
} rvm_hal_qspi_address_size_t;

/**
 * \enum      rvm_hal_qspi_alt_size_t
 * rief       QSPI alternate bytes
 */
typedef enum {
    RVM_HAL_QSPI_ALTERNATE_8_BITS = 0,
    RVM_HAL_QSPI_ALTERNATE_16_BITS,
    RVM_HAL_QSPI_ALTERNATE_24_BITS,
    RVM_HAL_QSPI_ALTERNATE_32_BITS,
} rvm_hal_qspi_alt_size_t;

typedef struct {
    struct
    {
        rvm_hal_qspi_bus_width_t bus_width; ///< Bus width for the instruction
        uint8_t value;                      ///< Instruction value
        bool disabled;                      ///< Instruction phase skipped if disabled is set to true
    } instruction;
    struct
    {
        rvm_hal_qspi_bus_width_t bus_width; ///< Bus width for the address
        rvm_hal_qspi_address_size_t size;   ///< Address size
        uint32_t value;                     ///< Address value
        bool disabled;                      ///< Address phase skipped if disabled is set to true
    } address;
    struct
    {
        rvm_hal_qspi_bus_width_t bus_width; ///< Bus width for alternative
        rvm_hal_qspi_alt_size_t size;       ///< Alternative size
        uint32_t value;                     ///< Alternative value
        bool disabled;                      ///< Alternative phase skipped if disabled is set to true
    } alt;
    uint8_t dummy_count; ///< Dummy cycles count
    struct
    {
        rvm_hal_qspi_bus_width_t bus_width; ///< Bus width for data
    } data;
    uint8_t ddr_enable;
} rvm_hal_qspi_cmd_t;

typedef struct {
    rvm_hal_qspi_work_mode_e mode;
    uint32_t freq;             /**< communication frequency Hz */
    bool dma_enable;
} rvm_hal_qspi_config_t;

#define rvm_hal_qspi_open(name) rvm_hal_device_open(name)
#define rvm_hal_qspi_close(dev) rvm_hal_device_close(dev)

/**
  \brief       Set config for qspi device
  \param[in]   dev    Pointer to device object.
  \param[in]   config Pointer to the configuration
  \return      0 : on success,  otherwise is error
*/
int rvm_hal_qspi_config(rvm_dev_t *dev, rvm_hal_qspi_config_t *config);

/**
  \brief       Get config for qspi device
  \param[in]   dev    Pointer to device object.
  \param[out]  config Pointer to the configuration
  \return      0 : on success,  otherwise is error
*/
int rvm_hal_qspi_config_get(rvm_dev_t *dev, rvm_hal_qspi_config_t *config);

/**
  \brief qspi send
  \param[in]  dev      Pointer to device object.
  \param[in]  cmd      tx command
  \param[in]  data     qspi send data
  \param[in]  size     qspi send data size
  \param[in]  timeout  timeout in milisecond, set this value to AOS_WAIT_FOREVER if you want to wait forever
  \return  0 : on success,  otherwise is error
 */
int rvm_hal_qspi_send(rvm_dev_t *dev, rvm_hal_qspi_cmd_t *cmd, const uint8_t *data, size_t size, uint32_t timeout);

/**
 \brief qspi_recv
 \param[in]   dev      Pointer to device object.
 \param[in]   cmd      rx command
 \param[out]  data     qspi recv data
 \param[in]   size     qspi recv data size
 \param[in]   timeout  timeout in milisecond, set this value to AOS_WAIT_FOREVER if you want to wait forever
 \return  0 : on success,  otherwise is error
 */
int rvm_hal_qspi_recv(rvm_dev_t *dev, rvm_hal_qspi_cmd_t *cmd, uint8_t *data, size_t size, uint32_t timeout);

/**
 \brief qspi send data and recv
 \param[in]  dev      Pointer to device object.
 \param[in]  cmd      tx/rx command
 \param[in]  tx_data  qspi send data
 \param[out] rx_data  qspi recv data
 \param[in]  size     qspi data to be sent and recived
 \param[in]  timeout  timeout in milisecond, set this value to AOS_WAIT_FOREVER if you want to wait forever
 \return  0, on success,  otherwise is error
 */
int rvm_hal_qspi_send_recv(rvm_dev_t *dev, rvm_hal_qspi_cmd_t *cmd, uint8_t *tx_data, uint8_t *rx_data, size_t size, uint32_t timeout);


#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <devices/vfs_qspi.h>
#endif

#ifdef __cplusplus
}
#endif

#endif
