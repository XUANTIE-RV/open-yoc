/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
/******************************************************************************
 * @file     sys_freq.c
 * @brief    source file for setting system frequency.
 * @version  V1.0
 * @date     18. July 2018
 ******************************************************************************/

#include <stdint.h>
#include <soc.h>
#include <sys_freq.h>
#include <silan_syscfg.h>

int32_t g_system_clock = SYSTEM_CLOCK;

int32_t drv_get_sys_freq(void)
{
    return g_system_clock;
}

int32_t drv_get_cpu_freq(int idx)
{
    return g_system_clock;
}

int32_t drv_get_timer_freq(void)
{
    return silan_get_timer_cclk();
}

uint32_t drv_get_cur_cpu_freq(void)
{
    return drv_get_cpu_freq(0);
}