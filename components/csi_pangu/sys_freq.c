/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     sys_freq.c
 * @brief    source file for setting system frequency.
 * @version  V1.0
 * @date     18. July 2018
 * @vendor   csky
 * @chip     pangu
 ******************************************************************************/

#include <stdint.h>
#include <soc.h>
#include <sys_freq.h>
#include "yun_pmu.h"

int g_system_clock = SYSTEM_CLOCK;
extern int32_t drv_get_cpu_id(void);

int32_t drv_get_sys_freq(void)
{
    return g_system_clock;
}

int32_t drv_get_rtc_freq(int32_t idx)
{
    return RTC_CLOCK;
}

int32_t drv_get_apb_freq(int32_t idx)
{
    if (idx == 0) {
        return (int32_t)yun_get_apb0_freq();
    } else if (idx == 1) {
        return (int32_t)yun_get_apb1_freq();
    }

    return -1;
}

int32_t drv_get_usi_freq(int32_t idx)
{
    if (idx < 2) {
        return (int32_t)yun_get_apb0_freq();
    } else {
        return (int32_t)yun_get_apb1_freq();
    }
}

int32_t drv_get_usart_freq(int32_t idx)
{
    if (idx < 2) {
        return (int32_t)yun_get_apb0_freq();
    } else {
        return (int32_t)yun_get_apb1_freq();
    }
}

int32_t drv_get_pwm_freq(int32_t idx)
{
    if (idx < 2) {
        return (int32_t)yun_get_apb0_freq();
    } else {
        return (int32_t)yun_get_apb1_freq();
    }
}

int32_t drv_get_i2s_freq(int32_t idx)
{
    return yun_get_audio_freq();
}

int32_t drv_get_timer_freq(int32_t idx)
{
    if (idx < 2) {
        return (int32_t)yun_get_apb0_freq();
    } else if (idx < 4) {
        return (32768);
    } else {
        return (int32_t)yun_get_apb1_freq();
    }
}

int32_t drv_get_cpu_freq(int32_t idx)
{
    return yun_get_cpu_freq(idx);
}

uint32_t drv_get_cur_cpu_freq(void)
{
    return drv_get_cpu_freq(drv_get_cpu_id());
}