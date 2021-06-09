/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     sys_clk.h
 * @brief    header file for setting system frequency.
 * @version  V1.0
 * @date     11. Aug 2020
 * @vendor   csky
 * @chip     pangu
 ******************************************************************************/

#ifndef _SYS_CLK_H_
#define _SYS_CLK_H_

#include <stdint.h>
#include "soc.h"
#include <drv/clk.h>
#include <drv/porting.h>
#include <wj_pmu_pangu_cpu0.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PMU_REG_BASE             (wj_pmu_reg_cpr0_t *)WJ_AONCPR_BASE
#define CPR1_REG_BASE            (wj_pmu_reg_cpr1_t *)WJ_CPR1_BASE
#define CPR2_REG_BASE            (wj_pmu_reg_cpr2_t *)WJ_CPR2_BASE

typedef enum {
    IHS_CLK       = 0U,        /* internal high speed clock */
    EHS_CLK,                   /* external high speed clock */
    ILS_CLK,                   /* internal low  speed clock */
    ELS_CLK,                   /* external low  speed clock */
    PLL_CLK                    /* PLL clock */
} clk_src_t;

typedef enum {
    CPU_300MHZ      = 300000000U,
    CPU_288MHZ      = 288000000U,
    CPU_276MHZ      = 276000000U,
    CPU_272MHZ      = 272000000U,
    CPU_270MHZ      = 270000000U,
    CPU_264MHZ      = 264000000U,
    CPU_256MHZ      = 256000000U,
    CPU_252MHZ      = 252000000U,
    CPU_245_76MHZ   = 245760000U,
    CPU_240MHZ      = 240000000U,
    CPU_228MHZ      = 228000000U,
    CPU_220MHZ      = 220000000U,
    CPU_216MHZ      = 216000000U,
    CPU_210MHZ      = 210000000U,
    CPU_200MHZ      = 200000000U,
    CPU_196_608MHZ  = 196608000U,
    CPU_204MHZ      = 204000000U,
    CPU_198MHZ      = 198000000U,
    CPU_192MHZ      = 192000000U,
    CPU_186MHZ      = 186000000U,
    CPU_180MHZ      = 180000000U,
    CPU_174MHZ      = 174000000U,
    CPU_168MHZ      = 168000000U,
    CPU_162MHZ      = 162000000U,
    CPU_156MHZ      = 156000000U,
    CPU_150MHZ      = 150000000U,
    CPU_144MHZ      = 144000000U,
    CPU_140MHZ      = 140000000U,
    CPU_138MHZ      = 138000000U,
    CPU_135MHZ      = 135000000U,
    CPU_132MHZ      = 132000000U,
    CPU_126MHZ      = 126000000U,
    CPU_120MHZ      = 120000000U,
    CPU_115_2MHZ    = 115200000U,
    CPU_108MHZ      = 108000000U,
    CPU_105MHZ      = 105000000U,
    CPU_100MHZ      = 100000000U,
    CPU_96MHZ       = 96000000U,
    CPU_90MHZ       = 90000000U,
    CPU_84MHZ       = 84000000U,
    CPU_75MHZ       = 75000000U,
    CPU_72MHZ       = 72000000U,
    CPU_64MHZ       = 64000000U,
    CPU_60MHZ       = 60000000U,
    CPU_57_6MHZ     = 57600000U,
    CPU_50MHZ       = 50000000U,
    CPU_48MHZ       = 48000000U,
    CPU_40MHZ       = 40000000U,
    CPU_38_4MHZ     = 38400000U,
    CPU_36MHZ       = 36000000U,
    CPU_33MHZ       = 33000000U,
    CPU_24MHZ       = 24000000U,
    CPU_BYPASS_EHS,
} sys_freq_t;

