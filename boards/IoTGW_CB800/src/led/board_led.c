/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <stdio.h>

#include <board.h>
#include <pinmux.h>

void board_led_init(void)
{
    drv_pinmux_config(BOARD_LED0_PWM_PIN, PA7_PWM);
}
