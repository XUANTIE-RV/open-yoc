/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <stdio.h>

#include <board.h>
#include <pin_name.h>
#include <pinmux.h>

void board_button_init(void)
{
    drv_pinmux_config(BOARD_BUTTON0_GPIO_PIN, PIN_FUNC_GPIO);
    drv_pinmux_config(BOARD_BUTTON1_GPIO_PIN, PIN_FUNC_GPIO);
}
