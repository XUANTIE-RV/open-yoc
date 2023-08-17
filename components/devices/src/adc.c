/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dev_internal.h>
#include <devices/impl/adc_impl.h>

#define ADC_DRIVER(dev)  ((adc_driver_t*)(dev->drv))
#define ADC_VAILD(dev) do { \
    if (device_valid(dev, "adc") != 0) \
        return -1; \
} while(0)

void rvm_hal_adc_config_default(rvm_hal_adc_config_t *config)
{
    config->mode          = RVM_ADC_CONTINUOUS;
    config->trigger       = 0;
    config->intrp_mode    = 0;
    config->sampling_time = 2;
    config->freq          = 128;
    config->offset        = 0;
}

int rvm_hal_adc_config(rvm_dev_t *dev, rvm_hal_adc_config_t *config)
{
    int ret;

    ADC_VAILD(dev);

    device_lock(dev);
    ret = ADC_DRIVER(dev)->config(dev, config);
    device_unlock(dev);

    return ret;
}

int rvm_hal_adc_pin2channel(rvm_dev_t *dev, int pin)
{
    int ret;

    ADC_VAILD(dev);

    device_lock(dev);
    ret = ADC_DRIVER(dev)->pin2channel(dev, pin);
    device_unlock(dev);

    return ret;
}

#ifdef CONFIG_CSI_V2
int rvm_hal_adc_read(rvm_dev_t *dev, uint8_t ch, void *output, uint32_t timeout)
#else
int rvm_hal_adc_read(rvm_dev_t *dev, void *output, uint32_t timeout)
#endif
{
    if (output == 0) {
        return -EINVAL;
    }

    int ret;

    ADC_VAILD(dev);

    device_lock(dev);
#ifdef CONFIG_CSI_V2
    ret = ADC_DRIVER(dev)->read(dev, ch, output, 1, timeout);
#else
    ret = ADC_DRIVER(dev)->read(dev, output, timeout);
#endif
    device_unlock(dev);

    return ret;
}

#ifdef CONFIG_CSI_V2
int rvm_hal_adc_read_multiple(rvm_dev_t *dev, uint8_t ch, void *output, size_t num, uint32_t timeout)
{
    if (output == 0 && num < 1) {
        return -EINVAL;
    }

    int ret;

    ADC_VAILD(dev);

    device_lock(dev);
    ret = ADC_DRIVER(dev)->read(dev, ch, output, num, timeout);
    device_unlock(dev);

    return ret;
}
#endif

int rvm_hal_adc_trans_dma_enable(rvm_dev_t *dev, bool enable)
{
    int ret;

    ADC_VAILD(dev);

    device_lock(dev);
    ret = ADC_DRIVER(dev)->trans_dma_enable(dev, enable);
    device_unlock(dev);

    return ret;
}
