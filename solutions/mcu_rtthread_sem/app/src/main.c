/*
 * Copyright (C) 2019-2024 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <rtthread.h>
#include "board.h"

extern void example_main(void *arg);
int main(void)
{
    board_init();
    /* show RT-Thread version */
    rt_show_version();
    int prio = RT_THREAD_PRIORITY_MAX / 2;
    rt_thread_t tid = rt_thread_create("example main task", example_main, NULL, CONFIG_INIT_TASK_STACK_SIZE, prio, 10u);
    if (tid != RT_NULL) {
        rt_thread_startup(tid);
    } else {
        printf("example main task fail\n");
    }
    return 0;
}