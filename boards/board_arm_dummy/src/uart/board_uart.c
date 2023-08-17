/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <stdio.h>
#include <devices/devicelist.h>
#include <board.h>

void board_uart_init(void)
{
    rvm_uart_drv_register(0);
}
