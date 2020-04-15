/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef HAL_LED_H
#define HAL_LED_H

#include <stdint.h>

#include <devices/driver.h>
#include <devices/led.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct led_pin_config {
    int pin_r;
    int pin_g;
    int pin_b;
    int flip;
} led_pin_config_t;

typedef struct led_config {
    int on_time;
    int off_time;
    int color;
} led_config_t;

typedef struct led_driver {
    driver_t    drv;
    int         (*control)(aos_dev_t *dev, int color, int on_time, int off_time);
} led_driver_t;

#ifdef __cplusplus
}
#endif

#endif
