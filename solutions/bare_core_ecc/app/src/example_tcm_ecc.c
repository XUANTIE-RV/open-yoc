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
#include <csi_config.h>
#include <stdio.h>
#include <string.h>
#include <csi_core.h>
#include <soc.h>
#include "ecc_stats.h"

#define ITCM_BASE      0x50200000
#define DTCM_BASE      0x50300000
#define TIMEOUT 0X000fffffUL

extern int __erodata;
extern int __data_start__;
extern int __data_end__;
extern int __itcm_code_start__;
extern int __itcm_code_end__;
extern int __dtcm_code_start__;
extern int __dtcm_code_end__;

extern ecc_stats_t g_ecc_stats;

extern void test_tcm(void);

static void dtcm_init()
{
    unsigned long size = csi_dtcm_get_size();
    printf("dtcm size:0x%lx\n", size);
    csi_dtcm_set_base_addr(DTCM_BASE);
    csi_dtcm_enable();
    memset((void *)DTCM_BASE, 0x98, size);
}

static void dtcm_read()
{
    volatile unsigned long *addr = (unsigned long *)DTCM_BASE;
    printf("read value[0x%lx] in dtcm[0x%lx].\n", *addr, (unsigned long)addr);
}

static void itcm_init()
{
    unsigned long size = csi_itcm_get_size();
    unsigned long data_len = (unsigned long)&__data_end__ - (unsigned long)&__data_start__;
    unsigned long itcm_len = (unsigned long)&__itcm_code_end__ - (unsigned long)&__itcm_code_start__;
    // unsigned long ram_code_len = (unsigned long)&__ram_code_end__ - (unsigned long)&__ram_code_start__;
    char *itcm_start_lma = (char *)((unsigned long)&__erodata + data_len);
    // char *dtcm_start_lma = (char*)((unsigned long)&__erodata + data_len + ram_code_len + itcm_len);
    // unsigned long len = (unsigned long)&__itcm_code_end__ - (unsigned long)&__itcm_code_start__;
    // printf("%x,%x,%x,%x,%x, %d\r\n", &__erodata, &__data_start__, &__data_end__, &__itcm_code_start__, &__itcm_code_end__, len);
    printf("itcm size:0x%lx\n", size);
    csi_itcm_set_base_addr(ITCM_BASE);
    csi_itcm_enable();

    memset((void *)(&__itcm_code_start__), 0, size);
    memcpy((void *)(ITCM_BASE), (void *)itcm_start_lma, itcm_len);
    csi_icache_invalid();
}

static int dtcm_ecc(void)
{
    unsigned long long mcer_b, mcer_a;
    unsigned long long mcer2_b, mcer2_a;
    unsigned long timeout = TIMEOUT;

    csi_dtcm_ecc_enable();
    g_ecc_stats.err_cnt_l1 = 0;
    g_ecc_stats.mcer_fault_ram = -1;

    mcer_b = __get_MCER();
    printf("DTCM ECC: before 1-bit error inject, mcer = 0x%llx\n", mcer_b);

    /* inject 1bit error for DTCM */
    __set_MEICR(0xc0000001);

    /* access dtcm and trigger ecc */
    printf("trigger ecc in DTCM: \r\n");
    dtcm_read();

    mcer_a = __get_MCER();
    printf("DTCM ECC: after 1-bit error inject, mcer = 0x%llx\n", mcer_a);
    if (mcer_b != mcer_a)
    {
        printf("DTCM ECC: 1-bit error corrected!\n");
    }
    else
    {
        printf("DTCM ECC: failed to detect 1-bit error!\n");
    }

    mcer2_b = __get_MCER();
    printf("DTCM ECC: 2-bit error inject, mcer = 0x%llx\n", mcer2_b);

    /* inject 2bit error for DTCM */
    __set_MEICR(0xc0000003);

    /* access dtcm and trigger ecc */
    dtcm_read();
    while (--timeout)
    {
        // printf("timeout=%llx\r\n",timeout);
        if (g_ecc_stats.mcer_fault_ram == MCER_FAULT_RAMID_DTCM)
        {
            break;
        }
    }

    mcer2_a = __get_MCER();
    printf("DTCM ECC: after 2-bit error inject, mcer = 0x%llx\n", mcer2_a);
    if (timeout > 0)
    {
        printf("DTCM ECC: 2bit runs successfully!\n");
        // printf("DTCM ECC: 2-bit error detected!\n");
    }
    else
    {
        printf("DTCM ECC: failed to detect 2-bit error\n");
    }
    return 0;
}

static int itcm_ecc(void)
{
    unsigned long long mcer_b, mcer_a;
    unsigned long long mcer2_b, mcer2_a;

    csi_itcm_ecc_enable();
    g_ecc_stats.err_cnt_l1 = 0;
    g_ecc_stats.mcer_fault_ram = -1;

    mcer_b = __get_MCER();
    printf("ITCM ECC: before 1-bit error inject, mcer = 0x%llx\n", mcer_b);

    /* inject 1bit error for ITCM */
    __set_MEICR(0xe0000001);

    /* trigger ITCM ECC */
    printf("trigger ecc in ITCM: \r\n");
    test_tcm();

    mcer_a = __get_MCER();
    printf("ITCM ECC: after 1-bit error inject, mcer = 0x%llx\n", mcer_a);

    if (mcer_b != mcer_a)
    {
        printf("ITCM ECC: 1-bit error corrected!\n");
    }
    else
    {
        printf("ITCM ECC: failed to detect 1-bit error!\n");
    }

    mcer2_b = __get_MCER();
    printf("ITCM ECC: before 2-bit error inject, mcer = 0x%llx\n", mcer2_b);

    /* inject 2bit error for ITCM */
    __set_MEICR(0xe0000003);

    /* trigger ITCM ECC */
    test_tcm();
    // TODO:goto Exception
    mcer2_a = __get_MCER();
    printf("ITCM ECC: after 2-bit error inject, mcer = 0x%llx\n", mcer2_a);
    unsigned long mcause = __get_MCAUSE();
    unsigned long mepc = __get_MEPC();
    if (!(mcause & (1UL << 63)) && (mcause & 0x3FF) == 0x1 && mepc == ITCM_BASE)
    {
        printf("ITCM ECC: 2bit runs successfully!\n");
    }
    else
    {
        printf("ITCM ECC: failed to detect 2-bit error\n");
    }
    return 0;
}

char *g_test_str = "execute in tcm region";
int example_core_tcm_ecc(void)
{
    dtcm_init();
    itcm_init();
    dtcm_ecc();
    itcm_ecc();
    return 0;
}
#endif
