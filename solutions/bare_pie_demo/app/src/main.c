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

int g_test_data_var = 100;
int g_test_bss_var;

extern unsigned long __stext, __rodata_end__;
extern unsigned long __data_start__, __data_end__;
extern unsigned long __bss_start__, __bss_end__;
extern unsigned long __dram_vma;
extern unsigned long g_heap_start, g_heap_end;

int main(void)
{
    board_init();
    void *p1 = malloc(1024);
    void *p2 = malloc(16);

    printf("hello PIE demo!\n");

    printf("__dram_vma:         0x%lx\n", __dram_vma);

    printf("__stext(dram_lma):  0x%lx\n", (unsigned long)&__stext);
    printf("__rodata_end__:     0x%lx\n", (unsigned long)&__rodata_end__);
    printf("__data_start__:     0x%lx\n", (unsigned long)&__data_start__);
    printf("__data_end__:       0x%lx\n", (unsigned long)&__data_end__);
    printf("__bss_start__:      0x%lx\n", (unsigned long)&__bss_start__);
    printf("__bss_end__:        0x%lx\n", (unsigned long)&__bss_end__);
    printf("g_heap_start:       0x%lx\n", (unsigned long)g_heap_start);
    printf("g_heap_end:         0x%lx\n", (unsigned long)g_heap_end);

    printf("Global Variable in data section Address:    0x%lx\n", (unsigned long)&g_test_data_var);
    printf("Global Variable in bss section Address:     0x%lx\n", (unsigned long)&g_test_bss_var);
    printf("malloc mem addr1: 0x%lx\n", (unsigned long)p1);
    printf("malloc mem addr2: 0x%lx\n", (unsigned long)p2);
    free(p1);
    free(p2);

    printf("PIE demo run ok.\n\n");
    return 0;
}