typedef enum {
    AONTIM_CLK   = (WJ_AONCPR_BASE + 0x4C),
    RTC_CLK      = (WJ_AONCPR_BASE + 0x78),
    FFT_CLK      = (WJ_CPR1_BASE + 0x0C),
    TIM1_CLK     = (WJ_CPR1_BASE + 0x10),
    FMC_AHB0_CLK = (WJ_CPR1_BASE + 0x14),
    ROM_CLK      = (WJ_CPR1_BASE + 0x18),
    GPIO0_CLK    = (WJ_CPR1_BASE + 0x1C),
    QSPI_CLK     = (WJ_CPR1_BASE + 0x20),
    USI0_CLK     = (WJ_CPR1_BASE + 0x24),
    USI1_CLK     = (WJ_CPR1_BASE + 0x28),
    UART0_CLK    = (WJ_CPR1_BASE + 0x2C),
    I2S0_CLK     = (WJ_CPR1_BASE + 0x30),
    I2S1_CLK     = (WJ_CPR1_BASE + 0x34),
    EFUSE_CLK    = (WJ_CPR1_BASE + 0x38),
    SASC_CLK     = (WJ_CPR1_BASE + 0x3C),
    TIPC_CLK     = (WJ_CPR1_BASE + 0x40),
    SDIO_CLK     = (WJ_CPR1_BASE + 0x44),
    SDMMC_CLK    = (WJ_CPR1_BASE + 0x48),
    AES_CLK      = (WJ_CPR1_BASE + 0x4C),
    RSA_CLK      = (WJ_CPR1_BASE + 0x50),
    SHA_CLK      = (WJ_CPR1_BASE + 0x54),
    TRNG_CLK     = (WJ_CPR1_BASE + 0x58),
    DMA0_CLK     = (WJ_CPR1_BASE + 0x5C),
    WDT_CLK      = (WJ_CPR1_BASE + 0x60),
    PWM0_CLK     = (WJ_CPR1_BASE + 0x64),
    UART1_CLK    = (WJ_CPR1_BASE + 0x7C),
    TIM2_CLK     = (WJ_CPR2_BASE + 0x08),
    TIM3_CLK     = (WJ_CPR2_BASE + 0x0C),
    GPIO1_CLK    = (WJ_CPR2_BASE + 0x10),
    USI2_CLK     = (WJ_CPR2_BASE + 0x14),
    USI3_CLK     = (WJ_CPR2_BASE + 0x18),
    UART2_CLK    = (WJ_CPR2_BASE + 0x1C),
    UART3_CLK    = (WJ_CPR2_BASE + 0x20),
    I2S2_CLK     = (WJ_CPR2_BASE + 0x24),
    I2S3_CLK     = (WJ_CPR2_BASE + 0x28),
    SPDIF_CLK    = (WJ_CPR2_BASE + 0x2C),
    FMC_AHB1_CLK = (WJ_CPR2_BASE + 0x30),
    TDM_CLK      = (WJ_CPR2_BASE + 0x34),
    PDM_CLK      = (WJ_CPR2_BASE + 0x38),
    DMA1_CLK     = (WJ_CPR2_BASE + 0x3C),
    PWM1_CLK     = (WJ_CPR2_BASE + 0x40),
    CODEC_CLK    = (WJ_CPR2_BASE + 0x6C)
} clk_module_t;

typedef enum {
    CLK_DIV0 = 0U,
    CLK_DIV1,
    CLK_DIV2,
    CLK_DIV3,
    CLK_DIV4,
    CLK_DIV5,
    CLK_DIV6,
    CLK_DIV7,
    CLK_DIV8,
    CLK_DIV9,
    CLK_DIV10,
    CLK_DIV11,
    CLK_DIV12,
    CLK_DIV13,
    CLK_DIV14,
    CLK_DIV15,
    CLK_DIV16
} apb_div_t;

typedef enum {
    CPU0_RESET,
    WDG_RESET,
    MCU_RESET,
    POWER_RESET,
} boot_reson_t;


/**
 * FOUTVCO = FREF/REFDIV x FBDIV
 * FOUTPOSTDIV = (FREF/REFDIV) x FBDIV/POSTDIV1/POSTDIV2
*/

typedef struct {

    uint32_t pll_is_used;          /* pll is used */
    uint32_t pll_source;           /* select pll input source clock */

    uint32_t pll_ref_div;
    uint32_t pll_fb_div;
    uint32_t pll_clk_div1;
    uint32_t pll_clk_div2;
} pll_clk_config_t;


typedef struct {
    uint32_t system_clk;              /* system clock */

    pll_clk_config_t pll_config;      /* pll config struct */

    uint32_t sys_d0_clk_divider;      /* ratio between mclk clock and pll_d0 clock */
    uint32_t sys_d1_clk_divider;      /* ratio between mclk clock and pll_d1 clock */
    uint32_t sys_d2_clk_divider;      /* ratio between mclk clock and pll_d2 clock */
    uint32_t sys_d3_clk_divider;      /* ratio between mclk clock and pll_d3 clock */

    uint32_t sys_0_clk_source;        /* select sys0 source clock */
    uint32_t sys_3_clk_source;        /* select sys3 source clock */
    uint32_t sdio_clk_source;         /* select sdio source clock */
    uint32_t sdmmc_clk_source;        /* select sdmmc source clock */
    uint32_t rtc_clk_source;          /* select rtcclk source clock */

    uint32_t sys_1_clk_divider;      /* ratio between mclk clock and sys 1 clock */
    uint32_t sys_2_clk_divider;      /* ratio between mclk clock and sys 2 clock */
    uint32_t apb0_clk_divider;       /* ratio between mclk clock and apb0 clock */
    uint32_t apb1_clk_divider;       /* ratio between mclk clock and apb1 clock */

} system_clk_config_t;

#ifdef __cplusplus
}
#endif
void drv_clk_enable(clk_module_t module);
void drv_clk_disable(clk_module_t module);
uint32_t soc_set_sdio_freq(uint32_t idx, uint32_t freq);
uint32_t soc_get_sdio_freq(uint32_t idx);
void soc_set_apb0_clk_div(uint8_t val);
void soc_set_apb1_clk_div(uint8_t val);
#endif /* _SYS_CLK_H_ */

