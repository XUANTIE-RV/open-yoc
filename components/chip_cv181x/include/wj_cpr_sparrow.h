/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     wj_sparrow_cpr.h
 * @brief    head file for sparrow cpr
 * @version  V1.0
 * @date     28. July 2020
 ******************************************************************************/
#ifndef _WJ_SPARROW_CPR_H_
#define _WJ_SPARROW_CPR_H_

#include <soc.h>
#include <csi_core.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    __IOM uint32_t CPR_SYS_CLK_DIV_REG;    /* Offset: 0x000 (R/W) System clock divider selection configuration register.          */
    __IOM uint32_t CPR_PERI_DIV_SEL_REG;   /* Offset: 0x004 (R/W) Peripheral working clock divider configuration register.        */
    __IOM uint32_t CPR_PERI_CLK_SEL_REG;   /* Offset: 0x008 (R/W) Audio peripheral clock source selection configuration register. */
    __IOM uint32_t CPR_PERI_CTRL_REG;      /* Offset: 0x00C (R/W) Peripheral control signal configuration register.               */
    __IOM uint32_t CPR_IP_CG_REG;          /* Offset: 0x010 (R/W) IP CG control register                                          */
    __IOM uint32_t CPR_IP_RST_REG;         /* Offset: 0x014 (R/W) IP reset control register                                       */
    __IOM uint32_t CPR_I2S0_BUSY_REG;      /* Offset: 0x018 (R/W) I2S0 BUSY status register.                                      */
    __IOM uint32_t CPR_I2S1_BUSY_REG;      /* Offset: 0x01C (R/W) I2S1 BUSY status register.                                      */
    __IOM uint32_t CPR_I2S2_BUSY_REG;      /* Offset: 0x020 (R/W) I2S2 BUSY status register.                                      */
    __IOM uint32_t CPR_I2SIN_BUSY_REG;     /* Offset: 0x024 (R/W) I2S-IN BUSY status register.                                    */
    __IOM uint32_t CPR_SPDIF0_BUSY_REG;    /* Offset: 0x028 (R/W) SPDIF0 BUSY status register.                                    */
    __IOM uint32_t CPR_SPDIF1_BUSY_REG;    /* Offset: 0x02C (R/W) SPDIF1 BUSY status register.                                    */
    __IOM uint32_t CPR_TDM_BUSY_REG;       /* Offset: 0x030 (R/W) TDM BUSY status register.                                       */
    __IOM uint32_t CPR_IIC0_BUSY_REG;      /* Offset: 0x034 (R/W) IIC0 BUSY status register.                                      */
    __IOM uint32_t CPR_IIC1_BUSY_REG;      /* Offset: 0x038 (R/W) IIC1 BUSY status register.                                      */
    __IOM uint32_t CPR_UART_BUSY_REG;      /* Offset: 0x03C (R/W) UART BUSY status register.                                      */
    __IOM uint32_t CPR_TIMER_BUSY_REG;     /* Offset: 0x040 (R/W) TIMER BUSY status register.                                     */
    __IOM uint32_t CPR_VAD_BUSY_REG;       /* Offset: 0x044 (R/W) VAD BUSY status register.                                       */
    __IOM uint32_t CPR_GPIO_BUSY_REG;      /* Offset: 0x048 (R/W) GPIO BUSY status register.                                      */
    __IOM uint32_t CPR_DMA_CH1_BUSY_REG;   /* Offset: 0x04C (R/W) DMA_CH1 BUSY status register.                                   */
    __IOM uint32_t CPR_DMA_CH2_BUSY_REG;   /* Offset: 0x050 (R/W) DMA_CH2 BUSY status register.                                   */
    __IOM uint32_t CPR_DMA_CH3_BUSY_REG;   /* Offset: 0x054 (R/W) DMA_CH3 BUSY status register.                                   */
    __IOM uint32_t CPR_DMA_CH4_BUSY_REG;   /* Offset: 0x058 (R/W) DMA_CH4 BUSY status register.                                   */
    __IOM uint32_t CPR_DMA_CH5_BUSY_REG;   /* Offset: 0x05C (R/W) DMA_CH5 BUSY status register.                                   */
    __IOM uint32_t CPR_DMA_CH6_BUSY_REG;   /* Offset: 0x060 (R/W) DMA_CH6 BUSY status register.                                   */
    __IOM uint32_t CPR_DMA_CH7_BUSY_REG;   /* Offset: 0x064 (R/W) DMA_CH7 BUSY status register.                                   */
    __IOM uint32_t CPR_DMA_CH8_BUSY_REG;   /* Offset: 0x068 (R/W) DMA_CH8 BUSY status register.                                   */
    __IOM uint32_t CPR_DMA_CH9_BUSY_REG;   /* Offset: 0x06C (R/W) DMA_CH9 BUSY status register.                                   */
    __IOM uint32_t CPR_DMA_CH10_BUSY_REG;  /* Offset: 0x070 (R/W) DMA_CH10 BUSY status register.                                  */
    __IM  uint32_t CPR_SYS_STATUS_REG;     /* Offset: 0x074 (R/ ) System status query register.                                   */
    __IOM uint32_t CPR_SYS_CTRL_REG;       /* Offset: 0x078 (R/W) System control register.                                        */
    __IM  uint32_t CPR_SYS_ID_REG;         /* Offset: 0x0A0 (R/ ) System version ID register                                      */
    __IOM uint32_t CPR_RESERVD1_REG;       /* Offset: 0x0A4 (R/W) System reserved register 1                                      */
} wj_cpr_regs_t;

/* CPR_SYS_CLK_DIV_REG, Offset: 0x0 */
#define CPR_SYS_CLK_SEL_Pos               0U
#define CPR_SYS_CLK_SEL_Msk               (0x1U << CPR_SYS_CLK_SEL_Pos)
#define CPR_SYS_CLK_SEL_EN                CPR_SYS_CLK_SEL_Msk
#define CPR_LPMD_SYSCLK_SEL_Pos           1U
#define CPR_LPMD_SYSCLK_SEL_Msk           (0x3U << CPR_LPMD_SYSCLK_SEL_Pos)
#define CPR_LPMD_SYSCLK_SEL(X)            (X << CPR_LPMD_SYSCLK_SEL_Pos)
#define CPR_CPU_DIV_SEL_Pos               4U
#define CPR_CPU_DIV_SEL_Msk               (0x3U << CPR_CPU_DIV_SEL_Pos)
#define CPR_CPU_DIV_SEL(X)                (X << CPR_CPU_DIV_SEL_Pos)
#define CPR_SYS_DIV_SEL_Pos               8U
#define CPR_SYS_DIV_SEL_Msk               (0x7U << CPR_SYS_DIV_SEL_Pos)
#define CPR_SYS_DIV_SEL(X)                (X << CPR_SYS_DIV_SEL_Pos)
#define CPR_AHB_DIV_SEL_Pos               12U
#define CPR_AHB_DIV_SEL_Msk               (0x7U << CPR_AHB_DIV_SEL_Pos)
#define CPR_AHB_DIV_SEL(X)                (X << CPR_AHB_DIV_SEL_Pos)
#define CPR_APB_DIV_SEL_Pos               16U
#define CPR_APB_DIV_SEL_Msk               (0x7U << CPR_APB_DIV_SEL_Pos)
#define CPR_APB_DIV_SEL(X)                (X << CPR_APB_DIV_SEL_Pos)

/* PERI_DIV_SEL_REG, Offset: 0x4 */
#define CPR_UART_DIV_SEL_Pos              0U
#define CPR_UART_DIV_SEL_Msk              (0xFU << CPR_UART_DIV_SEL_Pos)
#define CPR_UART_DIV_SEL(X)               (X << CPR_UART_DIV_SEL_Pos)
#define CPR_AUDIO_DIV_SEL_Pos             4U
#define CPR_AUDIO_DIV_SEL_Msk             (0xFU << CPR_AUDIO_DIV_SEL_Pos)
#define CPR_AUDIO_DIV_SEL(X)              (X << CPR_AUDIO_DIV_SEL_Pos)
#define CPR_VAD_DIV_SEL_Pos               8U
#define CPR_VAD_DIV_SEL_Msk               (0xFU << CPR_VAD_DIV_SEL_Pos)
#define CPR_VAD_DIV_SEL(X)                (X << CPR_VAD_DIV_SEL_Pos)
#define CPR_GPIO_CLK_SEL_Pos              12U
#define CPR_GPIO_CLK_SEL_Msk              (0x7U << CPR_GPIO_CLK_SEL_Pos)
#define CPR_GPIO_CLK_SEL(X)               (X << CPR_GPIO_CLK_SEL_Pos)


/* PERI_CLK_SEL_REG, Offset: 0x8 */
#define CPR_I2S0_SRC_SEL_Pos              0U
#define CPR_I2S0_SRC_SEL_Msk              (0x3U << CPR_I2S0_SRC_SEL_Pos)
#define CPR_I2S0_SRC_SEL(X)               (X << CPR_I2S0_SRC_SEL_Pos)
#define CPR_I2S1_SRC_SEL_Pos              4U
#define CPR_I2S1_SRC_SEL_Msk              (0x3U << CPR_I2S1_SRC_SEL_Pos)
#define CPR_I2S1_SRC_SEL(X)               (X << CPR_I2S1_SRC_SEL_Pos)
#define CPR_I2S2_SRC_SEL_Pos              8U
#define CPR_I2S2_SRC_SEL_Msk              (0x3U << CPR_I2S2_SRC_SEL_Pos)
#define CPR_I2S2_SRC_SEL(X)               (X << CPR_I2S2_SRC_SEL_Pos)
#define CPR_I2SIN_SRC_SEL_Pos             12U
#define CPR_I2SIN_SRC_SEL_Msk             (0x3U << CPR_I2SIN_SRC_SEL_Pos)
#define CPR_I2SIN_SRC_SEL(X)              (X << CPR_I2SIN_SRC_SEL_Pos)
#define CPR_TDM_SRC_SEL_Pos               16U
#define CPR_TDM_SRC_SEL_Msk               (0x3U << CPR_TDM_SRC_SEL_Pos)
#define CPR_TDM_SRC_SEL(X)                (X << CPR_TDM_SRC_SEL_Pos)
#define CPR_VAD_SRC_SEL_Pos               20U
#define CPR_VAD_SRC_SEL_Msk               (0x3U << CPR_VAD_SRC_SEL_Pos)
#define CPR_VAD_SRC_SEL(X)                (X << CPR_VAD_SRC_SEL_Pos)

