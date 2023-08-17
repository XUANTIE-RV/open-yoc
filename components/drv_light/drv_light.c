/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <devices/pwm.h>
#include <devices/devicelist.h>
#include <drv_light.h>

pwm_dev_res_t *pwm_devs = NULL;

#define LIGHT_PERIOD       500
#define LIGHT_CTL_TEMP_MIN (0x0320) // 800
#define LIGHT_CTL_TEMP_MAX (0x4E20) // 20000

// static struct led_light_cfg_t *cfg;

uint16_t duty_list[] = {
    0,   10,  10,  11,  11,  12,  12,  13,  14,  15,  15,  16,  17,  19,  20,  21,  22,   24,  25,  27,  29,
    31,  33,  35,  37,  40,  42,  45,  48,  51,  54,  57,  60,  64,  68,  72,  76,  80,   85,  90,  95,  100,
    105, 111, 117, 123, 129, 136, 142, 149, 157, 164, 172, 180, 189, 198, 207, 216, 226,  236, 246, 256, 267,
    279, 290, 302, 315, 327, 341, 354, 368, 382, 397, 412, 427, 443, 460, 476, 494, 511,  529, 548, 567, 587,
    607, 627, 648, 670, 692, 715, 738, 761, 786, 810, 836, 862, 888, 915, 943, 971, 1000,
};

typedef struct {
    uint8_t light_mode;
    uint8_t pin_mode;
} light_info_func_t;

light_info_func_t light_info;

/**@brief   Function for the LEDs initialization.
 *
 * @details Initializes all LEDs used by this application.
 */
static void _led_init(led_light_cfg_t *data_cfg)
{
    char name[12];
    pwm_port_func_t *io_port = (pwm_port_func_t *)data_cfg->pwm_port;
    pwm_devs                 = data_cfg->pwm_devs;
    light_info.light_mode    = data_cfg->show_mode;
    light_info.pin_mode      = data_cfg->pin_mode;

    uint16_t arr_size = data_cfg->channel_len;
    if (arr_size > LIGHT_TYPE_MAX) {
        return;
    }
    for (int i = 0; i < arr_size; i++) {
        pwm_devs[i].port = io_port[i].port;
        pwm_devs[i].channel = io_port[i].channel;
        if (data_cfg->show_mode == HIGH_LIGHT) {
            pwm_devs[i].config.duty_cycle = 0.5;
        } else {
            pwm_devs[i].config.duty_cycle = 0.8;
        }
        pwm_devs[i].config.freq = LIGHT_PERIOD;
        pwm_devs[i].config.polarity = RVM_HAL_PWM_POLARITY_NORMAL;
        rvm_pwm_drv_register(pwm_devs[i].port);
        snprintf(name, sizeof(name), "pwm%d", pwm_devs[i].port);
        pwm_devs[i].dev = rvm_hal_pwm_open(name);
        rvm_hal_pwm_config(pwm_devs[i].dev, &pwm_devs[i].config, pwm_devs[i].channel);
        rvm_hal_pwm_start(pwm_devs[i].dev, pwm_devs[i].channel);
    }
}

static void _get_led_duty(uint8_t *p_duty, uint16_t actual, uint16_t temperature)
{
    uint8_t cold = 0;
    uint8_t warm = 0;

    if (temperature > LIGHT_CTL_TEMP_MAX) {
        temperature = LIGHT_CTL_TEMP_MAX;
    }
    if (temperature < LIGHT_CTL_TEMP_MIN) {
        temperature = LIGHT_CTL_TEMP_MIN;
    }
    // 0-100
    cold = (temperature - LIGHT_CTL_TEMP_MIN) * 100 / (LIGHT_CTL_TEMP_MAX - LIGHT_CTL_TEMP_MIN);
    warm = 100 - cold;

    p_duty[LED_COLD_CHANNEL] = (actual * cold) / 65500;
    p_duty[LED_WARM_CHANNEL] = (actual * warm) / 65500;
    if (p_duty[LED_COLD_CHANNEL] == 0 && p_duty[LED_WARM_CHANNEL] == 0) {
        if (temperature > (LIGHT_CTL_TEMP_MAX - LIGHT_CTL_TEMP_MIN) >> 1) {
            p_duty[LED_COLD_CHANNEL] = 1;
        } else {
            p_duty[LED_WARM_CHANNEL] = 1;
        }
    }
    // LIGHT_DBG("%d %d [%d %d] [%d %d]", actual, temperature, warm, cold, p_duty[LED_COLD_CHANNEL],
    // p_duty[LED_WARM_CHANNEL]);
}

