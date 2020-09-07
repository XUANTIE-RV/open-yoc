/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <aos/log.h>
#include <aos/kernel.h>
#include <drv/adc.h>

#include "hal/adc_impl.h"

#define TAG "adc_drv"

typedef struct {
    aos_dev_t       device;
    adc_handle_t    handle;
    hal_adc_config_t    config;
} adc_dev_t;

#define adc(dev) ((adc_dev_t *)dev)

static aos_dev_t *adc_csky_init(driver_t *drv, void *config, int id)
{
    adc_dev_t *adc = (adc_dev_t *)device_new(drv, sizeof(adc_dev_t), id);

    return (aos_dev_t *)adc;
}

#define adc_csky_uninit device_free

static int adc_csky_open(aos_dev_t *dev)
{
    adc_handle_t adc_handle = drv_adc_initialize(dev->id, NULL);

    if (adc_handle == NULL) {
        LOGE(TAG, "csi_iic_initialize error\n");
        return -1;
    }
    adc(dev)->handle = adc_handle;
    return 0;
}

static int adc_csky_close(aos_dev_t *dev)
{
    drv_adc_uninitialize(adc(dev)->handle);
    return 0;
}

static int adc_csky_config(aos_dev_t *dev, hal_adc_config_t *config)
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

    memcpy(&adc(dev)->config, config, sizeof(hal_adc_config_t));

    return 0;
}

static int adc_csky_read(aos_dev_t *dev, void *output, uint32_t timeout)
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

static int adc_csky_pin2channel(aos_dev_t *dev, int pin)
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

void adc_csky_register(int idx)
{
    driver_register(&adc_driver.drv, NULL, idx);
}
