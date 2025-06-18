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
#include "board.h"

int main(void)
{
    board_init();

    /* show RT-Thread version */
    rt_show_version();
#if defined(CONFIG_SMP) && CONFIG_SMP
    extern void smp_example(void);
    smp_example();
#endif
    while (1) {
        // printf("Hello world! RT-Thread\r\n");
        sleep(3);
    }

    return 0;
}

