/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     sys_clk.c
 * @brief    source file for setting system frequency.
 * @version  V1.0
 * @date     18. July 2020
 ******************************************************************************/

#include <stdint.h>
#include <soc.h>
#include <sys_clk.h>
#include <drv/io.h>

uint32_t g_system_clock = IHS_VALUE;

uint32_t soc_get_cpu_freq(uint32_t idx)
{
    ///< TODO：返回cpu的频率g_system_clock
    return g_system_clock;
}

uint32_t soc_get_cur_cpu_freq(void)
{
    ///< TODO：返回当前cpu的频率g_system_clock
    return g_system_clock;
}

uint32_t soc_get_coretim_freq(void)
{
    ///< TODO：返回coretime的频率g_system_clock
    return g_system_clock;
}

uint32_t soc_get_uart_freq(uint32_t idx)
{
    ///< TODO：返回uart的频率g_system_clock
    return g_system_clock;
}

csi_error_t soc_sysclk_config(system_clk_config_t *config)
{
    return CSI_OK;
}

void soc_set_sys_freq(uint32_t val)
{
    g_system_clock = val;
}

void soc_clk_enable(int32_t module)
{
}

void soc_clk_disable(int32_t module)
{
}

void soc_reset_uart(uint32_t idx)
{
}

uint32_t soc_get_timer_freq(uint32_t idx)
{
    return g_system_clock;
}

