/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <stdlib.h>
#include <aos/aos.h>
#include "aos/hal/gpio.h"
#include "aos/hal/pwm.h"
#include <drv/pin.h>
#include "app_main.h"

extern const csi_pinmap_t pwm_pinmap[];

static uint32_t get_pwm_pin_func(uint8_t gpio_pin, uint8_t pwm_id, const csi_pinmap_t *pinmap)
{
    const csi_pinmap_t *map = pinmap;
    uint32_t ret = 0xFFU;

    while ((uint32_t)map->pin_name != 0xFFU) {
        if (map->pin_name == gpio_pin && map->idx == pwm_id) {
            ret = (uint32_t)map->pin_func;
            break;
        }

        map++;
    }

    return ret;
}

static uint32_t get_pwm_channel(uint8_t gpio_pin, uint8_t pwm_id, const csi_pinmap_t *pinmap)
{
    const csi_pinmap_t *map = pinmap;
    uint32_t ret = 0xFFU;

    while ((uint32_t)map->pin_name != 0xFFU) {
        if (map->pin_name == gpio_pin &&  map->idx == pwm_id) {
            ret = (uint32_t)map->channel;
            break;
        }

        map++;
    }

    return ret;
}

/* output one pwm signal */
int hal_pwm_out_demo(uint8_t gpio_pin, uint8_t pwm_id, uint32_t freq, float duty_cycle)
{
    int32_t ret;
    uint8_t port;
    pwm_dev_t pwm = {0};

    printf(" hal_pwm_out_demo start\r\n");

    uint32_t pin_func = get_pwm_pin_func(gpio_pin, pwm_id, pwm_pinmap);
    csi_pin_set_mux(gpio_pin, pin_func);

    port = get_pwm_channel(gpio_pin, pwm_id, pwm_pinmap);
    port &= PWM_PORT_MASK;
    if (pwm_id == 0) {
        port |= PWM0_GROUP_MASK;
    } else if (pwm_id == 1) {
        port |= PWM1_GROUP_MASK;
    }

    pwm.port = port;
    pwm.config.freq = freq;
    pwm.config.duty_cycle = duty_cycle;
    pwm.priv = NULL;

    ret = hal_pwm_init(&pwm);
    if(ret != 0){
        printf("hal_pwm_init fail,ret:%d\r\n",ret);
        return -1;
    }

    hal_pwm_start(&pwm);

    aos_msleep(5000);

    hal_pwm_stop(&pwm);

    hal_pwm_finalize(&pwm);

    printf("hal_pwm_out_demo end\r\n");

    return 0;
}
