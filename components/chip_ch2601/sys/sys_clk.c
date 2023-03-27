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

const system_clk_config_t system_config[] = {
    {CPU_24MHZ,     {1U, EHS_CLK, CLK_DIV1,  PLL_FN_32,    0U}, PLL_CLK, ILS_CLK, 8U, CLK_DIV1, CLK_DIV1},
    {CPU_36MHZ,     {1U, EHS_CLK, CLK_DIV1,  PLL_FN_30,    0U}, PLL_CLK, ILS_CLK, 5U, CLK_DIV1, CLK_DIV1},
    {CPU_48MHZ,     {1U, EHS_CLK, CLK_DIV1,  PLL_FN_32,    0U}, PLL_CLK, ILS_CLK, 4U, CLK_DIV1, CLK_DIV1},
    {CPU_60MHZ,     {1U, EHS_CLK, CLK_DIV1,  PLL_FN_30,    0U}, PLL_CLK, ILS_CLK, 3U, CLK_DIV1, CLK_DIV1},
    {CPU_72MHZ,     {1U, EHS_CLK, CLK_DIV1,  PLL_FN_36,    0U}, PLL_CLK, ILS_CLK, 3U, CLK_DIV2, CLK_DIV2},
    {CPU_84MHZ,     {1U, EHS_CLK, CLK_DIV1,  PLL_FN_28,    0U}, PLL_CLK, ILS_CLK, 2U, CLK_DIV2, CLK_DIV2},
    {CPU_96MHZ,     {1U, EHS_CLK, CLK_DIV1,  PLL_FN_32,    0U}, PLL_CLK, ILS_CLK, 2U, CLK_DIV2, CLK_DIV2},
    {CPU_108MHZ,    {1U, EHS_CLK, CLK_DIV1,  PLL_FN_36,    0U}, PLL_CLK, ILS_CLK, 2U, CLK_DIV2, CLK_DIV2},
    {CPU_120MHZ,    {1U, EHS_CLK, CLK_DIV1,  PLL_FN_40,    0U}, PLL_CLK, ILS_CLK, 2U, CLK_DIV2, CLK_DIV2},
    {CPU_132MHZ,    {1U, EHS_CLK, CLK_DIV1,  PLL_FN_44,    0U}, PLL_CLK, ILS_CLK, 2U, CLK_DIV2, CLK_DIV2},
    {CPU_135MHZ,    {1U, EHS_CLK, CLK_DIV1,  PLL_FN_45,    0U}, PLL_CLK, ILS_CLK, 2U, CLK_DIV2, CLK_DIV2},
    {CPU_144MHZ,    {1U, EHS_CLK, CLK_DIV1,  PLL_FN_48,    0U}, PLL_CLK, ILS_CLK, 2U, CLK_DIV4, CLK_DIV4},
    {CPU_156MHZ,    {1U, EHS_CLK, CLK_DIV1,  PLL_FN_26,    0U}, PLL_CLK, ILS_CLK, 1U, CLK_DIV4, CLK_DIV4},
    {CPU_168MHZ,    {1U, EHS_CLK, CLK_DIV1,  PLL_FN_28,    0U}, PLL_CLK, ILS_CLK, 1U, CLK_DIV4, CLK_DIV4},
    {CPU_180MHZ,    {1U, EHS_CLK, CLK_DIV1,  PLL_FN_30,    0U}, PLL_CLK, ILS_CLK, 1U, CLK_DIV4, CLK_DIV4},
    {CPU_192MHZ,    {1U, EHS_CLK, CLK_DIV1,  PLL_FN_32,    0U}, PLL_CLK, ILS_CLK, 1U, CLK_DIV4, CLK_DIV4},
    {CPU_196_608MHZ, {1U, EHS_CLK, CLK_DIV1,  PLL_FN_32, 3145U}, PLL_CLK, ILS_CLK, 1U, CLK_DIV4, CLK_DIV4},
    {CPU_204MHZ,    {1U, EHS_CLK, CLK_DIV1,  PLL_FN_34,    0U}, PLL_CLK, ILS_CLK, 1U, CLK_DIV4, CLK_DIV4},
    {CPU_216MHZ,    {1U, EHS_CLK, CLK_DIV1,  PLL_FN_36,    0U}, PLL_CLK, ILS_CLK, 1U, CLK_DIV4, CLK_DIV4},
    {CPU_228MHZ,    {1U, EHS_CLK, CLK_DIV1,  PLL_FN_38,    0U}, PLL_CLK, ILS_CLK, 1U, CLK_DIV4, CLK_DIV4},
    {CPU_240MHZ,    {1U, EHS_CLK, CLK_DIV1,  PLL_FN_40,    0U}, PLL_CLK, ILS_CLK, 1U, CLK_DIV4, CLK_DIV4},
    {CPU_245_76MHZ, {1U, EHS_CLK, CLK_DIV1,  PLL_FN_40, 3932U}, PLL_CLK, ILS_CLK, 1U, CLK_DIV4, CLK_DIV4},
    {CPU_252MHZ,    {1U, EHS_CLK, CLK_DIV1,  PLL_FN_42,    0U}, PLL_CLK, ILS_CLK, 1U, CLK_DIV4, CLK_DIV4},
    {CPU_264MHZ,    {1U, EHS_CLK, CLK_DIV1,  PLL_FN_44,    0U}, PLL_CLK, ILS_CLK, 1U, CLK_DIV4, CLK_DIV4},
    {CPU_270MHZ,    {1U, EHS_CLK, CLK_DIV1,  PLL_FN_45,    0U}, PLL_CLK, ILS_CLK, 1U, CLK_DIV8, CLK_DIV8},
    {CPU_276MHZ,    {1U, EHS_CLK, CLK_DIV1,  PLL_FN_46,    0U}, PLL_CLK, ILS_CLK, 1U, CLK_DIV8, CLK_DIV8},
    {CPU_288MHZ,    {1U, EHS_CLK, CLK_DIV1,  PLL_FN_48,    0U}, PLL_CLK, ILS_CLK, 1U, CLK_DIV8, CLK_DIV8},
    {CPU_300MHZ,    {1U, EHS_CLK, CLK_DIV1,  PLL_FN_49, 4055U}, PLL_CLK, ILS_CLK, 1U, CLK_DIV8, CLK_DIV8}
};

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
    return soc_get_apb_freq(idx);
}

