/*
 * Copyright (C) 2017-2024 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <csi_core.h>
#include "board.h"

extern int example_core_vic();
int main(void)
{
    board_init();
    printf("bare_core_vic demo start!\r\n");
    example_core_vic();

    return 0;
}

