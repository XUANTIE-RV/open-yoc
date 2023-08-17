/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */
#ifndef CONFIG_KERNEL_NONE
#include <board.h>
#include <stdlib.h>
#include <stdio.h>
#include <devices/devicelist.h>

void board_flash_init(void)
{
    rvm_spiflash_drv_register(0);
}
#else
void board_flash_init(void) {}
#endif