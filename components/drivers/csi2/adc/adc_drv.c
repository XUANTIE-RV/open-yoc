/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <aos/kernel.h>
#include <drv/adc.h>
#include <drv/pin.h>
#include <drv/clk.h>
#include <devices/impl/adc_impl.h>

#define TAG "adc_drv"

typedef struct {
    rvm_dev_t       device;
    csi_adc_t       handle;
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
    csi_error_t ret = csi_adc_init(&adc(dev)->handle, dev->id);

    if (ret != CSI_OK) {
        LOGE(TAG, "csi_adc_init error\n");
        return -1;
    }

    return 0;
}

static int adc_csky_close(rvm_dev_t *dev)
{
    csi_adc_uninit(&adc(dev)->handle);
    return 0;
}

static int adc_csky_clock(rvm_dev_t *dev, bool enable)
{
    if (enable) {
        csi_clk_enable(&adc(dev)->handle.dev);
    } else {
        csi_clk_disable(&adc(dev)->handle.dev);
    }
    return 0;
}

static int adc_csky_config(rvm_dev_t *dev, rvm_hal_adc_config_t *config)
{
    uint32_t ret = csi_adc_freq_div(&adc(dev)->handle, config->freq);
    if (ret <= 0) {
        LOGE(TAG, "csi_adc_freq_div error\n");
        return -1;
    }

    ret = csi_adc_sampling_time(&adc(dev)->handle, config->sampling_time);
    if (ret != CSI_OK) {
        LOGE(TAG, "csi_adc_sampling_time error\n");
        return -1;
    }

    ret = csi_adc_continue_mode(&adc(dev)->handle, true);
    if (ret != CSI_OK) {
        LOGE(TAG, "csi_adc_continue_mode error\n");
        return -1;
    }

    memcpy(&adc(dev)->config, config, sizeof(rvm_hal_adc_config_t));

    return 0;
}

static int adc_csky_read(rvm_dev_t *dev, uint8_t ch, void *output, uint32_t timeout)
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

    ret = csi_adc_stop(&adc(dev)->handle);
    if (ret != CSI_OK) {
        LOGE(TAG, "csi_adc_stop error\n");
        return -1;
    }

    return 0;
}

static int adc_csky_pin2channel(rvm_dev_t *dev, int pin)
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
        .clk_en = adc_csky_clock,
    },
    .config          = adc_csky_config,
    .read            = adc_csky_read,
    .pin2channel     = adc_csky_pin2channel,
};

void rvm_adc_drv_register(int idx)
{
    rvm_driver_register(&adc_driver.drv, NULL, idx);
}
