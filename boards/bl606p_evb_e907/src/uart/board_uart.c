/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#include <board.h>

#if defined(BOARD_UART_NUM) && BOARD_UART_NUM > 0

#include <stdlib.h>
#include <stdio.h>
#include <devices/devicelist.h>

void board_uart_init(void) {
    rvm_uart_drv_register(0);
    rvm_uart_drv_register(1);
    rvm_uart_drv_register(2);
}

#endif