uint32_t soc_get_iic_freq(uint32_t idx)
{
    return soc_get_apb_freq(idx);
}

uint32_t soc_get_spi_freq(uint32_t idx)
{
    return soc_get_apb_freq(idx);
}

uint32_t soc_get_qspi_freq(uint32_t idx)
{
    uint32_t div;
    div = wj_get_mclk_div(PMU_REG_BASE);
    return (uint32_t)(g_system_clock * div);
}

uint32_t soc_get_adc_freq(uint32_t idx)
{
    return soc_get_apb_freq(1U);
}

uint32_t soc_get_pwm_freq(uint32_t idx)
{
    return soc_get_apb_freq(0U);
}

uint32_t soc_get_wdt_freq(uint32_t idx)
{
    return soc_get_apb_freq(0U);
}

uint32_t soc_get_i2s_freq(uint32_t idx)
{
    uint32_t div;
    div = wj_get_mclk_div(PMU_REG_BASE);
    return (uint32_t)(g_system_clock * div);
}

uint32_t soc_get_timer_freq(uint32_t idx)
{
    return soc_get_apb_freq(idx / 2U);
}

uint32_t soc_get_rtc_freq(uint32_t idx)
{
    return (uint32_t)ILS_VALUE;
}

uint32_t soc_get_apb_freq(uint32_t idx)
{
    uint32_t div;

    if (idx == 0U) {
        div = wj_get_apb0_div(PMU_REG_BASE);
    } else {
        div = wj_get_apb1_div(PMU_REG_BASE);
    }

    return (uint32_t)(g_system_clock / div);
}

uint32_t soc_get_ahb_freq(uint32_t idx)
{
    return 0;
}

uint32_t soc_get_sys_freq(void)
{
    return 0;
}

uint32_t soc_get_usi_freq(uint32_t idx)
{
    return 0;
}