/* PERI_CTRL_REG, Offset: 0xC */
#define CPR_CNT1_2_LINK_Pos               0U
#define CPR_CNT1_2_LINK_Msk               (0x1U << CPR_CNT1_2_LINK_Pos)
#define CPR_CNT1_2_LINK_EN                CPR_CNT1_2_LINK_Msk
#define CPR_CNT2_3_LINK_Pos               1U
#define CPR_CNT2_3_LINK_Msk               (0x1U << CPR_CNT2_3_LINK_Pos)
#define CPR_CNT2_3_LINK_EN                CPR_CNT2_3_LINK_Msk
#define CPR_CNT3_4_LINK_Pos               2U
#define CPR_CNT3_4_LINK_Msk               (0x1U << CPR_CNT3_4_LINK_Pos)
#define CPR_CNT3_4_LINK_EN                CPR_CNT3_4_LINK_Msk
#define CPR_CNT1_PAUSE_Pos                4U
#define CPR_CNT1_PAUSE_Msk                (0x1U << CPR_CNT1_PAUSE_Pos)
#define CPR_CNT1_PAUSE_EN                 CPR_CNT1_PAUSE_Msk
#define CPR_CNT2_PAUSE_Pos                5U
#define CPR_CNT2_PAUSE_Msk                (0x1U << CPR_CNT2_PAUSE_Pos)
#define CPR_CNT2_PAUSE_EN                 CPR_CNT2_PAUSE_Msk
#define CPR_CNT3_PAUSE_Pos                6U
#define CPR_CNT3_PAUSE_Msk                (0x1U << CPR_CNT3_PAUSE_Pos)
#define CPR_CNT3_PAUSE_EN                 CPR_CNT3_PAUSE_Msk
#define CPR_CNT4_PAUSE_Pos                7U
#define CPR_CNT4_PAUSE_Msk                (0x1U << CPR_CNT4_PAUSE_Pos)
#define CPR_CNT4_PAUSE_EN                 CPR_CNT4_PAUSE_Msk
#define CPR_WDT_PAUSE_Pos                 8U
#define CPR_WDT_PAUSE_Msk                 (0x1U << CPR_WDT_PAUSE_Pos)
#define CPR_WDT_PAUSE_EN                  CPR_WDT_PAUSE_Msk
#define CPR_VAD_I2SIN_SYNC_Pos            12U
#define CPR_VAD_I2SIN_SYNC_Msk            (0x1U << CPR_VAD_I2SIN_SYNC_Pos)
#define CPR_VAD_I2SIN_SYNC_EN             CPR_VAD_I2SIN_SYNC_Msk
#define CPR_I2S_SYNC_Pos                  13U
#define CPR_I2S_SYNC_Msk                  (0x1U << CPR_I2S_SYNC_Pos)
#define CPR_I2S_SYNC_EN                   CPR_I2S_SYNC_Msk
#define CPR_SPDIF_SYNC_Pos                14U
#define CPR_SPDIF_SYNC_Msk                (0x1U << CPR_SPDIF_SYNC_Pos)
#define CPR_SPDIF_SYNC_EN                 CPR_SPDIF_SYNC_Msk
#define CPR_BSM_SEL_Pos                   16U
#define CPR_BSM_SEL_Msk                   (0x3U << CPR_BSM_SEL_Pos)
#define CPR_BSM_SEL_CPU                   (0U << CPR_BSM_SEL_Pos)
#define CPR_BSM_SEL_DMA                   (1U << CPR_BSM_SEL_Pos)
#define CPR_BSM_SEL_CP2AP                 (2U << CPR_BSM_SEL_Pos)

/* IP_CG_REG, Offset: 0x10 */
#define CPR_CPU_CG_Pos                    0U
#define CPR_CPU_CG_Msk                    (0x1U << CPR_CPU_CG_Pos)
#define CPR_CPU_CG_EN                     CPR_CPU_CG_Msk
#define CPR_SRAM0_CG_Pos                  1U
#define CPR_SRAM0_CG_Msk                  (0x1U << CPR_SRAM0_CG_Pos)
#define CPR_SRAM0_CG_EN                   CPR_SRAM0_CG_Msk
#define CPR_SRAM1_CG_Pos                  2U
#define CPR_SRAM1_CG_Msk                  (0x1U << CPR_SRAM1_CG_Pos)
#define CPR_SRAM1_CG_EN                   CPR_SRAM1_CG_Msk
#define CPR_DMA_CG_Pos                    3U
#define CPR_DMA_CG_Msk                    (0x1U << CPR_DMA_CG_Pos)
#define CPR_DMA_CG_EN                     CPR_DMA_CG_Msk
#define CPR_BSM_CG_Pos                    4U
#define CPR_BSM_CG_Msk                    (0x1U << CPR_BSM_CG_Pos)
#define CPR_BSM_CG_EN                     CPR_BSM_CG_Msk
#define CPR_TIMER_CG_Pos                  8U
#define CPR_TIMER_CG_Msk                  (0x1U << CPR_TIMER_CG_Pos)
#define CPR_TIMER_CG_EN                   CPR_TIMER_CG_Msk
#define CPR_TIMER_CNT1_CG_Pos             9U
#define CPR_TIMER_CNT1_CG_Msk             (0x1U << CPR_TIMER_CNT1_CG_Pos)
#define CPR_TIMER_CNT1_CG_EN              CPR_TIMER_CNT1_CG_Msk
#define CPR_TIMER_CNT2_CG_Pos             10U
#define CPR_TIMER_CNT2_CG_Msk             (0x1U << CPR_TIMER_CNT2_CG_Pos)
#define CPR_TIMER_CNT2_CG_EN              CPR_TIMER_CNT2_CG_Msk
#define CPR_TIMER_CNT3_CG_Pos             11U
#define CPR_TIMER_CNT3_CG_Msk             (0x1U << CPR_TIMER_CNT3_CG_Pos)
#define CPR_TIMER_CNT3_CG_EN              CPR_TIMER_CNT3_CG_Msk
#define CPR_TIMER_CNT4_CG_Pos             12U
#define CPR_TIMER_CNT4_CG_Msk             (0x1U << CPR_TIMER_CNT4_CG_Pos)
#define CPR_TIMER_CNT4_CG_EN              CPR_TIMER_CNT4_CG_Msk
#define CPR_WDR_CG_Pos                    13U
#define CPR_WDR_CG_Msk                    (0x1U << CPR_WDR_CG_Pos)
#define CPR_WDR_CG_EN                     CPR_WDR_CG_Msk
#define CPR_USI0_CG_Pos                   14U
#define CPR_USI0_CG_Msk                   (0x1U << CPR_USI0_CG_Pos)
#define CPR_USI0_CG_EN                    CPR_USI0_CG_Msk
#define CPR_USI1_CG_Pos                   15U
#define CPR_USI1_CG_Msk                   (0x1U << CPR_USI1_CG_Pos)
#define CPR_USI1_CG_EN                    CPR_USI1_CG_Msk
#define CPR_UART_CG_Pos                   16U
#define CPR_UART_CG_Msk                   (0x1U << CPR_UART_CG_Pos)
#define CPR_UART_CG_EN                    CPR_UART_CG_Msk
#define CPR_I2S0_CG_Pos                   17U
#define CPR_I2S0_CG_Msk                   (0x1U << CPR_I2S0_CG_Pos)
#define CPR_I2S0_CG_EN                    CPR_I2S0_CG_Msk
#define CPR_I2S1_CG_Pos                   18U
#define CPR_I2S1_CG_Msk                   (0x1U << CPR_I2S1_CG_Pos)
#define CPR_I2S1_CG_EN                    CPR_I2S1_CG_Msk
#define CPR_I2S2_CG_Pos                   19U
#define CPR_I2S2_CG_Msk                   (0x1U << CPR_I2S2_CG_Pos)
#define CPR_I2S2_CG_EN                    CPR_I2S2_CG_Msk
#define CPR_I2SIN_CG_Pos                  20U
#define CPR_I2SIN_CG_Msk                  (0x1U << CPR_I2SIN_CG_Pos)
#define CPR_I2SIN_CG_EN                   CPR_I2SIN_CG_Msk
#define CPR_TDM_CG_Pos                    21U
#define CPR_TDM_CG_Msk                    (0x1U << CPR_TDM_CG_Pos)
#define CPR_TDM_CG_EN                     CPR_TDM_CG_Msk
#define CPR_GPIO_CG_Pos                   22U
#define CPR_GPIO_CG_Msk                   (0x1U << CPR_GPIO_CG_Pos)
#define CPR_GPIO_CG_EN                    CPR_GPIO_CG_Msk
#define CPR_SPDIF0_CG_Pos                 23U
#define CPR_SPDIF0_CG_Msk                 (0x1U << CPR_SPDIF0_CG_Pos)
#define CPR_SPDIF0_CG_EN                  CPR_SPDIF0_CG_Msk
#define CPR_SPDIF1_CG_Pos                 24U
#define CPR_SPDIF1_CG_Msk                 (0x1U << CPR_SPDIF1_CG_Pos)
#define CPR_SPDIF1_CG_EN                  CPR_SPDIF1_CG_Msk
#define CPR_VAD_CG_Pos                    25U
#define CPR_VAD_CG_Msk                    (0x1U << CPR_VAD_CG_Pos)
#define CPR_VAD_CG_EN                     CPR_VAD_CG_Msk

/* IP_RST_REG, Offset: 0x14 */
#define CPR_DMA_SRST_N_Pos                3U
#define CPR_DMA_SRST_N_Msk                (0x1U << CPR_DMA_SRST_N_Pos)
#define CPR_DMA_SRST_N_EN                 CPR_DMA_SRST_N_Msk
#define CPR_BSM_SRST_N_Pos                4U
#define CPR_BSM_SRST_N_Msk                (0x1U << CPR_BSM_SRST_N_Pos)
#define CPR_BSM_SRST_N_EN                 CPR_BSM_SRST_N_Msk
#define CPR_TIMER_SRST_N_Pos              8U
#define CPR_TIMER_SRST_N_Msk              (0x1U << CPR_TIMER_SRST_N_Pos)
#define CPR_TIMER_SRST_N_EN               CPR_TIMER_SRST_N_Msk
#define CPR_TIM_CNT1_SRST_N_Pos           9U
#define CPR_TIM_CNT1_SRST_N_Msk           (0x1U << CPR_TIM_CNT1_SRST_N_Pos)
#define CPR_TIM_CNT1_SRST_N_EN            CPR_TIM_CNT1_SRST_N_Msk
#define CPR_TIM_CNT2_SRST_N_Pos           10U
#define CPR_TIM_CNT2_SRST_N_Msk           (0x1U << CPR_TIM_CNT2_SRST_N_Pos)
#define CPR_TIM_CNT2_SRST_N_EN            CPR_TIM_CNT2_SRST_N_Msk
#define CPR_TIM_CNT3_SRST_N_Pos           11U
#define CPR_TIM_CNT3_SRST_N_Msk           (0x1U << CPR_TIM_CNT3_SRST_N_Pos)
#define CPR_TIM_CNT3_SRST_N_EN            CPR_TIM_CNT3_SRST_N_Msk
#define CPR_TIM_CNT4_SRST_N_Pos           12U
#define CPR_TIM_CNT4_SRST_N_Msk           (0x1U << CPR_TIM_CNT4_SRST_N_Pos)
#define CPR_TIM_CNT4_SRST_N_EN            CPR_TIM_CNT4_SRST_N_Msk
#define CPR_WDR_SRST_N_Pos                13U
#define CPR_WDR_SRST_N_Msk                (0x1U << CPR_WDR_SRST_N_Pos)
#define CPR_WDR_SRST_N_EN                 CPR_WDR_SRST_N_Msk
#define CPR_USI0_SRST_N_Pos               14U
#define CPR_USI0_SRST_N_Msk               (0x1U << CPR_USI0_SRST_N_Pos)
#define CPR_USI0_SRST_N_EN                CPR_USI0_SRST_N_Msk
#define CPR_USI1_SRST_N_Pos               15U
#define CPR_USI1_SRST_N_Msk               (0x1U << CPR_USI1_SRST_N_Pos)
#define CPR_USI1_SRST_N_EN                CPR_USI1_SRST_N_Msk
#define CPR_UART_SRST_N_Pos               16U
#define CPR_UART_SRST_N_Msk               (0x1U << CPR_UART_SRST_N_Pos)
#define CPR_UART_SRST_N_EN                CPR_UART_SRST_N_Msk
#define CPR_I2S0_SRST_N_Pos               17U
#define CPR_I2S0_SRST_N_Msk               (0x1U << CPR_I2S0_SRST_N_Pos)
#define CPR_I2S0_SRST_N_EN                CPR_I2S0_SRST_N_Msk
#define CPR_I2S1_SRST_N_Pos               18U
#define CPR_I2S1_SRST_N_Msk               (0x1U << CPR_I2S1_SRST_N_Pos)
#define CPR_I2S1_SRST_N_EN                CPR_I2S1_SRST_N_Msk
#define CPR_I2S2_SRST_N_Pos               19U
#define CPR_I2S2_SRST_N_Msk               (0x1U << CPR_I2S2_SRST_N_Pos)
#define CPR_I2S2_SRST_N_EN                CPR_I2S2_SRST_N_Msk
#define CPR_I2SIN_SRST_N_Pos              20U
#define CPR_I2SIN_SRST_N_Msk              (0x1U << CPR_I2SIN_SRST_N_Pos)
#define CPR_I2SIN_SRST_N_EN               CPR_I2SIN_SRST_N_Msk
#define CPR_TDM_SRST_N_Pos                21U
#define CPR_TDM_SRST_N_Msk                (0x1U << CPR_TDM_SRST_N_Pos)
#define CPR_TDM_SRST_N_EN                 CPR_TDM_SRST_N_Msk
#define CPR_GPIO_SRST_N_Pos               22U
#define CPR_GPIO_SRST_N_Msk               (0x1U << CPR_GPIO_SRST_N_Pos)
#define CPR_GPIO_SRST_N_EN                CPR_GPIO_SRST_N_Msk
#define CPR_SPDIF0_SRST_N_Pos             23U
#define CPR_SPDIF0_SRST_N_Msk             (0x1U << CPR_SPDIF0_SRST_N_Pos)
#define CPR_SPDIF0_SRST_N_EN              CPR_SPDIF0_SRST_N_Msk
#define CPR_SPDIF1_SRST_N_Pos             24U
#define CPR_SPDIF1_SRST_N_Msk             (0x1U << CPR_SPDIF1_SRST_N_Pos)
#define CPR_SPDIF1_SRST_N_EN              CPR_SPDIF1_SRST_N_Msk
#define CPR_VAD_SRST_N_Pos                25U
#define CPR_VAD_SRST_N_Msk                (0x1U << CPR_VAD_SRST_N_Pos)
#define CPR_VAD_SRST_N_EN                 CPR_VAD_SRST_N_Msk
#define CPR_IOMUX_SRST_N_Pos              26U
#define CPR_IOMUX_SRST_N_Msk              (0x1U << CPR_IOMUX_SRST_N_Pos)
#define CPR_IOMUX_SRST_N_EN               CPR_IOMUX_SRST_N_Msk

