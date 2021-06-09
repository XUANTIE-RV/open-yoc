/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     sys_clk.c
 * @brief    source file for setting system frequency.
 * @version  V1.0
 * @date     26. Aug 2020
 * @vendor   csky
 * @chip     pangu
 ******************************************************************************/

#include <stdint.h>
#include <csi_config.h>
#include <soc.h>
#include <sys_clk.h>

#define ATTRIBUTE_DATA __attribute__((section(".tcm0")))

const system_clk_config_t system_config[] = {
    /**
     * default freq is 144MHz
    */
    {
        CPU_144MHZ,    {1U, EHS_CLK,  CLK_DIV1, 48, CLK_DIV1, CLK_DIV1}, \
        CLK_DIV4, CLK_DIV4, CLK_DIV2, CLK_DIV6, \
        PLL_CLK, PLL_CLK, PLL_CLK, PLL_CLK, ILS_CLK, \
        CLK_DIV1, CLK_DIV2, CLK_DIV3, CLK_DIV5
    },
    {
        CPU_33MHZ,    {1U, EHS_CLK,  CLK_DIV1, 44, CLK_DIV1, CLK_DIV1}, \
        CLK_DIV16, CLK_DIV4, CLK_DIV2, CLK_DIV6, \
        PLL_CLK, PLL_CLK, PLL_CLK, PLL_CLK, ILS_CLK, \
        CLK_DIV1, CLK_DIV2, CLK_DIV3, CLK_DIV5
    },
    {
        CPU_40MHZ,    {1U, EHS_CLK,  CLK_DIV1, 50, CLK_DIV1, CLK_DIV1}, \
        CLK_DIV15, CLK_DIV4, CLK_DIV2, CLK_DIV6, \
        PLL_CLK, PLL_CLK, PLL_CLK, PLL_CLK, ILS_CLK, \
        CLK_DIV1, CLK_DIV2, CLK_DIV3, CLK_DIV5
    },
    {
        CPU_50MHZ,    {1U, EHS_CLK,  CLK_DIV1, 50, CLK_DIV1, CLK_DIV1}, \
        CLK_DIV12, CLK_DIV4, CLK_DIV2, CLK_DIV6, \
        PLL_CLK, PLL_CLK, PLL_CLK, PLL_CLK, ILS_CLK, \
        CLK_DIV1, CLK_DIV2, CLK_DIV3, CLK_DIV5
    },
    {
        CPU_60MHZ,    {1U, EHS_CLK,  CLK_DIV1, 50, CLK_DIV1, CLK_DIV1}, \
        CLK_DIV10, CLK_DIV4, CLK_DIV2, CLK_DIV6, \
        PLL_CLK, PLL_CLK, PLL_CLK, PLL_CLK, ILS_CLK, \
        CLK_DIV1, CLK_DIV2, CLK_DIV3, CLK_DIV5
    },
    {
        CPU_75MHZ,    {1U, EHS_CLK,  CLK_DIV1, 50, CLK_DIV1, CLK_DIV1}, \
        CLK_DIV8, CLK_DIV4, CLK_DIV2, CLK_DIV6, \
        PLL_CLK, PLL_CLK, PLL_CLK, PLL_CLK, ILS_CLK, \
        CLK_DIV1, CLK_DIV2, CLK_DIV3, CLK_DIV5
    },
    {
        CPU_90MHZ,    {1U, EHS_CLK,  CLK_DIV1, 60, CLK_DIV1, CLK_DIV1}, \
        CLK_DIV8, CLK_DIV4, CLK_DIV2, CLK_DIV6, \
        PLL_CLK, PLL_CLK, PLL_CLK, PLL_CLK, ILS_CLK, \
        CLK_DIV1, CLK_DIV2, CLK_DIV3, CLK_DIV5
    },
    {
        CPU_100MHZ,    {1U, EHS_CLK,  CLK_DIV1, 50, CLK_DIV1, CLK_DIV1}, \
        CLK_DIV6, CLK_DIV4, CLK_DIV2, CLK_DIV6, \
        PLL_CLK, PLL_CLK, PLL_CLK, PLL_CLK, ILS_CLK, \
        CLK_DIV1, CLK_DIV2, CLK_DIV3, CLK_DIV5
    },
    {
        CPU_120MHZ,    {1U, EHS_CLK,  CLK_DIV1, 50, CLK_DIV1, CLK_DIV1}, \
        CLK_DIV5, CLK_DIV4, CLK_DIV2, CLK_DIV6, \
        PLL_CLK, PLL_CLK, PLL_CLK, PLL_CLK, ILS_CLK, \
        CLK_DIV1, CLK_DIV2, CLK_DIV3, CLK_DIV5
    },
    {
        CPU_132MHZ,    {1U, EHS_CLK,  CLK_DIV1, 44, CLK_DIV1, CLK_DIV1}, \
        CLK_DIV4, CLK_DIV4, CLK_DIV2, CLK_DIV6, \
        PLL_CLK, PLL_CLK, PLL_CLK, PLL_CLK, ILS_CLK, \
        CLK_DIV1, CLK_DIV2, CLK_DIV3, CLK_DIV5
    },
    {
        CPU_138MHZ,    {1U, EHS_CLK,  CLK_DIV1, 46, CLK_DIV1, CLK_DIV1}, \
        CLK_DIV4, CLK_DIV4, CLK_DIV2, CLK_DIV6, \
        PLL_CLK, PLL_CLK, PLL_CLK, PLL_CLK, ILS_CLK, \
        CLK_DIV1, CLK_DIV2, CLK_DIV3, CLK_DIV5
    },
    {
        CPU_150MHZ,    {1U, EHS_CLK,  CLK_DIV1, 50, CLK_DIV1, CLK_DIV1}, \
        CLK_DIV4, CLK_DIV4, CLK_DIV2, CLK_DIV6, \
        PLL_CLK, PLL_CLK, PLL_CLK, PLL_CLK, ILS_CLK, \
        CLK_DIV1, CLK_DIV2, CLK_DIV3, CLK_DIV5
    },
    {
        CPU_156MHZ,    {1U, EHS_CLK,  CLK_DIV1, 52, CLK_DIV1, CLK_DIV1}, \
        CLK_DIV4, CLK_DIV4, CLK_DIV2, CLK_DIV6, \
        PLL_CLK, PLL_CLK, PLL_CLK, PLL_CLK, ILS_CLK, \
        CLK_DIV1, CLK_DIV2, CLK_DIV3, CLK_DIV5
    },
    {
        CPU_162MHZ,    {1U, EHS_CLK,  CLK_DIV1, 54, CLK_DIV1, CLK_DIV1}, \
        CLK_DIV4, CLK_DIV4, CLK_DIV2, CLK_DIV6, \
        PLL_CLK, PLL_CLK, PLL_CLK, PLL_CLK, ILS_CLK, \
        CLK_DIV1, CLK_DIV2, CLK_DIV3, CLK_DIV5
    },
    {
        CPU_168MHZ,    {1U, EHS_CLK,  CLK_DIV1, 56, CLK_DIV1, CLK_DIV1}, \
        CLK_DIV4, CLK_DIV4, CLK_DIV2, CLK_DIV6, \
        PLL_CLK, PLL_CLK, PLL_CLK, PLL_CLK, ILS_CLK, \
        CLK_DIV1, CLK_DIV2, CLK_DIV3, CLK_DIV5
    },
    {
        CPU_174MHZ,    {1U, EHS_CLK,  CLK_DIV1, 58, CLK_DIV1, CLK_DIV1}, \
        CLK_DIV4, CLK_DIV4, CLK_DIV2, CLK_DIV6, \
        PLL_CLK, PLL_CLK, PLL_CLK, PLL_CLK, ILS_CLK, \
        CLK_DIV1, CLK_DIV2, CLK_DIV3, CLK_DIV5
    },
    {
        CPU_180MHZ,    {1U, EHS_CLK,  CLK_DIV1, 60, CLK_DIV1, CLK_DIV1}, \
        CLK_DIV4, CLK_DIV4, CLK_DIV2, CLK_DIV6, \
        PLL_CLK, PLL_CLK, PLL_CLK, PLL_CLK, ILS_CLK, \
        CLK_DIV1, CLK_DIV2, CLK_DIV3, CLK_DIV5
    },
    {
        CPU_186MHZ,    {1U, EHS_CLK,  CLK_DIV1, 62, CLK_DIV1, CLK_DIV1}, \
        CLK_DIV4, CLK_DIV4, CLK_DIV2, CLK_DIV6, \
        PLL_CLK, PLL_CLK, PLL_CLK, PLL_CLK, ILS_CLK, \
        CLK_DIV1, CLK_DIV2, CLK_DIV3, CLK_DIV5
    },
    {
        CPU_192MHZ,    {1U, EHS_CLK,  CLK_DIV1, 64, CLK_DIV1, CLK_DIV1}, \
        CLK_DIV4, CLK_DIV4, CLK_DIV2, CLK_DIV6, \
        PLL_CLK, PLL_CLK, PLL_CLK, PLL_CLK, ILS_CLK, \
        CLK_DIV1, CLK_DIV2, CLK_DIV3, CLK_DIV5
    },
    {
        CPU_198MHZ,    {1U, EHS_CLK,  CLK_DIV1, 66, CLK_DIV1, CLK_DIV1}, \
        CLK_DIV4, CLK_DIV4, CLK_DIV2, CLK_DIV6, \
        PLL_CLK, PLL_CLK, PLL_CLK, PLL_CLK, ILS_CLK, \
        CLK_DIV1, CLK_DIV2, CLK_DIV3, CLK_DIV5
    },
    {
        CPU_204MHZ,    {1U, EHS_CLK,  CLK_DIV1, 68, CLK_DIV1, CLK_DIV1}, \
        CLK_DIV4, CLK_DIV5, CLK_DIV3, CLK_DIV6, \
        PLL_CLK, PLL_CLK, PLL_CLK, PLL_CLK, ILS_CLK, \
        CLK_DIV1, CLK_DIV2, CLK_DIV3, CLK_DIV5
    },
    {
        CPU_220MHZ,    {1U, EHS_CLK,  CLK_DIV1, 55, CLK_DIV1, CLK_DIV1}, \
        CLK_DIV3, CLK_DIV4, CLK_DIV2, CLK_DIV6, \
        PLL_CLK, PLL_CLK, PLL_CLK, PLL_CLK, ILS_CLK, \
        CLK_DIV1, CLK_DIV2, CLK_DIV3, CLK_DIV5
    },
    {
        CPU_240MHZ,    {1U, EHS_CLK,  CLK_DIV1, 60, CLK_DIV1, CLK_DIV1}, \
        CLK_DIV3, CLK_DIV4, CLK_DIV2, CLK_DIV6, \
        PLL_CLK, PLL_CLK, PLL_CLK, PLL_CLK, ILS_CLK, \
        CLK_DIV1, CLK_DIV2, CLK_DIV3, CLK_DIV5
    },
    {
        CPU_256MHZ,    {1U, EHS_CLK,  CLK_DIV1, 64, CLK_DIV1, CLK_DIV1}, \
        CLK_DIV3, CLK_DIV4, CLK_DIV2, CLK_DIV6, \
        PLL_CLK, PLL_CLK, PLL_CLK, PLL_CLK, ILS_CLK, \
        CLK_DIV1, CLK_DIV2, CLK_DIV3, CLK_DIV5
    },
    {
        CPU_272MHZ,    {1U, EHS_CLK,  CLK_DIV1, 68, CLK_DIV1, CLK_DIV1}, \
        CLK_DIV3, CLK_DIV4, CLK_DIV2, CLK_DIV6, \
        PLL_CLK, PLL_CLK, PLL_CLK, PLL_CLK, ILS_CLK, \
        CLK_DIV1, CLK_DIV2, CLK_DIV3, CLK_DIV5
    }
};

