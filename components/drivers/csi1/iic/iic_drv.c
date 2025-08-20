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
#include <aos/kernel.h>
#include <drv/iic.h>

#include "devices/impl/iic_impl.h"

#define TAG "iic_drv"

typedef struct {
    rvm_dev_t      device;
    iic_handle_t   handle;
    aos_event_t    event;
    rvm_hal_iic_config_t   config;
} iic_dev_t;

#define EVT_TRANSFER_DONE   1
#define EVT_BUS_ERROR       (1<<1)

#define iic(dev) ((iic_dev_t *)dev)

static rvm_dev_t *iic_csky_init(driver_t *drv, void *config, int id)
{
    iic_dev_t *iic = (iic_dev_t *)rvm_hal_device_new(drv, sizeof(iic_dev_t), id);

    return (rvm_dev_t *)iic;
}

#define iic_csky_uninit rvm_hal_device_free

static void iic_event_cb_fun(int32_t idx, iic_event_e event)
{
    static iic_dev_t *iic_idx[6];
    iic_dev_t *iic;

    if (iic_idx[idx] == NULL) {
        iic_idx[idx] = (iic_dev_t *)rvm_hal_device_find("iic", idx);
    }

    iic = iic_idx[idx];
    switch (event) {
        case IIC_EVENT_TRANSFER_DONE:
            aos_event_set(&iic->event, EVT_TRANSFER_DONE, AOS_EVENT_OR);
            break;

        case IIC_EVENT_BUS_ERROR:
            csi_iic_abort_transfer(iic->handle);
            aos_event_set(&iic->event, EVT_BUS_ERROR, AOS_EVENT_OR);
            break;
        default: break;
    }
}

static int iic_csky_open(rvm_dev_t *dev)
{
    iic_handle_t iic_handle = csi_iic_initialize(dev->id, iic_event_cb_fun);

    if (iic_handle == NULL) {
        LOGE(TAG, "csi_iic_initialize error");
        return -1;
    }
    iic(dev)->handle = iic_handle;
    aos_event_new(&iic(dev)->event, 0);
    return 0;
}

static int iic_csky_close(rvm_dev_t *dev)
{
    csi_iic_uninitialize(iic(dev)->handle);
    aos_event_free(&iic(dev)->event);
    return 0;
}

static int iic_csky_config(rvm_dev_t *dev, rvm_hal_iic_config_t *config)
{
    int32_t ret = csi_iic_config(iic(dev)->handle, config->mode, config->speed,
                                 config->addr_mode, config->slave_addr);

    if (ret < 0) {
        return -EIO;
    }

    memcpy(&iic(dev)->config, config, sizeof(rvm_hal_iic_config_t));

    return 0;
}

static int iic_busy_stat(rvm_dev_t *dev)
{
    iic_status_t stat = csi_iic_get_status(iic(dev)->handle);

    if (stat.busy != 0) {
        LOGE(TAG, "iic(%d) read busy", dev->id);
        csi_iic_abort_transfer(iic(dev)->handle);
        return -EBUSY;
    }

    return 0;
}

static int iic_csky_master_send(rvm_dev_t *dev, uint16_t dev_addr, const void *data, uint32_t size, uint32_t timeout)
{
    int ret;
    unsigned int flags = 0;

    if (iic_busy_stat(dev) < 0) {
        return -1;
    }

    ret = csi_iic_master_send(iic(dev)->handle, dev_addr/*iic(dev)->config.slave_addr*/, data, size, 0);

    if (ret < 0) {
        LOGE(TAG, "iic send fail");
    }

    aos_event_get(&iic(dev)->event, EVT_TRANSFER_DONE | EVT_BUS_ERROR, AOS_EVENT_OR_CLEAR, &flags, timeout);

    if (flags & EVT_BUS_ERROR) {
        return -1;
    } else if (flags & EVT_TRANSFER_DONE) {
        return 0;
    }

    return -ETIMEDOUT;
}

static int iic_csky_master_recv(rvm_dev_t *dev, uint16_t dev_addr, void *data, uint32_t size, uint32_t timeout)
{
    int ret;
    unsigned int flags = 0;

    if (iic_busy_stat(dev) < 0) {
        return -1;
    }

    ret = csi_iic_master_receive(iic(dev)->handle, dev_addr, data, size, 0);

    if (ret < 0) {
        LOGE(TAG, "iic recv fail");
    }

    aos_event_get(&iic(dev)->event, EVT_TRANSFER_DONE | EVT_BUS_ERROR, AOS_EVENT_OR_CLEAR, &flags, timeout);

    if (flags & EVT_BUS_ERROR) {
        return -1;
    } else if (flags & EVT_TRANSFER_DONE) {
        return 0;
    }

    return -ETIMEDOUT;
}

static int iic_csky_slave_send(rvm_dev_t *dev, const void *data, uint32_t size, uint32_t timeout)
{
    int ret;
    unsigned int flags = 0;

    if (iic_busy_stat(dev) < 0) {
        return -1;
    }

    ret = csi_iic_slave_send(iic(dev)->handle, data, size);

    if (ret < 0) {
        LOGE(TAG, "iic slave send fail");
    }

    aos_event_get(&iic(dev)->event, EVT_TRANSFER_DONE | EVT_BUS_ERROR, AOS_EVENT_OR_CLEAR, &flags, timeout);

    if (flags & EVT_BUS_ERROR) {
        return -1;
    } else if (flags & EVT_TRANSFER_DONE) {
        return 0;
    }

    return -ETIMEDOUT;
}

