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

#define K_API_PARENT_PRIO    32

static TaskHandle_t example_main_task;
extern void example_main(void);

int main(void)
{
    BaseType_t ret;

    board_init();
    printf("FreeRTOS version:%s\n", tskKERNEL_VERSION_NUMBER);
    ret = xTaskCreate((TaskFunction_t)example_main, "example_main", CONFIG_APP_TASK_STACK_SIZE, NULL, K_API_PARENT_PRIO, &example_main_task);
    if (ret != pdPASS) {
        printf("create example_main fail\n");
    }
    return 0;
}

