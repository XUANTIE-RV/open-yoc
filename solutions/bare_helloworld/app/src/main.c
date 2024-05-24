/*
 * Copyright (C) 2017-2024 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <csi_core.h>
#include "board.h"

int main(void)
{
    board_init();
    printf("Hello World!\n");
#if !CONFIG_CPU_E9XX
    printf("cpu work mode = %s!\n", csi_get_cpu_work_mode() == 1 ? "Supervisor" : "Machine");
#endif

    printf("bare_helloworld runs success!\n");

    return 0;
}
