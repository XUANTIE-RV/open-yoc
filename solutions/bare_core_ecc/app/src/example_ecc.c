/*
 * Copyright (C) 2017-2024 Alibaba Group Holding Limited
 */

#if CONFIG_CPU_C907 || CONFIG_CPU_C907FD || CONFIG_CPU_C907FDV || CONFIG_CPU_C907FDVM \
    || CONFIG_CPU_C907_RV32 || CONFIG_CPU_C907FD_RV32 || CONFIG_CPU_C907FDV_RV32 || CONFIG_CPU_C907FDVM_RV32 \
    || CONFIG_CPU_C908 || CONFIG_CPU_C908V || CONFIG_CPU_C908I \
    || CONFIG_CPU_C910 || CONFIG_CPU_C920 || CONFIG_CPU_C910V2 || CONFIG_CPU_C920V2 \
    || CONFIG_CPU_R910 || CONFIG_CPU_R920
#include <csi_config.h>
#include <stdio.h>
#include <string.h>
#include <csi_core.h>

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
    printf("before L1 cache ecc inject, mcerh = 0x%lx, mcer = 0x%lx\n", mcerh_b, mcer_b);
    /* make data in cache, and inject error later */
    memset((void*)g_ecc_buf, 0x2, sizeof(g_ecc_buf));
    /* inject 1bit error for l1 data ram */
    __set_MEICR(0x20000001);
    /* trigger reading from cache */
    for (int i = 0; i < sizeof(g_ecc_buf) / sizeof(long); i++) {
        sum += g_ecc_buf[i];
    }

    mcerh_a = __get_MCERH();
    mcer_a  = __get_MCER();
    printf("after L1 cache ecc inject, mcerh = 0x%lx, mcer = 0x%lx\n", mcerh_a, mcer_a);
    if ((mcer_b != mcer_a) || (mcerh_b != mcerh_a))
        printf("l1 ecc runs successfully!\n");
    else
        printf("l1 ecc runs fail! this cpu may not support ecc module\n");

    mcer2h_b = __get_MCER2H();
    mcer2_b  = __get_MCER2();
    printf("before L2 cache ecc inject, mcer2h = 0x%lx, mcer2 = 0x%lx\n", mcer2h_b, mcer2_b);
    memset((void*)g_ecc_buf, 0x2, sizeof(g_ecc_buf));
    /* inject 1bit error for l2 data ram */
    __set_MEICR2(0x20000001);
    /* trigger reading from l2-cache and clean to memory */
    csi_dcache_clean_invalid_range((unsigned long*)g_ecc_buf, sizeof(g_ecc_buf));

    mcer2h_a = __get_MCER2H();
    mcer2_a = __get_MCER2();
    printf("after L2 cache ecc inject, mcer2h = 0x%lx, mcer2 = 0x%lx\n", mcer2h_a, mcer2_a);
    if ((mcer2_b != mcer2_a) || (mcer2h_b != mcer2h_a))
        printf("l2 ecc runs successfully!\n");
    else
        printf("l2 ecc runs fail! this cpu may not support ecc module\n");

    return 0;
}
#else
int example_core_ecc(void)
{
    volatile long sum = 0;
    unsigned long mcer_b, mcer_a;
    unsigned long mcer2_b, mcer2_a;

    mcer_b = __get_MCER();
    printf("before L1 cache ecc inject, mcer = 0x%lx\n", mcer_b);
    /* make data in cache, and inject error later */
    memset((void*)g_ecc_buf, 0x2, sizeof(g_ecc_buf));
    /* inject 1bit error for l1 data ram */
    __set_MEICR(0x20000001);
    /* trigger reading from cache */
    for (int i = 0; i < sizeof(g_ecc_buf) / sizeof(long); i++) {
        sum += g_ecc_buf[i];
    }

    mcer_a = __get_MCER();
    printf("after L1 cache ecc inject, mcer = 0x%lx\n", mcer_a);
    if (mcer_b != mcer_a)
        printf("l1 ecc runs successfully!\n");
    else
        printf("l1 ecc runs fail! this cpu may not support ecc module\n");

    mcer2_b = __get_MCER2();
    printf("before L2 cache ecc inject, mcer2 = 0x%lx\n", mcer2_b);
    memset((void*)g_ecc_buf, 0x2, sizeof(g_ecc_buf));
    /* inject 1bit error for l2 data ram */
    __set_MEICR2(0x20000001);
    /* trigger reading from l2-cache and clean to memory */
    csi_dcache_clean_invalid_range((unsigned long*)g_ecc_buf, sizeof(g_ecc_buf));

    mcer2_a = __get_MCER2();
    printf("after L2 cache ecc inject, mcer2 = 0x%lx\n", mcer2_a);
    if (mcer2_b != mcer2_a)
        printf("l2 ecc runs successfully!\n");
    else
        printf("l2 ecc runs fail! this cpu may not support ecc module\n");

    return 0;
}
#endif

#endif

