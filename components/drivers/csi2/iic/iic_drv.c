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
#include <debug/dbg.h>
#include <drv/iic.h>
#include <drv/clk.h>
#include <devices/impl/iic_impl.h>

#define TAG "iic_drv"

typedef struct {
    rvm_dev_t      device;
    csi_iic_t      handle;
    aos_event_t    event;
    rvm_hal_iic_config_t   config;
    int  dma_en;
    csi_dma_ch_t g_dma_ch_tx;
    csi_dma_ch_t g_dma_ch_rx;
} iic_dev_t;

#define EVT_SEND_DONE       1
#define EVT_RECV_DONE       1

#define IIC(dev) ((iic_dev_t *)dev)

static void iic_csky_event_cb_fun(csi_iic_t *iic_handler, csi_iic_event_t event, void *arg)
{
    rvm_dev_t *dev = (rvm_dev_t *)arg;

    if (event == IIC_EVENT_SEND_COMPLETE) {
        if (aos_event_is_valid(&IIC(dev)->event)) {
            aos_event_set(&IIC(dev)->event, EVT_SEND_DONE, AOS_EVENT_OR);
        }
    } else if (event == IIC_EVENT_RECEIVE_COMPLETE) {
        if (aos_event_is_valid(&IIC(dev)->event)) {
            aos_event_set(&IIC(dev)->event, EVT_RECV_DONE, AOS_EVENT_OR);
        }
    } else {
        printk("i2c err event %d\n", event);
    }
}

static rvm_dev_t *iic_csky_init(driver_t *drv, void *config, int id)
{
    iic_dev_t *iic = (iic_dev_t *)rvm_hal_device_new(drv, sizeof(iic_dev_t), id);

    return (rvm_dev_t *)iic;
}

#define iic_csky_uninit rvm_hal_device_free

static int iic_csky_open(rvm_dev_t *dev)
{
    csi_error_t ret = csi_iic_init(&IIC(dev)->handle, dev->id);

    if (ret != CSI_OK) {
        LOGE(TAG, "csi_iic_init error");
        return -1;
    }
    aos_event_new(&IIC(dev)->event, 0);
    IIC(dev)->dma_en = 0;
    return 0;
}

static int iic_csky_close(rvm_dev_t *dev)
{
    if (IIC(dev)->dma_en) {
        csi_iic_link_dma(&IIC(dev)->handle, NULL, NULL);
        IIC(dev)->dma_en = 0;
    }
    csi_iic_uninit(&IIC(dev)->handle);
    csi_iic_detach_callback(&IIC(dev)->handle);
    aos_event_free(&IIC(dev)->event);
    return 0;
}

static int iic_csky_clock(rvm_dev_t *dev, bool enable)
{
    if (enable) {
        csi_clk_enable(&IIC(dev)->handle.dev);
    } else {
        csi_clk_disable(&IIC(dev)->handle.dev);
    }
    return 0;
}

static int iic_csky_config(rvm_dev_t *dev, rvm_hal_iic_config_t *config)
{
    csi_error_t ret = csi_iic_mode(&IIC(dev)->handle, (csi_iic_mode_t)config->mode);
    if (ret != CSI_OK) {
        LOGE(TAG, "csi_iic set mode error");
        return -1;
    }

    ret = csi_iic_addr_mode(&IIC(dev)->handle, (csi_iic_addr_mode_t)config->addr_mode);
    if (ret != CSI_OK) {
        LOGE(TAG, "csi_iic_addr_mode error");
        return -1;
    }

    ret = csi_iic_speed(&IIC(dev)->handle, (csi_iic_speed_t)config->speed);
    if (ret != CSI_OK) {
        LOGE(TAG, "csi_iic set speed error");
        return -1;
    }

    if (config->mode == MODE_SLAVE) {
        ret = csi_iic_own_addr(&IIC(dev)->handle, config->slave_addr);
        if (ret != CSI_OK) {
            LOGE(TAG, "set csi_iic_own_addr error");
            return -1;
        }
    }

    ret = csi_iic_attach_callback(&IIC(dev)->handle, iic_csky_event_cb_fun, dev);
    if (ret != CSI_OK) {
        return -EIO;
    }

    memcpy(&IIC(dev)->config, config, sizeof(rvm_hal_iic_config_t));

    return 0;
}

static int iic_csky_config_get(rvm_dev_t *dev, rvm_hal_iic_config_t* config)
{
    if (!config) {
        return -EINVAL;
    }
    memcpy(config, &IIC(dev)->config, sizeof(IIC(dev)->config));
    return 0;
}

