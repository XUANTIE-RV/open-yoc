#ifndef _BODY_SENSOR_LED_H
#define _BODY_SENSOR_LED_H
#include "body_sensor_gpio.h"
#include <types.h>

typedef struct _body_sensor_led_config_s
{
    uint8_t port;
    uint8_t pol;
} _body_sensor_led_config_t;

#define BODY_SENSOR_LED_PIN(_port, _pol) \
    {                               \
        .port = (_port),            \
        .pol = (_pol),              \
    }

#define LED_IO_NUM (1)
#define LED_PIN_1 (P23)

int body_sensor_led_init(_body_sensor_led_config_t *io_config, uint8_t size);
int body_sensor_led_set(uint8_t index, bool onoff);

#endif