void soc_clkgate_config(void)
{
    /**
     * CPR0 CLOCK GATE OPEN
    */
    wj_pmu_write_rtc_pclk_cg_en(PMU_REG_BASE);
    wj_pmu_write_rtc_32k_cg_en(PMU_REG_BASE);
    wj_pmu_write_lptim_pclk_cg_en(PMU_REG_BASE);
    wj_pmu_write_lptim_cnt_cg_en(PMU_REG_BASE);

    /**
     * CPR1 CLOCK GATE OPEN
    */
    wj_pmu_write_fft_cg_en(CPR1_REG_BASE);
    wj_pmu_write_tim1_cg_en(CPR1_REG_BASE);
    wj_pmu_write_ahp0_cg_en(CPR1_REG_BASE);
    wj_pmu_write_gpio0_cg_en(CPR1_REG_BASE);
    wj_pmu_write_qspi_cg_en(CPR1_REG_BASE);
    wj_pmu_write_usi0_cg_en(CPR1_REG_BASE);
    wj_pmu_write_usi1_cg_en(CPR1_REG_BASE);
    wj_pmu_write_uart0_cg_en(CPR1_REG_BASE);
    wj_pmu_write_i2s0_cg_en(CPR1_REG_BASE);
    wj_pmu_write_i2s1_cg_en(CPR1_REG_BASE);
    wj_pmu_write_i2s01_en(CPR1_REG_BASE);
    wj_pmu_write_efuse_cg_en(CPR1_REG_BASE);
    wj_pmu_write_sasc_cg_en(CPR1_REG_BASE);
    wj_pmu_write_tipc_cg_en(CPR1_REG_BASE);
    wj_pmu_write_sdio_cg_en(CPR1_REG_BASE);
    wj_pmu_write_sdmmc_cg_en(CPR1_REG_BASE);
    wj_pmu_write_aes_cg_en(CPR1_REG_BASE);
    wj_pmu_write_rsa_cg_en(CPR1_REG_BASE);
    wj_pmu_write_sha_cg_en(CPR1_REG_BASE);
    wj_pmu_write_trng_cg_en(CPR1_REG_BASE);
    wj_pmu_write_dma0_cg_en(CPR1_REG_BASE);
    wj_pmu_write_wdt_cg_en(CPR1_REG_BASE);
    wj_pmu_write_pwm0_cg_en(CPR1_REG_BASE);
    wj_pmu_write_uart1_cg_en(CPR1_REG_BASE);

    /**
     * CPR2 CLOCK GATE OPEN
    */
    wj_pmu_write_tim2_cg_en(CPR2_REG_BASE);
    wj_pmu_write_tim3_cg_en(CPR2_REG_BASE);
    wj_pmu_write_gpio1_cg_en(CPR2_REG_BASE);
    wj_pmu_write_usi2_cg_en(CPR2_REG_BASE);
    wj_pmu_write_usi3_cg_en(CPR2_REG_BASE);
    wj_pmu_write_uart2_cg_en(CPR2_REG_BASE);
    wj_pmu_write_uart3_cg_en(CPR2_REG_BASE);
    wj_pmu_write_i2s2_cg_en(CPR2_REG_BASE);
    wj_pmu_write_i2s3_cg_en(CPR2_REG_BASE);
    wj_pmu_write_i2s23_en(CPR2_REG_BASE);
    wj_pmu_write_spdif_cg_en(CPR2_REG_BASE);
    wj_pmu_write_ahb1_cg_en(CPR2_REG_BASE);
    wj_pmu_write_tdm_cg_en(CPR2_REG_BASE);
    wj_pmu_write_pdm_cg_en(CPR2_REG_BASE);
    wj_pmu_write_dma1_cg_en(CPR2_REG_BASE);
    wj_pmu_write_pwm1_cg_en(CPR2_REG_BASE);
    wj_pmu_write_codec_cg_en(CPR2_REG_BASE);

    /**
     * USI SOFT RESET
    */
    wj_pmu_write_usi0_rst_dis(CPR1_REG_BASE);
    wj_pmu_write_usi2_rst_dis(CPR2_REG_BASE);
    wj_pmu_write_usi3_rst_dis(CPR2_REG_BASE);
    wj_pmu_write_usi1_rst_dis(CPR1_REG_BASE);
    wj_pmu_write_usi1_rst_en(CPR1_REG_BASE);
    wj_pmu_write_usi2_rst_en(CPR2_REG_BASE);
    wj_pmu_write_usi3_rst_en(CPR2_REG_BASE);
    wj_pmu_write_usi0_rst_en(CPR1_REG_BASE);
}

