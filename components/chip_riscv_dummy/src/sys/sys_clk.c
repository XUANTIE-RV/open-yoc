/*
 * Copyright (C) 2017-2024 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <soc.h>
#include <sys_clk.h>
#include <drv/io.h>

uint32_t g_system_clock = IHS_VALUE;

#if CONFIG_BOARD_XIAOHUI_EVB
uint32_t soc_get_cpu_freq(uint32_t idx)
{
    return 50*1000000;
}

uint32_t soc_get_coretim_freq(void)
{
    return 25*1000000;
}

uint32_t soc_get_uart_freq(uint32_t idx)
{
    return 36*1000000;
}

uint32_t soc_get_timer_freq(uint32_t idx)
{
    return 25*1000000;
}

#else
uint32_t soc_get_cpu_freq(uint32_t idx)
{
    return g_system_clock;
}

uint32_t soc_get_cur_cpu_freq(void)
{
    return g_system_clock;
}

uint32_t soc_get_coretim_freq(void)
{
    return g_system_clock;
}

uint32_t soc_get_uart_freq(uint32_t idx)
{
    return g_system_clock;
}

csi_error_t soc_sysclk_config(system_clk_config_t *config)
{
    return CSI_OK;
}

void soc_reset_uart(uint32_t idx)
{
}

uint32_t soc_get_timer_freq(uint32_t idx)
{
    return g_system_clock;
}
#endif

void soc_clk_enable(int32_t module)
{
}

void soc_clk_disable(int32_t module)
{
}

void soc_set_sys_freq(uint32_t val)
{
    g_system_clock = val;
}


