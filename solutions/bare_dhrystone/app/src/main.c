/*
 * Copyright (C) 2017-2024 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include "board.h"

extern void benchmark_dhry_main(void);

int main(void)
{
    board_init();
    printf("bare_dhrystore demo start!\r\n");
    benchmark_dhry_main();
    return 0;
}
