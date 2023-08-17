/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#include <board.h>
#include <devices/devicelist.h>
#include <stdlib.h>
#include <stdio.h>

void board_uart_init(void)
{
    rvm_uart_drv_register(0);
}
