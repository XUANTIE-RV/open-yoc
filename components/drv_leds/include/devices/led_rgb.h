/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef __LED_RGB_H__
#define __LED_RGB_H__

#include <stdint.h>
#include <devices/led.h>
#ifdef __cplusplus
extern "C" {
#endif

void led_rgb_register(led_pin_config_t *config, int idx);

#ifdef __cplusplus
}
#endif

#endif