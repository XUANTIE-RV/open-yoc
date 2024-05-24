/*
 * Copyright (C) 2018-2024 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <FreeRTOSConfig.h>
#include <FreeRTOS.h>
#include <task.h>

extern int main(void);
static void application_task_entry(void *arg)
{
    main();

    /* task exit by itself */
    vTaskDelete(NULL);
}

/* overide pre_main in chip component */
int pre_main(void)
{
    TaskHandle_t xHandle;
    int prio = configMAX_PRIORITIES / 2;

    BaseType_t ret = xTaskCreate(application_task_entry, "app_task", CONFIG_INIT_TASK_STACK_SIZE / sizeof( StackType_t ), NULL, prio, &xHandle);
    if (ret != pdPASS) {
        //printf("task create error, ret = %d\r\n", ret);
        while (1);
    }

    /* kernel start */
    vTaskStartScheduler();

    return 0;
}


