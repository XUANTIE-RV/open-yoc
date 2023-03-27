/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <stdio.h>

#include <board.h>
#include <pin_name.h>
#include <pinmux.h>

static pin_name_e gpio_pin_list[] = {
    (BOARD_GPIO_PIN0), (BOARD_GPIO_PIN1), (BOARD_GPIO_PIN2), (BOARD_GPIO_PIN3), (BOARD_GPIO_PIN4),  (BOARD_GPIO_PIN5),
    (BOARD_GPIO_PIN6), (BOARD_GPIO_PIN7), (BOARD_GPIO_PIN8), (BOARD_GPIO_PIN9), (BOARD_GPIO_PIN10),
};

void board_gpio_pin_init(void)
{
    int i = 0;
    for (i = 0; i < sizeof(gpio_pin_list) / sizeof(gpio_pin_list[0]); i++) {
        drv_pinmux_config(gpio_pin_list[i], PIN_FUNC_GPIO);
    }
}
