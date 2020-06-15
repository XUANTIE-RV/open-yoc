/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     cpr_regs.h
 * @brief    header file for the cpr
 * @version  V1.0
 * @date     07. Aug 2019
 * @vendor   csky
 * @chip     pangu
 ******************************************************************************/

#ifndef _CPR_REGS_H_
#define _CPR_REGS_H_

#define     __IM     volatile const       /*! Defines 'read only' structure member permissions */
#define     __OM     volatile             /*! Defines 'write only' structure member permissions */
#define     __IOM    volatile             /*! Defines 'read / write' structure member permissions */

typedef struct {
    __IOM uint32_t CPR0_CPU0CLK_SEL;
    __IOM uint32_t CPR0_SYSCLK0_DIV_CTL;
    __IOM uint32_t CPR0_RESERVED0;
    __IOM uint32_t CPR0_FFT_CTL;
    __IOM uint32_t CPR0_TIM1_CTL;
    __IOM uint32_t CPR0_FMC_AHB0_CTL;
    __IOM uint32_t CPR0_ROM_CTL;
    __IOM uint32_t CPR0_GPIO0_CTL;
    __IOM uint32_t CPR0_QSPI_CTL;
    __IOM uint32_t CPR0_USI0_CTL;
    __IOM uint32_t CPR0_USI1_CTL;
    __IOM uint32_t CPR0_UART0_CTL;
    __IOM uint32_t CPR0_I2S0_CLK_CTL;
    __IOM uint32_t CPR0_I2S1_CLK_CTL;
    __IOM uint32_t CPR0_EFUSE_CTL;
    __IOM uint32_t CPR0_SASC_CTL;
    __IOM uint32_t CPR0_TIPC_CTL;
    __IOM uint32_t CPR0_SDIO_CLK_CTL;
    __IOM uint32_t CPR0_SDMMC_CLK_CTL;
    __IOM uint32_t CPR0_AES_CLK_CTL;
    __IOM uint32_t CPR0_RSA_CLK_CTL;
    __IOM uint32_t CPR0_SHA_CLK_CTL;
    __IOM uint32_t CPR0_TRNG_CLK_CTL;
    __IOM uint32_t CPR0_DMA0_CLK_CTL;
    __IOM uint32_t CPR0_WDT_CLK_CTL;
    __IOM uint32_t CPR0_PWM0_CLK_CTL;
    __IOM uint32_t CPR0_MS0SWRST;
    __IOM uint32_t CPR0_AHB0SUB0SWRST;
    __IOM uint32_t CPR0_AHB0SUB1SWRST;
    __IOM uint32_t CPR0_AHB0SUB2SWRST;
    __IOM uint32_t CPR0_APB0SWRST;
    __IOM uint32_t CPR0_UART1_CTL;
    __IOM uint32_t CPR0_CK804_0_DAHBL_BASE;
    __IOM uint32_t CPR0_CK804_0_DAHBL_MASK;
    __IOM uint32_t CPR0_CK804_0_IAHBL_BASE;
    __IOM uint32_t CPR0_CK804_0_IAHBL_MASK;
    __IOM uint32_t CPR0_CK804_1_DAHBL_BASE;
    __IOM uint32_t CPR0_CK804_1_DAHBL_MASK;
    __IOM uint32_t CPR0_CK804_1_IAHBL_BASE;
    __IOM uint32_t CPR0_CK804_1_IAHBL_MASK;
    __IOM uint32_t CPR0_H2H0_REG0;
    __IOM uint32_t CPR0_H2H0_REG1;
    __IOM uint32_t CPR0_H2H0_REG2;
    __IOM uint32_t CPR0_H2H0_REG3;
} cpr0_reg_t;

typedef struct {
    __IOM uint32_t CPR1_CPU2CLK_SEL;
    __IOM uint32_t CPR1_SYSCLK1_DIV_CTL;
    __IOM uint32_t CPR1_TIM2_CTL;
    __IOM uint32_t CPR1_TIM3_CTL;
    __IOM uint32_t CPR1_GPIO1_CTL;
    __IOM uint32_t CPR1_USI2_CTL;
    __IOM uint32_t CPR1_USI3_CTL;
    __IOM uint32_t CPR1_UART2_CLK_CTL;
    __IOM uint32_t CPR1_UART3_CLK_CTL_REG ;
    __IOM uint32_t CPR1_I2S2_CTL;
    __IOM uint32_t CPR1_I2S3_CTL;
    __IOM uint32_t CPR1_SPDIF_CTL;
    __IOM uint32_t CPR1_FMC_AHB1_CTL;
    __IOM uint32_t CPR1_TDM_CTL;
    __IOM uint32_t CPR1_PDM_CTL;
    __IOM uint32_t CPR1_DMA1_CLK_CTL;
    __IOM uint32_t CPR1_PWM1_CLK_CTL;
    __IOM uint32_t CPR1_DMA_CH8_SEL;
    __IOM uint32_t CPR1_DMA_CH9_SEL;
    __IOM uint32_t CPR1_DMA_CH10_SEL;
    __IOM uint32_t CPR1_DMA_CH11_SEL;
    __IOM uint32_t CPR1_DMA_CH12_SEL;
    __IOM uint32_t CPR1_DMA_CH13_SEL;
    __IOM uint32_t CPR1_DMA_CH14_SEL;
    __IOM uint32_t CPR1_DMA_CH15_SEL;
    __IOM uint32_t CPR1_MS1SWRST;
    __IOM uint32_t CPR1_APB1SWRST;
    __IOM uint32_t CPR1_CODEC_CLK_CTL;
    __IOM uint32_t CPR1_H2H0_REG0;
    __IOM uint32_t CPR1_H2H0_REG1;
    __IOM uint32_t CPR1_H2H0_REG2;
    __IOM uint32_t CPR1_H2H0_REG3;
} cpr1_reg_t;

#endif
