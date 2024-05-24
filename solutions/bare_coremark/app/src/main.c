/*
 * Copyright (C) 2017-2024 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include "board.h"

extern void benchmark_coremark_main(void);

int main(void)
{
    board_init();
    printf("bare_coremark demo start!\r\n");
    benchmark_coremark_main();
    return 0;
}