static int _set_pwm_duty(uint8_t channel, uint8_t duty)
{
    int          err = -1;
    rvm_hal_pwm_config_t pwm_cfg;
    pwm_dev_res_t *  pwm_dev = NULL;

    if (duty > 100) {
        printf(">>duty invaild\r\n");
        return -1;
    }

    pwm_cfg.freq = LIGHT_PERIOD;

    if (light_info.light_mode == HIGH_LIGHT) {
        pwm_cfg.duty_cycle = (float)(duty_list[duty] * 0.001);
    } else {
        pwm_cfg.duty_cycle = 1.0 - (float)(duty_list[duty] * 0.001);
    }
    pwm_cfg.polarity = RVM_HAL_PWM_POLARITY_NORMAL;

    if (channel == LED_COLD_CHANNEL) {
        pwm_dev = &pwm_devs[0];
    } else if (channel == LED_WARM_CHANNEL) {
        pwm_dev = &pwm_devs[1];
    } else {
        return -1;
    }

    err = rvm_hal_pwm_config(pwm_dev->dev, &pwm_cfg, pwm_dev->channel);
    if (err) {
        printf("pwm err %d\n", err);
        return -1;
    }

    return 0;
}

// lightness 1-65535
static void _led_set_cw(uint8_t onoff, uint16_t actual, uint16_t temperature)
{
    static uint8_t last_duty[LED_CHANNEL_MAX] = { 0xFF, 0xFF };
    uint8_t        duty[LED_CHANNEL_MAX]; // 0~100
    // LIGHT_DBG("%d %d %d", onoff, actual, temperature);

    if (onoff == 0) {
        duty[LED_COLD_CHANNEL] = 0;
        duty[LED_WARM_CHANNEL] = 0;
    } else {
        _get_led_duty(duty, actual, temperature);
    }

    if (last_duty[LED_COLD_CHANNEL] != duty[LED_COLD_CHANNEL]) {
        last_duty[LED_COLD_CHANNEL] = duty[LED_COLD_CHANNEL];
        _set_pwm_duty(LED_COLD_CHANNEL, duty[LED_COLD_CHANNEL]);
    }
    if (last_duty[LED_WARM_CHANNEL] != duty[LED_WARM_CHANNEL]) {
        last_duty[LED_WARM_CHANNEL] = duty[LED_WARM_CHANNEL];
        _set_pwm_duty(LED_WARM_CHANNEL, duty[LED_WARM_CHANNEL]);
    }
}

int led_light_init(led_light_cfg_t *para_cfg)
{
    _led_init(para_cfg);
    return 0;
}

int led_light_control(void *config)
{
    if (pwm_devs == NULL) {
        printf("led light init err or not init\n");
        return -1;
    }
    if (GENIE_COLD_WARM_LIGHT == light_info.pin_mode) {
        struct genie_cold_warm_op *led_config = config;
        _led_set_cw(led_config->power_switch, led_config->actual, led_config->temperature);
    } else if (ON_OFF_LIGHT == light_info.pin_mode) {
        struct genie_on_off_op *on_off_config = config;
        rvm_hal_pwm_config_t            pwm_cfg;
        pwm_dev_res_t *                 pwm_dev = NULL;
        pwm_cfg.freq                    = LIGHT_PERIOD;
        if ((light_info.light_mode == HIGH_LIGHT && on_off_config->power_switch == 1)
            || (light_info.light_mode == LOW_LIGHT && on_off_config->power_switch == 0))
        {
            pwm_cfg.duty_cycle = 1.0;
        } else {
            pwm_cfg.duty_cycle = 0.0;
        }
        pwm_dev = &pwm_devs[0];

        int err = rvm_hal_pwm_config(pwm_dev->dev, &pwm_cfg, pwm_dev->channel);
        if (err) {
            printf("pwm err %d\n", err);
            return -1;
        }
    } else if (RGB_LIGHT == light_info.pin_mode) {
        static uint8_t       last_rgb_actual[LED_RGB_CHANNEL_MAX] = { 0xFF, 0xFF, 0xFF };
        struct genie_rgb_op *rgb_read                             = config;
        rvm_hal_pwm_config_t         pwm_cfg;
        pwm_dev_res_t *              pwm_dev = NULL;
        pwm_cfg.freq                 = LIGHT_PERIOD;

        for (int i = 0; i < LED_RGB_CHANNEL_MAX; i++) {
            if (rgb_read->rgb_config[i].power_switch == 0) {
                rgb_read->rgb_config[i].led_actual = 0;
            }
            if (last_rgb_actual[i] == rgb_read->rgb_config[i].led_actual) {
                continue;
            }

            if (light_info.light_mode == HIGH_LIGHT) {
                pwm_cfg.duty_cycle = rgb_read->rgb_config[i].led_actual / 255.0;
            } else {
                pwm_cfg.duty_cycle = 1.0 - rgb_read->rgb_config[i].led_actual / 255.0;
            }
            pwm_cfg.polarity = RVM_HAL_PWM_POLARITY_NORMAL;
            pwm_dev = &pwm_devs[i];

            int err = rvm_hal_pwm_config(pwm_dev->dev, &pwm_cfg, pwm_dev->channel);
            if (err) {
                printf("pwm err %d\n", err);
                return -1;
            }
            // printf("%f, \r\n",pwm_cfg.duty_cycle);
            last_rgb_actual[i] = rgb_read->rgb_config[i].led_actual;

            rvm_hal_pwm_start(pwm_dev->dev, pwm_dev->channel);
        }
    } else {
        printf("not support led type \n");
        return -1;
    }
    return 0;
}
