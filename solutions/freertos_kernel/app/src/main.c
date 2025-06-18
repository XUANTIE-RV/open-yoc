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
#include <FreeRTOSConfig.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include "board.h"
#include "app_main.h"

int main(void)
{
    board_init();
    printf("FreeRTOS version:%s\n", tskKERNEL_VERSION_NUMBER);

    /* start example */
    printf("-------------event_example_main start-------------\n");
    event_example_main();
    vTaskDelay(pdMS_TO_TICKS(500));
    printf("-------------event_example_main finish-------------\n\n");

    printf("-------------message_example_main start-------------\n");
    message_example_main();
    vTaskDelay(pdMS_TO_TICKS(500));
    printf("-------------message_example_main finish-------------\n\n");

    printf("-------------mutex_example_main start-------------\n");
    mutex_example_main();
    vTaskDelay(pdMS_TO_TICKS(500));
    printf("-------------mutex_example_main finish-------------\n\n");

    printf("-------------sem_example_main start-------------\n");
    sem_example_main();
    vTaskDelay(pdMS_TO_TICKS(500));
    printf("-------------sem_example_main finish-------------\n\n");

    printf("-------------task_example_main start-------------\n");
    task_example_main();
    vTaskDelay(pdMS_TO_TICKS(500));
    printf("-------------task_example_main finish-------------\n\n");

    printf("-------------time_example_main start-------------\n");
    time_example_main();
    vTaskDelay(pdMS_TO_TICKS(500));
    printf("-------------time_example_main finish-------------\n\n");

    printf("-------------timer_example_main start-------------\n");
    timer_example_main();
    vTaskDelay(pdMS_TO_TICKS(2000));
    printf("-------------timer_example_main finish-------------\n\n");
    printf("kernel all examples finish.\n");
    return 0;
}

