/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>

#include <aos/hal/pwm.h>
#include "drv_light.h"
#include <board_config.h>
#include <pin_name.h>
#include "genie_service.h"

#include "light.h"
#include "light_driver.h"

static pwm_port_func_t pwm_channel_config[] = {
    {PIN_LED_R, PIN_LED_R_CHANNEL, PWM_LED_R_PORT},
    {PIN_LED_G, PIN_LED_G_CHANNEL, PWM_LED_G_PORT},
};

static pwm_dev_t pmw_light[ARRAY_SIZE(pwm_channel_config)];

static led_light_cfg_t led_config[] = {
    LED_LIGHT_MODEL(GENIE_COLD_WARM_LIGHT, &pwm_channel_config[0], LOW_LIGHT, &pmw_light[0], ARRAY_SIZE(pwm_channel_config)), /* &pmw_light[0], */
};

void light_driver_init(void)
{
    led_light_init(led_config);
}

void light_driver_update(uint8_t onoff, uint16_t lightness, uint16_t temperature)
{
    struct genie_cold_warm_op  led_config = {
        .power_switch = onoff,
        .actual = lightness,
        .temperature = temperature,
    };

    led_light_control((void *)(&led_config));
}
