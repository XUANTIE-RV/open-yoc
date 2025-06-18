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
#include <rtthread.h>
#include <csi_core.h>
#include "board.h"

extern void example_main(void *arg);
extern void cxx_system_init(void);
int main(void)
{
    board_init();
    /* show RT-Thread version */
    rt_show_version();
    cxx_system_init();

    int prio = RT_THREAD_PRIORITY_MAX / 2;
    rt_thread_t tid = rt_thread_create("example main task", example_main, NULL, CONFIG_INIT_TASK_STACK_SIZE, prio, 10u);
    if (tid != RT_NULL) {
        rt_thread_startup(tid);
    } else {
        printf("example main task fail\n");
    }
    return 0;
}
