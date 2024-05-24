/*
 * Copyright (C) 2019-2024 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <rtthread.h>
#include "board.h"

int main(void)
{
    board_init();

    /* show RT-Thread version */
    rt_show_version();

    while (1) {
        printf("Hello world! RT-Thread\r\n");
        sleep(3);
    }

    return 0;
}

