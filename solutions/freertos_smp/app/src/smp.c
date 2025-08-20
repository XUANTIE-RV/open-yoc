/*
 * Copyright (C) 2017-2024 Alibaba Group Holding Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <FreeRTOSConfig.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <csi_core.h>
#include "board.h"
#include "app_main.h"

SemaphoreHandle_t xMutex;
int g_count = 0;

void task_entry(void *parameter)
{
    while (1)
    {
        int id = csi_get_cpu_id();
        if (xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE)
        {
            printf("[%s] in %lu core, count: %d\r\n", pcTaskGetName(NULL), (unsigned long)id, g_count++);
            xSemaphoreGive(xMutex);
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void smp_example(void)
{
    xMutex = xSemaphoreCreateMutex();
    /* create a task and pin them each cpu */
    for (UBaseType_t i = 0; i < CONFIG_NR_CPUS; i++)
    {
        char name[32];
        TaskHandle_t xHandle = NULL;
        snprintf(name, sizeof(name), "thread_%u", (unsigned int)i);
        UBaseType_t uxCoreAffinityMask = (1 << i); // core id
        BaseType_t ret = xTaskCreate(task_entry, name, CONFIG_APP_TASK_STACK_SIZE / sizeof(StackType_t), NULL, tskIDLE_PRIORITY + 5, &xHandle);
        if (ret != pdPASS)
        {
            printf("create task failed\r\n");
        }
        else
        {
            vTaskCoreAffinitySet(xHandle, uxCoreAffinityMask);
        }
    }
}
