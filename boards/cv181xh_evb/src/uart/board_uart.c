/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#include <board.h>
#include <devices/devicelist.h>

#if defined(BOARD_UART_NUM) && BOARD_UART_NUM > 0

#include <stdlib.h>
#include <stdio.h>

void board_uart_init(void) {
  rvm_uart_drv_register(0);
}

#endif
