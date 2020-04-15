/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
/******************************************************************************
 * @file     sys_freq.c
 * @brief    source file for setting system frequency.
 * @version  V1.0
 * @date     26. April 2019
 ******************************************************************************/
#include <csi_config.h>
#include <stdint.h>
#include <soc.h>
#ifdef CONFIG_TEE_CA
#include <drv/tee.h>
#endif
#include <io.h>
#include <sys_freq.h>

/* FIXME */
#define CK_EFLASH_TRC     0x4003f020
#define CK_EFLASH_TNVS    0x4003f024
#define CK_EFLASH_TPGS    0x4003f028
#define CK_EFLASH_TPROG   0x4003f02c
#define CK_EFLASH_TRCV    0x4003f030
#define CK_EFLASH_TERASE  0x4003f034

#define RTC_CLK_SOURCE    0x40002000
#define PMU_CSSCR         0x40002024


extern int g_system_clock;


int32_t drv_get_cpu_freq(int32_t idx)
{
    return g_system_clock;
}

int32_t drv_get_usi_freq(int32_t idx)
{
    return g_system_clock;
}

int32_t drv_get_usart_freq(int32_t idx)
{
    return g_system_clock;
}

int32_t drv_get_pwm_freq(int32_t idx)
{
    return g_system_clock;
}

int32_t drv_get_i2s_freq(int32_t idx)
{
    return g_system_clock;
}

int32_t drv_get_sys_freq(void)
{
    return g_system_clock;
}

int32_t drv_get_apb_freq(int32_t idx)
{
    return g_system_clock;
}

int32_t drv_get_timer_freq(int32_t idx)
{
    return g_system_clock;
}

void drv_clk_enable(uint32_t module)
{
}

void drv_clk_disable(uint32_t module)
{

}