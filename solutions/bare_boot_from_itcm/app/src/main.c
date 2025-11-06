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
#if CONFIG_CPU_XUANTIE_R908 || CONFIG_CPU_XUANTIE_R908FD || CONFIG_CPU_XUANTIE_R908FDV \
    || CONFIG_CPU_XUANTIE_R908_CP || CONFIG_CPU_XUANTIE_R908FD_CP || CONFIG_CPU_XUANTIE_R908FDV_CP \
    || CONFIG_CPU_XUANTIE_R908_CP_XT || CONFIG_CPU_XUANTIE_R908FD_CP_XT || CONFIG_CPU_XUANTIE_R908FDV_CP_XT
#include <stdio.h>
#include <csi_core.h>
#include <drv/tick.h>
#include "board.h"
#if CONFIG_COPY_ITCM_CODE
#include "cpu1_firmware.h"
#endif

#define CONFIG_ITCM_BASE_ADDR 0x50200000

int main(void)
{
    board_init();

#if CONFIG_COPY_ITCM_CODE
    printf("[cpuid: %d] start to copy itcm code to core1.\r\n", csi_get_cpu_id());
    // set wait
    *(volatile int *)0x18030120 = 1;
    // itcm_enable
    *(volatile int *)0x1803012c = 1;
    // itcm base address config
    *(volatile int *)0x18030130 = CONFIG_ITCM_BASE_ADDR;
    // core1 rvba address config
    *(volatile int *)0x18030010 = CONFIG_ITCM_BASE_ADDR;
    // core1 reset
    *(volatile int *)0x18030000 = 0xf;
    // wait core1 init_cmplt pull high
    while(1) {
        if (*(volatile int *)0x18030140) {
            break;
        }
        mdelay(5);
    }
    // copy data to strong-order address:0x3000_0000
    memcpy((void *)0x30000000, yoc_bin, yoc_bin_len);

    while(*(int *)0x30000000 != *(int *)yoc_bin) {}

    printf("[cpuid: %d] finish copy itcm code to core1.\r\n", csi_get_cpu_id());
    // when copy finish, pull low for wait reg
    *(volatile int *)0x18030120 = 0;
    mdelay(5);
#else
    printf("[cpuid: %d] start to run in itcm.\r\n", csi_get_cpu_id());
    void *p = malloc(100);
    if (p == NULL) {
        printf("[cpuid: %d] malloc failed.\r\n", csi_get_cpu_id());
        return -1;
    } else {
        printf("[cpuid: %d] malloc 100B success.\r\n", csi_get_cpu_id());
        free(p);
    }
    printf("[cpuid: %d] run in itcm success.\r\n", csi_get_cpu_id());
#endif

    return 0;
}
#endif
