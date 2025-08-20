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

#ifndef _DEVICE_SPI_H_
#define _DEVICE_SPI_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <devices/device.h>

#define DEFAULT_SPI_SERAIL_LEN 280

typedef enum
{
    RVM_HAL_SPI_MODE_MASTER,
    RVM_HAL_SPI_MODE_SLAVE,
} rvm_hal_spi_mode_e;

typedef enum
{
    RVM_HAL_SPI_FORMAT_CPOL0_CPHA0 = 0,              // CPOL = 0; CPHA = 0
    RVM_HAL_SPI_FORMAT_CPOL0_CPHA1,                  // CPOL = 1; CPHA = 0
    RVM_HAL_SPI_FORMAT_CPOL1_CPHA0,                  // CPOL = 0; CPHA = 1
    RVM_HAL_SPI_FORMAT_CPOL1_CPHA1,                  // CPOL = 1; CPHA = 1
} rvm_hal_spi_cp_format_e;

/* size of single spi frame data */
typedef enum
{
    RVM_HAL_SPI_DATA_SIZE_4BIT = 4,
    RVM_HAL_SPI_DATA_SIZE_5BIT,
    RVM_HAL_SPI_DATA_SIZE_6BIT,
    RVM_HAL_SPI_DATA_SIZE_7BIT,
    RVM_HAL_SPI_DATA_SIZE_8BIT,
    RVM_HAL_SPI_DATA_SIZE_9BIT,
    RVM_HAL_SPI_DATA_SIZE_10BIT,
    RVM_HAL_SPI_DATA_SIZE_11BIT,
    RVM_HAL_SPI_DATA_SIZE_12BIT,
    RVM_HAL_SPI_DATA_SIZE_13BIT,
    RVM_HAL_SPI_DATA_SIZE_14BIT,
    RVM_HAL_SPI_DATA_SIZE_15BIT,
    RVM_HAL_SPI_DATA_SIZE_16BIT,
} rvm_hal_spi_data_size_e;

/* Define spi config args */
typedef struct
{
    rvm_hal_spi_mode_e      mode;       /* spi communication mode */
    rvm_hal_spi_cp_format_e format;
    uint32_t                freq;       /* communication frequency Hz */
    uint16_t                serial_len; /* serial frame length, necessary for SPI running as Slave */
    bool                    dma_enable;
    rvm_hal_spi_data_size_e data_size;
} rvm_hal_spi_config_t;

#define rvm_hal_spi_open(name) rvm_hal_device_open(name)
#define rvm_hal_spi_close(dev) rvm_hal_device_close(dev)

/**
  \brief       Get default config for spi device
  \param[out]  config Pointer to the configuration
  \return      0 : on success,  otherwise is error
*/
int rvm_hal_spi_default_config_get(rvm_hal_spi_config_t *config);

/**
  \brief       Set config for spi device
  \param[in]   dev    Pointer to device object.
  \param[in]   config Pointer to the configuration
  \return      0 : on success,  otherwise is error
*/
int rvm_hal_spi_config(rvm_dev_t *dev, rvm_hal_spi_config_t *config);

/**
  \brief       Get config for spi device
  \param[in]   dev    Pointer to device object.
  \param[out]  config Pointer to the configuration
  \return      0 : on success,  otherwise is error
*/
int rvm_hal_spi_config_get(rvm_dev_t *dev, rvm_hal_spi_config_t *config);

/**
  \brief spi send
  \param[in]  dev      Pointer to device object.
  \param[in]  data     spi send data, need cache line align when enable DMA
  \param[in]  size     spi send data size
  \param[in]  timeout  timeout in milisecond, set this value to AOS_WAIT_FOREVER if you want to wait forever
  \return  0 : on success,  otherwise is error
 */
int rvm_hal_spi_send(rvm_dev_t *dev, const uint8_t *data, size_t size, uint32_t timeout);

/**
 \brief spi_recv
 \param[in]   dev      Pointer to device object.
 \param[out]  data     spi recv data, need cache line align when enable DMA
 \param[in]   size     spi recv data size
 \param[in]   timeout  timeout in milisecond, set this value to AOS_WAIT_FOREVER if you want to wait forever
 \return  0 : on success,  otherwise is error
 */
int rvm_hal_spi_recv(rvm_dev_t *dev, uint8_t *data, size_t size, uint32_t timeout);

/**
 \brief spi send data and recv
 \param[in]  dev      Pointer to device object.
 \param[in]  tx_data  spi send data, need cache line align when enable DMA
 \param[out] rx_data  spi recv data, need cache line align when enable DMA
 \param[in]  size     spi data to be sent and recived
 \param[in]  timeout  timeout in milisecond, set this value to AOS_WAIT_FOREVER if you want to wait forever
 \return  0, on success,  otherwise is error
 */
int rvm_hal_spi_send_recv(rvm_dev_t *dev, uint8_t *tx_data, uint8_t *rx_data, size_t size, uint32_t timeout);

/**
 \brief spi send data and then recv data
 \param[in]  dev      Pointer to device object.
 \param[in]  tx_data  the data to be sent, need cache line align when enable DMA
 \param[in]  tx_size  data size to be sent
 \param[out] rx_data  spi recv data, need cache line align when enable DMA
 \param[in]  rx_size  data size to be recived
 \param[in]  timeout  timeout in milisecond, set this value to AOS_WAIT_FOREVER if you want to wait forever
 \return  0, on success,  otherwise is error
*/
int rvm_hal_spi_send_then_recv(rvm_dev_t *dev, uint8_t *tx_data, size_t tx_size, uint8_t *rx_data,
                               size_t rx_size, uint32_t timeout);

/**
 \brief spi send data and then send data
 \param[in]  dev       Pointer to device object.
 \param[in]  tx1_data  the first data to be sent, need cache line align when enable DMA
 \param[in]  tx1_size  the first data size to be sent
 \param[out] tx2_data  the second data to be sent, need cache line align when enable DMA
 \param[in]  tx2_size  the second data size to be sent
 \param[in]  timeout   timeout in milisecond, set this value to AOS_WAIT_FOREVER if you want to wait forever
 \return  0, on success,  otherwise is error
 */
int rvm_hal_spi_send_then_send(rvm_dev_t *dev, uint8_t *tx1_data, size_t tx1_size, uint8_t *tx2_data,
                               size_t tx2_size, uint32_t timeout);


#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <devices/vfs_spi.h>
#endif

#ifdef __cplusplus
}
#endif

#endif