ATTRIBUTE_DATA void soc_sysclk_plldiv(system_clk_config_t *config)
{
    volatile uint32_t pll_div = 0U;

    pll_div |= 0x1U;
    pll_div |= ((config->sys_d0_clk_divider - 2U) << 4);
    pll_div |= 0x100U;
    pll_div |= ((config->sys_d1_clk_divider - 2U) << 12);
    pll_div |= 0x10000U;
    pll_div |= ((config->sys_d2_clk_divider - 2U) << 20);

    wj_pmu_write_sysclk_pll_div(PMU_REG_BASE, pll_div);

    wj_pmu_write_sdio_div(CPR1_REG_BASE, config->sys_d3_clk_divider);
}

static uint32_t g_boot_state = 0;

uint32_t soc_get_boot_state(void)
{
    return g_boot_state;
}

void soc_set_boot_state(uint32_t val)
{
    g_boot_state = val;
}

csi_error_t soc_sysclk_config(system_clk_config_t *config)
{
    /* config pll */
    if (config->pll_config.pll_is_used == 1U) {

        if (wj_pmu_read_boot_from_lp_sta(PMU_REG_BASE)) {
            soc_set_boot_state(1U);
        } else {
            soc_set_boot_state(0U);
        }

        wj_pmu_write_pll_bypass_dis(PMU_REG_BASE);
        wj_pmu_write_pll_refdiv(PMU_REG_BASE, config->pll_config.pll_ref_div);
        wj_pmu_write_pll_fbdiv(PMU_REG_BASE, config->pll_config.pll_fb_div);
        wj_pmu_write_pll_postdiv1(PMU_REG_BASE, config->pll_config.pll_clk_div1);
        wj_pmu_write_pll_postdiv2(PMU_REG_BASE, config->pll_config.pll_clk_div2);
        wj_pmu_write_sysclk_1_div(CPR1_REG_BASE, config->sys_1_clk_divider);
        wj_pmu_write_sysclk_2_div(CPR1_REG_BASE, config->sys_2_clk_divider);
        wj_pmu_write_apb0_div(CPR1_REG_BASE, config->apb0_clk_divider);
        wj_pmu_write_apb1_div(CPR2_REG_BASE, config->apb1_clk_divider);
        wj_pmu_write_qspi_cnt_end(PMU_REG_BASE, 30U);    ///< !!!must config
        soc_sysclk_plldiv(config);
        wj_pmu_write_sw_trigger_en(PMU_REG_BASE);

        while (0U == wj_pmu_read_pll_lock_sta_sta(PMU_REG_BASE));
    } else {
        wj_pmu_write_pll_bypass_en(PMU_REG_BASE);
    }

    /* config sys 0 clk */
    if (config->sys_0_clk_source == PLL_CLK) {
        wj_pmu_write_sysclk0_sel_d0(CPR1_REG_BASE);
    } else if (config->sys_0_clk_source == IHS_CLK) {
        wj_pmu_write_sysclk0_sel_12m(CPR1_REG_BASE);
    }

    /* config sys 3 clk */
    if (config->sys_3_clk_source == PLL_CLK) {
        wj_pmu_write_sysclk3_sel_d1(CPR2_REG_BASE);
    } else if (config->sys_3_clk_source == IHS_CLK) {
        wj_pmu_write_sysclk3_sel_12m(CPR2_REG_BASE);
    }

    /* config sdio clk */
    if (config->sdio_clk_source == PLL_CLK) {
        wj_pmu_write_sdio_sel_d3(CPR1_REG_BASE);
    } else if (config->sdio_clk_source == IHS_CLK) {
        wj_pmu_write_sdio_sel_12m(CPR1_REG_BASE);
    }

    /* config sdmmc clk */
    if (config->sdmmc_clk_source == PLL_CLK) {
        wj_pmu_write_sdmmc_sel_d3(CPR1_REG_BASE);
    } else if (config->sdmmc_clk_source == IHS_CLK) {
        wj_pmu_write_sdmmc_sel_12m(CPR1_REG_BASE);
    }

    /* config rtc_clk */
    if (config->rtc_clk_source == ILS_CLK) {
        wj_pmu_write_clksrc_sel_ils(PMU_REG_BASE);
    } else if (config->rtc_clk_source == ELS_CLK) {
        wj_pmu_write_clksrc_sel_els(PMU_REG_BASE);
    }

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
        i = 0U;              ///< can not find user set system clk freq use 144MHZ
    }

    config.pll_config.pll_is_used = system_config[i].pll_config.pll_is_used;
    config.pll_config.pll_source = system_config[i].pll_config.pll_source;
    config.pll_config.pll_fb_div = system_config[i].pll_config.pll_fb_div;
    config.pll_config.pll_ref_div = system_config[i].pll_config.pll_ref_div;
    config.pll_config.pll_clk_div1 = system_config[i].pll_config.pll_clk_div1;
    config.pll_config.pll_clk_div2 = system_config[i].pll_config.pll_clk_div2;

    config.sys_d0_clk_divider = system_config[i].sys_d0_clk_divider;
    config.sys_d1_clk_divider = system_config[i].sys_d1_clk_divider;
    config.sys_d2_clk_divider = system_config[i].sys_d2_clk_divider;
    config.sys_d3_clk_divider = system_config[i].sys_d3_clk_divider;

    config.sys_0_clk_source = system_config[i].sys_0_clk_source;
    config.sys_3_clk_source = system_config[i].sys_3_clk_source;
    config.sdio_clk_source = system_config[i].sdio_clk_source;
    config.sdmmc_clk_source = system_config[i].sdmmc_clk_source;
    config.rtc_clk_source = system_config[i].rtc_clk_source;

    config.sys_1_clk_divider = system_config[i].sys_1_clk_divider;
    config.sys_2_clk_divider = system_config[i].sys_2_clk_divider;
    config.apb0_clk_divider = system_config[i].apb0_clk_divider;
    config.apb1_clk_divider = system_config[i].apb1_clk_divider;

    soc_sysclk_config(&config);
    soc_clkgate_config();
}


