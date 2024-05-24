/*
 * Copyright (C) 2019-2024 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <FreeRTOSConfig.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include "board.h"
#include <app_main.h>

extern void example_main(void);
extern void cxx_system_init(void);

int main(void)
{
    board_init();
    cxx_system_init();

    printf("FreeRTOS version:%s\n", tskKERNEL_VERSION_NUMBER);
    example_main();
    return 0;
}
