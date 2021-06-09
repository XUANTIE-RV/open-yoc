/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <board.h>
#include <drv/pin.h>

static void board_pinmux_config(void)
{
    csi_pin_set_mux(CONSOLE_TXD, CONSOLE_TXD_FUNC);
    csi_pin_set_mux(CONSOLE_RXD, CONSOLE_RXD_FUNC);
}

void board_init(void)
{
    board_pinmux_config();
}
