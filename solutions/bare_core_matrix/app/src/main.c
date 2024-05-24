/*
 * Copyright (C) 2017-2024 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <csi_core.h>
#include "board.h"

extern int example_core_matrix();
int main(void)
{
    board_init();
    printf("bare_core_matrix demo start!\r\n");
#ifdef __riscv_matrix
    example_core_matrix();
#else
    printf("matrix is not support for this cpu!\n");
#endif

    return 0;
}

