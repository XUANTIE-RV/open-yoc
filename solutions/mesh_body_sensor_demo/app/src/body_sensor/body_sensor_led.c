#include "aos/hal/gpio.h"
#include "aos/kernel.h"
#include "body_sensor_led.h"
#include "ulog/ulog.h"

#define TAG "BODY_SENSOR_LED"

typedef struct _body_sensor_led_io_s {
    uint8_t led_on_pol;
    gpio_dev_t io;
} _body_sensor_led_io_t;

typedef struct _body_sensor_led_s {
    _body_sensor_led_io_t io_config[LED_IO_NUM];
} _body_sensor_led_t;

_body_sensor_led_t g_led_config;

int body_sensor_led_init(_body_sensor_led_config_t *io_config, uint8_t size)
{
    if (size > LED_IO_NUM) {
        LOGE(TAG,"suport no more than %d IO", LED_IO_NUM);
        return -1;
    }
    if (!io_config || !size) {
        return -1;
    }
    for (int index = 0; index < LED_IO_NUM; index++) {
        g_led_config.io_config[index].io.port = io_config[index].port;
        g_led_config.io_config[index].io.config = OUTPUT_PUSH_PULL;
        g_led_config.io_config[index].led_on_pol = io_config[index].pol;
        hal_gpio_init(&g_led_config.io_config[index].io);
        if (!io_config[index].pol) {
            hal_gpio_output_high(&g_led_config.io_config[index].io);
        } else {
            hal_gpio_output_low(&g_led_config.io_config[index].io);
        }
    }
    return 0;
}

int body_sensor_led_set(uint8_t index, bool onoff)
{
    if (index > LED_IO_NUM) {
        return -1;
    }
    uint8_t output_high = (onoff == g_led_config.io_config[index].led_on_pol ? 1 : 0);
    if (output_high) {
        hal_gpio_output_high(&g_led_config.io_config[index].io);
    } else {
        hal_gpio_output_low(&g_led_config.io_config[index].io);
    }
    return 0;
}
