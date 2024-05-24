/*
 * Copyright (C) 2019-2024 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <FreeRTOS.h>
#include <task.h>
#include "board.h"

int main(void)
{
    board_init();
    printf("FreeRTOS version:%s\n", tskKERNEL_VERSION_NUMBER);
    while (1) {
        printf("Hello world! FreeRTOS\r\n");
        sleep(3);
    }

    return 0;
}

