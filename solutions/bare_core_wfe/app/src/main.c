/*
 * Copyright (C) 2017-2024 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <csi_core.h>
#include "board.h"

extern int example_core_wfe();
int main(void)
{
    board_init();
    printf("bare_core_wfe demo start!\r\n");
#if CONFIG_CPU_E9XX
    example_core_wfe();
#else
    printf("nmi is not support for this cpu!\n");
#endif

    return 0;
}

