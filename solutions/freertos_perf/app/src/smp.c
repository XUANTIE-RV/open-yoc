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
#if defined(CONFIG_SMP) && CONFIG_SMP
#include <stdio.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

int g_smp_count;
SemaphoreHandle_t pxTaskMutex;

static void thread_entry(void *parameter)
{
    while (1) {
        xSemaphoreTake(pxTaskMutex, portMAX_DELAY);
        // printf("[%s] in %d core, count:%d \r\n", portGET_CORE_ID(), g_smp_count++);
        xSemaphoreGive(pxTaskMutex);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void smp_example(void)
{
    pxTaskMutex = xSemaphoreCreateMutex();
    for (UBaseType_t i = 0; i < CONFIG_NR_CPUS; i++)
    {
        char name[32];
        TaskHandle_t xHandle = NULL;
        snprintf(name, sizeof(name), "thread_%u", (unsigned int)i);
        UBaseType_t uxCoreAffinityMask = (1 << i);
        BaseType_t ret = xTaskCreateAffinitySet(thread_entry, name, 8192, NULL, 32, uxCoreAffinityMask, &xHandle);
        configASSERT(ret == pdPASS);
    }
}
#endif