/* I2S0_BUSY_REG, Offset: 0x18 */
#define CPR_I2S0_BUSY_Pos                 0U
#define CPR_I2S0_BUSY_Msk                 (0x1U << CPR_I2S0_BUSY_Pos)
#define CPR_I2S0_BUSY_EN                  CPR_I2S0_BUSY_Msk
#define CPR_I2S0_BUSY_CLR_Pos             1U
#define CPR_I2S0_BUSY_CLR_Msk             (0x1U << CPR_I2S0_BUSY_CLR_Pos)
#define CPR_I2S0_BUSY_CLR_EN              CPR_I2S0_BUSY_CLR_Msk

/* I2S1_BUSY_REG, Offset: 0x1C */
#define CPR_I2S1_BUSY_Pos                 0U
#define CPR_I2S1_BUSY_Msk                 (0x1U << CPR_I2S1_BUSY_Pos)
#define CPR_I2S1_BUSY_EN                  CPR_I2S1_BUSY_Msk
#define CPR_I2S1_BUSY_CLR_Pos             1U
#define CPR_I2S1_BUSY_CLR_Msk             (0x1U << CPR_I2S1_BUSY_CLR_Pos)
#define CPR_I2S1_BUSY_CLR_EN              CPR_I2S1_BUSY_CLR_Msk

/* I2S2_BUSY_REG, Offset: 0x20 */
#define CPR_I2S2_BUSY_Pos                 0U
#define CPR_I2S2_BUSY_Msk                 (0x1U << CPR_I2S2_BUSY_Pos)
#define CPR_I2S2_BUSY_EN                  CPR_I2S2_BUSY_Msk
#define CPR_I2S2_BUSY_CLR_Pos             1U
#define CPR_I2S2_BUSY_CLR_Msk             (0x1U << CPR_I2S2_BUSY_CLR_Pos)
#define CPR_I2S2_BUSY_CLR_EN              CPR_I2S2_BUSY_CLR_Msk

/* I2SIN_BUSY_REG, Offset: 0x24 */
#define CPR_I2SIN_BUSY_Pos                0U
#define CPR_I2SIN_BUSY_Msk                (0x1U << CPR_I2SIN_BUSY_Pos)
#define CPR_I2SIN_BUSY_EN                 CPR_I2SIN_BUSY_Msk
#define CPR_I2SIN_BUSY_CLR_Pos            1U
#define CPR_I2SIN_BUSY_CLR_Msk            (0x1U << CPR_I2SIN_BUSY_CLR_Pos)
#define CPR_I2SIN_BUSY_CLR_EN             CPR_I2SIN_BUSY_CLR_Msk

/* SPDIF0_BUSY_REG, Offset: 0x28 */
#define CPR_SPDIF0_BUSY_Pos               0U
#define CPR_SPDIF0_BUSY_Msk               (0x1U << CPR_SPDIF0_BUSY_Pos)
#define CPR_SPDIF0_BUSY_EN                CPR_SPDIF0_BUSY_Msk
#define CPR_SPDIF0_BUSY_CLR_Pos           1U
#define CPR_SPDIF0_BUSY_CLR_Msk           (0x1U << CPR_SPDIF0_BUSY_CLR_Pos)
#define CPR_SPDIF0_BUSY_CLR_EN            CPR_SPDIF0_BUSY_CLR_Msk

/* SPDIF1_BUSY_REG, Offset: 0x2C */
#define CPR_SPDIF1_BUSY_Pos               0U
#define CPR_SPDIF1_BUSY_Msk               (0x1U << CPR_SPDIF1_BUSY_Pos)
#define CPR_SPDIF1_BUSY_EN                CPR_SPDIF1_BUSY_Msk
#define CPR_SPDIF1_BUSY_CLR_Pos           1U
#define CPR_SPDIF1_BUSY_CLR_Msk           (0x1U << CPR_SPDIF1_BUSY_CLR_Pos)
#define CPR_SPDIF1_BUSY_CLR_EN            CPR_SPDIF1_BUSY_CLR_Msk

/* TDM_BUSY_REG, Offset: 0x30 */
#define CPR_TDM_BUSY_Pos                  0U
#define CPR_TDM_BUSY_Msk                  (0x1U << CPR_TDM_BUSY_Pos)
#define CPR_TDM_BUSY_EN                   CPR_TDM_BUSY_Msk
#define CPR_TDM_BUSY_CLR_Pos              1U
#define CPR_TDM_BUSY_CLR_Msk              (0x1U << CPR_TDM_BUSY_CLR_Pos)
#define CPR_TDM_BUSY_CLR_EN               CPR_TDM_BUSY_CLR_Msk

/* IIC0_BUSY_REG, Offset: 0x34 */
#define CPR_IIC0_BUSY_Pos                 0U
#define CPR_IIC0_BUSY_Msk                 (0x1U << CPR_IIC0_BUSY_Pos)
#define CPR_IIC0_BUSY_EN                  CPR_IIC0_BUSY_Msk
#define CPR_IIC0_BUSY_CLR_Pos             1U
#define CPR_IIC0_BUSY_CLR_Msk             (0x1U << CPR_IIC0_BUSY_CLR_Pos)
#define CPR_IIC0_BUSY_CLR_EN              CPR_IIC0_BUSY_CLR_Msk

/* IIC1_BUSY_REG, Offset: 0x38 */
#define CPR_IIC1_BUSY_Pos                 0U
#define CPR_IIC1_BUSY_Msk                 (0x1U << CPR_IIC1_BUSY_Pos)
#define CPR_IIC1_BUSY_EN                  CPR_IIC1_BUSY_Msk
#define CPR_IIC1_BUSY_CLR_Pos             1U
#define CPR_IIC1_BUSY_CLR_Msk             (0x1U << CPR_IIC1_BUSY_CLR_Pos)
#define CPR_IIC1_BUSY_CLR_EN              CPR_IIC1_BUSY_CLR_Msk

/* UART_BUSY_REG, Offset: 0x3C */
#define CPR_UART_BUSY_Pos                 0U
#define CPR_UART_BUSY_Msk                 (0x1U << CPR_UART_BUSY_Pos)
#define CPR_UART_BUSY_EN                  CPR_UART_BUSY_Msk
#define CPR_UART_BUSY_CLR_Pos             1U
#define CPR_UART_BUSY_CLR_Msk             (0x1U << CPR_UART_BUSY_CLR_Pos)
#define CPR_UART_BUSY_CLR_EN              CPR_UART_BUSY_CLR_Msk

/* TIMER_BUSY_REG, Offset: 0x40 */
#define CPR_TIMER_BUSY_Pos                0U
#define CPR_TIMER_BUSY_Msk                (0x1U << CPR_TIMER_BUSY_Pos)
#define CPR_TIMER_BUSY_EN                 CPR_TIMER_BUSY_Msk
#define CPR_TIMER_BUSY_CLR_Pos            1U
#define CPR_TIMER_BUSY_CLR_Msk            (0x1U << CPR_TIMER_BUSY_CLR_Pos)
#define CPR_TIMER_BUSY_CLR_EN             CPR_TIMER_BUSY_CLR_Msk

/* VAD_BUSY_REG, Offset: 0x44 */
#define CPR_VAD_BUSY_Pos                  0U
#define CPR_VAD_BUSY_Msk                  (0x1U << CPR_VAD_BUSY_Pos)
#define CPR_VAD_BUSY_EN                   CPR_VAD_BUSY_Msk
#define CPR_VAD_BUSY_CLR_Pos              1U
#define CPR_VAD_BUSY_CLR_Msk              (0x1U << CPR_VAD_BUSY_CLR_Pos)
#define CPR_VAD_BUSY_CLR_EN               CPR_VAD_BUSY_CLR_Msk

/* GPIO_BUSY_REG, Offset: 0x48 */
#define CPR_GPIO_BUSY_Pos                 0U
#define CPR_GPIO_BUSY_Msk                 (0x1U << CPR_GPIO_BUSY_Pos)
#define CPR_GPIO_BUSY_EN                  CPR_GPIO_BUSY_Msk
#define CPR_GPIO_BUSY_CLR_Pos             1U
#define CPR_GPIO_BUSY_CLR_Msk             (0x1U << CPR_GPIO_BUSY_CLR_Pos)
#define CPR_GPIO_BUSY_CLR_EN              CPR_GPIO_BUSY_CLR_Msk

/* DMA_CH1_BUSY_REG, Offset: 0x4C */
#define CPR_DMA_CH1_BUSY_Pos              0U
#define CPR_DMA_CH1_BUSY_Msk              (0x1U << CPR_DMA_CH1_BUSY_Pos)
#define CPR_DMA_CH1_BUSY_EN               CPR_DMA_CH1_BUSY_Msk
#define CPR_DMA_CH1_BUSY_CLR_Pos          1U
#define CPR_DMA_CH1_BUSY_CLR_Msk          (0x1U << CPR_DMA_CH1_BUSY_CLR_Pos)
#define CPR_DMA_CH1_BUSY_CLR_EN           CPR_DMA_CH1_BUSY_CLR_Msk

