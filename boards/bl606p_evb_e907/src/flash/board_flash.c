/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#include <board.h>

#include <stdlib.h>
#include <stdio.h>
#include <devices/devicelist.h>
#include <blyoc_flash.h>

void board_flash_init(void)
{
    bl_flash_init();
    rvm_spiflash_drv_register(0);
}
