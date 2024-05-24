/*
 * Copyright (C) 2017-2024 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <csi_core.h>
#include <soc.h>

#if (CONFIG_CPU_E907 || CONFIG_CPU_E907F || CONFIG_CPU_E907FD || CONFIG_CPU_E907P || CONFIG_CPU_E907FP || CONFIG_CPU_E907FDP)
#define ITCM_BASE      0x30000000
#define DTCM_BASE      0x30008000

extern int __erodata;
extern int __data_start__;
extern int __data_end__;
extern int __ram_code_start__;
extern int __ram_code_end__;
extern int __itcm_code_start__;
extern int __itcm_code_end__;
extern int __dtcm_code_start__;
extern int __dtcm_code_end__;

extern void tcm_test();

static void itcm_code_load()
{
    unsigned long size = csi_itcm_get_size();
    unsigned long data_len = (unsigned long)&__data_end__ - (unsigned long)&__data_start__;
    unsigned long ram_code_len = (unsigned long)&__ram_code_end__ - (unsigned long)&__ram_code_start__;
    unsigned long itcm_len = (unsigned long)&__itcm_code_end__ - (unsigned long)&__itcm_code_start__;
    char *itcm_start_lma = (char*)((unsigned long)&__erodata + data_len + ram_code_len);

    //unsigned long len = (unsigned long)&__itcm_code_end__ - (unsigned long)&__itcm_code_start__;
    //printf("%x,%x,%x,%x,%x, %d\r\n", &__erodata, &__data_start__, &__data_end__, &__itcm_code_start__, &__itcm_code_end__, len);

    csi_itcm_set_base_addr(ITCM_BASE);
    csi_itcm_enable();

    memset((void *)(&__itcm_code_start__), 0, size);
    memcpy((void *)(ITCM_BASE), (void *)itcm_start_lma, itcm_len);
}

static void dtcm_code_load()
{
    unsigned long size = csi_dtcm_get_size();
    unsigned long data_len = (unsigned long)&__data_end__ - (unsigned long)&__data_start__;
    unsigned long ram_code_len = (unsigned long)&__ram_code_end__ - (unsigned long)&__ram_code_start__;
    unsigned long itcm_len = (unsigned long)&__itcm_code_end__ - (unsigned long)&__itcm_code_start__;
    unsigned long dtcm_len = (unsigned long)&__dtcm_code_end__ - (unsigned long)&__dtcm_code_start__;
    char *dtcm_start_lma = (char*)((unsigned long)&__erodata + data_len + ram_code_len + itcm_len);

    csi_dtcm_set_base_addr(DTCM_BASE);
    csi_dtcm_enable();

    memset((void *)(&__dtcm_code_start__), 0, size);
    memcpy((void *)(DTCM_BASE), (void *)dtcm_start_lma, dtcm_len);
}

int example_core_tcm()
{
    itcm_code_load();
    dtcm_code_load();

    tcm_test();
    printf("bare_core_tcm runs success!\n");

    return 0;
}
#endif

