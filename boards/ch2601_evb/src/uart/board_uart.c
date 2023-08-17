/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#ifndef CONFIG_KERNEL_NONE
#include <board.h>
#include <stdlib.h>
#include <stdio.h>
#include <devices/devicelist.h>

void board_uart_init(void)
{
    rvm_uart_drv_register(0);
}
#else
void board_uart_init(void) {}
#endif