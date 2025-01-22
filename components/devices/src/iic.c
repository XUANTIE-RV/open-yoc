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
#include <devices/impl/iic_impl.h>

#define IIC_DRIVER(dev)  ((iic_driver_t*)(dev->drv))
#define IIC_VAILD(dev) do { \
    if (device_valid(dev, "iic") != 0) \
        return -1; \
} while(0)

void rvm_hal_iic_config_default(rvm_hal_iic_config_t *config)
{
    config->mode      = MODE_MASTER;
    config->speed     = BUS_SPEED_STANDARD;
    config->addr_mode = ADDR_7BIT;
}

int rvm_hal_iic_config(rvm_dev_t *dev, rvm_hal_iic_config_t *config)
{
    int ret;

    IIC_VAILD(dev);

    device_lock(dev);
    ret = IIC_DRIVER(dev)->config(dev, config);
    device_unlock(dev);

    return ret;
}

int rvm_hal_iic_config_get(rvm_dev_t *dev, rvm_hal_iic_config_t *config)
{
    int ret;

    IIC_VAILD(dev);

    device_lock(dev);
    ret = IIC_DRIVER(dev)->config_get(dev, config);
    device_unlock(dev);

    return ret;
}

int rvm_hal_iic_master_send(rvm_dev_t *dev, uint16_t dev_addr, const void *data, uint32_t size, uint32_t timeout)
{
    if (size == 0 || NULL == data) {
        return -EINVAL;
    }

    int ret;

    IIC_VAILD(dev);

    device_lock(dev);
    ret = IIC_DRIVER(dev)->master_send(dev, dev_addr, data, size, timeout);
    device_unlock(dev);

    return ret;
}

int rvm_hal_iic_master_recv(rvm_dev_t *dev, uint16_t dev_addr, void *data, uint32_t size, uint32_t timeout)
{
    int ret;

    IIC_VAILD(dev);

    device_lock(dev);
    ret = IIC_DRIVER(dev)->master_recv(dev, dev_addr, data, size, timeout);
    device_unlock(dev);

    return ret;
}

int rvm_hal_iic_slave_send(rvm_dev_t *dev, const void *data, uint32_t size, uint32_t timeout)
{
    int ret;

    IIC_VAILD(dev);

    device_lock(dev);
    ret = IIC_DRIVER(dev)->slave_send(dev, data, size, timeout);
    device_unlock(dev);

    return ret;
}

int rvm_hal_iic_slave_recv(rvm_dev_t *dev, void *data, uint32_t size, uint32_t timeout)
{
    int ret;

    IIC_VAILD(dev);

    device_lock(dev);
    ret = IIC_DRIVER(dev)->slave_recv(dev, data, size, timeout);
    device_unlock(dev);

    return ret;
}

int rvm_hal_iic_mem_write(rvm_dev_t *dev, uint16_t dev_addr, uint16_t mem_addr, uint16_t mem_addr_size,\
                  const void *data, uint32_t size, uint32_t timeout)
{
    int ret;

    IIC_VAILD(dev);

    device_lock(dev);
    ret = IIC_DRIVER(dev)->mem_write(dev, dev_addr, mem_addr, mem_addr_size, data, size, timeout);
    device_unlock(dev);

    return ret;
}

int rvm_hal_iic_mem_read(rvm_dev_t *dev, uint16_t dev_addr, uint16_t mem_addr, uint16_t mem_addr_size,\
                 void *data, uint32_t size, uint32_t timeout)
{
    int ret;

    IIC_VAILD(dev);

    device_lock(dev);
    ret = IIC_DRIVER(dev)->mem_read(dev, dev_addr, mem_addr, mem_addr_size, data, size, timeout);
    device_unlock(dev);

    return ret;
}

int rvm_hal_iic_trans_dma_enable(rvm_dev_t *dev, bool enable)
{
    int ret;

    IIC_VAILD(dev);

    device_lock(dev);
    ret = IIC_DRIVER(dev)->trans_dma_enable(dev, enable);
    device_unlock(dev);

    return ret;
}