uint32_t soc_get_sdio_freq(uint32_t idx)
{
    return 0;
}

uint32_t soc_get_emmc_freq(uint32_t idx)
{
    return 0;
}

uint32_t soc_get_usb_freq(uint32_t idx)
{
    return 0;
}

uint32_t soc_get_ref_clk_freq(uint32_t idx)
{
    return 0;
}

void soc_clk_init(void)
{
    return;
}

csi_error_t soc_sysclk_config(system_clk_config_t *config)
{
    /* config pll */
    if (config->pll_config.pll_is_used == 1U) {
        if (config->pll_config.pll_source == EHS_CLK) {
            /* set pll config */
            wj_set_pll_clk_src_ehs(PMU_REG_BASE);
            wj_set_pll_clkin_div(PMU_REG_BASE, config->pll_config.pll_src_clk_divider);
            wj_set_fn_ration(PMU_REG_BASE, config->pll_config.fn);
            wj_set_fd_ration(PMU_REG_BASE, config->pll_config.frac);
            /* open ehs pll */
            wj_set_ehs_input_enable(PMU_REG_BASE);
            wj_set_pll_output_enable(PMU_REG_BASE);
            /* enable mode change */
            wj_set_clk_change_enable(PMU_REG_BASE);

            while (wj_get_clk_change_state(PMU_REG_BASE));

            /* wait ehs pll clock stable */
            while (!wj_get_ehs_clock_state(PMU_REG_BASE));

            while (!wj_get_pll_clock_state(PMU_REG_BASE));

        } else if (config->pll_config.pll_source == IHS_CLK) {
            /* set pll config */
            wj_set_pll_clk_src_ihs(PMU_REG_BASE);
            wj_set_pll_clkin_div(PMU_REG_BASE, config->pll_config.pll_src_clk_divider);
            wj_set_fn_ration(PMU_REG_BASE, config->pll_config.fn);
            wj_set_fd_ration(PMU_REG_BASE, config->pll_config.frac);
            /* open ihs pll */
            wj_set_ihs_rc_control_enable(PMU_REG_BASE);
            wj_set_pll_output_enable(PMU_REG_BASE);
            /* enable mode change */
            wj_set_clk_change_enable(PMU_REG_BASE);

            while (wj_get_clk_change_state(PMU_REG_BASE));

            /* wait ihs pll clock stable*/
            while (!wj_get_ihs_clock_state(PMU_REG_BASE));

            while (!wj_get_pll_clock_state(PMU_REG_BASE));

        }
    }

    /* config sys_clk */
    if (config->sys_clk_source == PLL_CLK) {
        /* select pll to sys_clk */
        wj_set_clock1_pll(PMU_REG_BASE);
        wj_set_sysclk_select_clock1(PMU_REG_BASE);
        /* enable mode change */
        wj_set_clk_change_enable(PMU_REG_BASE);

        while (wj_get_clk_change_state(PMU_REG_BASE));
    } else if (config->sys_clk_source == EHS_CLK) {
        /* select ehs to sys_clk */
        wj_set_ehs_input_enable(PMU_REG_BASE);
        wj_set_clock1_ehs(PMU_REG_BASE);
        wj_set_sysclk_select_clock1(PMU_REG_BASE);
        /* enable mode change */
        wj_set_clk_change_enable(PMU_REG_BASE);

        while (wj_get_clk_change_state(PMU_REG_BASE));

        while (!wj_get_ehs_clock_state(PMU_REG_BASE));
    } else if (config->sys_clk_source == IHS_CLK) {
        /* select ihs to sys_clk */
        wj_set_ihs_rc_control_enable(PMU_REG_BASE);
        wj_set_clock0_ihs(PMU_REG_BASE);
        wj_set_sysclk_select_clock0(PMU_REG_BASE);
        /* enable mode change */
        wj_set_clk_change_enable(PMU_REG_BASE);

        while (wj_get_clk_change_state(PMU_REG_BASE));

        while (!wj_get_ihs_clock_state(PMU_REG_BASE));
    }

    /* config rtc_clk */
    if (config->rtc_clk_source == ILS_CLK) {
        /* select ils to rtc_clk */
        wj_set_els_output_disable(PMU_REG_BASE);    ///< disable els
        wj_set_els_input_disable(PMU_REG_BASE);
        wj_set_ils_rc_control_enable(PMU_REG_BASE);
        wj_set_ls_ils(PMU_REG_BASE);
        /* enable mode change */
        wj_set_clk_change_enable(PMU_REG_BASE);

        while (wj_get_clk_change_state(PMU_REG_BASE));

        while (!wj_get_ils_clock_state(PMU_REG_BASE));
    } else if (config->rtc_clk_source == (uint32_t)ELS_CLK) {
        /* select els to rtc_clk */
        wj_set_els_output_enable(PMU_REG_BASE);
        wj_set_els_input_enable(PMU_REG_BASE);
        wj_set_ls_els(PMU_REG_BASE);
        /* enable mode change */
        wj_set_clk_change_enable(PMU_REG_BASE);

        while (wj_get_clk_change_state(PMU_REG_BASE));

        while (!wj_get_els_clock_state(PMU_REG_BASE));
    }

    wj_set_mclk_div(PMU_REG_BASE, (uint32_t)config->mclk_divider);

    wj_set_mclk_dfcc_enable(PMU_REG_BASE);

    while (wj_get_mclk_dfcc_state(PMU_REG_BASE));

    wj_set_apb0_div(PMU_REG_BASE, (uint32_t)config->apb0_clk_divider);
    wj_set_apb1_div(PMU_REG_BASE, (uint32_t)config->apb1_clk_divider);

    wj_set_dfcc_enable(PMU_REG_BASE);

    while (wj_get_dfcc_state(PMU_REG_BASE));

    wj_pmu_i2s4_clock_on(PMU_REG_BASE);
    wj_codec_power_on(PMU_REG_BASE);

    return CSI_OK;
}

