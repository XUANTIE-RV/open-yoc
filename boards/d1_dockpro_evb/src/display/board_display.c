/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <board.h>
#include <drv/display.h>
#include <drv/g2d.h>

void board_display_init(void)
{
    int ret = csi_display_init();
    if (ret != 0) {
        printf("display init failed.\n");
    } else {
        printf("display init ok.\n");
        csi_g2d_init();
    }
}
