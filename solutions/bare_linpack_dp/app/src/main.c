/*
 * Copyright (C) 2017-2024 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include "board.h"

extern void benchmark_linpack_dp_main(void);

int main(void)
{
    board_init();
    printf("bare_linpack_dp demo start!\r\n");
    benchmark_linpack_dp_main();
    return 0;
}
