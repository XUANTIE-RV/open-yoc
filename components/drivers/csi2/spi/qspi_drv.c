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
#include <drv/qspi.h>
#include <drv/clk.h>
#include <devices/impl/qspi_impl.h>

#define TAG "qspi_drv"

typedef struct {
    rvm_dev_t device;
    csi_qspi_t handle;
    aos_sem_t qspi_sem;
    csi_dma_ch_t dma_tx_ch;
    csi_dma_ch_t dma_rx_ch;
    bool trans_dma_en;
    rvm_hal_qspi_config_t config;
} qspi_dev_t;

#define QSPI(dev) ((qspi_dev_t *)dev)

static void qspi_event_cb_fun(csi_qspi_t *handle, csi_qspi_event_t event, void *arg)
{
    qspi_dev_t *dev;

    dev = (qspi_dev_t *)arg;

    switch (event) {
        case QSPI_EVENT_COMMAND_COMPLETE:
            aos_sem_signal(&QSPI(dev)->qspi_sem);
            break;

        default:
            ;
    }
}

static rvm_dev_t *_qspi_init(driver_t *drv, void *config, int id)
{
    qspi_dev_t *qspi = (qspi_dev_t *)rvm_hal_device_new(drv, sizeof(qspi_dev_t), id);

    return (rvm_dev_t *)qspi;
}

#define _qspi_uninit rvm_hal_device_free

static int _qspi_open(rvm_dev_t *dev)
{
    csi_error_t ret;

    ret = csi_qspi_init(&QSPI(dev)->handle, dev->id);
    if (ret < 0) {
        return -1;
    }

    ret = csi_qspi_attach_callback(&QSPI(dev)->handle, qspi_event_cb_fun, dev);
    if (ret < 0) {
        return -1;
    }

    ret = aos_sem_new(&QSPI(dev)->qspi_sem, 0);
    if (ret != 0) {
        return -1;
    }

    return 0;
}

static int _qspi_close(rvm_dev_t *dev)
{
    aos_sem_free(&QSPI(dev)->qspi_sem);

    if (QSPI(dev)->config.dma_enable) {
        csi_qspi_link_dma(&QSPI(dev)->handle, NULL, NULL);
    }
    csi_qspi_detach_callback(&QSPI(dev)->handle);
    csi_qspi_uninit(&QSPI(dev)->handle);
    return 0;
}

static int _qspi_clock(rvm_dev_t *dev, bool enable)
{
    if (enable) {
        csi_clk_enable(&QSPI(dev)->handle.dev);
    } else {
        csi_clk_disable(&QSPI(dev)->handle.dev);
    }
    return 0;
}

static int _qspi_lpm(rvm_dev_t *dev, int state)
{
#ifdef CONFIG_PM
    if (state == 0) {
        csi_qspi_disable_pm(&QSPI(dev)->handle);
    } else {
        csi_qspi_enable_pm(&QSPI(dev)->handle);
    }
#endif
    return 0;
}

static int _qspi_config(rvm_dev_t *dev, rvm_hal_qspi_config_t *config)
{
    csi_error_t ret;

    if (!config) {
        return -EINVAL;
    }

    ret = csi_qspi_frequence(&QSPI(dev)->handle, config->freq);
    if (ret != CSI_OK) {
        return -1;
    }

    ret = csi_qspi_mode(&QSPI(dev)->handle, (csi_qspi_mode_t)config->mode);
    if (ret != CSI_OK) {
        return -1;
    }

    if (config->dma_enable) {
        ret = csi_qspi_link_dma(&QSPI(dev)->handle, &QSPI(dev)->dma_tx_ch, &QSPI(dev)->dma_rx_ch);
        if (ret != CSI_OK) {
            return -1;
        }
    } else {
        ret = csi_qspi_link_dma(&QSPI(dev)->handle, NULL, NULL);
        if (ret != CSI_OK) {
            return -1;
        }
    }

    memcpy(&QSPI(dev)->config, config, sizeof(rvm_hal_qspi_config_t));
    return 0;
}

static int _qspi_config_get(rvm_dev_t *dev, rvm_hal_qspi_config_t *config)
{
    if (!config) {
        return -EINVAL;
    }
    memcpy(config, &QSPI(dev)->config, sizeof(rvm_hal_qspi_config_t));
    return 0;
}

static int _qspi_send(rvm_dev_t *dev, rvm_hal_qspi_cmd_t *cmd, const uint8_t *data, size_t size, uint32_t timeout)
{
    int ret;
    csi_qspi_command_t qspi_cmd;

    if (data == NULL || size == 0 || cmd == NULL) {
        return -EINVAL;
    }

    memcpy(&qspi_cmd, cmd, sizeof(rvm_hal_qspi_cmd_t));
    ret = csi_qspi_send_async(&QSPI(dev)->handle, &qspi_cmd, data, size);
    if (ret < 0) {
        return -1;
    }

    ret = aos_sem_wait(&QSPI(dev)->qspi_sem, timeout);
    if (ret != 0) {
        return -1;
    }

    return 0;
}

int _qspi_recv(rvm_dev_t *dev, rvm_hal_qspi_cmd_t *cmd, uint8_t *data, size_t size, uint32_t timeout)
{
    int ret;
    csi_qspi_command_t qspi_cmd;

    if (data == NULL || size == 0 || cmd == NULL) {
        return -EINVAL;
    }

    memcpy(&qspi_cmd, cmd, sizeof(rvm_hal_qspi_cmd_t));
    ret = csi_qspi_receive_async(&QSPI(dev)->handle, &qspi_cmd, data, size);
    if (ret < 0) {
        return -1;
    }

    ret = aos_sem_wait(&QSPI(dev)->qspi_sem, timeout);
    if (ret != 0) {
        return -1;
    }

    return 0;
}

int _qspi_send_recv(rvm_dev_t *dev, rvm_hal_qspi_cmd_t *cmd, uint8_t *tx_data, uint8_t *rx_data, size_t size, uint32_t timeout)
{
    int ret;
    csi_qspi_command_t qspi_cmd;

    if (tx_data == NULL || rx_data == NULL || size == 0 || cmd == NULL) {
        return -EINVAL;
    }

    memcpy(&qspi_cmd, cmd, sizeof(rvm_hal_qspi_cmd_t));
    ret = csi_qspi_send_receive_async(&QSPI(dev)->handle, &qspi_cmd, tx_data, rx_data, size);
    if (ret < 0) {
        return -1;
    }

    ret = aos_sem_wait(&QSPI(dev)->qspi_sem, timeout);
    if (ret != 0) {
        return -1;
    }

    return 0;
}

static qspi_driver_t qspi_driver = {
    .drv = {
        .name   = "qspi",
        .init   = _qspi_init,
        .uninit = _qspi_uninit,
        .open   = _qspi_open,
        .close  = _qspi_close,
        .clk_en = _qspi_clock,
        .lpm    = _qspi_lpm
    },
    .config          = _qspi_config,
    .config_get      = _qspi_config_get,
    .send            = _qspi_send,
    .recv            = _qspi_recv,
    .send_recv       = _qspi_send_recv,
};

void rvm_qspi_drv_register(int idx)
{
    rvm_driver_register(&qspi_driver.drv, NULL, idx);
}