/**
 * 1-1
 * SYS CLK PLLOUT
*/
static uint32_t soc_get_pll_freq(void)
{
    volatile uint32_t freq = ILS_VALUE;
    volatile uint32_t refdiv = wj_pmu_read_pll_refdiv(PMU_REG_BASE);
    volatile uint32_t fb = wj_pmu_read_pll_fbdiv(PMU_REG_BASE);
    volatile uint32_t div1 = wj_pmu_read_pll_postdiv1(PMU_REG_BASE);
    volatile uint32_t div2 = wj_pmu_read_pll_postdiv2(PMU_REG_BASE);

    if (0U == wj_pmu_read_pll_bypass_sta(PMU_REG_BASE)) {
        freq = (((EHS_VALUE * fb) / refdiv) / div1) / div2;
    }

    return freq;
}
/**
 * 2-1
 * PLL D0 CLK
*/
static uint32_t soc_get_pll_d0(void)
{
    volatile uint32_t div = wj_pmu_read_sysclk_d0_div(PMU_REG_BASE);
    volatile uint32_t freq = ILS_VALUE;

    if (0U == wj_pmu_read_sysclk0_sta(CPR1_REG_BASE)) {
        freq = soc_get_pll_freq() / div;
    }

    return freq;
}
/**
 * 2-2
 * PLL D1 CLK
*/
static uint32_t soc_get_pll_d1(void)
{
    volatile uint32_t div = wj_pmu_read_sysclk_d1_div(PMU_REG_BASE);
    volatile uint32_t freq = ILS_VALUE;

    if (0U == wj_pmu_read_sysclk3_sta(CPR2_REG_BASE)) {
        freq = soc_get_pll_freq() / div;
    }

    return freq;
}
/**
 * 2-3
 * PLL D2 CLK
*/
static uint32_t soc_get_pll_d2(void)
{
    volatile uint32_t div = wj_pmu_read_sysclk_d2_div(PMU_REG_BASE);
    volatile uint32_t freq = soc_get_pll_freq() / div;

    return freq;
}
/**
 * 2-4
 * PLL D3 CLK
*/
uint32_t soc_get_pll_d3(void)
{
    volatile uint32_t div = wj_pmu_read_sdio_div_sta(CPR1_REG_BASE);
    volatile uint32_t freq = soc_get_pll_freq() / div;

    return freq;
}