/* DMA_CH2_BUSY_REG, Offset: 0x50 */
#define CPR_DMA_CH2_BUSY_Pos              0U
#define CPR_DMA_CH2_BUSY_Msk              (0x1U << CPR_DMA_CH2_BUSY_Pos)
#define CPR_DMA_CH2_BUSY_EN               CPR_DMA_CH2_BUSY_Msk
#define CPR_DMA_CH2_BUSY_CLR_Pos          1U
#define CPR_DMA_CH2_BUSY_CLR_Msk          (0x1U << CPR_DMA_CH2_BUSY_CLR_Pos)
#define CPR_DMA_CH2_BUSY_CLR_EN           CPR_DMA_CH2_BUSY_CLR_Msk

/* DMA_CH3_BUSY_REG, Offset: 0x54 */
#define CPR_DMA_CH3_BUSY_Pos              0U
#define CPR_DMA_CH3_BUSY_Msk              (0x1U << CPR_DMA_CH3_BUSY_Pos)
#define CPR_DMA_CH3_BUSY_EN               CPR_DMA_CH3_BUSY_Msk
#define CPR_DMA_CH3_BUSY_CLR_Pos          1U
#define CPR_DMA_CH3_BUSY_CLR_Msk          (0x1U << CPR_DMA_CH3_BUSY_CLR_Pos)
#define CPR_DMA_CH3_BUSY_CLR_EN           CPR_DMA_CH3_BUSY_CLR_Msk

/* DMA_CH4_BUSY_REG, Offset: 0x58 */
#define CPR_DMA_CH4_BUSY_Pos              0U
#define CPR_DMA_CH4_BUSY_Msk              (0x1U << CPR_DMA_CH4_BUSY_Pos)
#define CPR_DMA_CH4_BUSY_EN               CPR_DMA_CH4_BUSY_Msk
#define CPR_DMA_CH4_BUSY_CLR_Pos          1U
#define CPR_DMA_CH4_BUSY_CLR_Msk          (0x1U << CPR_DMA_CH4_BUSY_CLR_Pos)
#define CPR_DMA_CH4_BUSY_CLR_EN           CPR_DMA_CH4_BUSY_CLR_Msk

/* DMA_CH5_BUSY_REG, Offset: 0x5C */
#define CPR_DMA_CH5_BUSY_Pos              0U
#define CPR_DMA_CH5_BUSY_Msk              (0x1U << CPR_DMA_CH5_BUSY_Pos)
#define CPR_DMA_CH5_BUSY_EN               CPR_DMA_CH5_BUSY_Msk
#define CPR_DMA_CH5_BUSY_CLR_Pos          1U
#define CPR_DMA_CH5_BUSY_CLR_Msk          (0x1U << CPR_DMA_CH5_BUSY_CLR_Pos)
#define CPR_DMA_CH5_BUSY_CLR_EN           CPR_DMA_CH5_BUSY_CLR_Msk

/* DMA_CH6_BUSY_REG, Offset: 0x60 */
#define CPR_DMA_CH6_BUSY_Pos              0U
#define CPR_DMA_CH6_BUSY_Msk              (0x1U << CPR_DMA_CH6_BUSY_Pos)
#define CPR_DMA_CH6_BUSY_EN               CPR_DMA_CH6_BUSY_Msk
#define CPR_DMA_CH6_BUSY_CLR_Pos          1U
#define CPR_DMA_CH6_BUSY_CLR_Msk          (0x1U << CPR_DMA_CH6_BUSY_CLR_Pos)
#define CPR_DMA_CH6_BUSY_CLR_EN           CPR_DMA_CH6_BUSY_CLR_Msk

/* DMA_CH7_BUSY_REG, Offset: 0x64 */
#define CPR_DMA_CH7_BUSY_Pos              0U
#define CPR_DMA_CH7_BUSY_Msk              (0x1U << CPR_DMA_CH7_BUSY_Pos)
#define CPR_DMA_CH7_BUSY_EN               CPR_DMA_CH7_BUSY_Msk
#define CPR_DMA_CH7_BUSY_CLR_Pos          1U
#define CPR_DMA_CH7_BUSY_CLR_Msk          (0x1U << CPR_DMA_CH7_BUSY_CLR_Pos)
#define CPR_DMA_CH7_BUSY_CLR_EN           CPR_DMA_CH7_BUSY_CLR_Msk

/* DMA_CH8_BUSY_REG, Offset: 0x68 */
#define CPR_DMA_CH8_BUSY_Pos              0U
#define CPR_DMA_CH8_BUSY_Msk              (0x1U << CPR_DMA_CH8_BUSY_Pos)
#define CPR_DMA_CH8_BUSY_EN               CPR_DMA_CH8_BUSY_Msk
#define CPR_DMA_CH8_BUSY_CLR_Pos          1U
#define CPR_DMA_CH8_BUSY_CLR_Msk          (0x1U << CPR_DMA_CH8_BUSY_CLR_Pos)
#define CPR_DMA_CH8_BUSY_CLR_EN           CPR_DMA_CH8_BUSY_CLR_Msk

/* DMA_CH9_BUSY_REG, Offset: 0x6C */
#define CPR_DMA_CH9_BUSY_Pos              0U
#define CPR_DMA_CH9_BUSY_Msk              (0x1U << CPR_DMA_CH9_BUSY_Pos)
#define CPR_DMA_CH9_BUSY_EN               CPR_DMA_CH9_BUSY_Msk
#define CPR_DMA_CH9_BUSY_CLR_Pos          1U
#define CPR_DMA_CH9_BUSY_CLR_Msk          (0x1U << CPR_DMA_CH9_BUSY_CLR_Pos)
#define CPR_DMA_CH9_BUSY_CLR_EN           CPR_DMA_CH9_BUSY_CLR_Msk

/* DMA_CH10_BUSY_REG, Offset: 0x70 */
#define CPR_DMA_CH10_BUSY_Pos             0U
#define CPR_DMA_CH10_BUSY_Msk             (0x1U << CPR_DMA_CH10_BUSY_Pos)
#define CPR_DMA_CH10_BUSY_EN              CPR_DMA_CH10_BUSY_Msk
#define CPR_DMA_CH10_BUSY_CLR_Pos         1U
#define CPR_DMA_CH10_BUSY_CLR_Msk         (0x1U << CPR_DMA_CH10_BUSY_CLR_Pos)
#define CPR_DMA_CH10_BUSY_CLR_EN          CPR_DMA_CH10_BUSY_CLR_Msk

/* SYS_STATUS_REG, Offset: 0x74 */
#define CPR_CPU_RST_ST_Pos                0U
#define CPR_CPU_RST_ST_Msk                (0x1U << CPR_CPU_RST_ST_Pos)
#define CPR_CPU_RST_ST_EN                 CPR_CPU_RST_ST_Msk
#define CPR_CPU_HRST_ST_Pos               1U
#define CPR_CPU_HRST_ST_Msk               (0x1U << CPR_CPU_HRST_ST_Pos)
#define CPR_CPU_HRST_ST_EN                CPR_CPU_HRST_ST_Msk
#define CPR_SYS_HRST_ST_Pos               2U
#define CPR_SYS_HRST_ST_Msk               (0x1U << CPR_SYS_HRST_ST_Pos)
#define CPR_SYS_HRST_ST_EN                CPR_SYS_HRST_ST_Msk
#define CPR_SYS_CPU_LPMD_ST_Pos           4U
#define CPR_SYS_CPU_LPMD_ST_Msk           (0x3U << CPR_SYS_CPU_LPMD_ST_Pos)
#define CPR_SYS_CPU_LPMD_ST_SEL(X)        (X << CPR_SYS_CPU_LPMD_ST_Pos)
#define CPR_SYS_CPR_CP_ST_Pos             8U
#define CPR_SYS_CPR_CP_ST_Msk             (0x3U << CPR_SYS_CPR_CP_ST_Pos)
#define CPR_SYS_CPR_CP_ST_SEL(X)          (X << CPR_SYS_CPR_CP_ST_Pos)
#define CPR_SYS_CPR_FSM_ST_Pos            12U
#define CPR_SYS_CPR_FSM_ST_Msk            (0xFU << CPR_SYS_CPR_FSM_ST_Pos)
#define CPR_SYS_CPR_FSM_ST_SEL(X)         (X << CPR_SYS_CPR_FSM_ST_Pos)
#define CPR_SYS_WK_SRC_ST_Pos             16U
#define CPR_SYS_WK_SRC_ST_Msk             (0xFU << CPR_SYS_WK_SRC_ST_Pos)
#define CPR_SYS_WK_SRC_ST_SEL(X)          (X << CPR_SYS_WK_SRC_ST_Pos)

/* SYS_CTRL_REG, Offset: 0x78 */
#define CPR_CP_CPR_REQ_Pos                0U
#define CPR_CP_CPR_REQ_Msk                (0x1U << CPR_CP_CPR_REQ_Pos)
#define CPR_CP_CPR_REQ_EN                 CPR_CP_CPR_REQ_Msk
#define CPR_CP_CPR_ST_Pos                 1U
#define CPR_CP_CPR_ST_Msk                 (0x3U << CPR_CP_CPR_ST_Pos)
#define CPR_CP_CPR_ST_SEL(X)              (X << CPR_CP_CPR_ST_Pos)
#define CPR_SYSBUS_CPU_P_Pos              4U
#define CPR_SYSBUS_CPU_P_Msk              (0x3U << CPR_SYSBUS_CPU_P_Pos)
#define CPR_SYSBUS_CPU_P_SEL(X)           (X << CPR_SYSBUS_CPU_P_Pos)
#define CPR_SYSBUS_DMA_P_Pos              6U
#define CPR_SYSBUS_DMA_P_Msk              (0x3U << CPR_SYSBUS_DMA_P_Pos)
#define CPR_SYSBUS_DMA_P_SEL(X)           (X << CPR_SYSBUS_DMA_P_Pos)
#define CPR_SYSBUS_APM_P_Pos              10U
#define CPR_SYSBUS_APM_P_Msk              (0x3U << CPR_SYSBUS_APM_P_Pos)
#define CPR_SYSBUS_APM_P_SEL(X)           (X << CPR_SYSBUS_APM_P_Pos)
#define CPR_SYSBUS_SRAM0_P_Pos            13U
#define CPR_SYSBUS_SRAM0_P_Msk            (0x3U << CPR_SYSBUS_SRAM0_P_Pos)
#define CPR_SYSBUS_SRAM0_P_SEL(X)         (X << CPR_SYSBUS_SRAM0_P_Pos)
#define CPR_SYSBUS_SRAM1_P_Pos            16U
#define CPR_SYSBUS_SRAM1_P_Msk            (0x3U << CPR_SYSBUS_SRAM1_P_Pos)
#define CPR_SYSBUS_SRAM1_P_SEL(X)         (X << CPR_SYSBUS_SRAM1_P_Pos)
#define CPR_SYSBUS_PERI_P_Pos             19U
#define CPR_SYSBUS_PERI_P_Msk             (0x3U << CPR_SYSBUS_PERI_P_Pos)
#define CPR_SYSBUS_PERI_P_SEL(X)          (X << CPR_SYSBUS_PERI_P_Pos)
#define CPR_SYSBUS_DDR_P_Pos              22U
#define CPR_SYSBUS_DDR_P_Msk              (0x3U << CPR_SYSBUS_DDR_P_Pos)
#define CPR_SYSBUS_DDR_P_SEL(X)           (X << CPR_SYSBUS_DDR_P_Pos)
#define CPR_SYSBUS_APS_P_Pos              25U
#define CPR_SYSBUS_APS_P_Msk              (0x3U << CPR_SYSBUS_APS_P_Pos)
#define CPR_SYSBUS_APS_P_SEL(X)           (X << CPR_SYSBUS_APS_P_Pos)
#define CPR_WK_INTR_MASK_Pos              28U
#define CPR_WK_INTR_MASK_Msk              (0x3U << CPR_WK_INTR_MASK_Pos)
#define CPR_WK_INTR_MASK_SEL(X)           (X << CPR_WK_INTR_MASK_Pos)

