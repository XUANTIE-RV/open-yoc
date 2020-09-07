/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */


#include <stdlib.h>
#include <string.h>

#include <aos/log.h>
#include <aos/kernel.h>

#include <drv/gpio.h>
#include <pin_name.h>
#include <pinmux.h>
#include "app_main.h"

#define TAG "led"

static void gpio_set(int gpio_id, int val)
{
    gpio_pin_handle_t     handle;

    drv_pinmux_config(gpio_id, PIN_FUNC_GPIO);
    handle = csi_gpio_pin_initialize(gpio_id, NULL);
    if (handle == NULL) {
        LOGI(TAG, "pin init\n");
        return;
    }

    csi_gpio_pin_config_mode(handle, GPIO_MODE_PUSH_PULL);
    csi_gpio_pin_config_direction(handle, GPIO_DIRECTION_OUTPUT);
    csi_gpio_pin_write(handle, val);

    csi_gpio_pin_uninitialize(handle);
}

void main()
{
    board_yoc_init();
    LOGI(TAG, "led blink demo\n");

    int gpio_val = 0;

    while (1) {
        gpio_set(PA13, gpio_val);
        gpio_val ++;
        gpio_val %= 2;
        aos_msleep(500);
    }
}
