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
#include <drv/adc.h>

#include "devices/impl/adc_impl.h"

#define TAG "adc_drv"

typedef struct {
    rvm_dev_t       device;
    adc_handle_t    handle;
    rvm_hal_adc_config_t    config;
} adc_dev_t;

#define adc(dev) ((adc_dev_t *)dev)

static rvm_dev_t *adc_csky_init(driver_t *drv, void *config, int id)
{
    adc_dev_t *adc = (adc_dev_t *)rvm_hal_device_new(drv, sizeof(adc_dev_t), id);

    return (rvm_dev_t *)adc;
}

#define adc_csky_uninit rvm_hal_device_free

static int adc_csky_open(rvm_dev_t *dev)
{
    adc_handle_t adc_handle = drv_adc_initialize(dev->id, NULL);

    if (adc_handle == NULL) {
        LOGE(TAG, "csi_iic_initialize error\n");
        return -1;
    }
    adc(dev)->handle = adc_handle;
    return 0;
}

static int adc_csky_close(rvm_dev_t *dev)
{
    drv_adc_uninitialize(adc(dev)->handle);
    return 0;
}

static int adc_csky_config(rvm_dev_t *dev, rvm_hal_adc_config_t *config)
{
    int ret;
    adc_conf_t sconfig;

    sconfig.mode          = config->mode;
    sconfig.trigger       = config->trigger;
    sconfig.intrp_mode    = config->intrp_mode;
    sconfig.channel_array = config->channel;
    sconfig.channel_nbr   = 1;
    sconfig.conv_cnt      = 1;
    sconfig.sampling_time = config->sampling_time;
    ret = drv_adc_config(adc(dev)->handle, &sconfig);

    if (ret < 0) {
        return -EIO;
    }

    memcpy(&adc(dev)->config, config, sizeof(rvm_hal_adc_config_t));

    return 0;
}

static int adc_csky_read(rvm_dev_t *dev, void *output, uint32_t timeout)
{
    int ret;
    uint32_t recv_data = 0;

    if (dev == NULL || output == NULL) {
        return -EINVAL;
    }
    adc_handle_t handle = adc(dev)->handle;

    /* adc start */
    ret = drv_adc_start(handle);

    if (ret < 0) {
        return -1;
    }

    /* adc read */
    ret = drv_adc_read(handle, &recv_data, 1);
    // FIXME: why read twice?
    ret = drv_adc_read(handle, &recv_data, 1);

    if (ret == 0)
        *((uint32_t *)output) = recv_data;

    drv_adc_stop(handle);

    return ret;
}

static int adc_csky_pin2channel(rvm_dev_t *dev, int pin)
{
    int channel = drv_adc_pin2channel(pin);
    return channel;
}

static adc_driver_t adc_driver = {
    .drv = {
        .name   = "adc",
        .init   = adc_csky_init,
        .uninit = adc_csky_uninit,
        .open   = adc_csky_open,
        .close  = adc_csky_close,
    },
    .config          = adc_csky_config,
    .read            = adc_csky_read,
    .pin2channel     = adc_csky_pin2channel,
};

void rvm_adc_drv_register(int idx)
{
    rvm_driver_register(&adc_driver.drv, NULL, idx);
}
