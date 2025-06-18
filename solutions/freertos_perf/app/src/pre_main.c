 /*
 * Copyright (C) 2017-2024 Alibaba Group Holding Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <FreeRTOS.h>
#include <task.h>
#include <perf.h>

extern int main(void);
static void application_task_entry(void *arg)
{
#if defined(CONFIG_SMP) && CONFIG_SMP
    SecondaryCoresUp();
#endif /* CONFIG_SMP */
    main();
}

/* overide pre_main in chip component */
int pre_main(void)
{
    perf_stat_init();
    perf_record_init();

    // Create the application task
    BaseType_t result;
    TaskHandle_t appTaskHandle = NULL;
    // Task handle
    result = xTaskCreate(
        application_task_entry,
        "app_task", CONFIG_INIT_TASK_STACK_SIZE / sizeof(StackType_t),
        NULL,
        tskIDLE_PRIORITY + 1,
        &appTaskHandle);

    if (result != pdPASS)
    {
        for (;;);
    }

    vTaskStartScheduler();

    /* never reach here unless scheduler is stopped */
    perf_stat_deinit();
    perf_record_deinit();
    for (;;);

    return 0;
}