/* SYS_ID_REG, Offset: 0xA0 */


/* RESERVD1_REG, Offset: 0xA4 */


/* CPR_SYS_CLK_DIV_REG, Offset: 0x0 */
static inline void wj_cpr_sys_clk_sel_ehs(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_SYS_CLK_DIV_REG |= CPR_SYS_CLK_SEL_EN;
}
static inline void wj_cpr_sys_clk_sel_ihs(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_SYS_CLK_DIV_REG &= ~CPR_SYS_CLK_SEL_EN;
}
static inline void wj_cpr_lpmd_sysclk_offpll_chg24m(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_SYS_CLK_DIV_REG &= ~CPR_LPMD_SYSCLK_SEL_Msk;
    cpr_base->CPR_SYS_CLK_DIV_REG |= CPR_LPMD_SYSCLK_SEL(0);
}
static inline void wj_cpr_lpmd_sysclk_offpll_offbus(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_SYS_CLK_DIV_REG &= ~CPR_LPMD_SYSCLK_SEL_Msk;
    cpr_base->CPR_SYS_CLK_DIV_REG |= CPR_LPMD_SYSCLK_SEL(1);
}
static inline void wj_cpr_lpmd_sysclk_onpll_keepclk(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_SYS_CLK_DIV_REG &= ~CPR_LPMD_SYSCLK_SEL_Msk;
    cpr_base->CPR_SYS_CLK_DIV_REG |= CPR_LPMD_SYSCLK_SEL(2);
}
static inline void wj_cpr_cpu_div_sel(wj_cpr_regs_t *cpr_base, uint32_t div)
{
    cpr_base->CPR_SYS_CLK_DIV_REG &= ~CPR_CPU_DIV_SEL_Msk;
    cpr_base->CPR_SYS_CLK_DIV_REG |= CPR_CPU_DIV_SEL(div);
}
static inline void wj_cpr_sys_div_sel(wj_cpr_regs_t *cpr_base, uint32_t div)
{
    cpr_base->CPR_SYS_CLK_DIV_REG &= ~CPR_SYS_DIV_SEL_Msk;
    cpr_base->CPR_SYS_CLK_DIV_REG |= CPR_SYS_DIV_SEL(div);
}
static inline void wj_cpr_ahb_div_sel(wj_cpr_regs_t *cpr_base, uint32_t div)
{
    cpr_base->CPR_SYS_CLK_DIV_REG &= ~CPR_AHB_DIV_SEL_Msk;
    cpr_base->CPR_SYS_CLK_DIV_REG |= CPR_AHB_DIV_SEL(div);
}
static inline void wj_cpr_apb_div_sel(wj_cpr_regs_t *cpr_base, uint32_t div)
{
    cpr_base->CPR_SYS_CLK_DIV_REG &= ~CPR_APB_DIV_SEL_Msk;
    cpr_base->CPR_SYS_CLK_DIV_REG |= CPR_APB_DIV_SEL(div);
}
static inline uint32_t wj_cpr_get_cpu_div(wj_cpr_regs_t *cpr_base)
{
    return ((cpr_base->CPR_SYS_CLK_DIV_REG & CPR_CPU_DIV_SEL_Msk) >> CPR_CPU_DIV_SEL_Pos);
}
static inline uint32_t wj_cpr_get_sys_div(wj_cpr_regs_t *cpr_base)
{
    return ((cpr_base->CPR_SYS_CLK_DIV_REG & CPR_SYS_DIV_SEL_Msk) >> CPR_SYS_DIV_SEL_Pos);
}
static inline uint32_t wj_cpr_get_ahb_div(wj_cpr_regs_t *cpr_base)
{
    return ((cpr_base->CPR_SYS_CLK_DIV_REG & CPR_AHB_DIV_SEL_Msk) >> CPR_AHB_DIV_SEL_Pos);
}
static inline uint32_t wj_cpr_get_apb_div(wj_cpr_regs_t *cpr_base)
{
    return ((cpr_base->CPR_SYS_CLK_DIV_REG & CPR_APB_DIV_SEL_Msk) >> CPR_APB_DIV_SEL_Pos);
}
/* PERI_DIV_SEL_REG, Offset: 0x4 */
static inline void wj_cpr_uart_div_sel(wj_cpr_regs_t *cpr_base, uint32_t div)
{
    cpr_base->CPR_PERI_DIV_SEL_REG &= ~CPR_UART_DIV_SEL_Msk;
    cpr_base->CPR_PERI_DIV_SEL_REG |= CPR_UART_DIV_SEL(div);
}
static inline void wj_cpr_audio_div_sel(wj_cpr_regs_t *cpr_base, uint32_t div)
{
    cpr_base->CPR_PERI_DIV_SEL_REG &= ~CPR_AUDIO_DIV_SEL_Msk;
    cpr_base->CPR_PERI_DIV_SEL_REG |= CPR_AUDIO_DIV_SEL(div);
}
static inline void wj_cpr_vad_div_sel(wj_cpr_regs_t *cpr_base, uint32_t div)
{
    cpr_base->CPR_PERI_DIV_SEL_REG &= ~CPR_VAD_DIV_SEL_Msk;
    cpr_base->CPR_PERI_DIV_SEL_REG |= CPR_VAD_DIV_SEL(div);
}
static inline void wj_cpr_gpio_clk_sel(wj_cpr_regs_t *cpr_base, uint32_t div)
{
    cpr_base->CPR_PERI_DIV_SEL_REG &= ~CPR_GPIO_CLK_SEL_Msk;
    cpr_base->CPR_PERI_DIV_SEL_REG |= CPR_GPIO_CLK_SEL(div);
}
static inline uint32_t wj_cpr_get_uart_div(wj_cpr_regs_t *cpr_base)
{
    return ((cpr_base->CPR_PERI_DIV_SEL_REG & CPR_UART_DIV_SEL_Msk) >> CPR_UART_DIV_SEL_Pos);
}
static inline uint32_t wj_cpr_get_audio_div(wj_cpr_regs_t *cpr_base)
{
    return ((cpr_base->CPR_PERI_DIV_SEL_REG & CPR_AUDIO_DIV_SEL_Msk) >> CPR_AUDIO_DIV_SEL_Pos);
}
static inline uint32_t wj_cpr_get_vad_div(wj_cpr_regs_t *cpr_base)
{
    return ((cpr_base->CPR_PERI_DIV_SEL_REG & CPR_VAD_DIV_SEL_Msk) >> CPR_VAD_DIV_SEL_Pos);
}
/* PERI_CLK_SEL_REG, Offset: 0x8 */
static inline void wj_cpr_i2s0_src_sel_24m(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_PERI_CLK_SEL_REG &= ~CPR_I2S0_SRC_SEL_Msk;
    cpr_base->CPR_PERI_CLK_SEL_REG |= CPR_I2S0_SRC_SEL(1);
}
static inline void wj_cpr_i2s0_src_sel_divclk(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_PERI_CLK_SEL_REG &= ~CPR_I2S0_SRC_SEL_Msk;
    cpr_base->CPR_PERI_CLK_SEL_REG |= CPR_I2S0_SRC_SEL(0);
}
static inline void wj_cpr_i2s1_src_sel_24m(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_PERI_CLK_SEL_REG &= ~CPR_I2S1_SRC_SEL_Msk;
    cpr_base->CPR_PERI_CLK_SEL_REG |= CPR_I2S1_SRC_SEL(1);
}
static inline void wj_cpr_i2s1_src_sel_divclk(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_PERI_CLK_SEL_REG &= ~CPR_I2S1_SRC_SEL_Msk;
    cpr_base->CPR_PERI_CLK_SEL_REG |= CPR_I2S1_SRC_SEL(0);
}
static inline void wj_cpr_i2s2_src_sel_24m(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_PERI_CLK_SEL_REG &= ~CPR_I2S2_SRC_SEL_Msk;
    cpr_base->CPR_PERI_CLK_SEL_REG |= CPR_I2S2_SRC_SEL(1);
}
static inline void wj_cpr_i2s2_src_sel_divclk(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_PERI_CLK_SEL_REG &= ~CPR_I2S2_SRC_SEL_Msk;
    cpr_base->CPR_PERI_CLK_SEL_REG |= CPR_I2S2_SRC_SEL(0);
}
static inline void wj_cpr_i2sin_src_sel_24m(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_PERI_CLK_SEL_REG &= ~CPR_I2SIN_SRC_SEL_Msk;
    cpr_base->CPR_PERI_CLK_SEL_REG |= CPR_I2SIN_SRC_SEL(1);
}
static inline void wj_cpr_i2sin_src_sel_divclk(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_PERI_CLK_SEL_REG &= ~CPR_I2SIN_SRC_SEL_Msk;
    cpr_base->CPR_PERI_CLK_SEL_REG |= CPR_I2SIN_SRC_SEL(0);
}
static inline void wj_cpr_tdm_src_sel_24m(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_PERI_CLK_SEL_REG &= ~CPR_TDM_SRC_SEL_Msk;
    cpr_base->CPR_PERI_CLK_SEL_REG |= CPR_TDM_SRC_SEL(1);
}
static inline void wj_cpr_tdm_src_sel_divclk(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_PERI_CLK_SEL_REG &= ~CPR_TDM_SRC_SEL_Msk;
    cpr_base->CPR_PERI_CLK_SEL_REG |= CPR_TDM_SRC_SEL(0);
}
static inline void wj_cpr_vad_src_sel_24m(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_PERI_CLK_SEL_REG &= ~CPR_VAD_SRC_SEL_Msk;
    cpr_base->CPR_PERI_CLK_SEL_REG |= CPR_VAD_SRC_SEL(1);
}
static inline void wj_cpr_vad_src_sel_divclk(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_PERI_CLK_SEL_REG &= ~CPR_VAD_SRC_SEL_Msk;
    cpr_base->CPR_PERI_CLK_SEL_REG |= CPR_VAD_SRC_SEL(0);
}

