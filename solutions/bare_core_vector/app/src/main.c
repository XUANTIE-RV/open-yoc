/*
 * Copyright (C) 2017-2024 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <csi_core.h>
#include "board.h"

extern int example_core_vector();
int main(void)
{
    board_init();
    printf("bare_core_vector demo start!\r\n");
#ifdef __riscv_vector
    example_core_vector();
#else
    printf("vector is not support for this cpu!\n");
#endif

    return 0;
}

