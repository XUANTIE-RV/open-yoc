/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#include <board.h>
#include <stdlib.h>
#include <stdio.h>
#include <devices/devicelist.h>

void board_spi_init(void) {
    rvm_spi_drv_register(0);
}
