/*
 * Copyright (C) 2017-2024 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <csi_core.h>
#include "board.h"

extern int example_core_dsp();
int main(void)
{
    board_init();
    printf("bare_core_dsp demo start!\r\n");
#if (CONFIG_CPU_E906P || CONFIG_CPU_E906FP || CONFIG_CPU_E906FDP|| CONFIG_CPU_E907P || CONFIG_CPU_E907FP || CONFIG_CPU_E907FDP)
    example_core_dsp();
#else
    printf("dsp is not support for this cpu!\n");
#endif

    return 0;
}