/* PERI_CTRL_REG, Offset: 0xC */
static inline void wj_cpr_cnt1_2_link_en(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_PERI_CTRL_REG |= CPR_CNT1_2_LINK_EN;
}
static inline void wj_cpr_cnt1_2_link_dis(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_PERI_CTRL_REG &= ~CPR_CNT1_2_LINK_EN;
}
static inline void wj_cpr_cnt2_3_link_en(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_PERI_CTRL_REG |= CPR_CNT2_3_LINK_EN;
}
static inline void wj_cpr_cnt2_3_link_dis(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_PERI_CTRL_REG &= ~CPR_CNT2_3_LINK_EN;
}
static inline void wj_cpr_cnt3_4_link_en(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_PERI_CTRL_REG |= CPR_CNT3_4_LINK_EN;
}
static inline void wj_cpr_cnt3_4_link_dis(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_PERI_CTRL_REG &= ~CPR_CNT3_4_LINK_EN;
}
static inline void wj_cpr_cnt1_pause_en(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_PERI_CTRL_REG |= CPR_CNT1_PAUSE_EN;
}
static inline void wj_cpr_cnt1_pause_dis(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_PERI_CTRL_REG &= ~CPR_CNT1_PAUSE_EN;
}
static inline void wj_cpr_cnt2_pause_en(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_PERI_CTRL_REG |= CPR_CNT2_PAUSE_EN;
}
static inline void wj_cpr_cnt2_pause_dis(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_PERI_CTRL_REG &= ~CPR_CNT2_PAUSE_EN;
}
static inline void wj_cpr_cnt3_pause_en(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_PERI_CTRL_REG |= CPR_CNT3_PAUSE_EN;
}
static inline void wj_cpr_cnt3_pause_dis(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_PERI_CTRL_REG &= ~CPR_CNT3_PAUSE_EN;
}
static inline void wj_cpr_cnt4_pause_en(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_PERI_CTRL_REG |= CPR_CNT4_PAUSE_EN;
}
static inline void wj_cpr_cnt4_pause_dis(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_PERI_CTRL_REG &= ~CPR_CNT4_PAUSE_EN;
}
static inline void wj_cpr_wdt_pause_en(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_PERI_CTRL_REG |= CPR_WDT_PAUSE_EN;
}
static inline void wj_cpr_wdt_pause_dis(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_PERI_CTRL_REG &= ~CPR_WDT_PAUSE_EN;
}
static inline void wj_cpr_vad_i2sin_sync_pause_en(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_PERI_CTRL_REG |= CPR_VAD_I2SIN_SYNC_EN;
}
static inline void wj_cpr_vad_i2sin_sync_pause_dis(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_PERI_CTRL_REG &= ~CPR_VAD_I2SIN_SYNC_EN;
}
static inline void wj_cpr_i2s_sync_pause_en(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_PERI_CTRL_REG |= CPR_I2S_SYNC_EN;
}
static inline void wj_cpr_i2s_sync_pause_dis(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_PERI_CTRL_REG &= ~CPR_I2S_SYNC_EN;
}
static inline void wj_cpr_spdif_sync_pause_en(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_PERI_CTRL_REG |= CPR_SPDIF_SYNC_EN;
}
static inline void wj_cpr_spdif_sync_pause_dis(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_PERI_CTRL_REG &= ~CPR_SPDIF_SYNC_EN;
}
static inline void wj_cpr_bsm_sel_cpu(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_PERI_CTRL_REG &= ~CPR_BSM_SEL_Msk;
    cpr_base->CPR_PERI_CTRL_REG |= CPR_BSM_SEL_CPU;
}
static inline void wj_cpr_bsm_sel_dma(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_PERI_CTRL_REG &= ~CPR_BSM_SEL_Msk;
    cpr_base->CPR_PERI_CTRL_REG |= CPR_BSM_SEL_DMA;
}
static inline void wj_cpr_bsm_sel_cp2ap(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_PERI_CTRL_REG &= ~CPR_BSM_SEL_Msk;
    cpr_base->CPR_PERI_CTRL_REG |= CPR_BSM_SEL_CP2AP;
}

/* IP_CG_REG, Offset: 0x10 */
static inline void wj_cpr_cpu_cg_en(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IP_CG_REG |= CPR_CPU_CG_EN;
}
static inline void wj_cpr_cpu_cg_dis(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IP_CG_REG &= ~CPR_CPU_CG_EN;
}
static inline void wj_cpr_sram0_cg_en(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IP_CG_REG |= CPR_SRAM0_CG_EN;
}
static inline void wj_cpr_sram0_cg_dis(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IP_CG_REG &= ~CPR_SRAM0_CG_EN;
}
static inline void wj_cpr_sram1_cg_en(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IP_CG_REG |= CPR_SRAM1_CG_EN;
}
static inline void wj_cpr_sram1_cg_dis(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IP_CG_REG &= ~CPR_SRAM1_CG_EN;
}
static inline void wj_cpr_dma_cg_en(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IP_CG_REG |= CPR_DMA_CG_EN;
}
static inline void wj_cpr_dma_cg_dis(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IP_CG_REG &= ~CPR_DMA_CG_EN;
}
static inline void wj_cpr_bsm_cg_en(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IP_CG_REG |= CPR_BSM_CG_EN;
}
static inline void wj_cpr_bsm_cg_dis(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IP_CG_REG &= ~CPR_BSM_CG_EN;
}
static inline void wj_cpr_timer_cg_en(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IP_CG_REG |= CPR_TIMER_CG_EN;
}
static inline void wj_cpr_timer_cg_dis(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IP_CG_REG &= ~CPR_TIMER_CG_EN;
}
static inline void wj_cpr_timer_cnt1_cg_en(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IP_CG_REG |= CPR_TIMER_CNT1_CG_EN;
}
static inline void wj_cpr_timer_cnt1_cg_dis(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IP_CG_REG &= ~CPR_TIMER_CNT1_CG_EN;
}
static inline void wj_cpr_timer_cnt2_cg_en(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IP_CG_REG |= CPR_TIMER_CNT2_CG_EN;
}
static inline void wj_cpr_timer_cnt2_cg_dis(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IP_CG_REG &= ~CPR_TIMER_CNT2_CG_EN;
}
static inline void wj_cpr_timer_cnt3_cg_en(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IP_CG_REG |= CPR_TIMER_CNT3_CG_EN;
}
static inline void wj_cpr_timer_cnt3_cg_dis(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IP_CG_REG &= ~CPR_TIMER_CNT3_CG_EN;
}
static inline void wj_cpr_timer_cnt4_cg_en(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IP_CG_REG |= CPR_TIMER_CNT4_CG_EN;
}
static inline void wj_cpr_timer_cnt4_cg_dis(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IP_CG_REG &= ~CPR_TIMER_CNT4_CG_EN;
}
static inline void wj_cpr_wdt_cg_en(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IP_CG_REG |= CPR_WDR_CG_EN;
}
static inline void wj_cpr_wdt_cg_dis(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IP_CG_REG &= ~CPR_WDR_CG_EN;
}
static inline void wj_cpr_usi0_cg_en(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IP_CG_REG |= CPR_USI0_CG_EN;
}
static inline void wj_cpr_usi0_cg_dis(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IP_CG_REG &= ~CPR_USI0_CG_EN;
}
static inline void wj_cpr_usi1_cg_en(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IP_CG_REG |= CPR_USI1_CG_EN;
}
static inline void wj_cpr_usi1_cg_dis(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IP_CG_REG &= ~CPR_USI1_CG_EN;
}
static inline void wj_cpr_uart_cg_en(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IP_CG_REG |= CPR_UART_CG_EN;
}
static inline void wj_cpr_uart_cg_dis(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IP_CG_REG &= ~CPR_UART_CG_EN;
}
static inline void wj_cpr_i2s0_cg_en(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IP_CG_REG |= CPR_I2S0_CG_EN;
}
static inline void wj_cpr_i2s0_cg_dis(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IP_CG_REG &= ~CPR_I2S0_CG_EN;
}
static inline void wj_cpr_i2s1_cg_en(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IP_CG_REG |= CPR_I2S1_CG_EN;
}
static inline void wj_cpr_i2s1_cg_dis(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IP_CG_REG &= ~CPR_I2S1_CG_EN;
}
static inline void wj_cpr_i2s2_cg_en(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IP_CG_REG |= CPR_I2S2_CG_EN;
}
static inline void wj_cpr_i2s2_cg_dis(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IP_CG_REG &= ~CPR_I2S2_CG_EN;
}
static inline void wj_cpr_i2sin_cg_en(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IP_CG_REG |= CPR_I2SIN_CG_EN;
}
static inline void wj_cpr_i2sin_cg_dis(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IP_CG_REG &= ~CPR_I2SIN_CG_EN;
}
static inline void wj_cpr_tdm_cg_en(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IP_CG_REG |= CPR_TDM_CG_EN;
}
static inline void wj_cpr_tdm_cg_dis(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IP_CG_REG &= ~CPR_TDM_CG_EN;
}
static inline void wj_cpr_gpio_cg_en(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IP_CG_REG |= CPR_GPIO_CG_EN;
}
static inline void wj_cpr_gpio_cg_dis(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IP_CG_REG &= ~CPR_GPIO_CG_EN;
}
static inline void wj_cpr_spdif0_cg_en(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IP_CG_REG |= CPR_SPDIF0_CG_EN;
}
static inline void wj_cpr_spdif0_cg_dis(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IP_CG_REG &= ~CPR_SPDIF0_CG_EN;
}
static inline void wj_cpr_spdif1_cg_en(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IP_CG_REG |= CPR_SPDIF1_CG_EN;
}
static inline void wj_cpr_spdif1_cg_dis(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IP_CG_REG &= ~CPR_SPDIF1_CG_EN;
}
static inline void wj_cpr_vad_cg_en(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IP_CG_REG |= CPR_VAD_CG_EN;
}
static inline void wj_cpr_vad_cg_dis(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IP_CG_REG &= ~CPR_VAD_CG_EN;
}

