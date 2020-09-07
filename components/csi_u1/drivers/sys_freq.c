/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */
/******************************************************************************
 * @file     sys_freq.c
 * @brief    source file for setting system frequency.
 * @version  V1.0
 * @date     26. April 2019
 ******************************************************************************/
#include <stdint.h>
#include <soc.h>
#include <io.h>
#include <sys_freq.h>
#include <ck_pmu.h>

extern int g_system_clock;
extern int g_uart_clock;

int32_t drv_get_cpu_freq(int32_t idx)
{
    return g_system_clock;
}

int32_t drv_get_usart_freq(int32_t idx)
{
    return g_uart_clock;
}

int32_t drv_get_sys_freq(void)
{
    return g_system_clock;
}

uint32_t drv_get_cur_cpu_freq(void)
{
    return drv_get_cpu_freq(0);
}
