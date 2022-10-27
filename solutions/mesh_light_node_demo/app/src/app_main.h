/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#ifndef _APP_MAIN_H_
#define  _APP_MAIN_H_

#include <stdbool.h>
#include <stdint.h>
#include <aos/aos.h>
#include <yoc/yoc.h>

enum {
    LED_OFF = 0,
    LED_ON,

    LED_ATTENTION_ON,
    LED_ATTENTION_OFF,

    LED_UNPROVED,
    LED_PROVED,
};

void app_set_led_state(int state);

#endif
