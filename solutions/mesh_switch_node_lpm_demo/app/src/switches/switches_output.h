#ifndef _SWITCHES_OUTPUT_H
#define _SWITCHES_OUTPUT_H
#include "switches_gpio.h"
#include <types.h>

typedef struct _output_io_config_s
{
    uint8_t port_a;
    uint8_t port_b;
} _output_io_config_t;

#define SWITCH_OUTPUT_PIN(_porta, _portb) \
    {                                     \
        .port_a = (_porta),               \
        .port_b = (_portb),               \
    }

#define OUTPUT_IO_NUM (3)
#define OUTPUT_PIN_1_A (P18)
#define OUTPUT_PIN_1_B (P20)
#define OUTPUT_PIN_2_A (P25)
#define OUTPUT_PIN_2_B (P26)
#define OUTPUT_PIN_3_A (P33)
#define OUTPUT_PIN_3_B (P34)
#define OUTPUT_DEFAULT_PLUSE_WIDTH (30) //uint ms

int switch_output_gpio_init(_output_io_config_t *io_config, uint8_t size);
int switch_output_gpio_set(uint8_t index, bool onoff);

#endif