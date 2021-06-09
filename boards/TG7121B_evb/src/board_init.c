/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */


#include <stdlib.h>
#include <stdio.h>
#include "io_config.h"
#include "board_config.h"
#include <devices/console_uart.h>

void platform_init(void);

void board_init(void)
{    
    platform_init();
    uart1_io_init(PB00,PB01);

//[genie]add by lgy at 2021-03-04
#if defined(CONIFG_GENIE_MESH_BINARY_CMD) || defined(CONFIG_GENIE_MESH_AT_CMD)
    uart2_io_init(PB08, PB09);
#endif

    console_init(CONSOLE_UART_IDX, 115200, 512);
}
