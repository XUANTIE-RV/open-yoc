/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <board.h>

static void board_pinmux_config(void)
{
    //drv_pinmux_config(CONSOLE_TXD, CONSOLE_TXD_FUNC);
    //drv_pinmux_config(CONSOLE_RXD, CONSOLE_RXD_FUNC);
}

/**
 * @brief  init the board for default: pin mux, etc.
 * re-implement if need.
 * @return
 */
void board_init(void)
{
    board_pinmux_config();
}
