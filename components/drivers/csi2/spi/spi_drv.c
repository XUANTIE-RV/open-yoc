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
#include <drv/spi.h>
#include <drv/clk.h>
#include <devices/impl/spi_impl.h>

#define TAG "spi_drv"

typedef struct {
    rvm_dev_t device;
    csi_spi_t handle;
    aos_sem_t spi_sem;
    csi_dma_ch_t dma_tx_ch;
    csi_dma_ch_t dma_rx_ch;
    bool trans_dma_en;
    rvm_hal_spi_config_t config;
} spi_dev_t;

#define SPI(dev) ((spi_dev_t *)dev)

static void spi_event_cb_fun(csi_spi_t *handle, csi_spi_event_t event, void *arg)
{
    spi_dev_t *dev;

    dev = (spi_dev_t *)arg;

    switch (event) {
        case SPI_EVENT_SEND_COMPLETE:
        case SPI_EVENT_RECEIVE_COMPLETE:
        case SPI_EVENT_SEND_RECEIVE_COMPLETE:
            aos_sem_signal(&SPI(dev)->spi_sem);
            break;

        default:
            ;
    }
}

static rvm_dev_t *_spi_init(driver_t *drv, void *config, int id)
{
    spi_dev_t *spi = (spi_dev_t *)rvm_hal_device_new(drv, sizeof(spi_dev_t), id);

    return (rvm_dev_t *)spi;
}

#define _spi_uninit rvm_hal_device_free

static int _spi_open(rvm_dev_t *dev)
{
    csi_error_t ret;

    ret = csi_spi_init(&SPI(dev)->handle, dev->id);
    if (ret < 0) {
        return -1;
    }

    ret = csi_spi_attach_callback(&SPI(dev)->handle, spi_event_cb_fun, dev);
    if (ret < 0) {
        return -1;
    }

    ret = aos_sem_new(&SPI(dev)->spi_sem, 0);
    if (ret != 0) {
        return -1;
    }

    return 0;
}

static int _spi_close(rvm_dev_t *dev)
{
    aos_sem_free(&SPI(dev)->spi_sem);

    if (SPI(dev)->config.dma_enable) {
        csi_spi_link_dma(&SPI(dev)->handle, NULL, NULL);
    }
    csi_spi_detach_callback(&SPI(dev)->handle);
    csi_spi_uninit(&SPI(dev)->handle);
    return 0;
}

static int _spi_clock(rvm_dev_t *dev, bool enable)
{
    if (enable) {
        csi_clk_enable(&SPI(dev)->handle.dev);
    } else {
        csi_clk_disable(&SPI(dev)->handle.dev);
    }
    return 0;
}

static int _spi_lpm(rvm_dev_t *dev, int state)
{
#ifdef CONFIG_PM
    if (state == 0) {
        csi_spi_disable_pm(&SPI(dev)->handle);
    } else {
        csi_spi_enable_pm(&SPI(dev)->handle);
    }
#endif
    return 0;
}

static int _spi_config(rvm_dev_t *dev, rvm_hal_spi_config_t *config)
{
    csi_error_t ret;

    if (!config) {
        return -EINVAL;
    }

    if(config->mode == RVM_HAL_SPI_MODE_MASTER) {
        ret = csi_spi_mode(&SPI(dev)->handle, SPI_MASTER);
        if (ret < 0) {
            return -1;
        }
    } else if(config->mode == RVM_HAL_SPI_MODE_SLAVE) {
        ret = csi_spi_mode(&SPI(dev)->handle, SPI_SLAVE);
        if (ret < 0) {
            return -1;
        }
    }

    if (config->mode == RVM_HAL_SPI_MODE_MASTER) {
        csi_spi_baud(&SPI(dev)->handle, config->freq);
        csi_spi_select_slave(&SPI(dev)->handle, 0);
    }

    if (config->dma_enable) {
        csi_spi_link_dma(&SPI(dev)->handle, &SPI(dev)->dma_tx_ch, &SPI(dev)->dma_rx_ch);
    } else {
        csi_spi_link_dma(&SPI(dev)->handle, NULL, NULL);
    }

    ret = csi_spi_cp_format(&SPI(dev)->handle, (csi_spi_cp_format_t)config->format);
    if (ret < 0) {
        return -1;
    }

    ret = csi_spi_frame_len(&SPI(dev)->handle, (csi_spi_frame_len_t)config->data_size);
    if (ret < 0) {
        return -1;
    }

    memcpy(&SPI(dev)->config, config, sizeof(rvm_hal_spi_config_t));
    return 0;
}

