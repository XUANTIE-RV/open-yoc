/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#include <board.h>

#if defined(BOARD_GPIO_PIN_NUM) && BOARD_GPIO_PIN_NUM > 0
#include <stdlib.h>
#include <stdio.h>

#include <sys_clk.h>

void board_gpio_pin_init(void)
{
    // csi_gpio_t gpio_handler;

    // soc_clk_init();
    // csi_gpio_init(&gpio_handler, 0);
}
#endif
