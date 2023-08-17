#ifndef LIGHT_DRIVER_H
#define LIGHT_DRIVER_H

#include <stdint.h>
#include <devices/pwm.h>

enum {
    HIGH_LIGHT,
    LOW_LIGHT,
};

enum {
    GENIE_COLD_WARM_LIGHT,
    ON_OFF_LIGHT,
    RGB_LIGHT,
    LIGHT_TYPE_MAX,
};

enum {
    LED_COLD_CHANNEL = 0,
    LED_WARM_CHANNEL,
    LED_CHANNEL_MAX,
};

typedef struct {
    uint8_t port;
    uint8_t channel;
} pwm_port_func_t;

typedef struct {
    uint8_t port;
    uint8_t channel;
    rvm_dev_t *dev;
    rvm_hal_pwm_config_t config;
} pwm_dev_res_t;

typedef struct {
    uint8_t          pin_mode;  // support  GENIE_COLD_WARM_LIGHT  RGB_LIGHT  ON_OFF_LIGHT
    pwm_port_func_t *pwm_port;  // support one , twe ,three port
    uint8_t          show_mode; // HIGH_LIGHT , LOW_LIGHT
    pwm_dev_res_t    *pwm_devs;
    uint8_t          channel_len;
} led_light_cfg_t;

struct genie_cold_warm_op {
    uint8_t  power_switch;
    uint16_t actual;
    uint16_t temperature;
};

struct genie_on_off_op {
    uint8_t power_switch;
};

enum {
    LED_R_CHANNEL = 0,
    LED_G_CHANNEL,
    LED_B_CHANNEL,
    LED_RGB_CHANNEL_MAX,
};

typedef struct {
    uint8_t power_switch;
    uint8_t led_actual;
} rgb_show_config;

struct genie_rgb_op {
    rgb_show_config rgb_config[3];
};

#define LED_LIGHT_MODEL(_pin_mode, _pwm_port, _show_mode, _dev_res, _channel_len)                                    \
    {                                                                                                                  \
        .pin_mode = _pin_mode, .pwm_port = _pwm_port, .show_mode = _show_mode, .pwm_devs = _dev_res,                \
        .channel_len = _channel_len,                                                                                   \
    }

int led_light_init(led_light_cfg_t *para_cfg);
int led_light_control(void *config);

#endif