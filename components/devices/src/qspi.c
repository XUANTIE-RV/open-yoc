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
#include <devices/impl/qspi_impl.h>

#define QSPI_DRIVER(dev)  ((qspi_driver_t*)(dev->drv))
#define QSPI_VAILD(dev) do { \
    if (device_valid(dev, "qspi") != 0) \
        return -1; \
} while(0)

int rvm_hal_qspi_config(rvm_dev_t *dev, rvm_hal_qspi_config_t *config)
{
    int ret;

    QSPI_VAILD(dev);

    device_lock(dev);
    ret = QSPI_DRIVER(dev)->config(dev, config);
    device_unlock(dev);

    return ret;
}

int rvm_hal_qspi_config_get(rvm_dev_t *dev, rvm_hal_qspi_config_t *config)
{
    int ret;

    QSPI_VAILD(dev);

    device_lock(dev);
    ret = QSPI_DRIVER(dev)->config_get(dev, config);
    device_unlock(dev);

    return ret;
}

int rvm_hal_qspi_send(rvm_dev_t *dev, rvm_hal_qspi_cmd_t *cmd, const uint8_t *data, size_t size, uint32_t timeout)
{
    int ret;

    QSPI_VAILD(dev);

    device_lock(dev);
    ret = QSPI_DRIVER(dev)->send(dev, cmd, data, size, timeout);
    device_unlock(dev);

    return ret;  
}

int rvm_hal_qspi_recv(rvm_dev_t *dev, rvm_hal_qspi_cmd_t *cmd, uint8_t *data, size_t size, uint32_t timeout)
{
    int ret;

    QSPI_VAILD(dev);

    device_lock(dev);
    ret = QSPI_DRIVER(dev)->recv(dev, cmd, data, size, timeout);
    device_unlock(dev);

    return ret;  
}

int rvm_hal_qspi_send_recv(rvm_dev_t *dev, rvm_hal_qspi_cmd_t *cmd, uint8_t *tx_data, uint8_t *rx_data, size_t size, uint32_t timeout)
{
    int ret;

    QSPI_VAILD(dev);

    device_lock(dev);
    ret = QSPI_DRIVER(dev)->send_recv(dev, cmd, tx_data, rx_data, size, timeout);
    device_unlock(dev);

    return ret;  
}
