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
    aos_event_t     event_intr;
    rvm_hal_adc_config_t    config;
    int             dma_en;
    csi_dma_ch_t    dma_ch;
} adc_dev_t;

#define ADC(dev) ((adc_dev_t *)dev)

#define EVENT_ADC_INTR_COMPLETE 0x000000F0
#define adc_check(ret, log) \
    do { \
        if (ret < 0) { \
            LOGE(TAG, log); \
            return -1; \
        } \
    } while(0)

static void adc_event_cb(csi_adc_t *adc, csi_adc_event_t event, void *arg)
{
    aos_event_t *event_intr = (aos_event_t *)arg;
    if (event == ADC_EVENT_CONVERT_COMPLETE) {
        if (aos_event_is_valid(event_intr)) {
            aos_event_set(event_intr, EVENT_ADC_INTR_COMPLETE, AOS_EVENT_OR);
        }
    }
}

static rvm_dev_t *adc_csky_init(driver_t *drv, void *config, int id)
{
    adc_dev_t *adc = (adc_dev_t *)rvm_hal_device_new(drv, sizeof(adc_dev_t), id);

    return (rvm_dev_t *)adc;
}

#define adc_csky_uninit rvm_hal_device_free

static int adc_csky_open(rvm_dev_t *dev)
{
    csi_error_t ret = csi_adc_init(&ADC(dev)->handle, dev->id);

    if (ret != CSI_OK) {
        LOGE(TAG, "csi_adc_init error\n");
        return -1;
    }
    aos_event_new(&ADC(dev)->event_intr, 0);
    ADC(dev)->dma_en = 0;
    return 0;
}

static int adc_csky_close(rvm_dev_t *dev)
{
    csi_adc_uninit(&ADC(dev)->handle);
    csi_adc_detach_callback(&ADC(dev)->handle);
    aos_event_free(&ADC(dev)->event_intr);
    return 0;
}

static int adc_csky_clock(rvm_dev_t *dev, bool enable)
{
    if (enable) {
        csi_clk_enable(&ADC(dev)->handle.dev);
    } else {
        csi_clk_disable(&ADC(dev)->handle.dev);
    }
    return 0;
}

static int adc_csky_config(rvm_dev_t *dev, rvm_hal_adc_config_t *config)
{
    uint32_t ret = csi_adc_freq_div(&ADC(dev)->handle, config->freq);
    if (ret <= 0) {
        LOGE(TAG, "csi_adc_freq_div error\n");
        return -1;
    }

    ret = csi_adc_sampling_time(&ADC(dev)->handle, config->sampling_time);
    if (ret != CSI_OK) {
        LOGE(TAG, "csi_adc_sampling_time error\n");
        return -1;
    }
    if (config->mode == RVM_ADC_CONTINUOUS)
        ret = csi_adc_continue_mode(&ADC(dev)->handle, true);
    else
        ret = csi_adc_continue_mode(&ADC(dev)->handle, false);
    if (ret != CSI_OK) {
        LOGE(TAG, "csi_adc_continue_mode error\n");
        return -1;
    }

    memcpy(&ADC(dev)->config, config, sizeof(rvm_hal_adc_config_t));

    return 0;
}

static int32_t adc_value_buffer_get(rvm_dev_t *dev, uint8_t ch, uint32_t *output, size_t num, uint32_t timeout)
{
    int ret = CSI_OK;
    unsigned int actl_flags = 0;

    ret = csi_adc_channel_enable(&ADC(dev)->handle, ch, true);
    adc_check(ret, "csi_adc_channel_enable error");

    ret = csi_adc_attach_callback(&ADC(dev)->handle, adc_event_cb, (void *)&ADC(dev)->event_intr);
    adc_check(ret, "csi_adc_attach_callback error");

    ret = csi_adc_set_buffer(&ADC(dev)->handle, output, num);
    adc_check(ret, "csi_adc_set_buffer error");

    if (ADC(dev)->dma_en) {
        ret = csi_adc_link_dma(&ADC(dev)->handle, &ADC(dev)->dma_ch);
        adc_check(ret, "csi_adc_link_dma error");
    }

    ret = aos_event_set(&ADC(dev)->event_intr, ~EVENT_ADC_INTR_COMPLETE, AOS_EVENT_AND); // clean event
    adc_check(ret, "aos_event_set error");

    ret = csi_adc_start_async(&ADC(dev)->handle);
    adc_check(ret, "csi_adc_start_async error");

    ret = aos_event_get(&ADC(dev)->event_intr, EVENT_ADC_INTR_COMPLETE, AOS_EVENT_OR, &actl_flags, timeout); // block waiting for interrupt
    adc_check(ret, "timeout error");

    ret = csi_adc_channel_enable(&ADC(dev)->handle, ch, false);
    adc_check(ret, "csi_adc_channel_enable error");

    ret = csi_adc_stop_async(&ADC(dev)->handle);
    adc_check(ret, "csi_adc_stop_async error");

    if (ADC(dev)->dma_en) {
        ret = csi_adc_link_dma(&ADC(dev)->handle, NULL);
        adc_check(ret, "csi_adc_link_dma error");
    }

    return 0;
}

static int adc_csky_read(rvm_dev_t *dev, uint8_t ch, void *output, size_t num, uint32_t timeout)
{
    if (dev == NULL || output == NULL) {
        return -EINVAL;
    }

    return adc_value_buffer_get(dev, ch, output, num, timeout);
}

static int adc_csky_pin2channel(rvm_dev_t *dev, int pin)
{
    return csi_pin_get_adc_channel(pin);
}

static int adc_csky_trans_dma_enable(rvm_dev_t *dev, bool enable)
{
    if (!enable)
        ADC(dev)->dma_en = 0;
    else
        ADC(dev)->dma_en = 1;
    return 0;
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
    .trans_dma_enable    = adc_csky_trans_dma_enable,
};

void rvm_adc_drv_register(int idx)
{
    rvm_driver_register(&adc_driver.drv, NULL, idx);
}
