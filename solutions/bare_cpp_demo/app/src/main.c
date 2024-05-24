/*
 * Copyright (C) 2017-2024 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include "board.h"

extern void cpp_demo_main(void);
extern void cxx_system_init(void);

int main(void)
{
    board_init();
    cxx_system_init();

    printf("bare_cpp_demo demo start!\r\n");
    cpp_demo_main();
    return 0;
}
