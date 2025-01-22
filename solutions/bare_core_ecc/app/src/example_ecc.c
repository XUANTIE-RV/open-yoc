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

#if CONFIG_CPU_XUANTIE_C907 || CONFIG_CPU_XUANTIE_C907FD || CONFIG_CPU_XUANTIE_C907FDV || CONFIG_CPU_XUANTIE_C907FDVM \
    || CONFIG_CPU_XUANTIE_C907_RV32 || CONFIG_CPU_XUANTIE_C907FD_RV32 || CONFIG_CPU_XUANTIE_C907FDV_RV32 || CONFIG_CPU_XUANTIE_C907FDVM_RV32 \
    || CONFIG_CPU_XUANTIE_C908 || CONFIG_CPU_XUANTIE_C908V || CONFIG_CPU_XUANTIE_C908I \
    || CONFIG_CPU_XUANTIE_C910V2 || CONFIG_CPU_XUANTIE_C910V3 || CONFIG_CPU_XUANTIE_C910V3_CP \
    || CONFIG_CPU_XUANTIE_C920V2 || CONFIG_CPU_XUANTIE_C920V3 || CONFIG_CPU_XUANTIE_C920V3_CP \
    || CONFIG_CPU_XUANTIE_R910 || CONFIG_CPU_XUANTIE_R920 \
    || CONFIG_CPU_XUANTIE_R908 || CONFIG_CPU_XUANTIE_R908FD || CONFIG_CPU_XUANTIE_R908FDV \
    || CONFIG_CPU_XUANTIE_R908_CP || CONFIG_CPU_XUANTIE_R908FD_CP || CONFIG_CPU_XUANTIE_R908FDV_CP
#include <csi_config.h>
#include <stdio.h>
#include <string.h>
#include <csi_core.h>
#include <soc.h>
#include "ecc_stats.h"

#define TIMEOUT (0x0000ffffUL)

ecc_stats_t g_ecc_stats = {0, 0, -1, -1};

static volatile long g_ecc_buf[4 * 1024] __attribute__((aligned(64)));
#if __riscv_xlen == 32
int example_core_ecc(void)
{
    volatile long sum = 0;
    unsigned long mcer_b, mcer_a;
    unsigned long mcerh_b, mcerh_a;
    unsigned long mcer2_b, mcer2_a;
    unsigned long mcer2h_b, mcer2h_a;

    mcerh_b = __get_MCERH();
    mcer_b  = __get_MCER();
    printf("Before 1-bit error inject to L1 Cache, mcerh = 0x%lx, mcer = 0x%lx.\n", mcerh_b, mcer_b);

    /* make data in cache, and inject error later */
    memset((void*)g_ecc_buf, 0x2, sizeof(g_ecc_buf));

    /* inject 1bit error for l1 data ram */
    __set_MEICR(0x20000001);

    /* trigger reading from cache */
    for (int i = 0; i < sizeof(g_ecc_buf) / sizeof(long); i++)
    {
        sum += g_ecc_buf[i];
    }

    mcerh_a = __get_MCERH();
    mcer_a  = __get_MCER();
    printf("After 1-bit error inject to L1 Cache, mcerh = 0x%lx, mcer = 0x%lx.\n", mcerh_a, mcer_a);
    if ((mcer_b != mcer_a) || (mcerh_b != mcerh_a))
        printf("L1 ecc 1-bit error corrected!\n");
    else
        printf("L1 ecc failed to correct 1-bit error! This cpu may not support ecc module.\n");

    mcer2h_b = __get_MCER2H();
    mcer2_b  = __get_MCER2();
    printf("Before 1-bit error inject to L2 Cache, mcer2h = 0x%lx, mcer2 = 0x%lx.\n", mcer2h_b, mcer2_b);
    memset((void*)g_ecc_buf, 0x2, sizeof(g_ecc_buf));
    /* inject 1bit error for l2 data ram */
    __set_MEICR2(0x20000001);
    /* trigger reading from l2-cache and clean to memory */
    csi_dcache_clean_invalid_range((unsigned long*)g_ecc_buf, sizeof(g_ecc_buf));

    mcer2h_a = __get_MCER2H();
    mcer2_a = __get_MCER2();
    printf("After 1-bit error inject to L2 Cache, mcer2h = 0x%lx, mcer2 = 0x%lx.\n", mcer2h_a, mcer2_a);
    if ((mcer2_b != mcer2_a) || (mcer2h_b != mcer2h_a))
        printf("L2 ecc 1-bit error corrected!\n");
    else
        printf("L2 ecc feiled to correct 1-bit error! This cpu may not support ecc module.\n");
    return 0;
}

