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
#include <csi_core.h>
#include "board.h"

extern int example_core_tcm();
int main(void)
{
    board_init();
    printf("bare_core_tcm demo start!\r\n");
#if (CONFIG_CPU_XUANTIE_E907 || CONFIG_CPU_XUANTIE_E907F || CONFIG_CPU_XUANTIE_E907FD || CONFIG_CPU_XUANTIE_E907P || CONFIG_CPU_XUANTIE_E907FP || CONFIG_CPU_XUANTIE_E907FDP \
    || CONFIG_CPU_XUANTIE_R908 || CONFIG_CPU_XUANTIE_R908FD || CONFIG_CPU_XUANTIE_R908FDV \
    || CONFIG_CPU_XUANTIE_R908_CP || CONFIG_CPU_XUANTIE_R908FD_CP || CONFIG_CPU_XUANTIE_R908FDV_CP \
    || CONFIG_CPU_XUANTIE_R908_CP_XT || CONFIG_CPU_XUANTIE_R908FD_CP_XT || CONFIG_CPU_XUANTIE_R908FDV_CP_XT)
    example_core_tcm();
#else
    printf("tcm is not support for this cpu!\n");
#endif

    return 0;
}