/**
 * 3-1
 * SYS CLK 0
*/
uint32_t soc_get_sys_clk0(void)
{
    return soc_get_pll_d0();
}

/**
 * 3-2
 * SYS CLK 1
*/
uint32_t soc_get_sys_clk1(void)
{
    volatile uint32_t freq;

    if (0U == wj_pmu_read_sysclk0_sta(CPR1_REG_BASE)) {
        uint32_t div = wj_pmu_read_sysclk_1_div_sta(CPR1_REG_BASE);
        freq = soc_get_pll_d0() / div;
    } else {
        freq = ILS_VALUE;
    }

    return freq;
}

/**
 * 3-3
 * SYS CLK 2
*/
uint32_t soc_get_sys_clk2(void)
{
    uint32_t div = wj_pmu_read_sysclk_2_div_sta(CPR1_REG_BASE);
    volatile uint32_t freq = soc_get_pll_d0() / div;

    return freq;
}

/**
 * 3-4
 * SYS APB 0
*/
uint32_t soc_get_sys_apb0(void)
{
    uint32_t div = wj_pmu_read_apb0_sta(CPR1_REG_BASE);
    volatile uint32_t freq = soc_get_pll_d0() / div;

    return freq;
}

/**
 * 3-5
 * SYS CLK 3
*/
uint32_t soc_get_sys_clk3(void)
{
    volatile uint32_t freq;

    if (0U == wj_pmu_read_sysclk3_sta(CPR2_REG_BASE)) {
        uint32_t div = wj_pmu_read_sysclk_2_div_sta(CPR1_REG_BASE);
        freq = soc_get_pll_freq() / div;
    } else {
        freq = ILS_VALUE;
    }

    return freq;
}

