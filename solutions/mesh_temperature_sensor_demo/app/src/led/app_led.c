/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include <aos/hal/pwm.h>
#include <board.h>
#include <ulog/ulog.h>

#include "app_main.h"
#include "drv_light.h"

#define TAG "APP_LED"

static int init_flag = 1;

#define ON  1
#define OFF 0

#if defined(BOARD_LED_NUM) && BOARD_LED_NUM
#if defined(BOARD_LED_PWM_NUM) && BOARD_LED_PWM_NUM

static pwm_dev_t pmw_light[3];

static pwm_port_func_t pwm_channel_config[] = {
#ifdef BOARD_LED0_PWM_CH
    {BOARD_LED0_PWM_CH},
#endif
#ifdef BOARD_LED1_PWM_CH
    {BOARD_LED1_PWM_CH},
#endif
#ifdef BOARD_LED2_PWM_CH
    {BOARD_LED2_PWM_CH},
#endif
};

static led_light_cfg_t led_light_config[]
    = { LED_LIGHT_MODEL(RGB_LIGHT, &pwm_channel_config[0], LOW_LIGHT, &pmw_light[0], BOARD_LED_PWM_NUM > 3? 3 : BOARD_LED_PWM_NUM) };

static void _led_init(void)
{
    if (init_flag) {
        led_light_init(led_light_config);
        init_flag = 0;
    }
}

static void _led_set(bool on, uint8_t r_s, uint8_t g_s, uint8_t b_s, uint8_t r_a, uint8_t g_a, uint8_t b_a)
{
    struct genie_rgb_op rgb_config;

    if (on) {
        rgb_config.rgb_config[0].power_switch = r_s;
        rgb_config.rgb_config[1].power_switch = g_s;
        rgb_config.rgb_config[2].power_switch = b_s;
        rgb_config.rgb_config[0].led_actual   = r_a;
        rgb_config.rgb_config[1].led_actual   = g_a;
        rgb_config.rgb_config[2].led_actual   = b_a;
    } else {
        rgb_config.rgb_config[0].power_switch = 0;
        rgb_config.rgb_config[1].power_switch = 0;
        rgb_config.rgb_config[2].power_switch = 0;
        rgb_config.rgb_config[0].led_actual   = 0;
        rgb_config.rgb_config[1].led_actual   = 0;
        rgb_config.rgb_config[2].led_actual   = 0;
    }
    led_light_control((void *)&rgb_config);
}
#elif defined(BOARD_LED_GPIO_NUM) && BOARD_LED_GPIO_NUM

static gpio_dev_t led0;

static void _led_init(void)
{
    if (init_flag) {
        led0->port = BOARD_LED0_GPIO_PIN;
        led0->config = OUTPUT_PUSH_PULL;
        hal_gpio_init(&led0);
        init_flag = 0;
    }
}

static void _led_set(bool on, uint8_t r_s, uint8_t g_s, uint8_t b_s, uint8_t r_a, uint8_t g_a, uint8_t b_a)
{
    (void)r_s;
    (void)g_s;
    (void)b_s,
    (void)r_a;
    (void)g_a;
    (void)b_a;
    if (on)
    {
        hal_gpio_output_high(&led0);
    else
    {
        hal_gpio_output_low(&led0);
    }
}

#endif
#endif

void app_set_led_state(int state)
{
#if defined(BOARD_LED_NUM) && BOARD_LED_NUM
if(BOARD_LED_PWM_NUM == 3){
    _led_init();
    switch (state) {
        case LED_OFF:
            _led_set(OFF, OFF, OFF, OFF, 0, 0, 0);
            break;

        case LED_ON:
            _led_set(ON, ON, ON, ON, 255, 255, 255);
            break;

        case LED_ATTENTION_ON:
            _led_set(ON, ON, OFF, OFF, 255, 0, 0);
            break;

        case LED_ATTENTION_OFF:
            _led_set(OFF, OFF, OFF, OFF, 0, 0, 0);
            break;

        case LED_UNPROVED:
            _led_set(ON, OFF, ON, OFF, 0, 255, 0);
            break;

        case LED_PROVED:
            _led_set(ON, OFF, OFF, ON, 0, 0, 255);
            break;

        default:
            _led_set(OFF, OFF, OFF, OFF, 0, 0, 0);
            break;
    }
}else{
    LOGI(TAG, "LED State %s", state ? "on" : "off");
}
#else
    LOGI(TAG, "LED State %d", state);
#endif
}