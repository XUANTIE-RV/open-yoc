/*
 * Copyright (C) 2017-2024 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <csi_core.h>
#include "board.h"

extern int example_core_ecc();
int main(void)
{
    board_init();
    printf("bare_core_ecc demo start!\r\n");
#if CONFIG_CPU_C907 || CONFIG_CPU_C907FD || CONFIG_CPU_C907FDV || CONFIG_CPU_C907FDVM \
    || CONFIG_CPU_C907_RV32 || CONFIG_CPU_C907FD_RV32 || CONFIG_CPU_C907FDV_RV32 || CONFIG_CPU_C907FDVM_RV32 \
    || CONFIG_CPU_C908 || CONFIG_CPU_C908V || CONFIG_CPU_C908I \
    || CONFIG_CPU_C910 || CONFIG_CPU_C920 || CONFIG_CPU_C910V2 || CONFIG_CPU_C920V2 \
    || CONFIG_CPU_R910 || CONFIG_CPU_R920
    example_core_ecc();
#else
    printf("ecc is not support for this cpu!\n");
#endif

    return 0;
}

