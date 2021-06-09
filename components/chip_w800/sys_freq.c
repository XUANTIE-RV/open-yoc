/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
/******************************************************************************
 * @file     sys_freq.c
 * @brief    source file for setting system frequency.
 * @version  V1.0
 * @date     26. April 2019
 ******************************************************************************/
#include <stdint.h>
#include <soc.h>
#ifdef CONFIG_TEE_CA
#include <drv/tee.h>
#endif
#include <io.h>
#include <sys_freq.h>

#include "wm_type_def.h"
#include "wm_cpu.h"
#include "wm_osal.h"

int32_t drv_get_cpu_id(void)
{
    return 0;
}

int32_t drv_get_sys_freq(void)
{
    return drv_get_cpu_freq(drv_get_cpu_id());//80000000;
}

int32_t drv_get_apb_freq(int32_t idx)
{
    tls_sys_clk sysclk;
    tls_sys_clk_get(&sysclk);
    return sysclk.apbclk * UNIT_MHZ;
}

int32_t drv_get_cpu_freq(int32_t idx)
{
    tls_sys_clk sysclk;
    tls_sys_clk_get(&sysclk);
    return sysclk.cpuclk * UNIT_MHZ;
}

uint32_t drv_get_cur_cpu_freq(void)
{
    return drv_get_cpu_freq(drv_get_cpu_id());
}