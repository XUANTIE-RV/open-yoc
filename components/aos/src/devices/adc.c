/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <devices/hal/adc_impl.h>

#define ADC_DRIVER(dev)  ((adc_driver_t*)(dev->drv))
#define ADC_VAILD(dev) do { \
    if (device_valid(dev, "adc") != 0) \
        return -1; \
} while(0)

void adc_config_default(hal_adc_config_t *config)
{
    config->mode          = HAL_ADC_SINGLE;
    config->trigger       = 0;
    config->intrp_mode    = 0;
    config->sampling_time = 0;
}

int adc_config(aos_dev_t *dev, hal_adc_config_t *config)
{
    int ret;

    ADC_VAILD(dev);

    device_lock(dev);
    ret = ADC_DRIVER(dev)->config(dev, config);
    device_unlock(dev);

    return ret;
}

int adc_pin2channel(aos_dev_t *dev, int pin)
{
    int ret;

    ADC_VAILD(dev);

    device_lock(dev);
    ret = ADC_DRIVER(dev)->pin2channel(dev, pin);
    device_unlock(dev);

    return ret; 
}

int adc_read(aos_dev_t *dev, void *output, uint32_t timeout)
{
    if (output == 0) {
        return -EINVAL;
    }

    int ret;

    ADC_VAILD(dev);

    device_lock(dev);
    ret = ADC_DRIVER(dev)->read(dev, output, timeout);
    device_unlock(dev);

    return ret;
}


