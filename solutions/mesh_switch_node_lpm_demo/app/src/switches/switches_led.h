#ifndef _SWITCHES_LED_H
#define _SWITCHES_LED_H
#include "switches_gpio.h"
#include <types.h>

typedef struct _switch_led_config_s
{
    uint8_t port;
    uint8_t pol;
} _switch_led_config_t;

#define SWITCH_LED_PIN(_port, _pol) \
    {                               \
        .port = (_port),            \
        .pol = (_pol),              \
    }

#define LED_IO_NUM (3)
#define LED_PIN_1 (P23)
#define LED_PIN_2 (P31)
#define LED_PIN_3 (P32)

int switch_led_init(_switch_led_config_t *io_config, uint8_t size);
int switch_led_set(uint8_t index, bool onoff);

#endif