/* IP_RST_REG, Offset: 0x14 */
static inline void wj_cpr_dma_srst_n_en(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IP_RST_REG |= CPR_DMA_SRST_N_EN;
}
static inline void wj_cpr_dma_srst_n_dis(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IP_RST_REG &= ~CPR_DMA_SRST_N_EN;
}
static inline void wj_cpr_bsm_srst_n_en(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IP_RST_REG |= CPR_BSM_SRST_N_EN;
}
static inline void wj_cpr_bsm_srst_n_dis(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IP_RST_REG &= ~CPR_BSM_SRST_N_EN;
}
static inline void wj_cpr_timer_srst_n_en(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IP_RST_REG |= CPR_TIMER_SRST_N_EN;
}
static inline void wj_cpr_timer_srst_n_dis(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IP_RST_REG &= ~CPR_TIMER_SRST_N_EN;
}
static inline void wj_cpr_tim_cnt1_srst_n_en(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IP_RST_REG |= CPR_TIM_CNT1_SRST_N_EN;
}
static inline void wj_cpr_tim_cnt1_srst_n_dis(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IP_RST_REG &= ~CPR_TIM_CNT1_SRST_N_EN;
}
static inline void wj_cpr_tim_cnt2_srst_n_en(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IP_RST_REG |= CPR_TIM_CNT2_SRST_N_EN;
}
static inline void wj_cpr_tim_cnt2_srst_n_dis(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IP_RST_REG &= ~CPR_TIM_CNT2_SRST_N_EN;
}
static inline void wj_cpr_tim_cnt3_srst_n_en(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IP_RST_REG |= CPR_TIM_CNT3_SRST_N_EN;
}
static inline void wj_cpr_tim_cnt3_srst_n_dis(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IP_RST_REG &= ~CPR_TIM_CNT3_SRST_N_EN;
}
static inline void wj_cpr_tim_cnt4_srst_n_en(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IP_RST_REG |= CPR_TIM_CNT4_SRST_N_EN;
}
static inline void wj_cpr_tim_cnt4_srst_n_dis(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IP_RST_REG &= ~CPR_TIM_CNT4_SRST_N_EN;
}
static inline void wj_cpr_wdr_srst_n_en(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IP_RST_REG |= CPR_WDR_SRST_N_EN;
}
static inline void wj_cpr_wdr_srst_n_dis(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IP_RST_REG &= ~CPR_WDR_SRST_N_EN;
}
static inline void wj_cpr_usi0_srst_n_en(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IP_RST_REG |= CPR_USI0_SRST_N_EN;
}
static inline void wj_cpr_usi0_srst_n_dis(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IP_RST_REG &= ~CPR_USI0_SRST_N_EN;
}
static inline void wj_cpr_usi1_srst_n_en(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IP_RST_REG |= CPR_USI1_SRST_N_EN;
}
static inline void wj_cpr_usi1_srst_n_dis(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IP_RST_REG &= ~CPR_USI1_SRST_N_EN;
}
static inline void wj_cpr_uart_srst_n_en(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IP_RST_REG |= CPR_UART_SRST_N_EN;
}
static inline void wj_cpr_uart_srst_n_dis(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IP_RST_REG &= ~CPR_UART_SRST_N_EN;
}
static inline void wj_cpr_i2s0_srst_n_en(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IP_RST_REG |= CPR_I2S0_SRST_N_EN;
}
static inline void wj_cpr_i2s0_srst_n_dis(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IP_RST_REG &= ~CPR_I2S0_SRST_N_EN;
}
static inline void wj_cpr_i2s1_srst_n_en(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IP_RST_REG |= CPR_I2S1_SRST_N_EN;
}
static inline void wj_cpr_i2s1_srst_n_dis(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IP_RST_REG &= ~CPR_I2S1_SRST_N_EN;
}
static inline void wj_cpr_i2s2_srst_n_en(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IP_RST_REG |= CPR_I2S2_SRST_N_EN;
}
static inline void wj_cpr_i2s2_srst_n_dis(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IP_RST_REG &= ~CPR_I2S2_SRST_N_EN;
}
static inline void wj_cpr_i2sin_srst_n_en(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IP_RST_REG |= CPR_I2SIN_SRST_N_EN;
}
static inline void wj_cpr_i2sin_srst_n_dis(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IP_RST_REG &= ~CPR_I2SIN_SRST_N_EN;
}
static inline void wj_cpr_tdm_srst_n_en(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IP_RST_REG |= CPR_TDM_SRST_N_EN;
}
static inline void wj_cpr_tdm_srst_n_dis(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IP_RST_REG &= ~CPR_TDM_SRST_N_EN;
}
static inline void wj_cpr_gpio_srst_n_en(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IP_RST_REG |= CPR_GPIO_SRST_N_EN;
}
static inline void wj_cpr_gpio_srst_n_dis(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IP_RST_REG &= ~CPR_GPIO_SRST_N_EN;
}
static inline void wj_cpr_spdif0_srst_n_en(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IP_RST_REG |= CPR_SPDIF0_SRST_N_EN;
}
static inline void wj_cpr_spdif0_srst_n_dis(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IP_RST_REG &= ~CPR_SPDIF0_SRST_N_EN;
}
static inline void wj_cpr_spdif1_srst_n_en(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IP_RST_REG |= CPR_SPDIF1_SRST_N_EN;
}
static inline void wj_cpr_spdif1_srst_n_dis(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IP_RST_REG &= ~CPR_SPDIF1_SRST_N_EN;
}
static inline void wj_cpr_vad_srst_n_en(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IP_RST_REG |= CPR_VAD_SRST_N_EN;
}
static inline void wj_cpr_vad_srst_n_dis(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IP_RST_REG &= ~CPR_VAD_SRST_N_EN;
}
static inline void wj_cpr_iomux_srst_n_en(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IP_RST_REG |= CPR_IOMUX_SRST_N_EN;
}
static inline void wj_cpr_iomux_srst_n_dis(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IP_RST_REG &= ~CPR_IOMUX_SRST_N_EN;
}

/* I2S0_BUSY_REG, Offset: 0x18 */
static inline uint32_t wj_cpr_i2s0_busy_sta(wj_cpr_regs_t *cpr_base)
{
    return ((cpr_base->CPR_I2S0_BUSY_REG & CPR_I2S0_BUSY_EN) ? 1U : 0U);
}
static inline void wj_cpr_i2s0_busy_clr(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_I2S0_BUSY_REG |= CPR_I2S0_BUSY_CLR_EN;
}

/* I2S1_BUSY_REG, Offset: 0x1C */
static inline uint32_t wj_cpr_i2s1_busy_sta(wj_cpr_regs_t *cpr_base)
{
    return ((cpr_base->CPR_I2S1_BUSY_REG & CPR_I2S1_BUSY_EN) ? 1U : 0U);
}
static inline void wj_cpr_i2s1_busy_clr(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_I2S1_BUSY_REG |= CPR_I2S1_BUSY_CLR_EN;
}

/* I2S2_BUSY_REG, Offset: 0x20 */
static inline uint32_t wj_cpr_i2s2_busy_sta(wj_cpr_regs_t *cpr_base)
{
    return ((cpr_base->CPR_I2S2_BUSY_REG & CPR_I2S2_BUSY_EN) ? 1U : 0U);
}
static inline void wj_cpr_i2s2_busy_clr(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_I2S2_BUSY_REG |= CPR_I2S2_BUSY_CLR_EN;
}

/* I2SIN_BUSY_REG, Offset: 0x24 */
static inline uint32_t wj_cpr_i2sin_busy_sta(wj_cpr_regs_t *cpr_base)
{
    return ((cpr_base->CPR_I2SIN_BUSY_REG & CPR_I2SIN_BUSY_EN) ? 1U : 0U);
}
static inline void wj_cpr_i2sin_busy_clr(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_I2SIN_BUSY_REG |= CPR_I2SIN_BUSY_CLR_EN;
}

/* SPDIF0_BUSY_REG, Offset: 0x28 */
static inline uint32_t wj_cpr_spdif0_busy_sta(wj_cpr_regs_t *cpr_base)
{
    return ((cpr_base->CPR_SPDIF0_BUSY_REG & CPR_SPDIF0_BUSY_EN) ? 1U : 0U);
}
static inline void wj_cpr_spdif0_busy_clr(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_SPDIF0_BUSY_REG |= CPR_SPDIF0_BUSY_CLR_EN;
}

/* SPDIF1_BUSY_REG, Offset: 0x2C */
static inline uint32_t wj_cpr_spdif1_busy_sta(wj_cpr_regs_t *cpr_base)
{
    return ((cpr_base->CPR_SPDIF1_BUSY_REG & CPR_SPDIF1_BUSY_EN) ? 1U : 0U);
}
static inline void wj_cpr_spdif1_busy_clr(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_SPDIF1_BUSY_REG |= CPR_SPDIF1_BUSY_CLR_EN;
}

/* TDM_BUSY_REG, Offset: 0x30 */
static inline uint32_t wj_cpr_tdm_busy_sta(wj_cpr_regs_t *cpr_base)
{
    return ((cpr_base->CPR_TDM_BUSY_REG & CPR_TDM_BUSY_EN) ? 1U : 0U);
}
static inline void wj_cpr_tdm_busy_clr(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_TDM_BUSY_REG |= CPR_TDM_BUSY_CLR_EN;
}

/* IIC0_BUSY_REG, Offset: 0x34 */
static inline uint32_t wj_cpr_iic0_busy_sta(wj_cpr_regs_t *cpr_base)
{
    return ((cpr_base->CPR_IIC0_BUSY_REG & CPR_IIC0_BUSY_EN) ? 1U : 0U);
}
static inline void wj_cpr_iic0_busy_clr(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IIC0_BUSY_REG |= CPR_IIC0_BUSY_CLR_EN;
}

/* IIC1_BUSY_REG, Offset: 0x38 */
static inline uint32_t wj_cpr_iic1_busy_sta(wj_cpr_regs_t *cpr_base)
{
    return ((cpr_base->CPR_IIC1_BUSY_REG & CPR_IIC1_BUSY_EN) ? 1U : 0U);
}
static inline void wj_cpr_iic1_busy_clr(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_IIC1_BUSY_REG |= CPR_IIC1_BUSY_CLR_EN;
}

/* UART_BUSY_REG, Offset: 0x3C */
static inline uint32_t wj_cpr_uart_busy_sta(wj_cpr_regs_t *cpr_base)
{
    return ((cpr_base->CPR_UART_BUSY_REG & CPR_UART_BUSY_EN) ? 1U : 0U);
}
static inline void wj_cpr_uart_busy_clr(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_UART_BUSY_REG |= CPR_UART_BUSY_CLR_EN;
}

/* TIMER_BUSY_REG, Offset: 0x40 */
static inline uint32_t wj_cpr_timer_busy_sta(wj_cpr_regs_t *cpr_base)
{
    return ((cpr_base->CPR_TIMER_BUSY_REG & CPR_TIMER_BUSY_EN) ? 1U : 0U);
}
static inline void wj_cpr_timer_busy_clr(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_TIMER_BUSY_REG |= CPR_TIMER_BUSY_CLR_EN;
}

/* VAD_BUSY_REG, Offset: 0x44 */
static inline uint32_t wj_cpr_vad_busy_sta(wj_cpr_regs_t *cpr_base)
{
    return ((cpr_base->CPR_VAD_BUSY_REG & CPR_VAD_BUSY_EN) ? 1U : 0U);
}
static inline void wj_cpr_vad_busy_clr(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_VAD_BUSY_REG |= CPR_VAD_BUSY_CLR_EN;
}

/* GPIO_BUSY_REG, Offset: 0x48 */
static inline uint32_t wj_cpr_gpio_busy_sta(wj_cpr_regs_t *cpr_base)
{
    return ((cpr_base->CPR_GPIO_BUSY_REG & CPR_GPIO_BUSY_EN) ? 1U : 0U);
}
static inline void wj_cpr_gpio_busy_clr(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_GPIO_BUSY_REG |= CPR_GPIO_BUSY_CLR_EN;
}

/* DMA_CH1_BUSY_REG, Offset: 0x4C */
static inline uint32_t wj_cpr_dma_ch1_busy_sta(wj_cpr_regs_t *cpr_base)
{
    return ((cpr_base->CPR_DMA_CH1_BUSY_REG & CPR_DMA_CH1_BUSY_EN) ? 1U : 0U);
}
static inline void wj_cpr_dma_ch1_busy_clr(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_DMA_CH1_BUSY_REG |= CPR_DMA_CH1_BUSY_CLR_EN;
}

/* DMA_CH2_BUSY_REG, Offset: 0x50 */
static inline uint32_t wj_cpr_dma_ch2_busy_sta(wj_cpr_regs_t *cpr_base)
{
    return ((cpr_base->CPR_DMA_CH2_BUSY_REG & CPR_DMA_CH2_BUSY_EN) ? 1U : 0U);
}
static inline void wj_cpr_dma_ch2_busy_clr(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_DMA_CH2_BUSY_REG |= CPR_DMA_CH2_BUSY_CLR_EN;
}

/* DMA_CH3_BUSY_REG, Offset: 0x54 */
static inline uint32_t wj_cpr_dma_ch3_busy_sta(wj_cpr_regs_t *cpr_base)
{
    return ((cpr_base->CPR_DMA_CH3_BUSY_REG & CPR_DMA_CH3_BUSY_EN) ? 1U : 0U);
}
static inline void wj_cpr_dma_ch3_busy_clr(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_DMA_CH3_BUSY_REG |= CPR_DMA_CH3_BUSY_CLR_EN;
}

