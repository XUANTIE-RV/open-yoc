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
#include <shell.h>
#include "board.h"

int main(void)
{
    board_init();

    /* show RT-Thread version */
    rt_show_version();
    printf("Hello world! RT-Thread\r\n");

#if CONFIG_CPU_XUANTIE_E902 || CONFIG_CPU_XUANTIE_E902M || CONFIG_CPU_XUANTIE_E902T || CONFIG_CPU_XUANTIE_E902MT \
    || CONFIG_CPU_XUANTIE_E901_CP || CONFIG_CPU_XUANTIE_E901_B_CP || CONFIG_CPU_XUANTIE_E901_M_CP || CONFIG_CPU_XUANTIE_E901_BM_CP \
    || CONFIG_CPU_XUANTIE_E901MINI_CP || CONFIG_CPU_XUANTIE_E901MINI_B_CP || CONFIG_CPU_XUANTIE_E901MINI_ZM_CP || CONFIG_CPU_XUANTIE_E901MINI_BZM_CP
// Smartl ISRAM Limited
#else
    extern int utest_init(void);
    utest_init();
#endif

    finsh_system_init();

    while (1) {
        sleep(3);
    }

    return 0;
}
