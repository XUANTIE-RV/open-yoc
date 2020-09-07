/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     yun_pmu.h
 * @brief    header file for the pmu
 * @version  V1.0
 * @date     07. Aug 2019
 * @vendor   csky
 * @chip     pangu
 ******************************************************************************/

#ifndef _YUN_PMU_
#define _YUN_PMU_

#include <soc.h>
#include <sys_freq.h>

typedef enum {
    RESET_STATUS_POWER_ON,
    RESET_STATUS_MCU,
    RESET_STATUS_WDT,
    RESET_STATUS_CPU0
} yun_reset_status_e;

typedef struct {
    int32_t bypass_ehs_en;
    uint8_t prescale;       //1~63
    uint16_t freq_doubling; //16~1600
    uint8_t pll_clk_div1;   //1~7
    uint8_t pll_clk_div2;   //1~7
} pll_config_t;

typedef struct {
    int32_t bypass_ehs_en;
    uint8_t prescale;
    uint16_t freq_doubling; //20~160
    uint8_t pll_clk_div1;
    uint8_t pll_clk_div2;
    uint32_t frac_val;
} pll_frac_config_t;

void yun_enable_lpm_mode(void);
void yun_enter_hw_vad0(void);
void yun_enter_hw_vad1(void);
uint32_t yun_is_boot_from_lpm(void);
yun_reset_status_e yun_get_reset_status(void);
void yun_enable_ils_clk(void);
void yun_disable_ils_clk(void);
void yun_enable_els_clk(void);
void yun_disable_els_clk(void);
void yun_set_cpu0_clk_src(clk_src_e src);
void yun_set_cpu2_clk_src(clk_src_e src);
void yun_pll_config(pll_config_t *param);
void yun_audio_clk_config(pll_frac_config_t *param);
void yun_set_cpu0_cpu1_clk_div(uint8_t val);
void yun_set_cpu2_clk_div(uint8_t val);
void yun_qspi_clock_en(int en);
void yun_qspi_clk_div(uint8_t val);
void yun_set_ahb0_sub1_clk_div(uint8_t val);
void yun_set_ahb0_sub2_clk_div(uint8_t val);
void yun_set_apb0_clk_div(uint8_t val);
void yun_set_apb1_clk_div(uint8_t val);
void yun_cpu0_cpu1_clk_enable(int en);
void yun_cpu2_clk_enable(int en);
uint32_t yun_get_ahb0_sub1_freq();
uint32_t yun_get_ahb0_sub2_freq();
uint32_t yun_get_apb0_freq();
uint32_t yun_get_apb1_freq();
uint32_t yun_get_audio_freq();
int32_t yun_get_cpu_freq(int idx);
void yun_pll_power_down(int en);
int yun_enter_lpm(void);
void yun_sdram_auto_flash_enable(int en);
int yun_enter_lpm(void);
void yun_set_sdio_div(int idx, int div);

#endif