/* DMA_CH4_BUSY_REG, Offset: 0x58 */
static inline uint32_t wj_cpr_dma_ch4_busy_sta(wj_cpr_regs_t *cpr_base)
{
    return ((cpr_base->CPR_DMA_CH4_BUSY_REG & CPR_DMA_CH4_BUSY_EN) ? 1U : 0U);
}
static inline void wj_cpr_dma_ch4_busy_clr(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_DMA_CH4_BUSY_REG |= CPR_DMA_CH4_BUSY_CLR_EN;
}

/* DMA_CH5_BUSY_REG, Offset: 0x5C */
static inline uint32_t wj_cpr_dma_ch5_busy_sta(wj_cpr_regs_t *cpr_base)
{
    return ((cpr_base->CPR_DMA_CH5_BUSY_REG & CPR_DMA_CH5_BUSY_EN) ? 1U : 0U);
}
static inline void wj_cpr_dma_ch5_busy_clr(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_DMA_CH5_BUSY_REG |= CPR_DMA_CH5_BUSY_CLR_EN;
}

/* DMA_CH6_BUSY_REG, Offset: 0x60 */
static inline uint32_t wj_cpr_dma_ch6_busy_sta(wj_cpr_regs_t *cpr_base)
{
    return ((cpr_base->CPR_DMA_CH6_BUSY_REG & CPR_DMA_CH6_BUSY_EN) ? 1U : 0U);
}
static inline void wj_cpr_dma_ch6_busy_clr(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_DMA_CH6_BUSY_REG |= CPR_DMA_CH6_BUSY_CLR_EN;
}

/* DMA_CH7_BUSY_REG, Offset: 0x64 */
static inline uint32_t wj_cpr_dma_ch7_busy_sta(wj_cpr_regs_t *cpr_base)
{
    return ((cpr_base->CPR_DMA_CH7_BUSY_REG & CPR_DMA_CH7_BUSY_EN) ? 1U : 0U);
}
static inline void wj_cpr_dma_ch7_busy_clr(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_DMA_CH7_BUSY_REG |= CPR_DMA_CH7_BUSY_CLR_EN;
}

/* DMA_CH8_BUSY_REG, Offset: 0x68 */
static inline uint32_t wj_cpr_dma_ch8_busy_sta(wj_cpr_regs_t *cpr_base)
{
    return ((cpr_base->CPR_DMA_CH8_BUSY_REG & CPR_DMA_CH8_BUSY_EN) ? 1U : 0U);
}
static inline void wj_cpr_dma_ch8_busy_clr(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_DMA_CH8_BUSY_REG |= CPR_DMA_CH8_BUSY_CLR_EN;
}

/* DMA_CH9_BUSY_REG, Offset: 0x6C */
static inline uint32_t wj_cpr_dma_ch9_busy_sta(wj_cpr_regs_t *cpr_base)
{
    return ((cpr_base->CPR_DMA_CH9_BUSY_REG & CPR_DMA_CH9_BUSY_EN) ? 1U : 0U);
}
static inline void wj_cpr_dma_ch9_busy_clr(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_DMA_CH9_BUSY_REG |= CPR_DMA_CH9_BUSY_CLR_EN;
}

/* DMA_CH10_BUSY_REG, Offset: 0x70 */
static inline uint32_t wj_cpr_dma_ch10_busy_sta(wj_cpr_regs_t *cpr_base)
{
    return ((cpr_base->CPR_DMA_CH10_BUSY_REG & CPR_DMA_CH10_BUSY_EN) ? 1U : 0U);
}
static inline void wj_cpr_dma_ch10_busy_clr(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_DMA_CH10_BUSY_REG |= CPR_DMA_CH10_BUSY_CLR_EN;
}

/* SYS_STATUS_REG, Offset: 0x74 */
static inline uint32_t wj_cpr_cpu_rst_sta(wj_cpr_regs_t *cpr_base)
{
    return ((cpr_base->CPR_SYS_STATUS_REG & CPR_CPU_RST_ST_EN) ? 1U : 0U);
}
static inline uint32_t wj_cpr_cpu_lpmd_sta(wj_cpr_regs_t *cpr_base)
{
    return ((cpr_base->CPR_SYS_STATUS_REG & CPR_SYS_CPU_LPMD_ST_Msk) >> CPR_SYS_CPU_LPMD_ST_Pos);
}
static inline uint32_t wj_cpr_cpr_cp_sta(wj_cpr_regs_t *cpr_base)
{
    return ((cpr_base->CPR_SYS_STATUS_REG & CPR_SYS_CPR_CP_ST_Msk) >> CPR_SYS_CPR_CP_ST_Pos);
}
static inline uint32_t wj_cpr_cpr_fsm_sta(wj_cpr_regs_t *cpr_base)
{
    return ((cpr_base->CPR_SYS_STATUS_REG & CPR_SYS_CPR_FSM_ST_Msk) >> CPR_SYS_CPR_FSM_ST_Pos);
}
static inline uint32_t wj_cpr_wk_src_sta(wj_cpr_regs_t *cpr_base)
{
    return ((cpr_base->CPR_SYS_STATUS_REG & CPR_SYS_WK_SRC_ST_Msk) >> CPR_SYS_WK_SRC_ST_Pos);
}

/* SYS_CTRL_REG, Offset: 0x78 */
static inline void wj_cpr_cp_cpr_req(wj_cpr_regs_t *cpr_base)
{
    cpr_base->CPR_SYS_CTRL_REG |= CPR_CP_CPR_REQ_EN;
}
static inline void wj_cpr_cp_cpr_sta_wr(wj_cpr_regs_t *cpr_base, uint32_t sta)
{
    cpr_base->CPR_SYS_CTRL_REG |= CPR_CP_CPR_ST_SEL(sta);
}
static inline uint32_t wj_cpr_cp_cpr_sta(wj_cpr_regs_t *cpr_base)
{
    return ((cpr_base->CPR_SYS_CTRL_REG & CPR_CP_CPR_ST_Msk) >> CPR_CP_CPR_ST_Pos);
}
static inline void wj_cpr_sysbus_cpu_p_wr(wj_cpr_regs_t *cpr_base, uint32_t sta)
{
    cpr_base->CPR_SYS_CTRL_REG |= CPR_SYSBUS_CPU_P_SEL(sta);
}
static inline uint32_t wj_cpr_sysbus_cpu_p_sta(wj_cpr_regs_t *cpr_base)
{
    return ((cpr_base->CPR_SYS_CTRL_REG & CPR_SYSBUS_CPU_P_Msk) >> CPR_SYSBUS_CPU_P_Pos);
}
static inline void wj_cpr_sysbus_dma_p_wr(wj_cpr_regs_t *cpr_base, uint32_t sta)
{
    cpr_base->CPR_SYS_CTRL_REG |= CPR_SYSBUS_DMA_P_SEL(sta);
}
static inline uint32_t wj_cpr_sysbus_dma_p_sta(wj_cpr_regs_t *cpr_base)
{
    return ((cpr_base->CPR_SYS_CTRL_REG & CPR_SYSBUS_DMA_P_Msk) >> CPR_SYSBUS_DMA_P_Pos);
}
static inline void wj_cpr_sysbus_apm_p_wr(wj_cpr_regs_t *cpr_base, uint32_t sta)
{
    cpr_base->CPR_SYS_CTRL_REG |= CPR_SYSBUS_APM_P_SEL(sta);
}
static inline uint32_t wj_cpr_sysbus_apm_p_sta(wj_cpr_regs_t *cpr_base)
{
    return ((cpr_base->CPR_SYS_CTRL_REG & CPR_SYSBUS_APM_P_Msk) >> CPR_SYSBUS_APM_P_Pos);
}
static inline void wj_cpr_sysbus_sram0_p_wr(wj_cpr_regs_t *cpr_base, uint32_t sta)
{
    cpr_base->CPR_SYS_CTRL_REG |= CPR_SYSBUS_SRAM0_P_SEL(sta);
}
static inline uint32_t wj_cpr_sysbus_sram0_p_sta(wj_cpr_regs_t *cpr_base)
{
    return ((cpr_base->CPR_SYS_CTRL_REG & CPR_SYSBUS_SRAM0_P_Msk) >> CPR_SYSBUS_SRAM0_P_Pos);
}
static inline void wj_cpr_sysbus_sram1_p_wr(wj_cpr_regs_t *cpr_base, uint32_t sta)
{
    cpr_base->CPR_SYS_CTRL_REG |= CPR_SYSBUS_SRAM1_P_SEL(sta);
}
static inline uint32_t wj_cpr_sysbus_sram1_p_sta(wj_cpr_regs_t *cpr_base)
{
    return ((cpr_base->CPR_SYS_CTRL_REG & CPR_SYSBUS_SRAM1_P_Msk) >> CPR_SYSBUS_SRAM1_P_Pos);
}
static inline void wj_cpr_sysbus_peri_p_wr(wj_cpr_regs_t *cpr_base, uint32_t sta)
{
    cpr_base->CPR_SYS_CTRL_REG |= CPR_SYSBUS_PERI_P_SEL(sta);
}
static inline uint32_t wj_cpr_sysbus_peri_p_sta(wj_cpr_regs_t *cpr_base)
{
    return ((cpr_base->CPR_SYS_CTRL_REG & CPR_SYSBUS_PERI_P_Msk) >> CPR_SYSBUS_PERI_P_Pos);
}
static inline void wj_cpr_sysbus_ddr_p_wr(wj_cpr_regs_t *cpr_base, uint32_t sta)
{
    cpr_base->CPR_SYS_CTRL_REG |= CPR_SYSBUS_DDR_P_SEL(sta);
}
static inline uint32_t wj_cpr_sysbus_ddr_p_sta(wj_cpr_regs_t *cpr_base)
{
    return ((cpr_base->CPR_SYS_CTRL_REG & CPR_SYSBUS_DDR_P_Msk) >> CPR_SYSBUS_DDR_P_Pos);
}
static inline void wj_cpr_sysbus_aps_p_wr(wj_cpr_regs_t *cpr_base, uint32_t sta)
{
    cpr_base->CPR_SYS_CTRL_REG |= CPR_SYSBUS_APS_P_SEL(sta);
}
static inline uint32_t wj_cpr_sysbus_aps_p_sta(wj_cpr_regs_t *cpr_base)
{
    return ((cpr_base->CPR_SYS_CTRL_REG & CPR_SYSBUS_APS_P_Msk) >> CPR_SYSBUS_APS_P_Pos);
}

static inline void wj_cpr_wk_intr_mask_wr(wj_cpr_regs_t *cpr_base, uint32_t sta)
{
    cpr_base->CPR_SYS_CTRL_REG |= CPR_WK_INTR_MASK_SEL(sta);
}

static inline uint32_t wj_cpr_wk_intr_mask_sta(wj_cpr_regs_t *cpr_base)
{
    return ((cpr_base->CPR_SYS_CTRL_REG & CPR_WK_INTR_MASK_Msk) >> CPR_WK_INTR_MASK_Pos);
}

/* SYS_ID_REG, Offset: 0xA0 */
static inline uint32_t wj_cpr_system_id(wj_cpr_regs_t *cpr_base)
{
    return (cpr_base->CPR_SYS_ID_REG);
}

/* RESERVD1_REG, Offset: 0xA4 */
static inline uint32_t wj_cpr_system_rav(wj_cpr_regs_t *cpr_base)
{
    return (cpr_base->CPR_RESERVD1_REG);
}

#ifdef __cplusplus
}
#endif

#endif
