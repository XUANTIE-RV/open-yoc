/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <aos/kernel.h>
#include <drv/adc.h>
#include <drv/pin.h>
#include "hal/adc_impl.h"

#define TAG "adc_drv"

typedef struct {
    aos_dev_t       device;
    csi_adc_t       handle;
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
    csi_error_t ret = csi_adc_init(&adc(dev)->handle, dev->id);

    if (ret != CSI_OK) {
        LOGE(TAG, "csi_adc_init error\n");
        return -1;
    }

    return 0;
}

static int adc_csky_close(aos_dev_t *dev)
{
    csi_adc_uninit(&adc(dev)->handle);
    return 0;
}

static int adc_csky_config(aos_dev_t *dev, hal_adc_config_t *config)
{
    //uint32_t csi_adc_freq_div(csi_adc_t *adc, uint32_t div);
    csi_error_t ret = csi_adc_sampling_time(&adc(dev)->handle, config->sampling_time);
    if (ret != CSI_OK) {
        LOGE(TAG, "csi_adc_sampling_time error\n");
        return -1;
    }

    ret = csi_adc_continue_mode(&adc(dev)->handle, true);
    memcpy(&adc(dev)->config, config, sizeof(hal_adc_config_t));

    return 0;
}

static int adc_csky_read(aos_dev_t *dev, uint8_t ch, void *output, uint32_t timeout)
{
    if (dev == NULL || output == NULL) {
        return -EINVAL;
    }

    csi_error_t ret = csi_adc_channel_enable(&adc(dev)->handle, ch, true);
    if (ret != CSI_OK) {
        LOGE(TAG, "csi_adc_channel_enable error\n");
        return -1;
    }

    ret = csi_adc_start(&adc(dev)->handle);
    if (ret != CSI_OK) {
        LOGE(TAG, "csi_adc_start error\n");
        return -1;
    }

    *((uint32_t *)output) = csi_adc_read(&adc(dev)->handle);

    return 0;
}

static int adc_csky_pin2channel(aos_dev_t *dev, int pin)
{
    return csi_pin_get_adc_channel(pin);
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
