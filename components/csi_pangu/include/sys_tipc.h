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
#include <ck_tipc.h>

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
    APB0_WDT_TRUST,
    APB0_RTC_TRUST,
    APB0_TIM0_TRUST,
    APB0_TIM1_TRUST,
    APB0_QSPI0_TRUST,
    APB0_PADREG_TRUST,
    APB0_GPIO0_TRUST,
    APB0_PWM0_TRUST,
    APB0_I2S0_TRUST,
    APB0_I2S1_TRUST,
    APB0_USI0_TRUST,
    APB0_USI1_TRUST,
    APB0_UART0_TRUST,
    APB0_UART1_TRUST,
} ckenum_tipc_module_e;
ck_tipc_info_t ck_tipc_info[] = {
    {AHB0_SDMMC_TRUST,  0x0, 0x01, 0x0, CSKY_SDIO1_BASE + 0x4},
    {AHB0_SDIO_TRUST,   0x0, 0x02, 0x0, CSKY_SDIO0_BASE + 0x4},
    {AHB0_DMAC0_TRUST,  0x0, 0x03, 0x0, CSKY_DMAC0_BASE},
    //{AHB0_AES_TRUST,    0x0, 0x04, 0x1, CSKY_AES_BASE},
    //{AHB0_SHA_TRUST,    0x0, 0x05, 0x1, CSKY_SHA_BASE},
    //{AHB0_RSA_TRUST,    0x0, 0x06, 0x1, CSKY_RSA_BASE},
    //{AHB0_TRNG_TRUST,   0x0, 0x07, 0x1, CSKY_TRNG_BASE},
    {AHB1_DMAC1_TRUST,  0x0, 0x08, 0x0, CSKY_DMAC1_BASE},
    {APB0_WDT_TRUST,    0x4, 0x00, 0x0, CSKY_WDT_BASE + 0x4},
    {APB0_RTC_TRUST,    0x4, 0x01, 0x0, CSKY_RTC_BASE + 0x4},
    {APB0_TIM0_TRUST,   0x4, 0x02, 0x0, CSKY_TIM0_BASE},
    {APB0_TIM1_TRUST,   0x4, 0x03, 0x0, CSKY_LPTIM_BASE},
    {APB0_QSPI0_TRUST,  0x4, 0x05, 0x0, CSKY_QSPIC0_BASE + 0x94},
    {APB0_PADREG_TRUST, 0x4, 0x06, 0x0, CSKY_PADREG_BASE + 0xc},
    //{APB0_GPIO0_TRUST,  0x4, 0x08, 0x0, CSKY_GPIO0_BASE + 0x30}, gpio0 can not config secure
    {APB0_PWM0_TRUST,   0x4, 0x09, 0x0, CSKY_PWM0_BASE},
    {APB0_I2S0_TRUST,   0x4, 0x0a, 0x0, CSKY_I2S0_BASE},
    {APB0_I2S1_TRUST,   0x4, 0x0b, 0x0, CSKY_I2S1_BASE},
    {APB0_USI0_TRUST,   0x4, 0x0c, 0x0, CSKY_USI0_BASE + 0x4},
    {APB0_USI1_TRUST,   0x4, 0x0d, 0x0, CSKY_USI1_BASE + 0x4},
    {APB0_UART0_TRUST,  0x4, 0x0e, 0x0, CSKY_UART0_BASE + 0xc},
    {APB0_UART1_TRUST,  0x4, 0x0f, 0x0, CSKY_UART1_BASE + 0xc},
};

#define TIPC_MODULE_NUM  (sizeof(ck_tipc_info) / sizeof(ck_tipc_info_t))
#ifdef __cplusplus
}
#endif

#endif

