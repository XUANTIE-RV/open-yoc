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
#if CONFIG_CPU_XUANTIE_E907 || CONFIG_CPU_XUANTIE_E907F || CONFIG_CPU_XUANTIE_E907FD \
    || CONFIG_CPU_XUANTIE_E907P || CONFIG_CPU_XUANTIE_E907FP || CONFIG_CPU_XUANTIE_E907FDP
#include <stdio.h>
#include <csi_core.h>
#include <drv/tick.h>
#include "board.h"
#if CONFIG_COPY_ITCM_CODE
#include "e907_firmware.h"
#endif

#define CONFIG_ITCM_BASE_ADDR 0x30000000

int main(void)
{
    board_init();

#if CONFIG_COPY_ITCM_CODE
    // Run in sram
    printf("copy itcm code to itcm region.\n");
    // itcm base address config
    csi_itcm_set_base_addr(CONFIG_ITCM_BASE_ADDR);
    // itcm enable
    csi_itcm_enable();
    // set the address map of itcm slave pad
    *(volatile int *)0x4004000C = 0x60;
    // copy firmware to itcm
    memcpy((void *)0x60000000, yoc_bin, yoc_bin_len);
    printf("finish copy itcm code to itcm region.\n");
    // set boot from itcm when reset
    *(volatile int *)0x40040010 = 0x1;
    // set rvba address when reset
    *(volatile int *)0x40040018 = CONFIG_ITCM_BASE_ADDR;
    printf("will reset current core.\n");
    // reset current core
    __asm volatile("csrsi mexstatus, 1");
#else
    // Run in itcm
    printf("start to run in itcm.\n");
    void *p = malloc(100);
    if (p == NULL) {
        printf("malloc failed.\n");
        return -1;
    } else {
        printf("malloc 100B success.\n");
        free(p);
    }
    printf("run in itcm success.\n");
#endif

    return 0;
}
#endif
