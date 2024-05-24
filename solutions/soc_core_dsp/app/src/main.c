/*
 * Copyright (C) 2019-2024 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include "aos/cli.h"
#include "app_main.h"
#include "app_init.h"

#define TAG "app"

extern int example_core_dsp();
int main(void)
{
    board_yoc_init();

    printf("soc_core_dsp demo start!\r\n");
#ifdef __riscv_dsp
    example_core_dsp();
#else
    printf("dsp is not support for this cpu!\n");
#endif
    return 0;
}