/**
 * 3-6
 * SYS APB 1
*/
uint32_t soc_get_sys_apb1(void)
{
    uint32_t div = wj_pmu_read_apb1_sta(CPR2_REG_BASE);
    volatile uint32_t freq = soc_get_pll_d1() / div;

    return freq;
}


uint32_t soc_get_cpu_freq(uint32_t idx)
{
    volatile uint32_t freq;

    if ((idx == 0U) || (idx == 1U)) {
        freq = soc_get_sys_clk0();
    } else {
        freq = soc_get_sys_clk3();
    }

    return freq;
}

uint32_t soc_get_cur_cpu_freq(void)
{
    return soc_get_cpu_freq(soc_get_cpu_id());
}


uint32_t soc_get_coretim_freq(void)
{
    return soc_get_cpu_freq(soc_get_cpu_id());
}

uint32_t soc_get_apb_freq(uint32_t idx)
{
    volatile uint32_t freq = 0U;

    if (idx == 0U) {
        freq = soc_get_sys_apb0();
    } else if (idx == 1U) {
        freq = soc_get_sys_apb1();
    }

    return freq;
}

uint32_t soc_get_qspi_freq(uint32_t idx)
{
    return soc_get_pll_d2();
}

uint32_t soc_get_rtc_freq(uint32_t idx)
{
    return RTC_CLOCK;
}

uint32_t soc_get_usi_freq(uint32_t idx)
{
    volatile uint32_t freq;

    if (idx < 2U) {
        freq = soc_get_sys_apb0();
    } else {
        freq = soc_get_sys_apb1();
    }

    return freq;
}

uint32_t soc_get_uart_freq(uint32_t idx)
{
    volatile uint32_t freq;

    if (idx < 2U) {
        freq = soc_get_sys_apb0();
    } else {
        freq = soc_get_sys_apb1();
    }

    return freq;
}

uint32_t soc_get_pwm_freq(uint32_t idx)
{
    volatile uint32_t freq;

    if (idx < 1U) {
        freq = soc_get_sys_apb0();
    } else {
        freq = soc_get_sys_apb1();
    }

    return freq;
}

uint32_t soc_get_i2s_freq(uint32_t idx)
{
    return AUDIO_CLOCK;
}

uint32_t soc_get_timer_freq(uint32_t idx)
{
    volatile uint32_t freq;

    if (idx < 2U) {
        freq = soc_get_sys_apb0();
    } else if (idx < 4U) {
        freq = 32000;
    } else {
        freq = soc_get_sys_apb1();
    }

    return freq;
}

uint32_t soc_get_sdmmc_freq(uint32_t idx)
{
    volatile uint32_t freq;

    if (wj_pmu_write_sdmmc_sel_sta(CPR1_REG_BASE)) {
        freq = ILS_VALUE;
    } else {
        freq = soc_get_pll_d3();
    }

    return freq;
}