static int iic_csky_master_send(rvm_dev_t *dev, uint16_t dev_addr, const void *data, uint32_t size, uint32_t timeout)
{
    unsigned int flags = 0;

    csi_error_t ret = csi_iic_master_send_async(&IIC(dev)->handle, dev_addr, data, size);
    if (ret < 0) {
        LOGE(TAG, "iic send fail");
        return -1;
    }

    aos_event_get(&IIC(dev)->event, EVT_SEND_DONE, AOS_EVENT_OR_CLEAR, &flags, timeout);

    if (flags & EVT_SEND_DONE) {
        return 0;
    } else {
        return -1;
    }

    return -ETIMEDOUT;
}

static int iic_csky_master_recv(rvm_dev_t *dev, uint16_t dev_addr, void *data, uint32_t size, uint32_t timeout)
{
    int ret;
    unsigned int flags = 0;

    ret = csi_iic_master_receive_async(&IIC(dev)->handle, dev_addr, data, size);

    if (ret < 0) {
        LOGE(TAG, "iic recv fail");
    }

    aos_event_get(&IIC(dev)->event, EVT_RECV_DONE, AOS_EVENT_OR_CLEAR, &flags, timeout);

    if (flags & EVT_RECV_DONE) {
        return 0;
    } else {
        return -1;
    }

    return -ETIMEDOUT;
}

static int iic_csky_slave_send(rvm_dev_t *dev, const void *data, uint32_t size, uint32_t timeout)
{
    int ret;
    unsigned int flags = 0;

    ret = csi_iic_slave_send_async(&IIC(dev)->handle, data, size);

    if (ret < 0) {
        LOGE(TAG, "iic slave send fail");
    }

    aos_event_get(&IIC(dev)->event, EVT_SEND_DONE, AOS_EVENT_OR_CLEAR, &flags, timeout);

    if (flags & EVT_SEND_DONE) {
        return 0;
    } else {
        return -1;
    }

    return -ETIMEDOUT;
}

static int iic_csky_slave_recv(rvm_dev_t *dev, void *data, uint32_t size, uint32_t timeout)
{
    int ret;
    unsigned int flags = 0;

    ret = csi_iic_slave_receive_async(&IIC(dev)->handle, data, size);

    if (ret < 0) {
        LOGE(TAG, "iic slave recv fail");
    }

    aos_event_get(&IIC(dev)->event, EVT_RECV_DONE, AOS_EVENT_OR_CLEAR, &flags, timeout);

    if (flags & EVT_RECV_DONE) {
        return 0;
    } else {
        return -1;
    }

    return -ETIMEDOUT;
}

static int iic_csky_mem_write(rvm_dev_t *dev, uint16_t dev_addr, uint16_t mem_addr, uint16_t mem_addr_size,
                                const void *data, uint32_t size, uint32_t timeout)
{
    int32_t ret_len = csi_iic_mem_send(&IIC(dev)->handle, dev_addr, mem_addr, mem_addr_size, data, size, timeout);
    if (ret_len != size) {
        return -ETIMEDOUT;
    }

    return 0;
}

static int iic_csky_mem_read(rvm_dev_t *dev, uint16_t dev_addr, uint16_t mem_addr, uint16_t mem_addr_size,
                                void *data, uint32_t size, uint32_t timeout)
{
    int32_t ret_len = csi_iic_mem_receive(&IIC(dev)->handle, dev_addr, mem_addr, mem_addr_size, data, size, timeout);
    if (ret_len != size) {
        return -ETIMEDOUT;
    }

    return 0;
}

static int iic_csky_trans_dma_enable(rvm_dev_t *dev, bool enable)
{
    csi_error_t ret;
    if (!enable) {
        ret = csi_iic_link_dma(&IIC(dev)->handle, NULL, NULL);
        if (ret != CSI_OK) {
            return -1;
        }
        IIC(dev)->dma_en = 0;
    } else {
        ret = csi_iic_link_dma(&IIC(dev)->handle, &IIC(dev)->g_dma_ch_tx, &IIC(dev)->g_dma_ch_rx);
        if (ret != CSI_OK) {
            return -1;
        }
        IIC(dev)->dma_en = 1;
    }
    return 0;
}

static iic_driver_t iic_driver = {
    .drv = {
        .name   = "iic",
        .init   = iic_csky_init,
        .uninit = iic_csky_uninit,
        .open   = iic_csky_open,
        .close  = iic_csky_close,
        .clk_en = iic_csky_clock
    },
    .timeout         = AOS_WAIT_FOREVER,
    .config          = iic_csky_config,
    .config_get      = iic_csky_config_get,
    .master_send     = iic_csky_master_send,
    .master_recv     = iic_csky_master_recv,
    .slave_send      = iic_csky_slave_send,
    .slave_recv      = iic_csky_slave_recv,
    .mem_write       = iic_csky_mem_write,
    .mem_read        = iic_csky_mem_read,
    .trans_dma_enable = iic_csky_trans_dma_enable
};

void rvm_iic_drv_register(int idx)
{
    rvm_driver_register(&iic_driver.drv, NULL, idx);
}
