/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     sys_freq.h
 * @brief    header file for setting system frequency.
 * @version  V1.0
 * @date     18. July 2018
 * @vendor   csky
 * @chip     pangu
 ******************************************************************************/
#ifndef _SYS_FREQ_H_
#define _SYS_FREQ_H_

#include <stdint.h>
#include "soc.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    IHS_CLK       = 0,         /* internal high speed clock */
    EHS_CLK       = 1,         /* external high speed clock */
    ILS_CLK       = 2,         /* internal low speed clock */
    ELS_CLK       = 3,         /* enternal low speed clock */
    PLL_CLK       = 4
} clk_src_e;

typedef enum {
    AONTIM_CLK   = (CSKY_AONCPR_BASE + 0x4C),
    RTC_CLK      = (CSKY_AONCPR_BASE + 0x78),
    FFT_CLK      = (CSKY_CPR0_BASE + 0x0C),
    TIM1_CLK     = (CSKY_CPR0_BASE + 0x10),
    FMC_AHB0_CLK = (CSKY_CPR0_BASE + 0x14),
    ROM_CLK      = (CSKY_CPR0_BASE + 0x18),
    GPIO0_CLK    = (CSKY_CPR0_BASE + 0x1C),
    QSPI_CLK     = (CSKY_CPR0_BASE + 0x20),
    USI0_CLK     = (CSKY_CPR0_BASE + 0x24),
    USI1_CLK     = (CSKY_CPR0_BASE + 0x28),
    UART0_CLK    = (CSKY_CPR0_BASE + 0x2C),
    I2S0_CLK     = (CSKY_CPR0_BASE + 0x30),
    I2S1_CLK     = (CSKY_CPR0_BASE + 0x34),
    EFUSE_CLK    = (CSKY_CPR0_BASE + 0x38),
    SASC_CLK     = (CSKY_CPR0_BASE + 0x3C),
    TIPC_CLK     = (CSKY_CPR0_BASE + 0x40),
    SDIO_CLK     = (CSKY_CPR0_BASE + 0x44),
    SDMMC_CLK    = (CSKY_CPR0_BASE + 0x48),
    AES_CLK      = (CSKY_CPR0_BASE + 0x4C),
    RSA_CLK      = (CSKY_CPR0_BASE + 0x50),
    SHA_CLK      = (CSKY_CPR0_BASE + 0x54),
    TRNG_CLK     = (CSKY_CPR0_BASE + 0x58),
    DMA0_CLK     = (CSKY_CPR0_BASE + 0x5C),
    WDT_CLK      = (CSKY_CPR0_BASE + 0x60),
    PWM0_CLK     = (CSKY_CPR0_BASE + 0x64),
    UART1_CLK    = (CSKY_CPR0_BASE + 0x7C),
    TIM2_CLK     = (CSKY_CPR1_BASE + 0x08),
    TIM3_CLK     = (CSKY_CPR1_BASE + 0x0C),
    GPIO1_CLK    = (CSKY_CPR1_BASE + 0x10),
    USI2_CLK     = (CSKY_CPR1_BASE + 0x14),
    USI3_CLK     = (CSKY_CPR1_BASE + 0x18),
    UART2_CLK    = (CSKY_CPR1_BASE + 0x1C),
    UART3_CLK    = (CSKY_CPR1_BASE + 0x20),
    I2S2_CLK     = (CSKY_CPR1_BASE + 0x24),
    I2S3_CLK     = (CSKY_CPR1_BASE + 0x28),
    SPDIF_CLK    = (CSKY_CPR1_BASE + 0x2C),
    FMC_AHB1_CLK = (CSKY_CPR1_BASE + 0x30),
    TDM_CLK      = (CSKY_CPR1_BASE + 0x34),
    PDM_CLK      = (CSKY_CPR1_BASE + 0x38),
    DMA1_CLK     = (CSKY_CPR1_BASE + 0x3C),
    PWM1_CLK     = (CSKY_CPR1_BASE + 0x40),
    CODEC_CLK    = (CSKY_CPR1_BASE + 0x6C)
} clk_module_e;

typedef enum {
    CPU_192MHZ = 192000000,
    CPU_144MHZ = 144000000,
    CPU_115_2MHZ = 115200000,
    CPU_96MHZ = 96000000,
    CPU_72MHZ = 72000000,
    CPU_64MHZ = 64000000,
    CPU_57_6MHZ = 57600000,
    CPU_48MHZ = 48000000,
    CPU_38_4MHZ = 38400000,
    CPU_36MHZ = 36000000,
    CPU_BYPASS_EHS,
} cpu_freq_t;

typedef enum {
    CPU0_RESET,
    WDG_RESET,
    MCU_RESET,
    POWER_RESET,
} boot_reson_t;

void drv_set_pmu_clk_src(clk_src_e src);
void drv_set_aon_clk_src(clk_src_e src);
void drv_reset_cpu1(void);
void drv_reset_cpu2(void);
int32_t drv_get_sys_freq(void);
int32_t drv_get_apb_freq(int32_t idx);
int32_t drv_get_rtc_freq(int32_t idx);
int32_t drv_get_timer_freq(int32_t idx);
int32_t drv_get_cpu_freq(int32_t idx);
int32_t drv_get_usi_freq(int32_t idx);
int32_t drv_get_usart_freq(int32_t idx);
int32_t drv_get_pwm_freq(int32_t idx);
int32_t drv_get_i2s_freq(int32_t idx);
void drv_clk_enable(clk_module_e module);
void drv_clk_disable(clk_module_e module);
int32_t drv_set_cpu_freq(int idx, cpu_freq_t freq);
boot_reson_t drv_get_boot_reason(void);
int drv_set_sdio_freq(int idx, int freq);
int32_t drv_get_sdio_freq(int idx);
uint32_t drv_get_cur_cpu_freq(void);

#include <yun_pmu.h>

#ifdef __cplusplus
}
#endif

#endif /* _SYS_FREQ_H_ */

