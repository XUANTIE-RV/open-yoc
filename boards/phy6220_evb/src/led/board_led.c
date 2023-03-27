/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <stdio.h>

#include <board.h>
#include <pin_name.h>
#include <pinmux.h>

void board_led_init(void)
{
    drv_pinmux_config(BOARD_LED0_PWM_PIN, FMUX_PWM0);
    drv_pinmux_config(BOARD_LED1_PWM_PIN, FMUX_PWM1);
    drv_pinmux_config(BOARD_LED2_PWM_PIN, FMUX_PWM2);
}