uint32_t soc_get_sdio_freq(uint32_t idx)
{
    volatile uint32_t freq;

    if (wj_pmu_write_sdio_sel_sta(CPR1_REG_BASE)) {
        freq = ILS_VALUE;
    } else {
        freq = soc_get_pll_d3() / 2;
    }

    return freq;
}

uint32_t soc_get_wdt_freq(uint32_t idx)
{
    return soc_get_apb_freq(0U);
}

/**
  \brief        get boot mode
  \param[in]    NULL
  \return       0:   boot normal
                1:   boot low power
*/
uint32_t soc_get_boot_from(void)
{
    return wj_pmu_read_boot_from_lp_sta(PMU_REG_BASE);
}

/**
  \brief        set rc trim value
  \param[in]    val:   trim value
  \return       NULL
*/
void soc_set_32k_rc_trim(uint32_t val)
{
    wj_pmu_write_ils_rc_trim(PMU_REG_BASE, val);
}

/**
  \brief        get rc trim value
  \param[in]    NULL
  \return       trim value
*/
uint32_t soc_get_32k_rc_trim(void)
{
    return wj_pmu_read_ils_rc_trim(PMU_REG_BASE);
}

/**
  \brief        get cpu boot address
  \param[in]    NULL
  \return       boot address
*/
uint32_t soc_get_cpu_bootaddr(void)
{
    return (0U == soc_get_cpu_id() ? wj_pmu_read_bootaddr_804_0(PMU_REG_BASE) : \
            1U == soc_get_cpu_id() ? wj_pmu_read_booaddr_804_1(PMU_REG_BASE) : \
            wj_pmu_read_bootaddr_805(PMU_REG_BASE));
}

/**
  \brief        set cpu boot address
  \param[in]    boot address
  \return       NULL
*/
void soc_set_cpu_bootaddr(uint32_t addr)
{
    if (0U == soc_get_cpu_id()) {
        wj_pmu_write_booaddr_804_0(PMU_REG_BASE, addr);
    } else if (1U == soc_get_cpu_id()) {
        wj_pmu_write_booaddr_804_1(PMU_REG_BASE, addr);
    } else if (2U == soc_get_cpu_id()) {
        wj_pmu_write_booaddr_805(PMU_REG_BASE, addr);
    }
}

/**
  \brief        get cpu lp boot address
  \param[in]    NULL
  \return       lp boot address
*/
uint32_t soc_get_cpu_lp_bootaddr(void)
{
    return (0U == soc_get_cpu_id() ? wj_pmu_read_lpbootaddr_804_0(PMU_REG_BASE) : \
            1U == soc_get_cpu_id() ? wj_pmu_read_lpbootaddr_804_1(PMU_REG_BASE) : \
            wj_pmu_read_lpbootaddr_805(PMU_REG_BASE));
}

/**
  \brief        set cpu lp boot address
  \param[in]    lp boot address
  \return       NULL
*/
void soc_set_cpu_lp_bootaddr(uint32_t addr)
{
    if (0U == soc_get_cpu_id()) {
        wj_pmu_write_lpbootaddr_804_0(PMU_REG_BASE, addr);
    } else if (1U == soc_get_cpu_id()) {
        wj_pmu_write_lpbootaddr_804_1(PMU_REG_BASE, addr);
    } else if (2U == soc_get_cpu_id()) {
        wj_pmu_write_lpbootaddr_805(PMU_REG_BASE, addr);
    }
}

/**
  \brief        get cpu dtcm baseaddress
  \param[in]    NULL
  \return       dtcm baseaddress
*/
uint32_t soc_get_cpu_dtcm_baseaddr(void)
{
    return (0U == soc_get_cpu_id() ? wj_pmu_read_dtcm_baseaddr_804_0(CPR1_REG_BASE) : \
            1U == soc_get_cpu_id() ? wj_pmu_read_dtcm_baseaddr_804_1(CPR1_REG_BASE) : \
            0U);
}

/**
  \brief        set cpu dtcm baseaddress
  \param[in]    dtcm baseaddress
  \return       NULL
*/
void soc_set_cpu_dtcm_baseaddr(uint32_t addr)
{
    if (0U == soc_get_cpu_id()) {
        wj_pmu_write_dtcm_baseaddr_804_0(CPR1_REG_BASE, addr);
    } else if (1U == soc_get_cpu_id()) {
        wj_pmu_write_dtcm_baseaddr_804_1(CPR1_REG_BASE, addr);
    }
}

/**
  \brief        get cpu dtcm maskaddress
  \param[in]    NULL
  \return       dtcm maskaddress
*/
uint32_t soc_get_cpu_dtcm_maskaddr(void)
{
    return (0U == soc_get_cpu_id() ? wj_pmu_read_dtcm_maskaddr_804_0(CPR1_REG_BASE) : \
            1U == soc_get_cpu_id() ? wj_pmu_read_dtcm_maskaddr_804_1(CPR1_REG_BASE) : \
            0U);
}