static int _spi_config_get(rvm_dev_t *dev, rvm_hal_spi_config_t *config)
{
    if (!config) {
        return -EINVAL;
    }
    memcpy(config, &SPI(dev)->config, sizeof(rvm_hal_spi_config_t));
    return 0;
}

static int _spi_send(rvm_dev_t *dev, const uint8_t *data, size_t size, uint32_t timeout)
{
    int ret;

    if (data == NULL || size == 0) {
        return -EINVAL;
    }

    ret = csi_spi_send_async(&SPI(dev)->handle, data, size);
    if (ret < 0) {
        return -1;
    }

    ret = aos_sem_wait(&SPI(dev)->spi_sem, timeout);
    if (ret != 0) {
        return -1;
    }

    return 0;
}

int _spi_recv(rvm_dev_t *dev, uint8_t *data, size_t size, uint32_t timeout)
{
    int ret;

    if (data == NULL || size == 0) {
        return -EINVAL;
    }

    ret = csi_spi_receive_async(&SPI(dev)->handle, data, size);
    if (ret < 0) {
        return -1;
    }

    ret = aos_sem_wait(&SPI(dev)->spi_sem, timeout);
    if (ret != 0) {
        return -1;
    }

    return 0;
}

int _spi_send_recv(rvm_dev_t *dev, uint8_t *tx_data, uint8_t *rx_data, size_t size, uint32_t timeout)
{
    int ret;

    if (tx_data == NULL || rx_data == NULL || size == 0) {
        return -EINVAL;
    }

    ret = csi_spi_send_receive_async(&SPI(dev)->handle, tx_data, rx_data, size);
    if (ret < 0) {
        return -1;
    }

    ret = aos_sem_wait(&SPI(dev)->spi_sem, timeout);
    if (ret != 0) {
        return -1;
    }

    return 0;
}

int _spi_send_then_recv(rvm_dev_t *dev, uint8_t *tx_data, size_t tx_size, uint8_t *rx_data,
                        size_t rx_size, uint32_t timeout)
{
    int ret;

    if (tx_data == NULL || tx_size == 0 || rx_data == NULL || rx_size == 0)
        return -EINVAL;

    ret = _spi_send(dev, tx_data, tx_size, timeout);
    if (ret != 0) {
        return -1;
    }

    ret = _spi_recv(dev, rx_data, rx_size, timeout);
    if (ret != 0) {
        return -1;
    }

    return 0;
}

int _spi_send_then_send(rvm_dev_t *dev, uint8_t *tx1_data, size_t tx1_size, uint8_t *tx2_data,
                        size_t tx2_size, uint32_t timeout)
{
    int ret;

    if (tx1_data == NULL || tx1_size == 0 || tx2_data == NULL || tx2_size == 0)
        return -EINVAL;

    ret = _spi_send(dev, tx1_data, tx1_size, timeout);
    if (ret != 0) {
        return -1;
    }

    ret = _spi_send(dev, tx2_data, tx2_size, timeout);
    if (ret != 0) {
        return -1;
    }

    return 0;
}

static spi_driver_t spi_driver = {
    .drv = {
        .name   = "spi",
        .init   = _spi_init,
        .uninit = _spi_uninit,
        .open   = _spi_open,
        .close  = _spi_close,
        .clk_en = _spi_clock,
        .lpm    = _spi_lpm
    },
    .timeout         = AOS_WAIT_FOREVER,
    .config          = _spi_config,
    .config_get      = _spi_config_get,
    .send            = _spi_send,
    .recv            = _spi_recv,
    .send_recv       = _spi_send_recv,
    .send_then_recv  = _spi_send_then_recv,
    .send_then_send  = _spi_send_then_send
};

void rvm_spi_drv_register(int idx)
{
    rvm_driver_register(&spi_driver.drv, NULL, idx);
}
