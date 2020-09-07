/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
/******************************************************************************
 * @file     sys_freq.h
 * @brief    header file for setting system frequency.
 * @version  V1.0
 * @date     18. July 2018
 ******************************************************************************/
#ifndef _SYS_FREQ_H_
#define _SYS_FREQ_H_

#include <stdint.h>
#include "soc.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PMU_MCLK_SEL  (CSKY_CLKGEN_BASE + 0x4)
#define MCLK_REG_VAL  0x8UL

#define PMU_CLK_STABLE  (CSKY_CLKGEN_BASE + 0x18)
#define PMU_PLL_CTRL  (CSKY_CLKGEN_BASE + 0x2c)

typedef enum {
    IHS_CLK       = 0,         /* internal high speed clock */
    EHS_CLK       = 1,         /* external high speed clock */
    ILS_CLK       = 2,         /* internal low speed clock */
    ELS_CLK       = 3          /* enternal low speed clock */
} clk_src_e;

typedef enum {
    OSR_8M_CLK_16M      = 0x80204,
    OSR_8M_CLK_24M      = 0x80206,
    OSR_8M_CLK_32M      = 0x80208,
    OSR_8M_CLK_40M      = 0x8020a,
    OSR_8M_CLK_48M      = 0x8020c
} clk_val_e;

/**
  \brief       set sys freq
  \param[in]   source \ref clk_src_e
  \param[in]   val    \ref clk_val_e
*/
void drv_set_sys_freq(clk_src_e source, clk_val_e val);

/**
  \brief       config rtc clock source
  \param[in]   source \ref clk_src_e, only support ILS_CLK and ELS_CLK
*/
void drv_rtcclk_config(clk_src_e source);

/**
  \brief       get eflash write/program latency
  \param[out]  prog   return write latency
  \param[out]  erase  return program latency
*/
void drv_get_eflash_params(clk_val_e val, uint16_t *prog, uint16_t *erase);

/**
  \brief       get i2s freq
  return       frequency value
*/
int32_t drv_get_i2s_freq(int32_t idx);

/**
  \brief       get pwm freq
  return       frequency value
*/
int32_t drv_get_pwm_freq(int32_t idx);

/**
  \brief       get uart freq
  return       frequency value
*/
int32_t drv_get_usart_freq(int32_t idx);

/**
  \brief       get sys freq
  return       frequency value
*/
int32_t drv_get_sys_freq(void);

/**
  \brief       get apb freq
  return       frequency value
*/
int32_t drv_get_apb_freq(int32_t idx);

/**
  \brief       get rtc freq
  return       frequency value
*/
int32_t drv_get_rtc_freq(int32_t idx);

/**
  \brief       get timer freq
  return       frequency value
*/
int32_t drv_get_timer_freq(int32_t idx);

/**
  \brief       get cpu freq
  return       frequency value
*/
int32_t drv_get_cpu_freq(int32_t idx);

/**
  \brief       config clock sources stable counter
  \param[in]   source \ref count
*/
void drv_set_clk_stable_counter(uint32_t count);

/**
  \brief       get cur cpu clock
*/
uint32_t drv_get_cur_cpu_freq(void);

#ifdef __cplusplus
}
#endif

#endif /* _SYS_FREQ_H_ */