/**
  \brief        set cpu dtcm maskaddress
  \param[in]    dtcm maskaddress
  \return       NULL
*/
void soc_set_cpu_dtcm_maskaddr(uint32_t addr)
{
    if (0U == soc_get_cpu_id()) {
        wj_pmu_write_dtcm_maskaddr_804_0(CPR1_REG_BASE, addr);
    } else if (1U == soc_get_cpu_id()) {
        wj_pmu_write_dtcm_maskaddr_804_1(CPR1_REG_BASE, addr);
    }
}

/**
  \brief        get cpu itcm baseaddress
  \param[in]    NULL
  \return       itcm baseaddress
*/
uint32_t soc_get_cpu_itcm_baseaddr(void)
{
    return (0U == soc_get_cpu_id() ? wj_pmu_read_itcm_baseaddr_804_0(CPR1_REG_BASE) : \
            1U == soc_get_cpu_id() ? wj_pmu_read_itcm_baseaddr_804_1(CPR1_REG_BASE) : \
            0U);
}

/**
  \brief        set cpu itcm baseaddress
  \param[in]    itcm baseaddress
  \return       NULL
*/
void soc_set_cpu_itcm_baseaddr(uint32_t addr)
{
    if (0U == soc_get_cpu_id()) {
        wj_pmu_write_itcm_baseaddr_804_0(CPR1_REG_BASE, addr);
    } else if (1U == soc_get_cpu_id()) {
        wj_pmu_write_itcm_baseaddr_804_1(CPR1_REG_BASE, addr);
    }
}

/**
  \brief        get cpu itcm maskaddress
  \param[in]    NULL
  \return       itcm maskaddress
*/
uint32_t soc_get_cpu_itcm_maskaddr(void)
{
    return (0U == soc_get_cpu_id() ? wj_pmu_read_itcm_maskaddr_804_0(CPR1_REG_BASE) : \
            1U == soc_get_cpu_id() ? wj_pmu_read_itcm_maskaddr_804_1(CPR1_REG_BASE) : \
            0U);
}

/**
  \brief        set cpu itcm maskaddress
  \param[in]    itcm maskaddress
  \return       NULL
*/
void soc_set_cpu_itcm_maskaddr(uint32_t addr)
{
    if (0U == soc_get_cpu_id()) {
        wj_pmu_write_itcm_maskaddr_804_0(CPR1_REG_BASE, addr);
    } else if (1U == soc_get_cpu_id()) {
        wj_pmu_write_itcm_maskaddr_804_1(CPR1_REG_BASE, addr);
    }
}

void drv_clk_enable(clk_module_t module)
{
    uint32_t val;
    uint8_t bit_mask = 0x1U;

    if (module == AONTIM_CLK || module == RTC_CLK) {
        bit_mask = 0x11U;
    }

    if (module == TIM1_CLK) {
        *(volatile uint32_t *)0x30000010 &= ~(0x01 << 19);
    }

    val = *(uint32_t *)module;
    val |= bit_mask;
    *(uint32_t *)module = val;
}

void drv_clk_disable(clk_module_t module)
{
    uint32_t val;
    uint8_t bit_mask = 0x1U;

    if (module == AONTIM_CLK || module == RTC_CLK) {
        bit_mask = 0x11U;
    }

    val = *(uint32_t *)module;
    val &= ~bit_mask;
    *(uint32_t *)module = val;
}


uint32_t soc_set_sdio_freq(uint32_t idx, uint32_t freq)
{
    /* sdio clk shoud not over 50M */
    if (freq > 50000000) {
        return -1;
    }

    uint32_t div = soc_get_pll_freq() / 2 / freq;

    if (div > 8) {
        return -1;
    }

    /* sdio clk shoud not over 50M */
    if ((soc_get_pll_freq() / 2 / div > 50000000) && div < 8) {
        div += 1;
    }

    wj_pmu_write_sdio_div(CPR1_REG_BASE, div);
    return 0;
}

void soc_set_apb0_clk_div(uint8_t val)
{
    wj_pmu_write_apb0_div(CPR1_REG_BASE, val);
}

void soc_set_apb1_clk_div(uint8_t val)
{
    wj_pmu_write_apb1_div(CPR2_REG_BASE, val);
}

void soc_clk_enable(int32_t module)
{
    uint32_t val;
    uint8_t bit_mask = 0x1U;
    clk_module_t t_module = (clk_module_t) module;

    if (t_module == AONTIM_CLK || t_module == RTC_CLK) {
        bit_mask = 0x11U;
    }

    val = *((volatile uint32_t *)t_module);
    val |= bit_mask;
    *(volatile uint32_t *)t_module = val;
}

void soc_clk_disable(int32_t module)
{
    uint8_t bit_mask = 0x1U;
    clk_module_t t_module = (clk_module_t) module;

    if (t_module == AONTIM_CLK || t_module == RTC_CLK) {
        bit_mask = 0x11U;
    }

    *(volatile uint32_t *)t_module &= ~bit_mask;
}