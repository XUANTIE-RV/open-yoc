/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */


/******************************************************************************
 * @file     pinmux.c
 * @brief    source file for the pinmux
 * @version  V1.0
 * @date     02. June 2017
 * @vendor   csky
 * @chip     pangu
 ******************************************************************************/
#include <stdint.h>
#include "pinmux.h"
#include "pin_name.h"
#include <drv/gpio.h>

void af_io_init(gpio_pin_t *pin,enum GPIO_AF af);
void set_gpio_mode(gpio_pin_t *pin);

int32_t drv_pinmux_config(pin_name_e pin, pin_func_e pin_func)
{
    if(pin_func == PIN_FUNC_GPIO)
    {
        set_gpio_mode((gpio_pin_t *)&pin);
    }else
    {
        af_io_init((gpio_pin_t *)&pin,pin_func);
    }
    return 0;
}

int32_t drv_pin_config_mode(port_name_e port, uint8_t offset, gpio_mode_e pin_mode)
{
    return 0;
}
