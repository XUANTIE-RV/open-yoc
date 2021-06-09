/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     sys_clk.h
 * @brief    header file for setting system frequency.
 * @version  V1.0
 * @date     9. April 2020
 ******************************************************************************/
#ifndef _SYS_CLK_H_
#define _SYS_CLK_H_

#include <stdint.h>
#include <soc.h>
#include <drv/clk.h>
#include <drv/porting.h>
#include <wj_pmu_alkaid.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PMU_REG_BASE            (wj_pmu_reg_t *)WJ_PMU_BASE

typedef enum {
    IHS_CLK       = 0U,        /* internal high speed clock */
    EHS_CLK,                   /* external high speed clock */
    ILS_CLK,                   /* internal low  speed clock */
    ELS_CLK,                   /* external low  speed clock */
    PLL_CLK                    /* PLL clock */
} clk_src_t;

typedef enum {
    CPU_300MHZ    = 300000000U,
    CPU_288MHZ    = 288000000U,
    CPU_276MHZ    = 276000000U,
    CPU_270MHZ    = 270000000U,
    CPU_264MHZ    = 264000000U,
    CPU_252MHZ    = 252000000U,
    CPU_245_76MHZ = 245760000U,
    CPU_240MHZ    = 240000000U,
    CPU_228MHZ    = 228000000U,
    CPU_216MHZ    = 216000000U,
    CPU_204MHZ    = 204000000U,
    CPU_196_608MHZ= 196608000U,
    CPU_192MHZ    = 192000000U,
    CPU_180MHZ    = 180000000U,
    CPU_168MHZ    = 168000000U,
    CPU_156MHZ    = 156000000U,
    CPU_144MHZ    = 144000000U,
    CPU_135MHZ    = 135000000U,
    CPU_132MHZ    = 132000000U,
    CPU_120MHZ    = 120000000U,
    CPU_108MHZ    = 108000000U,
    CPU_96MHZ     = 96000000U,
    CPU_84MHZ     = 84000000U,
    CPU_72MHZ     = 72000000U,
    CPU_60MHZ     = 60000000U,
    CPU_48MHZ     = 48000000U,
    CPU_36MHZ     = 36000000U,
    CPU_24MHZ     = 24000000U
} sys_freq_t;


/* pllclkout : ( pllclkin / 2)*( FN + Frac/4096 ) */
typedef struct {

    uint32_t pll_is_used;          /* pll is used */

    uint32_t pll_source;           /* select pll input source clock */

    uint32_t pll_src_clk_divider;  /* ratio between pll_srcclk clock and pll_clkin clock */

    uint32_t fn;                   /* integer value of frequency division */

    uint32_t frac;                 /* decimal value of frequency division */

} pll_config_t;


typedef struct {
    uint32_t system_clk;            /* system clock */

    pll_config_t pll_config;        /* pll config struct */

    uint32_t sys_clk_source;        /* select sysclk source clock */

    uint32_t rtc_clk_source;        /* select rtcclk source clock */

    uint32_t mclk_divider;          /* ratio between fs_mclk clock and mclk clock */

    uint32_t apb0_clk_divider;      /* ratio between mclk clock and apb0 clock */

    uint32_t apb1_clk_divider;      /* ratio between mclk clock and apb1 clock */

} system_clk_config_t;

typedef enum {
    CLK_DIV1 = 0U,
    CLK_DIV2,
    CLK_DIV4,
    CLK_DIV8
} apb_div_t;

typedef enum {
    PLL_FN_18 = 0U,
    PLL_FN_19,
    PLL_FN_20,
    PLL_FN_21,
    PLL_FN_22,
    PLL_FN_23,
    PLL_FN_24,
    PLL_FN_25,
    PLL_FN_26,
    PLL_FN_27,
    PLL_FN_28,
    PLL_FN_29,
    PLL_FN_30,
    PLL_FN_31,
    PLL_FN_32,
    PLL_FN_33,
    PLL_FN_34,
    PLL_FN_35,
    PLL_FN_36,
    PLL_FN_37,
    PLL_FN_38,
    PLL_FN_39,
    PLL_FN_40,
    PLL_FN_41,
    PLL_FN_42,
    PLL_FN_43,
    PLL_FN_44,
    PLL_FN_45,
    PLL_FN_46,
    PLL_FN_47,
    PLL_FN_48,
    PLL_FN_49
} pll_fn_t;

typedef enum {
    TIM0_CLK = 0U,
    TIM1_CLK,
    RTC0_CLK,
    WDT_CLK,
    SPI0_CLK,
    UART0_CLK,
    IIC0_CLK,
    PWM_CLK,
    QSPI0_CLK,
    PWMR_CLK,
    EFUSE_CLK,
    I2S0_CLK,
    I2S1_CLK,
    GPIO0_CLK,

    TIM2_CLK = 32U,
    TIM3_CLK,
    SPI1_CLK,
    UART1_CLK,
    I2S567_CLK,
    ADC_CLK,
    ETB_CLK,
    I2S2_CLK,
    I2S3_CLK,
    IOC_CLK,
    CODEC_CLK
} clk_module_t;


/**
  \brief       Set the system clock according to the parameter
  \param[in]   config    system clock config.
  \return      error code
*/
csi_error_t soc_sysclk_config(system_clk_config_t *config);

/**
  \brief       Set iic reset
  \param[in]   idx    iic idx.
  \return      Null
*/
void soc_reset_iic(uint32_t idx);

#ifdef __cplusplus
}
#endif

#endif /* _SYS_CLK_H_ */

