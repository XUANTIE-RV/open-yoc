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

extern int example_core_tcm_ecc();
extern int example_core_ecc();
extern int example_core_ecc2b();
int main(void) {
    board_init();
    printf("bare_core_ecc demo start!\r\n");
    printf("\r\n=========CACHE ECC============\r\n\r\n");
#if CONFIG_CPU_XUANTIE_C907 || CONFIG_CPU_XUANTIE_C907FD || CONFIG_CPU_XUANTIE_C907FDV || CONFIG_CPU_XUANTIE_C907FDVM \
    || CONFIG_CPU_XUANTIE_C907_RV32 || CONFIG_CPU_XUANTIE_C907FD_RV32 || CONFIG_CPU_XUANTIE_C907FDV_RV32 || CONFIG_CPU_XUANTIE_C907FDVM_RV32 \
    || CONFIG_CPU_XUANTIE_C908 || CONFIG_CPU_XUANTIE_C908V || CONFIG_CPU_XUANTIE_C908I \
    || CONFIG_CPU_XUANTIE_C910V2 || CONFIG_CPU_XUANTIE_C910V3 || CONFIG_CPU_XUANTIE_C910V3_CP \
    || CONFIG_CPU_XUANTIE_C920V2 || CONFIG_CPU_XUANTIE_C920V3 || CONFIG_CPU_XUANTIE_C920V3_CP\
    || CONFIG_CPU_XUANTIE_R910 || CONFIG_CPU_XUANTIE_R920 \
    || CONFIG_CPU_XUANTIE_R908 || CONFIG_CPU_XUANTIE_R908FD || CONFIG_CPU_XUANTIE_R908FDV \
    || CONFIG_CPU_XUANTIE_R908_CP || CONFIG_CPU_XUANTIE_R908FD_CP || CONFIG_CPU_XUANTIE_R908FDV_CP
    example_core_ecc();
    example_core_ecc2b();
#else
    printf("ecc is not support for this cpu!\n");
#endif
#if CONFIG_CPU_XUANTIE_R908 || CONFIG_CPU_XUANTIE_R908FD || CONFIG_CPU_XUANTIE_R908FDV \
    || CONFIG_CPU_XUANTIE_R908_CP || CONFIG_CPU_XUANTIE_R908FD_CP || CONFIG_CPU_XUANTIE_R908FDV_CP
    printf("\r\n=========TCM ECC============\r\n\r\n");
    example_core_tcm_ecc();
#endif
    return 0;
}

