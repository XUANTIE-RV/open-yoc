/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     wj_tipc_ll.h
 * @brief
 * @version
 * @date     21. April 2020
 ******************************************************************************/

#ifndef _WJ_TIPC_LL_H_
#define _WJ_TIPC_LL_H_

#include <soc.h>
#include <csi_core.h>
#include "sys_tipc.h"

#ifdef __cplusplus
extern "C" {
#endif

/* MAIN, offset: 0x00 */
#define WJ_TIPC_AHB0_SDIO_Pos                       (1U)
#define WJ_TIPC_AHB0_SDIO_Msk                       (0x1U << WJ_TIPC_AHB0_SDIO_Pos)
#define WJ_TIPC_AHB0_SDIO_EN                        WJ_TIPC_AHB0_SDIO_Msk

#define WJ_TIPC_AHB0_SDMMC_Pos                      (2U)
#define WJ_TIPC_AHB0_SDMMC_Msk                      (0x1U << WJ_TIPC_AHB0_SDMMC_Pos)
#define WJ_TIPC_AHB0_SDMMC_EN                       WJ_TIPC_AHB0_SDMMC_Msk

#define WJ_TIPC_AHB0_DMA0_Pos                       (3U)
#define WJ_TIPC_AHB0_DMA0_Msk                       (0x1U << WJ_TIPC_AHB0_DMA0_Pos)
#define WJ_TIPC_AHB0_DMA0_EN                        WJ_TIPC_AHB0_DMA0_Msk

#define WJ_TIPC_AHB0_AES_Pos                        (4U)
#define WJ_TIPC_AHB0_AES_Msk                        (0x1U << WJ_TIPC_AHB0_AES_Pos)
#define WJ_TIPC_AHB0_AES_EN                         WJ_TIPC_AHB0_AES_Msk

#define WJ_TIPC_AHB0_SHA_Pos                        (5U)
#define WJ_TIPC_AHB0_SHA_Msk                        (0x1U << WJ_TIPC_AHB0_SHA_Pos)
#define WJ_TIPC_AHB0_SHA_EN                         WJ_TIPC_AHB0_SHA_Msk

#define WJ_TIPC_AHB0_RSA_Pos                        (6U)
#define WJ_TIPC_AHB0_RSA_Msk                        (0x1U << WJ_TIPC_AHB0_RSA_Pos)
#define WJ_TIPC_AHB0_RSA_EN                         WJ_TIPC_AHB0_RSA_Msk

#define WJ_TIPC_AHB0_TRNG_Pos                       (7U)
#define WJ_TIPC_AHB0_TRNG_Msk                       (0x1U << WJ_TIPC_AHB0_TRNG_Pos)
#define WJ_TIPC_AHB0_TRNG_EN                        WJ_TIPC_AHB0_TRNG_Msk

#define WJ_TIPC_AHB0_DMA1_Pos                       (8U)
#define WJ_TIPC_AHB0_DMA1_Msk                       (0x1U << WJ_TIPC_AHB0_DMA1_Pos)
#define WJ_TIPC_AHB0_DMA1_EN                        WJ_TIPC_AHB0_DMA1_Msk

#define WJ_TIPC_AHB0_ALL_Pos                       (1U)
#define WJ_TIPC_AHB0_ALL_Msk                       (0xFFU << WJ_TIPC_AHB0_ALL_Pos)
#define WJ_TIPC_AHB0_ALL_EN                        WJ_TIPC_AHB0_ALL_Msk
/* APB0, offset: 0x04 */
#define WJ_TIPC_APB0_WDT_Pos                       (0U)
#define WJ_TIPC_APB0_WDT_Msk                       (0x1U << WJ_TIPC_APB0_WDT_Pos)
#define WJ_TIPC_APB0_WDT_EN                        WJ_TIPC_APB0_WDT_Msk

#define WJ_TIPC_APB0_RTC_Pos                       (1U)
#define WJ_TIPC_APB0_RTC_Msk                       (0x1U << WJ_TIPC_APB0_RTC_Pos)
#define WJ_TIPC_APB0_RTC_EN                        WJ_TIPC_APB0_RTC_Msk

#define WJ_TIPC_APB0_TIM0_Pos                       (2U)
#define WJ_TIPC_APB0_TIM0_Msk                       (0x1U << WJ_TIPC_APB0_TIM0_Pos)
#define WJ_TIPC_APB0_TIM0_EN                        WJ_TIPC_APB0_TIM0_Msk

#define WJ_TIPC_APB0_TIM1_Pos                       (3U)
#define WJ_TIPC_APB0_TIM1_Msk                       (0x1U << WJ_TIPC_APB0_TIM1_Pos)
#define WJ_TIPC_APB0_TIM1_EN                        WJ_TIPC_APB0_TIM1_Msk

#define WJ_TIPC_APB0_QSPI_Pos                       (5U)
#define WJ_TIPC_APB0_QSPI_Msk                       (0x1U << WJ_TIPC_APB0_QSPI_Pos)
#define WJ_TIPC_APB0_QSPI_EN                        WJ_TIPC_APB0_QSPI_Msk

#define WJ_TIPC_APB0_PADREG_Pos                     (6U)
#define WJ_TIPC_APB0_PADREG_Msk                     (0x1U << WJ_TIPC_APB0_PADREG_Pos)
#define WJ_TIPC_APB0_PADREG_EN                      WJ_TIPC_APB0_PADREG_Msk

#define WJ_TIPC_APB0_PMU_Pos                        (8U)
#define WJ_TIPC_APB0_PMU_Msk                        (0x1U << WJ_TIPC_APB0_PMU_Pos )
#define WJ_TIPC_APB0_PMU_EN                         WJ_TIPC_APB0_PMU_Msk

#define WJ_TIPC_APB0_PWM0_Pos                       (9U)
#define WJ_TIPC_APB0_PWM0_Msk                       (0x1U << WJ_TIPC_APB0_PWM0_Pos)
#define WJ_TIPC_APB0_PWM0_EN                        WJ_TIPC_APB0_PWM0_Msk

#define WJ_TIPC_APB0_I2S0_Pos                       (10U)
#define WJ_TIPC_APB0_I2S0_Msk                       (0x1U << WJ_TIPC_APB0_I2S0_Pos)
#define WJ_TIPC_APB0_I2S0_EN                        WJ_TIPC_APB0_I2S0_Msk

#define WJ_TIPC_APB0_I2S1_Pos                       (11U)
#define WJ_TIPC_APB0_I2S1_Msk                       (0x1U << WJ_TIPC_APB0_I2S1_Pos)
#define WJ_TIPC_APB0_I2S1_EN                        WJ_TIPC_APB0_I2S1_Msk

#define WJ_TIPC_APB0_USI0_Pos                       (12U)
#define WJ_TIPC_APB0_USI0_Msk                       (0x1U << WJ_TIPC_APB0_USI0_Pos)
#define WJ_TIPC_APB0_USI0_EN                        WJ_TIPC_APB0_USI0_Msk

#define WJ_TIPC_APB0_USI1_Pos                       (13U)
#define WJ_TIPC_APB0_USI1_Msk                       (0x1U << WJ_TIPC_APB0_USI1_Pos)
#define WJ_TIPC_APB0_USI1_EN                        WJ_TIPC_APB0_USI1_Msk

#define WJ_TIPC_APB0_UART0_Pos                      (14U)
#define WJ_TIPC_APB0_UART0_Msk                      (0x1U << WJ_TIPC_APB0_UART0_Pos)
#define WJ_TIPC_APB0_UART0_EN                       WJ_TIPC_APB0_UART0_Msk

#define WJ_TIPC_APB0_UART1_Pos                      (15U)
#define WJ_TIPC_APB0_UART1_Msk                      (0x1U << WJ_TIPC_APB0_UART1_Pos)
#define WJ_TIPC_APB0_UART1_EN                       WJ_TIPC_APB0_UART1_Msk

#define WJ_TIPC_APB0_ALL_Pos                      (0U)
#define WJ_TIPC_APB0_ALL_Msk                      (0xFF6FU << WJ_TIPC_APB0_UART1_Pos)
#define WJ_TIPC_APB0_ALL_EN                       WJ_TIPC_APB0_UART1_Msk
typedef struct {
    __IOM uint32_t   MAIN;             /* Offset: 0x000 (W/R)  RBG AHB0 Register */
    __IOM  uint32_t  APB0;             /* Offset: 0x004 (W/R)  RBG APB0 Register */
} wj_tipc_regs_t;

static inline void wj_tipc_enable_all_ahb0(wj_tipc_regs_t *tipc)
{
    tipc->MAIN |= WJ_TIPC_AHB0_ALL_EN;
}

static inline void wj_tipc_disenable_all_ahb0(wj_tipc_regs_t *tipc)
{
    tipc->MAIN &= ~WJ_TIPC_AHB0_ALL_Msk;
}

static inline void wj_tipc_enable_all_apb0(wj_tipc_regs_t *tipc)
{
    tipc->APB0 |= WJ_TIPC_APB0_ALL_EN;
}

static inline void wj_tipc_disenable_all_apb0(wj_tipc_regs_t *tipc)
{
    tipc->APB0 &= ~WJ_TIPC_APB0_ALL_Msk;
}

static inline void wj_tipc_enable_secure(wj_tipc_regs_t *tipc, uint32_t ip)
{
    if (ip < 8) {
        tipc->MAIN |= (uint32_t)0x1 << (uint32_t)(ip+1);
    } else {
        ip -= 8;
        tipc->APB0 |= (uint32_t)0x1 << (uint32_t)ip;
    }
}

static inline void wj_tipc_disable_secure(wj_tipc_regs_t *tipc, uint32_t ip)
{
    if (ip < 8) {
        tipc->MAIN &= ~((uint32_t)0x1 << (uint32_t)(ip+1));
    } else {
        ip -= 8;
        tipc->APB0 &= ~((uint32_t)0x1 << (uint32_t)ip);
    }
}

#ifdef __cplusplus
}
#endif

#endif  /* _WJ_TIPC_LL_H_*/