void soc_set_sys_freq(uint32_t val)
{
    system_clk_config_t config;
    uint16_t i;

    for (i = 0U; i < (sizeof(system_config) / sizeof(system_clk_config_t)); i++) {
        if (system_config[i].system_clk == val) {
            break;
        }
    }

    if (i >= (sizeof(system_config) / sizeof(system_clk_config_t))) {
        i = 10U; ///< can not find user set system clk freq use 135MHZ
    }

    config.pll_config.pll_is_used = system_config[i].pll_config.pll_is_used;
    config.pll_config.pll_source = system_config[i].pll_config.pll_source;
    config.pll_config.pll_src_clk_divider = system_config[i].pll_config.pll_src_clk_divider;
    config.pll_config.fn = system_config[i].pll_config.fn;
    config.pll_config.frac = system_config[i].pll_config.frac;
    config.sys_clk_source = system_config[i].sys_clk_source;
    config.rtc_clk_source = system_config[i].rtc_clk_source;
    config.mclk_divider   = system_config[i].mclk_divider;
    config.apb0_clk_divider = system_config[i].apb0_clk_divider;
    config.apb1_clk_divider = system_config[i].apb1_clk_divider;

    soc_sysclk_config(&config);
    g_system_clock = system_config[i].system_clk;
}

void soc_clk_enable(int32_t module)
{
    if ((uint32_t)module < 32U) {
        wj_set_apb0_clock_gate_on(PMU_REG_BASE, (uint32_t)module);
    } else {
        wj_set_apb1_clock_gate_on(PMU_REG_BASE, ((uint32_t)module - 32U));
    }
}

void soc_clk_disable(int32_t module)
{
    if ((uint32_t)module < 32U) {
        wj_set_apb0_clock_gate_off(PMU_REG_BASE, (uint32_t)module);
    } else {
        wj_set_apb1_clock_gate_off(PMU_REG_BASE, ((uint32_t)module - 32U));
    }
}

void soc_reset_iic(uint32_t idx)
{
    if (idx == 0U) {
        mdelay(10);
        wj_set_apb0_peripheral_reset(PMU_REG_BASE, 6U);
        mdelay(10);     ///< wait iic ip Stable
    }
}