static int iic_csky_slave_recv(rvm_dev_t *dev, void *data, uint32_t size, uint32_t timeout)
{
    int ret;
    unsigned int flags = 0;

    if (iic_busy_stat(dev) < 0) {
        return -1;
    }

    ret = csi_iic_slave_receive(iic(dev)->handle, data, size);

    if (ret < 0) {
        LOGE(TAG, "iic slave recv fail");
    }

    aos_event_get(&iic(dev)->event, EVT_TRANSFER_DONE | EVT_BUS_ERROR, AOS_EVENT_OR_CLEAR, &flags, timeout);

    if (flags & EVT_BUS_ERROR) {
        return -1;
    } else if (flags & EVT_TRANSFER_DONE) {
        return 0;
    }

    return -ETIMEDOUT;
}

static int iic_csky_mem_write(rvm_dev_t *dev, uint16_t dev_addr, uint16_t mem_addr, uint16_t mem_addr_size,
                                const void *data, uint32_t size, uint32_t timeout)
{
    int ret  = 0;
    uint8_t *buf;
    unsigned int flags = 0;

    buf = malloc(mem_addr_size + size);
    if (buf == NULL) {
        return -1;
    }
    if (mem_addr_size == I2C_MEM_ADDR_SIZE_8BIT) {
        buf[0] = (uint8_t)(mem_addr & 0xff);
    } else if (mem_addr_size == I2C_MEM_ADDR_SIZE_16BIT) {
        buf[0] = (uint8_t)((mem_addr & 0xff00) >> 8);
        buf[1]= (uint8_t)(mem_addr & 0xff);
    } else {
        return -EINVAL;
    }

    memcpy(&buf[mem_addr_size], data, size);
    ret = csi_iic_master_send(iic(dev)->handle, dev_addr, buf, mem_addr_size + size, 0);
    if (ret < 0) {
        LOGE(TAG, "iic mem write fail");
    }
    aos_event_get(&iic(dev)->event, EVT_TRANSFER_DONE | EVT_BUS_ERROR, AOS_EVENT_OR_CLEAR, &flags, timeout);

    free(buf);

    if (flags & EVT_BUS_ERROR) {
        return -1;
    } else if (flags & EVT_TRANSFER_DONE) {
        return 0;
    }

    return -ETIMEDOUT;
}

static int iic_csky_mem_read(rvm_dev_t *dev, uint16_t dev_addr, uint16_t mem_addr, uint16_t mem_addr_size,
                                void *data, uint32_t size, uint32_t timeout)
{
    int ret;
    unsigned int flags = 0;

    if (mem_addr_size == I2C_MEM_ADDR_SIZE_8BIT) {
         mem_addr &= 0xff;
    } else if (mem_addr_size == I2C_MEM_ADDR_SIZE_16BIT) {
        uint8_t temp = (uint8_t)((mem_addr & 0xff00) >> 8);
        mem_addr = (mem_addr & 0xff) << 8;
        mem_addr |= temp;
    } else {
        return -EINVAL;
    }

    ret = csi_iic_master_send(iic(dev)->handle, dev_addr, &mem_addr, mem_addr_size, 0);
    if (ret != 0) {
        return -1;
    }
    aos_event_get(&iic(dev)->event, EVT_TRANSFER_DONE | EVT_BUS_ERROR, AOS_EVENT_OR_CLEAR, &flags, timeout);
    if (flags & EVT_BUS_ERROR) {
        return -1;
    } else if (flags & EVT_TRANSFER_DONE) {
        
    } else {
        return -ETIMEDOUT;
    }
    ret = csi_iic_master_receive(iic(dev)->handle, dev_addr, data, size, 0);
    if (ret != 0) {
        return -1;
    }
    aos_event_get(&iic(dev)->event, EVT_TRANSFER_DONE | EVT_BUS_ERROR, AOS_EVENT_OR_CLEAR, &flags, timeout);
    if (flags & EVT_BUS_ERROR) {
        return -1;
    } else if (flags & EVT_TRANSFER_DONE) {
        return 0;
    }

    return -ETIMEDOUT;
}

static iic_driver_t iic_driver = {
    .drv = {
        .name   = "iic",
        .init   = iic_csky_init,
        .uninit = iic_csky_uninit,
        .open   = iic_csky_open,
        .close  = iic_csky_close,
    },
    .config          = iic_csky_config,
    .master_send     = iic_csky_master_send,
    .master_recv     = iic_csky_master_recv,
    .slave_send      = iic_csky_slave_send,
    .slave_recv      = iic_csky_slave_recv,
    .mem_write       = iic_csky_mem_write,
    .mem_read        = iic_csky_mem_read
};

void rvm_iic_drv_register(int idx)
{
    rvm_driver_register(&iic_driver.drv, NULL, idx);
}
