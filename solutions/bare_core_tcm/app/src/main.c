/*
 * Copyright (C) 2017-2024 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <csi_core.h>
#include "board.h"

extern int example_core_tcm();
int main(void)
{
    board_init();
    printf("bare_core_tcm demo start!\r\n");
#if (CONFIG_CPU_E907 || CONFIG_CPU_E907F || CONFIG_CPU_E907FD || CONFIG_CPU_E907P || CONFIG_CPU_E907FP || CONFIG_CPU_E907FDP)
    example_core_tcm();
#else
    printf("tcm is not support for this cpu!\n");
#endif

    return 0;
}

