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
#include <devices/impl/spi_impl.h>

#define SPI_DRIVER(dev)  ((spi_driver_t*)(dev->drv))
#define SPI_VAILD(dev) do { \
    if (device_valid(dev, "spi") != 0) \
        return -1; \
} while(0)

int rvm_hal_spi_default_config_get(rvm_hal_spi_config_t *config)
{
    if (!config)
        return -EINVAL;

    config->mode = RVM_HAL_SPI_MODE_MASTER;
    config->format = RVM_HAL_SPI_FORMAT_CPOL0_CPHA0;
    config->freq = 500 * 1000;  // 500KHz
    config->serial_len = DEFAULT_SPI_SERAIL_LEN;
    config->data_size = RVM_HAL_SPI_DATA_SIZE_8BIT;
    return 0;
}

int rvm_hal_spi_config(rvm_dev_t *dev, rvm_hal_spi_config_t *config)
{
    int ret;

    SPI_VAILD(dev);

    device_lock(dev);
    ret = SPI_DRIVER(dev)->config(dev, config);
    device_unlock(dev);

    return ret;
}

int rvm_hal_spi_config_get(rvm_dev_t *dev, rvm_hal_spi_config_t *config)
{
    int ret;

    SPI_VAILD(dev);

    device_lock(dev);
    ret = SPI_DRIVER(dev)->config_get(dev, config);
    device_unlock(dev);

    return ret;
}

int rvm_hal_spi_send(rvm_dev_t *dev, const uint8_t *data, size_t size, uint32_t timeout)
{
    int ret;

    SPI_VAILD(dev);

    device_lock(dev);
    ret = SPI_DRIVER(dev)->send(dev, data, size, timeout);
    device_unlock(dev);

    return ret;  
}

int rvm_hal_spi_recv(rvm_dev_t *dev, uint8_t *data, size_t size, uint32_t timeout)
{
    int ret;

    SPI_VAILD(dev);

    device_lock(dev);
    ret = SPI_DRIVER(dev)->recv(dev, data, size, timeout);
    device_unlock(dev);

    return ret;  
}

int rvm_hal_spi_send_recv(rvm_dev_t *dev, uint8_t *tx_data, uint8_t *rx_data, size_t size, uint32_t timeout)
{
    int ret;

    SPI_VAILD(dev);

    device_lock(dev);
    ret = SPI_DRIVER(dev)->send_recv(dev, tx_data, rx_data, size, timeout);
    device_unlock(dev);

    return ret; 
}

int rvm_hal_spi_send_then_recv(rvm_dev_t *dev, uint8_t *tx_data, size_t tx_size,
                               uint8_t *rx_data, size_t rx_size, uint32_t timeout)
{
    int ret;

    SPI_VAILD(dev);

    device_lock(dev);
    ret = SPI_DRIVER(dev)->send_then_recv(dev, tx_data, tx_size, rx_data, rx_size, timeout);
    device_unlock(dev);

    return ret;   
}

int rvm_hal_spi_send_then_send(rvm_dev_t *dev, uint8_t *tx1_data, size_t tx1_size,
                               uint8_t *tx2_data, size_t tx2_size, uint32_t timeout)
{
    int ret;

    SPI_VAILD(dev);

    device_lock(dev);
    ret = SPI_DRIVER(dev)->send_then_send(dev, tx1_data, tx1_size, tx2_data, tx2_size, timeout);
    device_unlock(dev);

    return ret;
}
