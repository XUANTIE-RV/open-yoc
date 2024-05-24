/*
 * Copyright (C) 2017-2024 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include "board.h"

extern void benchmark_linpack_sp_main(void);

int main(void)
{
    board_init();
    printf("bare_linpack_sp demo start!\r\n");
    benchmark_linpack_sp_main();
    return 0;
}
