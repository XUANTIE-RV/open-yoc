/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */


#include <stdlib.h>
#include <stdio.h>
#include <aos/aos.h>
#include <pin.h>

static void board_pinmux_config(void)
{
    drv_pinmux_config(CONSOLE_TXD, CONSOLE_TXD_FUNC);
    drv_pinmux_config(CONSOLE_RXD, CONSOLE_RXD_FUNC);
}

void board_init(void)
{
    board_pinmux_config();
    //flash_csky_register(0);
}

