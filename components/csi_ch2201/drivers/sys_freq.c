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
#include <ck_pmu.h>

/* FIXME */
#define CK_EFLASH_TRC     0x4003f020
#define CK_EFLASH_TNVS    0x4003f024
#define CK_EFLASH_TPGS    0x4003f028
#define CK_EFLASH_TPROG   0x4003f02c
#define CK_EFLASH_TRCV    0x4003f030
#define CK_EFLASH_TERASE  0x4003f034

#define RTC_CLK_SOURCE    0x40002000
#define PMU_CSSCR         0x40002024

typedef struct {
    uint8_t trc;
    uint16_t tnvs;
    uint8_t tpgs;
    uint16_t tprog;
    uint16_t trcv_erase;
} eflash_opt_time_t;

extern int g_system_clock;

#ifndef CONFIG_TEE_CA
static const eflash_opt_time_t eflash_opt_time[] = {
    {0x0, 0x35, 0x16, 0x35, 0x1b9},
    {0x0, 0x6a, 0x2d, 0x6b, 0x371},
    {0x0, 0x9f, 0x44, 0xa1, 0x528},
    {0x1, 0xd4, 0x5a, 0xd7, 0x6e1},
    {0x1, 0x109, 0x71, 0x10c, 0x89b},
    {0x1, 0x13e, 0x88, 0x141, 0xa56},
};
#endif

void drv_set_sys_freq(clk_src_e source, clk_val_e val)
{
#ifdef CONFIG_TEE_CA
    csi_tee_set_sys_freq(source, val);
#else
    /* calculate the pllout frequence */
    uint8_t osr_freq = val >> 16;
    uint8_t pllm = val & 0x3f;
    uint8_t plln = (val >> 8) & 0x3f;
    uint8_t pllout = osr_freq * (pllm / plln);

    if (osr_freq != (EHS_VALUE / 1000000) || pllout < 16 || pllout >= 60) {
        return;
    }

    g_system_clock = pllout * 1000000;

    /* set eflash control operation time */
    uint8_t index = ((pllout + 7) / 8) - 1;

    if (index >= sizeof(eflash_opt_time) / sizeof(eflash_opt_time_t)) {
        return;
    }

    uint32_t flag = csi_irq_save();

    eflash_opt_time_t *opt_time = (eflash_opt_time_t *)&eflash_opt_time[index];

    putreg32(opt_time->trc, (uint32_t *)CK_EFLASH_TRC);
    putreg32(opt_time->tnvs, (uint32_t *)CK_EFLASH_TNVS);
    putreg32(opt_time->tpgs, (uint32_t *)CK_EFLASH_TPGS);
    putreg32(opt_time->tprog, (uint32_t *)CK_EFLASH_TPROG);
    putreg32(opt_time->trcv_erase, (uint32_t *)CK_EFLASH_TRCV);

    val = val & 0xFFF;
    int timeout = 10000;

    /* config pll and wait until stable */

    val |= (3 << 18);

    if (source == IHS_CLK) {
        val &= 0xFFF7FFFF;
    }

    putreg32(val, (uint32_t *)PMU_PLL_CTRL);

    while (timeout--) {
        if (getreg32((uint32_t *)PMU_CLK_STABLE) & 0x00000010) {
            break;
        }
    }

    putreg32(MCLK_REG_VAL, (uint32_t *)PMU_MCLK_SEL);
    csi_irq_restore(flag);
#endif
}

void drv_rtcclk_config(clk_src_e source)
{
#ifdef CONFIG_TEE_CA
    uint32_t val;
    csi_tee_read_reg((uint32_t)RTC_CLK_SOURCE, &val);
    val |= 0x1;
    csi_tee_write_reg((uint32_t)RTC_CLK_SOURCE, val);
#else
    ck_pmu_reg_t *pmu = (ck_pmu_reg_t *)CSKY_PMU_BASE;

    if (source == ELS_CLK) {
        pmu->LPCR |= LPCR_RTC_CLOCK_SEL_Msk;
    } else if (source == ILS_CLK) {
        pmu->LPCR &= ~LPCR_RTC_CLOCK_SEL_Msk;
    }

#endif
    return;
}

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
#ifdef CONFIG_TEE_CA
    uint32_t val;
    csi_tee_get_sys_freq(&val);
    // FIXME:
    return g_system_clock;
#else
    return g_system_clock;
#endif
}

int32_t drv_get_apb_freq(int32_t idx)
{
#ifdef CONFIG_TEE_CA
    uint32_t val;
    csi_tee_get_sys_freq(&val);
    return val;
#else
    return g_system_clock;
#endif
}

int32_t drv_get_timer_freq(int32_t idx)
{
#ifdef CONFIG_TEE_CA
    uint32_t val;
    csi_tee_get_sys_freq(&val);
    return val;
#else
    return g_system_clock;
#endif
}

int32_t drv_get_rtc_freq(int32_t idx)
{
    ck_pmu_reg_t *pmu = (ck_pmu_reg_t *)CSKY_PMU_BASE;

    if (pmu->LPCR & LPCR_RTC_CLOCK_SEL_Msk) {
        return ELS_VALUE;
    } else {
        return ILS_VALUE;
    }
}

void drv_get_eflash_params(clk_val_e val, uint16_t *prog, uint16_t *erase)
{
    if (prog == NULL || erase == NULL) {
        return;
    }
    switch (val) {
        case OSR_8M_CLK_16M:
            *prog  = 0x59;
            *erase = 0x371;
            break;

        case OSR_8M_CLK_24M:
            *prog  = 0x84;
            *erase = 0x528;
            break;

        case OSR_8M_CLK_32M:
            *prog  = 0xb1;
            *erase = 0x6e1;
            break;

        case OSR_8M_CLK_40M:
            *prog  = 0xdd;
            *erase = 0x89b;
            break;

        case OSR_8M_CLK_48M:
            *prog  = 0x109;
            *erase = 0xa56;
            break;

        default:
            *prog  = 0x2d;
            *erase = 0x1b9;
            return;
    }

    return;
}

void drv_set_clk_stable_counter(uint32_t count)
{
#ifdef CONFIG_TEE_CA
    csi_tee_write_reg((uint32_t)PMU_CSSCR, count);
#else
    ck_pmu_reg_t *pmu = (ck_pmu_reg_t *)CSKY_PMU_BASE;

    pmu->CSSCR = count;

#endif
}
