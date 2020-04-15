/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include "app_main.h"
#include <aos/log.h>


#define ADC_CON_DATA_NUM 1

#define TAG "GADC"

int app_adc_value_read(int pin, int *vol)
{
    int ret;

    adc_dev_t adc_hd = {0, {0}, NULL};
    adc_hd.port = drv_adc_pin2channel(pin);
    adc_hd.config.sampling_cycle = 0;

    ret = hal_adc_init(&adc_hd);

    if(ret == 0) {
        ret = hal_adc_value_get(&adc_hd, vol, 0);
    }

    hal_adc_finalize(&adc_hd);

    return 0;
}

int app_gpio_read(int pin, int *val)
{
    gpio_pin_handle_t     handle;

    bool bval = false;
    drv_pinmux_config(pin, PIN_FUNC_GPIO);
    handle = csi_gpio_pin_initialize(pin, NULL);
    if (handle) {
        csi_gpio_pin_config_mode(handle, GPIO_MODE_PULLNONE);
        csi_gpio_pin_config_direction(handle, GPIO_DIRECTION_INPUT);
        csi_gpio_pin_read(handle, &bval);
        csi_gpio_pin_uninitialize(handle);
        *val = (int)bval;
    } else {
        return -1;
    }

    return 0;
}

int app_gpio_write(int pin, int val)
{
    gpio_pin_handle_t     handle;
    drv_pinmux_config(pin, PIN_FUNC_GPIO);
    handle = csi_gpio_pin_initialize(pin, NULL);

    if (handle) {
        csi_gpio_pin_config_mode(handle, GPIO_MODE_PUSH_PULL);
        csi_gpio_pin_config_direction(handle, GPIO_DIRECTION_OUTPUT);
        csi_gpio_pin_write(handle, val);
        csi_gpio_pin_uninitialize(handle);
    } else {
        return -1;
    }

    return 0;
}
