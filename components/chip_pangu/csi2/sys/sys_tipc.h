/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     sys_tipc.h
 * @brief    header file for setting system frequency.
 * @version  V1.0
 * @date     18. July 2018
 * @vendor   csky
 * @chip     pangu
 ******************************************************************************/
#ifndef _SYS_TIPC_H_
#define _SYS_TIPC_H_

#include <soc.h>
#include "drv/tipc.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    AHB0_SDIO_TRUST,
    AHB0_SDMMC_TRUST,
    AHB0_DMAC0_TRUST,
    AHB0_AES_TRUST,
    AHB0_SHA_TRUST,
    AHB0_RSA_TRUST,
    AHB0_TRNG_TRUST,
    AHB1_DMAC1_TRUST,
    APB0_WDT_TRUST = 8,
    APB0_RTC_TRUST,
    APB0_TIM0_TRUST,
    APB0_TIM1_TRUST,
    APB0_RESERVED1,
    APB0_QSPI0_TRUST,
    APB0_PADREG_TRUST,
    APB0_RESERVED2,
    APB0_GPIO0_TRUST,
    APB0_PWM0_TRUST,
    APB0_I2S0_TRUST,
    APB0_I2S1_TRUST,
    APB0_USI0_TRUST,
    APB0_USI1_TRUST,
    APB0_UART0_TRUST,
    APB0_UART1_TRUST
} soc_tipc_module_t;

/**
  \brief        SOC tipc secure enbale
  \return       none 
*/
void soc_tipc_enable_secure(soc_tipc_module_t ip);

/**
  \brief        SOC tipc secure disable
  \return       none
*/
void soc_tipc_disable_secure(soc_tipc_module_t ip);

#ifdef __cplusplus
}
#endif

#endif