int example_core_ecc2b(void)
{
    volatile long sum = 0;
    unsigned long mcer_b, mcer_a;
    unsigned long mcerh_b, mcerh_a;
    unsigned long mcer2_b, mcer2_a;
    unsigned long mcer2h_b, mcer2h_a;
    unsigned long timeout = TIMEOUT;

    /* L1 Cache */
    mcerh_b = __get_MCERH();
    mcer_b  = __get_MCER();
    printf("Before 2-bits error inject to L1 Cache, mcerh = 0x%lx, mcer = 0x%lx.\n", mcerh_b, mcer_b);
    /* make data in cache, and inject error later */
    memset((void*)g_ecc_buf, 0x2, sizeof(g_ecc_buf));
    /* inject 2bit error for l1 data ram */
    __set_MEICR(0x20000003);
    /* trigger reading from cache */
    for (int i = 0; i < sizeof(g_ecc_buf) / sizeof(long); i++)
    {
        sum += g_ecc_buf[i];
    }
    while (--timeout)
    {
        if (g_ecc_stats.mcer_fault_ram == MCER_FAULT_RAMID_L1_ICACHE_TAG
          || g_ecc_stats.mcer_fault_ram == MCER_FAULT_RAMID_L1_ICACHE_DATA
          || g_ecc_stats.mcer_fault_ram == MCER_FAULT_RAMID_L1_DCACHE_TAG
          || g_ecc_stats.mcer_fault_ram == MCER_FAULT_RAMID_L1_DCACHE_DATA)
        {
            break;
        }
    }
    mcerh_a = __get_MCERH();
    mcer_a = __get_MCER();
    printf("After 2-bits error inject to L1 Cache, mcerh = 0x%lx, mcer = 0x%lx.\n", mcerh_a, mcer_a);
    if (timeout > 0)
    {
        printf("L1 ecc 2-bits error detected!\n");
    }
    else
    {
        printf("L1 ecc failed to detect 2-bits error! This cpu may not support ecc module.\n");
    }

    /* L2 Cache */
    timeout = TIMEOUT;

    mcer2h_b = __get_MCER2H();
    mcer2_b  = __get_MCER2();
    printf("Before 2-bits error inject to L2 Cache, mcer2h = 0x%lx, mcer2 = 0x%lx.\n", mcer2h_b, mcer2_b);
    memset((void*)g_ecc_buf, 0x2, sizeof(g_ecc_buf));
    /* inject 2bit error for l2 data ram */
    __set_MEICR2(0x20000003);
    /* trigger reading from l2-cache and clean to memory */
    csi_dcache_clean_invalid_range((unsigned long*)g_ecc_buf, sizeof(g_ecc_buf));
    while (--timeout)
    {
        if(g_ecc_stats.mcer2_fault_ram == MCER2_FAULT_RAMID_L2_CACHE_TAG
          || g_ecc_stats.mcer2_fault_ram == MCER2_FAULT_RAMID_L2_CACHE_DATA
          || g_ecc_stats.mcer2_fault_ram == MCER2_FAULT_RAMID_L2_CACHE_DIRTY)
        {
            break;
        }
    }

    mcer2h_a = __get_MCER2H();
    mcer2_a = __get_MCER2();
    printf("After 2-bits error inject to L2 Cache, mcer2h = 0x%lx, mcer2 = 0x%lx.\n", mcer2h_a, mcer2_a);
    if(timeout > 0)
    {
        printf("L2 ecc 2-bits error detected!\n");
    }
    else
    {
        printf("L2 ecc failed to detect 2-bits error! This cpu may not support ecc module.\n");
    }
    return 0;
}
#else
int example_core_ecc(void)
{
    volatile int sum = 0;
    unsigned long long mcer_b, mcer_a;
    unsigned long long  mcer2_b, mcer2_a;
    mcer_b  = __get_MCER();
    printf("Before 1-bit error inject to L1 Cache, mcer = 0x%llx.\n", mcer_b);
    /* make data in cache, and inject error later */
    memset((void*)g_ecc_buf, 0x2, sizeof(g_ecc_buf));
    /* inject 1bit error for l1 data ram */
    __set_MEICR(0x60000001);
    /* trigger reading from cache */
    for (int i = 0; i < sizeof(g_ecc_buf) / sizeof(long); i++)
    {
        sum += g_ecc_buf[i];
    }

    mcer_a  = __get_MCER();
    printf("After 1-bit error inject to L1 Cache, mcer = 0x%llx.\n", mcer_a);
    if (mcer_b != mcer_a)
        printf("L1 ecc 1-bit error corrected!\n");
    else
        printf("L1 ecc failed to correct 1-bit eror! This cpu may not support ecc module.\n");

    mcer2_b  = __get_MCER2();
    printf("Before 1-bit error inject to L2 Cache, mcer2 = 0x%llx.\n", mcer2_b);
    memset((void*)g_ecc_buf, 0x2, sizeof(g_ecc_buf));
    /* inject 1bit error for l2 data ram */
    __set_MEICR2(0x20000001);
    /* trigger reading from l2-cache and clean to memory */
    csi_dcache_clean_invalid_range((unsigned long*)g_ecc_buf, sizeof(g_ecc_buf));

    mcer2_a = __get_MCER2();
    printf("After 1-bit error inject to L2 Cache, mcer2 = 0x%llx.\n", mcer2_a);
    if (mcer2_b != mcer2_a)
        printf("L2 ecc 1-bit error corrected!\n");
    else
        printf("L2 ecc failed to correct 1-bit error! This cpu may not support ecc module.\n");
    return 0;
}

