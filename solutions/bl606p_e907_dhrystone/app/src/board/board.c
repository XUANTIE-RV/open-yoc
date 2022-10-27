/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <board.h>
#include <drv/pin.h>
#include <drv/gpio.h>
#include <drv/dma.h>
#include <sys_clk.h>

static void board_pinmux_config(void)
{
}

void board_pre_init(void)
{
}

void board_gpio_init(void)
{

}

void board_dma_init(void)
{

}

void board_init(void)
{
    board_pinmux_config();
}