int example_core_ecc2b(void)
{
    volatile long sum = 0;
    unsigned long timeout = TIMEOUT;

    g_ecc_stats.err_cnt_l1 = 0;
    g_ecc_stats.err_cnt_l2 = 0;
    g_ecc_stats.mcer_fault_ram = -1;
    g_ecc_stats.mcer2_fault_ram = -1;

    // clear
    __set_MCER(0);
    /* L1 Cache */
    /* make data in cache, and inject error later */
    memset((void*)g_ecc_buf, 0x2, sizeof(g_ecc_buf));
    /* inject 2bit error for l1 data ram */
    __set_MEICR(0x60000003);
    /* trigger reading from cache */
    for (int i = 0; i < sizeof(g_ecc_buf) / sizeof(long); i++)
    {
        sum += g_ecc_buf[i];
    }
    while(--timeout)
    {
        if(g_ecc_stats.mcer_fault_ram == MCER_FAULT_RAMID_L1_ICACHE_TAG
          || g_ecc_stats.mcer_fault_ram == MCER_FAULT_RAMID_L1_ICACHE_DATA
          || g_ecc_stats.mcer_fault_ram == MCER_FAULT_RAMID_L1_DCACHE_TAG
          || g_ecc_stats.mcer_fault_ram == MCER_FAULT_RAMID_L1_DCACHE_DATA)
        {
            break;
        }
    }
    if(timeout > 0)
    {
        printf("L1 ecc 2-bits error detected!\n");
    }
    else
    {
        printf("L1 ecc failed to detect 2-bits error! This cpu may not support ecc module.\n");
    }

    /* L2 Cache */
    __set_MCER2(0);
    timeout = TIMEOUT;
    memset((void*)g_ecc_buf, 0x2, sizeof(g_ecc_buf));

    /* inject 2bit error for l2 data ram */
    __set_MEICR2(0x20000003);

    /* trigger reading from l2-cache and clean to memory */
    csi_dcache_clean_invalid_range((unsigned long*)g_ecc_buf, sizeof(g_ecc_buf));
    while(--timeout)
    {
        if(g_ecc_stats.mcer2_fault_ram == MCER2_FAULT_RAMID_L2_CACHE_TAG
          || g_ecc_stats.mcer2_fault_ram == MCER2_FAULT_RAMID_L2_CACHE_DATA
          || g_ecc_stats.mcer2_fault_ram == MCER2_FAULT_RAMID_L2_CACHE_DIRTY)
        {
            break;
        }
    }
    if(timeout > 0)
    {
        printf("L2 ecc 2-bits error detected!\n");
    }
    else
    {
        printf("L2 ecc failed to detect 2-bits error! this cpu may not support ecc module.\n");
    }
    return 0;
}
#endif

#endif

