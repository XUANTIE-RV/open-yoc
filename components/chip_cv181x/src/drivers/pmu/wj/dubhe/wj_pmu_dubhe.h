/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     wj_dubhe_pmu.h
 * @brief    head file for dubhe pmu
 * @version  V1.0
 * @date     17. Feb 2020
 ******************************************************************************/
#ifndef _WJ_DUBHE_PMU_H_
#define _WJ_DUBHE_PMU_H_

#include <soc.h>
#include <csi_core.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    __IOM uint32_t LPCR;              /* Offset: 0x000 (R/W) low power control register */
    uint32_t RESERVED0[5];
    __IM uint32_t CLKSTBR;            /* Offset: 0x018 (R) clock stable register */
    __IM uint32_t CLKSTBST;           /* Offset: 0x01c (R) clock stable interrupt status register */
    __IOM uint32_t CLKSTBMK;          /* Offset: 0x020 (R/W) clock stable interrupt mask register */
    __IOM uint32_t CSSCR;             /* Offset: 0x024 (R/W) clock source stable counter register */
    __IOM uint32_t DFCC;              /* Offset: 0x028 (R/W) dynamic frequence conversion control register */
    __IOM uint32_t PCR;               /* Offset: 0x02c (R/W) pll control register */
    uint32_t RESERVED1[10];
    __IOM uint32_t PMU_TRIM;          /* Offset: 0x058 (R/W) rc/pll triming control register */
    uint32_t RESERVED2[9];
    __IOM uint32_t WIME0;             /* Offset: 0x080 (R/W) wic interrupt mask0 register */
    uint32_t RESERVED3[31];
    __IOM uint32_t MODE_CHG_EN;       /* Offset: 0x100 (R/W) src/dcu mode change register */
    __IOM uint32_t SRC_CLKMD;         /* Offset: 0x104 (R/W) src clock mode register */
    __IOM uint32_t SRC_LDOMD;         /* Offset: 0x108 (R/W) src ldo mode register */
    uint32_t RESERVED4[9];
    __IOM uint32_t CODC_DCU_MODE;     /* Offset: 0x130 (R/W) CODC DCU mode register */
    uint32_t RESERVED5[11];
    __IOM uint32_t RETU_DCU_MODE;     /* Offset: 0x160 (R/W) RETU DCU mode register */
    uint32_t RESERVED6[2];
    __OM uint32_t RETU_DCU_CGCR;      /* Offset: 0x16c (W) RETU DCU clock gate control register */
    __IM uint32_t RETU_DCU_CGSR;      /* Offset: 0x170 (R) RETU DCU clock gate status register */
    __IOM uint32_t RETU_DCU_SWRST;    /* Offset: 0x174 (R/W) RETU DCU software reset control register */
    uint32_t RESERVED7[6];
    __IOM uint32_t PDU_DCU_MODE;      /* Offset: 0x190 (R/W) PDU DCU mode register */
    uint32_t RESERVED8[2];
    __OM uint32_t PDU_DCU_MCGCR;      /* Offset: 0x19c (W) PDU DCU matrix clock gate control register */
    __IM uint32_t PDU_DCU_MCGSR;      /* Offset: 0x1a0 (R) PDU DCU matrix clock gate status register */
    __IOM uint32_t PDU_DCU_MSWRST;    /* Offset: 0x1a4 (R/W) PDU DCU matrix software reset control register */
    __OM uint32_t PDU_DCU_P0CGCR;     /* Offset: 0x1a8 (W) PDU DCU apb0 clock gate control register */
    __IM uint32_t PDU_DCU_P0CGSR;     /* Offset: 0x1ac (R) PDU DCU apb0 clock gate status register */
    __IOM uint32_t PDU_DCU_P0SWRST;   /* Offset: 0x1b0 (R/W) PDU DCU apb0 software reset control register */
    __OM uint32_t PDU_DCU_P1CGCR;     /* Offset: 0x1b4 (W) PDU DCU apb1 clock gate control register */
    __IM uint32_t PDU_DCU_P1CGSR;     /* Offset: 0x1b8 (R) PDU DCU apb1 clock gate status register */
    __IOM uint32_t PDU_DCU_P1SWRST;   /* Offset: 0x1bc (R/W) PDU DCU apb1 software reset control register */
    __IOM uint32_t ANA_DCU_MODE;      /* Offset: 0x1c0 (R/W) ANA DCU mode register */
    uint32_t RESERVED9[11];
    __IOM uint32_t CPU_DCU_MODE;      /* Offset: 0x1f0 (R/W) CPU DCU mode register */
    uint32_t RESERVED10[130];
    __IM uint32_t SOC_ID;            /* Offset: 0x3fc (R) SOC_ID register */
    __IOM uint32_t USER_DEFINE[4];   /* Offset: 0x400-0x40c (R/W) save data in lowpower mode */
    uint32_t RESERVED11[60];
    __IOM uint32_t DCU_RST_DUR;      /* Offset: 0x500 (R/W) DCU reset duration register */
    __IOM uint32_t DCU_ISO_DUR;      /* Offset: 0x504 (R/W) DCU isolation duration register */
    __IOM uint32_t LDO_STB_CNT;      /* Offset: 0x508 (R/W) LDO stable counter register */
    __IOM uint32_t MCLK_RATIO;       /* Offset: 0x50c (R/W) MCLK ratio register */
    __IOM uint32_t RST_STATE;        /* Offset: 0x510 (R/W) RESET state register */
    __IOM uint32_t IP_CTRL;          /* Offset: 0x514 (R/W) IP control register */
    __IOM uint32_t DFC_CTRL;         /* Offset: 0x518 (R/W) DFC control register */
    uint32_t RESERVED12[1];
    __IOM uint32_t MODE_CTRL;        /* Offset: 0x520 (R/W) MODE control register */
    __IOM uint32_t MEM_RET_CTRL;     /* Offset: 0x524 (R/W) MEM retention control register */
} wj_pmu_reg_t;

/* LPCR, offset: 0x00 */
#define WJ_LPCR_LOW_MODE_Pos                      (2U)
#define WJ_LPCR_LOW_MODE_Msk                      (0x1U << WJ_LPCR_LOW_MODE_Pos)
#define WJ_LPCR_LOW_MODE_EN                       WJ_LPCR_LOW_MODE_Msk

#define WJ_LPCR_LP_WAIT_MODE_Pos                  (3U)
#define WJ_LPCR_LP_WAIT_CLK_ON                    (0x0U << WJ_LPCR_LP_WAIT_MODE_Pos)
#define WJ_LPCR_LP_WAIT_CLK_OFF                   (0x1U << WJ_LPCR_LP_WAIT_MODE_Pos)

#define WJ_LPCR_LP_STOP_MODE_Pos                  (4U)
#define WJ_LPCR_LP_STOP_MODE                      (0x0U << WJ_LPCR_LP_STOP_MODE_Pos)
#define WJ_LPCR_LP_STANDBY_MODE                   (0x1U << WJ_LPCR_LP_STOP_MODE_Pos)

#define WJ_LPCR_LP_STATE_Pos                      (5U)
#define WJ_LPCR_LP_STATE_Msk                      (0x1U << WJ_LPCR_LP_STATE_Pos)
#define WJ_LPCR_LP_STATE                          WJ_LPCR_LP_STATE_Msk

#define WJ_LPCR_SOC_RUN_Pos                       (6U)
#define WJ_LPCR_SOC_RUN_Msk                       (0x1U << WJ_LPCR_SOC_RUN_Pos)
#define WJ_LPCR_SOC_RUN                           WJ_LPCR_SOC_RUN_Msk

#define WJ_LPCR_BOOT_FROM_Pos                     (11U)
#define WJ_LPCR_BOOT_FROM_Msk                     (0x1U << WJ_LPCR_WAKE_FROM_Pos)
#define WJ_LPCR_BOOT_FROM                         WJ_LPCR_WAKE_FROM_Msk

#define WJ_LPCR_CPU_ENTRY_LP_Pos                  (12U)
#define WJ_LPCR_CPU_ENTRY_LP_Msk                  (0x1U << WJ_LPCR_CPU_ENTRY_LP_Pos)
#define WJ_LPCR_CPU_ENTRY_LP                      WJ_LPCR_CPU_ENTRY_LP_Msk

#define WJ_LPCR_CPU_EXIT_LP_Pos                   (13U)
#define WJ_LPCR_CPU_EXIT_LP_Msk                   (0x1U << WJ_LPCR_CPU_EXIT_LP_Pos)
#define WJ_LPCR_CPU_EXIT_LP_FROM                  WJ_LPCR_CPU_EXIT_LP_Msk

#define WJ_LPCR_LP_MODE_SWITCH_Pos                (16U)
#define WJ_LPCR_LP_MODE_SWITCH_Msk                (0x3U << WJ_LPCR_LP_MODE_SWITCH_Pos)
#define WJ_LPCR_LP_MODE_WAIT_CLK_ON_MODE          (0x0U << WJ_LPCR_LP_MODE_SWITCH_Pos)
#define WJ_LPCR_LP_MODE_WAIT_CLK_OFF_MODE         (0x1U << WJ_LPCR_LP_MODE_SWITCH_Pos)
#define WJ_LPCR_LP_MODE_STOP_MODE                 (0x2U << WJ_LPCR_LP_MODE_SWITCH_Pos)
#define WJ_LPCR_LP_MODE_STANDBY_MODE              (0x3U << WJ_LPCR_LP_MODE_SWITCH_Pos)


/* CLKSTBR, offset: 0x18 */
#define WJ_CLKSTBR_ILSSTB_Pos                     (0U)
#define WJ_CLKSTBR_ILSSTB_Msk                     (0x1U << WJ_CLKSTBR_ILSSTB_Pos)
#define WJ_CLKSTBR_ILSSTB_STATE                   WJ_CLKSTBR_ILSSTB_Msk

#define WJ_CLKSTBR_IHSSTB_Pos                     (1U)
#define WJ_CLKSTBR_IHSSTB_Msk                     (0x1U << WJ_CLKSTBR_IHSSTB_Pos)
#define WJ_CLKSTBR_IHSSTB_STATE                   WJ_CLKSTBR_IHSSTB_Msk

#define WJ_CLKSTBR_ELSSTB_Pos                     (2U)
#define WJ_CLKSTBR_ELSSTB_Msk                     (0x1U << WJ_CLKSTBR_ELSSTB_Pos)
#define WJ_CLKSTBR_ELSSTB_STATE                   WJ_CLKSTBR_ELSSTB_Msk

#define WJ_CLKSTBR_EHSSTB_Pos                     (3U)
#define WJ_CLKSTBR_EHSSTB_Msk                     (0x1U << WJ_CLKSTBR_EHSSTB_Pos)
#define WJ_CLKSTBR_EHSSTB_STATE                   WJ_CLKSTBR_EHSSTB_Msk

#define WJ_CLKSTBR_PLLSTB_Pos                     (4U)
#define WJ_CLKSTBR_PLLSTB_Msk                     (0x1U << WJ_CLKSTBR_PLLSTB_Pos)
#define WJ_CLKSTBR_PLLSTB_STATE                   WJ_CLKSTBR_PLLSTB_Msk

/* CLKSTBST, offset: 0x1c */
#define WJ_CLKSTBST_ILSIS_Pos                     (0U)
#define WJ_CLKSTBST_ILSIS_Msk                     (0x1U << WJ_CLKSTBST_ILSIS_Pos)
#define WJ_ILS_INTERRUPT_EN                       WJ_CLKSTBST_ILSIS_Msk

#define WJ_CLKSTBST_IHSIS_Pos                     (1U)
#define WJ_CLKSTBST_IHSIS_Msk                     (0x1U << WJ_CLKSTBST_IHSIS_Pos)
#define WJ_IHS_INTERRUPT_EN                       WJ_CLKSTBST_IHSIS_Msk

#define WJ_CLKSTBST_ELSIS_Pos                     (2U)
#define WJ_CLKSTBST_ELSIS_Msk                     (0x1U << WJ_CLKSTBST_ELSIS_Pos)
#define WJ_ELS_INTERRUPT_EN                       WJ_CLKSTBST_ELSIS_Msk

#define WJ_CLKSTBST_EHSIS_Pos                     (3U)
#define WJ_CLKSTBST_EHSIS_Msk                     (0x1U << WJ_CLKSTBST_EHSIS_Pos)
#define WJ_EHS_INTERRUPT_EN                       WJ_CLKSTBST_EHSIS_Msk

#define WJ_CLKSTBST_PLLIS_Pos                     (4U)
#define WJ_CLKSTBST_PLLIS_Msk                     (0x1U << WJ_CLKSTBST_PLLIS_Pos)
#define WJ_PLL_INTERRUPT_EN                       WJ_CLKSTBST_PLLIS_Msk

/* CLKSTBST, offset: 0x20 */
#define WJ_CLKSTBST_ILSIM_Pos                     (0U)
#define WJ_CLKSTBST_ILSIM_Msk                     (0x1U << WJ_CLKSTBST_ILSIM_Pos)
#define WJ_ILS_INTERRUPT_MASK_EN                  WJ_CLKSTBST_ILSIM_Msk

#define WJ_CLKSTBST_IHSIM_Pos                     (1U)
#define WJ_CLKSTBST_IHSIM_Msk                     (0x1U << WJ_CLKSTBST_IHSIM_Pos)
#define WJ_IHS_INTERRUPT_MASK_EN                  WJ_CLKSTBST_IHSIM_Msk

#define WJ_CLKSTBST_ELSIM_Pos                     (2U)
#define WJ_CLKSTBST_ELSIM_Msk                     (0x1U << WJ_CLKSTBST_ELSIM_Pos)
#define WJ_ELS_INTERRUPT_MASK_EN                  WJ_CLKSTBST_ELSIM_Msk

#define WJ_CLKSTBST_EHSIM_Pos                     (3U)
#define WJ_CLKSTBST_EHSIM_Msk                     (0x1U << WJ_CLKSTBST_EHSIM_Pos)
#define WJ_EHS_INTERRUPT_MASK_EN                  WJ_CLKSTBST_EHSIM_Msk

#define WJ_CLKSTBST_PLLIM_Pos                     (4U)
#define WJ_CLKSTBST_PLLIM_Msk                     (0x1U << WJ_CLKSTBST_PLLIM_Pos)
#define WJ_PLL_INTERRUPT_MASK_EN                  WJ_CLKSTBST_PLLIM_Msk

/* DFCC, offset: 0x28 */
#define WJ_DFCC_DFCEN_PDU_Pos                     (0U)
#define WJ_DFCC_DFCEN_PDU_Msk                     (0x1U << WJ_DFCC_DFCEN_PDU_Pos)
#define WJ_DFCC_DFC_EN                            WJ_DFCC_DFCEN_PDU_Msk

/* PCR, offset: 0x2C */
#define WJ_PCR_FN_RATION_Pos                      (0U)
#define WJ_PCR_FN_RATION_Msk                      (0x1FU << WJ_PCR_FN_RATION_Pos)

#define WJ_PCR_FD_RATION_Pos                      (5U)
#define WJ_PCR_FD_RATION_Msk                      (0xFFFU << WJ_PCR_FD_RATION_Pos)

#define WJ_PCR_EN_FRAC_Pos                        (17U)
#define WJ_PCR_EN_FRAC_Msk                        (0x1U << WJ_PCR_EN_FRAC_Pos)
#define WJ_PDU_EN_FRAC_EN                         WJ_PCR_EN_FRAC_Msk

#define WJ_PCR_PLLSRC_SEL_Pos                     (18U)
#define WJ_PCR_PLLSRC_SEL_Msk                     (0x3U << WJ_PCR_PLLSRC_SEL_Pos)
#define WJ_PCR_PLLSRC_SEL_IHS                     (0x0U << WJ_PCR_PLLSRC_SEL_Pos)
#define WJ_PCR_PLLSRC_SEL_USB_PHY                 (0x1U << WJ_PCR_PLLSRC_SEL_Pos)
#define WJ_PCR_PLLSRC_SEL_EHS                     (0x2U << WJ_PCR_PLLSRC_SEL_Pos)

#define WJ_PCR_PLLSRC_DIV_Pos                     (20U)
#define WJ_PCR_PLLSRC_DIV_Msk                     (0x3U << WJ_PCR_PLLSRC_DIV_Pos)

/* PMU_TRIM, offset: 0x58 */
#define WJ_IHS_RC_TRIM_Pos                        (0U)
#define WJ_IHS_RC_TRIM_Msk                        (0x1FU << WJ_IHS_RC_TRIM_Pos)

#define WJ_ILS_RC_TRIM_Pos                        (6U)
#define WJ_ILS_RC_TRIM_Msk                        (0x1FU << WJ_ILS_RC_TRIM_Pos)

#define WJ_PD_LDO_VTR12_TRIM_Pos                  (12U)
#define WJ_PD_LDO_VTR12_TRIM_Msk                  (0x7U << WJ_PD_LDO_VTR12_TRIM_Pos)

#define WJ_PD_LDO_VTR3318_TRIM_Pos                (15U)
#define WJ_PD_LDO_VTR3318_TRIM_Msk                (0x7U << WJ_PD_LDO_VTR3318_TRIM_Pos)

#define WJ_BOD_TRIM_Pos                           (18U)
#define WJ_BOD_TRIM_Msk                           (0x7U << WJ_BOD_TRIM_Pos)

/* WIME0, offset: 0x80 */
#define WJ_WIME0_GPIO0_Pos                        (0U)
#define WJ_WIME0_GPIO0_Msk                        (0x1U << WJ_WIME0_GPIO0_Pos)
#define WJ_WIME0_GPIO0_MASK                       WJ_WIME0_GPIO0_Msk

#define WJ_WIME0_TIME0_0_Pos                      (1U)
#define WJ_WIME0_TIME0_0_Msk                      (0x1U << WJ_WIME0_TIME0_0_Pos)
#define WJ_WIME0_TIME0_0_MASK                     WJ_WIME0_TIME0_0_Msk

#define WJ_WIME0_TIME0_1_Pos                      (2U)
#define WJ_WIME0_TIME0_1_Msk                      (0x1U << WJ_WIME0_TIME0_1_Pos)
#define WJ_WIME0_TIME0_1_MASK                      WJ_WIME0_TIME0_1_Msk

#define WJ_WIME0_WDT_Pos                          (3U)
#define WJ_WIME0_WDT_Msk                          (0x1U << WJ_WIME0_WDT_Pos)
#define WJ_WIME0_WDT_MASK                         WJ_WIME0_WDT_Msk

#define WJ_WIME0_UART0_Pos                        (4U)
#define WJ_WIME0_UART0_Msk                        (0x1U << WJ_WIME0_UART0_Pos)
#define WJ_WIME0_UART0_MASK                       WJ_WIME0_UART0_Msk

#define WJ_WIME0_UART1_Pos                        (5U)
#define WJ_WIME0_UART1_Msk                        (0x1U << WJ_WIME0_UART1_Pos)
#define WJ_WIME0_UART1_MASK                       WJ_WIME0_UART1_Msk

#define WJ_WIME0_USB_Pos                          (6U)
#define WJ_WIME0_USB_Msk                          (0x1U << WJ_WIME0_USB_Pos)
#define WJ_WIME0_USB_MASK                         WJ_WIME0_USB_Msk

#define WJ_WIME0_I2C0_Pos                         (7U)
#define WJ_WIME0_I2C0_Msk                         (0x1U << WJ_WIME0_I2C0_Pos)
#define WJ_WIME0_I2C0_MASK                        WJ_WIME0_I2C0_Msk

#define WJ_WIME0_I2S456_Pos                       (8U)
#define WJ_WIME0_I2S456_Msk                       (0x1U << WJ_WIME0_I2S456_Pos)
#define WJ_WIME0_I2S456_MASK                      WJ_WIME0_I2S456_Msk

#define WJ_WIME0_SPI1_Pos                         (9U)
#define WJ_WIME0_SPI1_Msk                         (0x1U << WJ_WIME0_SPI1_Pos)
#define WJ_WIME0_SPI1_MASK                        WJ_WIME0_SPI1_Msk

#define WJ_WIME0_SPI0_Pos                         (10U)
#define WJ_WIME0_SPI0_Msk                         (0x1U << WJ_WIME0_SPI0_Pos)
#define WJ_WIME0_SPI0_MASK                        WJ_WIME0_SPI0_Msk

#define WJ_WIME0_RTC0_Pos                         (11U)
#define WJ_WIME0_RTC0_Msk                         (0x1U << WJ_WIME0_RTC0_Pos)
#define WJ_WIME0_RTC0_MASK                        WJ_WIME0_RTC0_Msk

#define WJ_WIME0_I2S0_Pos                         (12U)
#define WJ_WIME0_I2S0_Msk                         (0x1U << WJ_WIME0_I2S0_Pos)
#define WJ_WIME0_I2S0_MASK                        WJ_WIME0_I2S0_Msk

#define WJ_WIME0_ADC_Pos                          (13U)
#define WJ_WIME0_ADC_Msk                          (0x1U << WJ_WIME0_ADC_Pos)
#define WJ_WIME0_ADC_MASK                         WJ_WIME0_ADC_Msk

#define WJ_WIME0_I2S1_Pos                         (14U)
#define WJ_WIME0_I2S1_Msk                         (0x1U << WJ_WIME0_I2S1_Pos)
#define WJ_WIME0_I2S1_MASK                        WJ_WIME0_I2S1_Msk

#define WJ_WIME0_DMA0_Pos                         (15U)
#define WJ_WIME0_DMA0_Msk                         (0x1U << WJ_WIME0_DMA0_Pos)
#define WJ_WIME0_DMA0_MASK                        WJ_WIME0_DMA0_Msk

#define WJ_WIME0_PWM_Pos                          (16U)
#define WJ_WIME0_PWM_Msk                          (0x1U << WJ_WIME0_PWM_Pos)
#define WJ_WIME0_PWM_MASK                         WJ_WIME0_PWM_Msk

#define WJ_WIME0_PWMR_Pos                         (17U)
#define WJ_WIME0_PWMR_Msk                         (0x1U << WJ_WIME0_PWMR_Pos)
#define WJ_WIME0_PWMR_MASK                        WJ_WIME0_PWMR_Msk

#define WJ_WIME0_I2S2_Pos                         (18U)
#define WJ_WIME0_I2S2_Msk                         (0x1U << WJ_WIME0_I2S2_Pos)
#define WJ_WIME0_I2S2_MASK                        WJ_WIME0_I2S2_Msk

#define WJ_WIME0_I2S3_Pos                         (19U)
#define WJ_WIME0_I2S3_Msk                         (0x1U << WJ_WIME0_I2S3_Pos)
#define WJ_WIME0_I2S3_MASK                        WJ_WIME0_I2S3_Msk

#define WJ_WIME0_TIME1_0_Pos                      (20U)
#define WJ_WIME0_TIME1_0_Msk                      (0x1U << WJ_WIME0_TIME1_0_Pos)
#define WJ_WIME0_TIME1_0_MASK                     WJ_WIME0_TIME1_0_Msk

#define WJ_WIME0_TIME1_1_Pos                      (21U)
#define WJ_WIME0_TIME1_1_Msk                      (0x1U << WJ_WIME0_TIME1_1_Pos)
#define WJ_WIME0_TIME1_1_MASK                     WJ_WIME0_TIME1_1_Msk

#define WJ_WIME0_QSPIC0_Pos                       (22U)
#define WJ_WIME0_QSPIC0_Msk                       (0x1U << WJ_WIME0_QSPIC0_Pos)
#define WJ_WIME0_QSPIC0_MASK                      WJ_WIME0_QSPIC0_Msk

#define WJ_WIME0_TIME2_0_Pos                      (23U)
#define WJ_WIME0_TIME2_0_Msk                      (0x1U << WJ_WIME0_TIME2_0_Pos)
#define WJ_WIME0_TIME2_0_MASK                     WJ_WIME0_TIME2_0_Msk

#define WJ_WIME0_TIME2_1_Pos                      (24U)
#define WJ_WIME0_TIME2_1_Msk                      (0x1U << WJ_WIME0_TIME2_1_Pos)
#define WJ_WIME0_TIME2_1_MASK                     WJ_WIME0_TIME2_1_Msk

#define WJ_WIME0_TIME3_0_Pos                      (25U)
#define WJ_WIME0_TIME3_0_Msk                      (0x1U << WJ_WIME0_TIME3_0_Pos)
#define WJ_WIME0_TIME3_0_MASK                     WJ_WIME0_TIME3_0_Msk

#define WJ_WIME0_TIME3_1_Pos                      (26U)
#define WJ_WIME0_TIME3_1_Msk                      (0x1U << WJ_WIME0_TIME3_1_Pos)
#define WJ_WIME0_TIME3_1_MASK                     WJ_WIME0_TIME3_1_Msk

#define WJ_WIME0_USBPHY_Pos                       (27U)
#define WJ_WIME0_USBPHY_Msk                       (0x1U << WJ_WIME0_USBPHY_Pos)
#define WJ_WIME0_USBPHY_MASK                      WJ_WIME0_USBPHY_Msk

#define WJ_WIME0_HAD_Pos                          (28U)
#define WJ_WIME0_HAD_Msk                          (0x1U << WJ_WIME0_HAD_Pos)
#define WJ_WIME0_HAD_MASK                         WJ_WIME0_HAD_Msk

#define WJ_WIME0_IO_Pos                           (29U)
#define WJ_WIME0_IO_Msk                           (0x1U << WJ_WIME0_GPIO1_Pos)
#define WJ_WIME0_IO_MASK                          WJ_WIME0_GPIO1_Msk

/* MODE_CHG_EN, offset: 0x100 */
#define WJ_MODE_CHG_EN_DCU_MD_Pos                 (0U)
#define WJ_MODE_CHG_EN_DCU_MD_Msk                 (0x1U << WJ_MODE_CHG_EN_DCU_MD_Pos)
#define WJ_MODE_CHG_EN_DCU_MD_EN                  WJ_MODE_CHG_EN_DCU_MD_Msk

#define WJ_MODE_CHG_EN_SRC_CLK_Pos                (1U)
#define WJ_MODE_CHG_EN_SRC_CLK_Msk                (0x1U << WJ_MODE_CHG_EN_SRC_CLK_Pos)
#define WJ_MODE_CHG_EN_SRC_CLK_EN                 WJ_MODE_CHG_EN_SRC_CLK_Msk

#define WJ_MODE_CHG_EN_SRC_LDO_Pos                (2U)
#define WJ_MODE_CHG_EN_SRC_LDO_Msk                (0x1U << WJ_MODE_CHG_EN_SRC_LDO_Pos)
#define WJ_MODE_CHG_EN_SRC_LDO_EN                 WJ_MODE_CHG_EN_SRC_LDO_Msk

/* SRC_CLKMD, offset: 0x104 */
#define WJ_SRC_CLKMD_LS_SEL_Pos                   (0U)
#define WJ_SRC_CLKMD_LS_SEL_Msk                   (0x1U << WJ_SRC_CLKMD_LS_SEL_Pos)
#define WJ_SRC_CLKMD_LS_SEL                       WJ_SRC_CLKMD_LS_SEL_Msk

#define WJ_SRC_CLKMD_CLK_SEL0_Pos                 (1U)
#define WJ_SRC_CLKMD_CLK_SEL0_Msk                 (0x1U << WJ_SRC_CLKMD_CLK_SEL0_Pos)
#define WJ_SRC_CLKMD_CLK_SEL0                     WJ_SRC_CLKMD_CLK_SEL0_Msk

#define WJ_SRC_CLKMD_CLK_SEL1_Pos                 (2U)
#define WJ_SRC_CLKMD_CLK_SEL1_Msk                 (0x1U << WJ_SRC_CLKMD_CLK_SEL1_Pos)
#define WJ_SRC_CLKMD_CLK_SEL1                     WJ_SRC_CLKMD_CLK_SEL1_Msk

#define WJ_SRC_CLKMD_CLK_SEL2_Pos                 (3U)
#define WJ_SRC_CLKMD_CLK_SEL2_Msk                 (0x1U << WJ_SRC_CLKMD_CLK_SEL2_Pos)
#define WJ_SRC_CLKMD_CLK_SEL2                     WJ_SRC_CLKMD_CLK_SEL2_Msk

#define WJ_SRC_CLKMD_ILS_EN_Pos                   (8U)
#define WJ_SRC_CLKMD_ILS_EN_Msk                   (0x1U << WJ_SRC_CLKMD_ILS_EN_Pos)
#define WJ_SRC_CLKMD_ILS_EN                       WJ_SRC_CLKMD_ILS_EN_Msk

#define WJ_SRC_CLKMD_IHS_EN_Pos                   (9U)
#define WJ_SRC_CLKMD_IHS_EN_Msk                   (0x1U << WJ_SRC_CLKMD_IHS_EN_Pos)
#define WJ_SRC_CLKMD_IHS_EN                       WJ_SRC_CLKMD_IHS_EN_Msk

#define WJ_SRC_CLKMD_ELS_OE_Pos                   (10U)
#define WJ_SRC_CLKMD_ELS_OE_Msk                   (0x1U << WJ_SRC_CLKMD_ELS_OE_Pos)
#define WJ_SRC_CLKMD_ELS_OE                       WJ_SRC_CLKMD_ELS_OE_Msk

#define WJ_SRC_CLKMD_ELS_IE_Pos                   (11U)
#define WJ_SRC_CLKMD_ELS_IE_Msk                   (0x1U << WJ_SRC_CLKMD_ELS_IE_Pos)
#define WJ_SRC_CLKMD_ELS_IE                       WJ_SRC_CLKMD_ELS_IE_Msk

#define WJ_SRC_CLKMD_EHS_OE_Pos                   (12U)
#define WJ_SRC_CLKMD_EHS_OE_Msk                   (0x1U << WJ_SRC_CLKMD_EHS_OE_Pos)
#define WJ_SRC_CLKMD_EHS_OE                       WJ_SRC_CLKMD_EHS_OE_Msk

#define WJ_SRC_CLKMD_EHS_IE_Pos                   (13U)
#define WJ_SRC_CLKMD_EHS_IE_Msk                   (0x1U << WJ_SRC_CLKMD_EHS_IE_Pos)
#define WJ_SRC_CLKMD_EHS_IE                       WJ_SRC_CLKMD_EHS_IE_Msk

#define WJ_SRC_CLKMD_PLL_EN_Pos                   (14U)
#define WJ_SRC_CLKMD_PLL_EN_Msk                   (0x1U << WJ_SRC_CLKMD_PLL_EN_Pos)
#define WJ_SRC_CLKMD_PLL_EN                       WJ_SRC_CLKMD_PLL_EN_Msk

#define WJ_SRC_CLKMD_USBPHY_EN_Pos                (15U)
#define WJ_SRC_CLKMD_USBPHY_EN_Msk                (0x1U << WJ_SRC_CLKMD_USBPHY_EN_Pos)
#define WJ_SRC_CLKMD_USBPHY_EN                    WJ_SRC_CLKMD_USBPHY_EN_Msk

/* SRC_LDOMD, offset: 0x108 */
#define WJ_SRC_LDOMD_PD_LDO_12V_Pos               (0U)
#define WJ_SRC_LDOMD_PD_LDO_12V_Msk               (0x1U << WJ_SRC_LDOMD_PD_LDO_12V_Pos)
#define WJ_SRC_LDOMD_PD_LDO_12V                   WJ_SRC_LDOMD_PD_LDO_12V_Msk

#define WJ_SRC_LDOMD_PD_LDO_3318V_Pos             (1U)
#define WJ_SRC_LDOMD_PD_LDO_3318V_Msk             (0x1U << WJ_SRC_LDOMD_PD_LDO_3318V_Pos)
#define WJ_SRC_LDOMD_PD_LDO_3318V                 WJ_SRC_LDOMD_PD_LDO_3318V_Msk

#define WJ_SRC_LDOMD_PD_BYP_EN3318_Pos            (2U)
#define WJ_SRC_LDOMD_PD_BYP_EN3318_Msk            (0x1U << WJ_SRC_LDOMD_PD_BYP_EN3318_Pos)
#define WJ_SRC_LDOMD_PD_BYP_EN3318                WJ_SRC_LDOMD_PD_BYP_EN3318_Msk

/* CODC_DCU_MODE, offset: 0x130 */
#define WJ_CODC_DCU_MODE_LP_MODE_Pos              (2U)
#define WJ_CODC_DCU_MODE_LP_MODE_Msk              (0x3U << WJ_CODC_DCU_MODE_LP_MODE_Pos)
#define WJ_CODC_DCU_MODE_CKOFF_PON                (0x0U << WJ_CODC_DCU_MODE_LP_MODE_Pos)
#define WJ_CODC_DCU_MODE_CKOFF_POFF               (0x1U << WJ_CODC_DCU_MODE_LP_MODE_Pos)
#define WJ_CODC_DCU_MODE_PON                      (0x2U << WJ_CODC_DCU_MODE_LP_MODE_Pos)

/* RETU_DCU_MODE, offset: 0x160 */
#define WJ_RETU_DCU_MODE_LP_MODE_Pos              (2U)
#define WJ_RETU_DCU_MODE_LP_MODE_Msk              (0x3U << WJ_RETU_DCU_MODE_LP_MODE_Pos)
#define WJ_RETU_DCU_MODE_CKOFF_PON                (0x0U << WJ_RETU_DCU_MODE_LP_MODE_Pos)
#define WJ_RETU_DCU_MODE_CKOFF_POFF               (0x1U << WJ_RETU_DCU_MODE_LP_MODE_Pos)
#define WJ_RETU_DCU_MODE_PON                      (0x2U << WJ_RETU_DCU_MODE_LP_MODE_Pos)

/* RETU_DCU_CGCR, offset: 0x16C */
#define WJ_RETU_DCU_CGCR_SMS_CLK_Pos              (0U)
#define WJ_RETU_DCU_CGCR_SMS_CLK_Msk              (0x1U << WJ_RETU_DCU_CGCR_SMS_CLK_Pos)
#define WJ_RETU_DCU_CGCR_SMS_CLK_EN               WJ_RETU_DCU_CGCR_SMS_CLK_Msk

/* RETU_DCU_CGSR, offset: 0x170 */
#define WJ_RETU_DCU_CGSR_SMS_CLK_Pos              (0U)
#define WJ_RETU_DCU_CGSR_SMS_CLK_Msk              (0x1U << WJ_RETU_DCU_CGSR_SMS_CLK_Pos)
#define WJ_RETU_DCU_CGSR_SMS_CLK_STATE            WJ_RETU_DCU_CGSR_SMS_CLK_Msk

/* RETU_DCU_CGSR, offset: 0x174 */
#define WJ_RETU_DCU_SWRST_SMS_CLK_Pos             (0U)
#define WJ_RETU_DCU_SWRST_SMS_CLK_Msk             (0x1U << WJ_RETU_DCU_SWRST_SMS_CLK_Pos)
#define WJ_RETU_DCU_SWRST_SMS_CLK_RESET           WJ_RETU_DCU_SWRST_SMS_CLK_Msk

/* PDU_DCU_MODE, offset: 0x190 */
#define WJ_PDU_DCU_MODE_LP_MODE_Pos               (2U)
#define WJ_PDU_DCU_MODE_LP_MODE_Msk               (0x3U << WJ_PDU_DCU_MODE_LP_MODE_Pos)
#define WJ_PDU_DCU_MODE_CKOFF_PON                 (0x0U << WJ_PDU_DCU_MODE_LP_MODE_Pos)
#define WJ_PDU_DCU_MODE_CKOFF_POFF                (0x1U << WJ_PDU_DCU_MODE_LP_MODE_Pos)
#define WJ_PDU_DCU_MODE_PON                       (0x2U << WJ_PDU_DCU_MODE_LP_MODE_Pos)

#define WJ_PDU_DCU_MODE_CR0_Pos                   (4U)
#define WJ_PDU_DCU_MODE_CR0_DIV1                  ((uint32_t)0x0U << WJ_PDU_DCU_MODE_CR0_Pos)
#define WJ_PDU_DCU_MODE_CR0_DIV2                  ((uint32_t)0x1U << WJ_PDU_DCU_MODE_CR0_Pos)
#define WJ_PDU_DCU_MODE_CR0_DIV4                  ((uint32_t)0x2U << WJ_PDU_DCU_MODE_CR0_Pos)
#define WJ_PDU_DCU_MODE_CR0_DIV8                  ((uint32_t)0x3U << WJ_PDU_DCU_MODE_CR0_Pos)

#define WJ_PDU_DCU_MODE_CR1_Pos                   (7U)
#define WJ_PDU_DCU_MODE_CR1_Msk                   (0x3U << WJ_PDU_DCU_MODE_CR1_Pos)
#define WJ_PDU_DCU_MODE_CR1_DIV1                  ((uint32_t)0x0U << WJ_PDU_DCU_MODE_CR1_Pos)
#define WJ_PDU_DCU_MODE_CR1_DIV2                  ((uint32_t)0x1U << WJ_PDU_DCU_MODE_CR1_Pos)
#define WJ_PDU_DCU_MODE_CR1_DIV4                  ((uint32_t)0x2U << WJ_PDU_DCU_MODE_CR1_Pos)
#define WJ_PDU_DCU_MODE_CR1_DIV8                  ((uint32_t)0x3U << WJ_PDU_DCU_MODE_CR1_Pos)

#define WJ_PDU_DCU_MODE_CR2_Pos                   (10U)
#define WJ_PDU_DCU_MODE_CR2_Msk                   (0x3U << WJ_PDU_DCU_MODE_CR2_Pos)
#define WJ_PDU_DCU_MODE_CR2_DIV1                  ((uint32_t)0x0U << WJ_PDU_DCU_MODE_CR2_Pos)
#define WJ_PDU_DCU_MODE_CR2_DIV2                  ((uint32_t)0x1U << WJ_PDU_DCU_MODE_CR2_Pos)
#define WJ_PDU_DCU_MODE_CR2_DIV4                  ((uint32_t)0x2U << WJ_PDU_DCU_MODE_CR2_Pos)
#define WJ_PDU_DCU_MODE_CR2_DIV8                  ((uint32_t)0x3U << WJ_PDU_DCU_MODE_CR2_Pos)

#define WJ_PDU_DCU_MODE_CR3_Pos                   (13U)
#define WJ_PDU_DCU_MODE_CR3_DIV1                  ((uint32_t)0x0U << WJ_PDU_DCU_MODE_CR3_Pos)
#define WJ_PDU_DCU_MODE_CR3_DIV2                  ((uint32_t)0x1U << WJ_PDU_DCU_MODE_CR3_Pos)
#define WJ_PDU_DCU_MODE_CR3_DIV4                  ((uint32_t)0x2U << WJ_PDU_DCU_MODE_CR3_Pos)
#define WJ_PDU_DCU_MODE_CR3_DIV8                  ((uint32_t)0x3U << WJ_PDU_DCU_MODE_CR3_Pos)

#define WJ_PDU_DCU_MODE_CR4_Pos                   (16U)
#define WJ_PDU_DCU_MODE_CR4_DIV1                  ((uint32_t)0x0U << WJ_PDU_DCU_MODE_CR4_Pos)
#define WJ_PDU_DCU_MODE_CR4_DIV2                  ((uint32_t)0x1U << WJ_PDU_DCU_MODE_CR4_Pos)
#define WJ_PDU_DCU_MODE_CR4_DIV4                  ((uint32_t)0x2U << WJ_PDU_DCU_MODE_CR4_Pos)
#define WJ_PDU_DCU_MODE_CR4_DIV8                  ((uint32_t)0x3U << WJ_PDU_DCU_MODE_CR4_Pos)

#define WJ_PDU_DCU_MODE_CR5_Pos                   (19U)
#define WJ_PDU_DCU_MODE_CR5_DIV1                  ((uint32_t)0x0U << WJ_PDU_DCU_MODE_CR5_Pos)
#define WJ_PDU_DCU_MODE_CR5_DIV2                  ((uint32_t)0x1U << WJ_PDU_DCU_MODE_CR5_Pos)
#define WJ_PDU_DCU_MODE_CR5_DIV4                  ((uint32_t)0x2U << WJ_PDU_DCU_MODE_CR5_Pos)
#define WJ_PDU_DCU_MODE_CR5_DIV8                  ((uint32_t)0x3U << WJ_PDU_DCU_MODE_CR5_Pos)

/* ANA_DCU_MODE, offset: 0x1C0 */
#define WJ_ANA_DCU_MODE_LPMODE_Pos                (2U)
#define WJ_ANA_DCU_MODE_LPMODE_Msk                (0x3U << WJ_ANA_DCU_MODE_LPMODE_Pos)
#define WJ_ANA_DCU_MODE_CKOFF_PON                 (0x0U << WJ_ANA_DCU_MODE_LPMODE_Pos)
#define WJ_ANA_DCU_MODE_CKOFF_POFF                (0x1U << WJ_ANA_DCU_MODE_LPMODE_Pos)
#define WJ_ANA_DCU_MODE_PON                       (0x2U << WJ_ANA_DCU_MODE_LPMODE_Pos)

/* CPU_DCU_MODE, offset: 0x1C0 */
#define WJ_CPU_DCU_MODE_LPMODE_Pos                (2U)
#define WJ_CPU_DCU_MODE_LPMODE_Msk                (0x3U << WJ_CPU_DCU_MODE_LPMODE_Pos)
#define WJ_CPU_DCU_MODE_CKOFF_PON                 (0x0U << WJ_CPU_DCU_MODE_LPMODE_Pos)
#define WJ_CPU_DCU_MODE_CKOFF_POFF                (0x1U << WJ_CPU_DCU_MODE_LPMODE_Pos)
#define WJ_CPU_DCU_MODE_PON                       (0x2U << WJ_CPU_DCU_MODE_LPMODE_Pos)

/* MCLK_RATIO, offset: 0x50c */
#define WJ_MCLK_RATIO_MCLK_DIV_Pos                (0U)
#define WJ_MCLK_RATIO_MCLK_DIV_Msk                (0xFU << WJ_MCLK_RATIO_MCLK_DIV_Pos)

#define WJ_MCLK_RATIO_MCLK_DFCCEN_Pos             (4U)
#define WJ_MCLK_RATIO_MCLK_DFCCEN_Msk             (0x1U << WJ_MCLK_RATIO_MCLK_DFCCEN_Pos)
#define WJ_MCLK_RATIO_MCLK_DFCCEN                 WJ_MCLK_RATIO_MCLK_DFCCEN_Msk

/* RESET_STATE, offset: 0x510 */
#define WJ_RESET_STATE_POR_Pos                    (0U)
#define WJ_RESET_STATE_POR_Msk                    (0x1U << WJ_RESET_STATE_POR_Pos)
#define WJ_RESET_STATE_POR_RESET                  WJ_RESET_STATE_POR_Msk

#define WJ_RESET_STATE_BOD_Pos                    (1U)
#define WJ_RESET_STATE_BOD_Msk                    (0x1U << WJ_RESET_STATE_BOD_Pos)
#define WJ_RESET_STATE_BOD_RESET                  WJ_RESET_STATE_BOD_Msk

#define WJ_RESET_STATE_PAD_Pos                    (2U)
#define WJ_RESET_STATE_PAD_Msk                    (0x1U << WJ_RESET_STATE_PAD_Pos)
#define WJ_RESET_STATE_PAD_RESET                  WJ_RESET_STATE_PAD_Msk

#define WJ_RESET_STATE_WDT_Pos                    (3U)
#define WJ_RESET_STATE_WDT_Msk                    (0x1U << WJ_RESET_STATE_WDT_Pos)
#define WJ_RESET_STATE_WDT_RESET                  WJ_RESET_STATE_WDT_Msk

#define WJ_RESET_STATE_CPU_Pos                    (4U)
#define WJ_RESET_STATE_CPU_Msk                    (0x1U << WJ_RESET_STATE_CPU_Pos)
#define WJ_RESET_STATE_CPU_RESET                  WJ_RESET_STATE_CPU_Msk

/* DFC_CTRL, offset: 0x518 */
#define WJ_DFC_CTRL_DFC_CNT_Pos                   (0U)
#define WJ_DFC_CTRL_DFC_CNT_Msk                   (0xFFU << WJ_DFC_CTRL_DFC_CNT_Pos)

#define WJ_DFC_CTRL_CLK_SW_MODE_Pos               (8U)
#define WJ_DFC_CTRL_CLK_SW_MODE_Msk               (0x1U << WJ_DFC_CTRL_CLK_SW_MODE_Pos)
#define WJ_DFC_CTRL_WAIT_CPU_QSPI_IDLE            WJ_DFC_CTRL_CLK_SW_MODE_Msk

/* MODE_CTRL, offset: 0x520 */
#define WJ_MODE_CTRL_LDO_HOLD_Pos                 (0U)
#define WJ_MODE_CTRL_LDO_HOLD_Msk                 (0x1U << WJ_MODE_CTRL_LDO_HOLD_Pos)
#define WJ_MODE_CTRL_LDO_HOLD                     WJ_MODE_CTRL_LDO_HOLD_Msk

#define WJ_MODE_CTRL_SCLK_HOLD_Pos                (1U)
#define WJ_MODE_CTRL_SCLK_HOLD_Msk                (0x1U << WJ_MODE_CTRL_SCLK_HOLD_Pos)
#define WJ_MODE_CTRL_SCLK_HOLD                    WJ_MODE_CTRL_SCLK_HOLD_Msk

#define WJ_MODE_CTRL_CODC_HOLD_Pos                (2U)
#define WJ_MODE_CTRL_CODC_HOLD_Msk                (0x1U << WJ_MODE_CTRL_CODC_HOLD_Pos)
#define WJ_MODE_CTRL_CODC_HOLD                    WJ_MODE_CTRL_CODC_HOLD_Msk

#define WJ_MODE_CTRL_RETU_DCU_HOLD_Pos            (3U)
#define WJ_MODE_CTRL_RETU_DCU_HOLD_Msk            (0x1U << WJ_MODE_CTRL_RETU_DCU_HOLD_Pos)
#define WJ_MODE_CTRL_RETU_DCU_HOLD                WJ_MODE_CTRL_RETU_DCU_HOLD_Msk

#define WJ_MODE_CTRL_PDU_DCU_HOLD_Pos             (4U)
#define WJ_MODE_CTRL_PDU_DCU_HOLD_Msk             (0x1U << WJ_MODE_CTRL_PDU_DCU_HOLD_Pos)
#define WJ_MODE_CTRL_PDU_DCU_HOLD                 WJ_MODE_CTRL_PDU_DCU_HOLD_Msk

#define WJ_MODE_CTRL_ANA_DCU_HOLD_Pos             (5U)
#define WJ_MODE_CTRL_ANA_DCU_HOLD_Msk             (0x1U << WJ_MODE_CTRL_ANA_DCU_HOLD_Pos)
#define WJ_MODE_CTRL_ANA_DCU_HOLD                 WJ_MODE_CTRL_ANA_DCU_HOLD_Msk

#define WJ_MODE_CTRL_CPU_DCU_HOLD_Pos             (6U)
#define WJ_MODE_CTRL_CPU_DCU_HOLD_Msk             (0x1U << WJ_MODE_CTRL_CPU_DCU_HOLD_Pos)
#define WJ_MODE_CTRL_CPU_DCU_HOLD                 WJ_MODE_CTRL_CPU_DCU_HOLD_Msk

#define WJ_MODE_CTRL_POWER_SWITCH_Pos             (7U)
#define WJ_MODE_CTRL_POWER_SWITCH_OFF_Msk         (0x0U << WJ_MODE_CTRL_POWER_SWITCH_Pos)
#define WJ_MODE_CTRL_POWER_SWITCH_ON_Msk          (0x1U << WJ_MODE_CTRL_POWER_SWITCH_Pos)
#define WJ_MODE_CTRL_POWER_SWITCH_OFF             WJ_MODE_CTRL_POWER_SWITCH_OFF_Msk
#define WJ_MODE_CTRL_POWER_SWITCH_ON              WJ_MODE_CTRL_POWER_SWITCH_ON_Msk

#define WJ_MODE_CTRL_POWER_SWITCH_AWJ_Pos         (8U)
#define WJ_MODE_CTRL_POWER_SWITCH_AWJ_OFF_Msk     (0x1U << WJ_MODE_CTRL_POWER_SWITCH_AWJ_Pos)
#define WJ_MODE_CTRL_POWER_SWITCH_AWJ_ON_Msk      (0x0U << WJ_MODE_CTRL_POWER_SWITCH_AWJ_Pos)
#define WJ_MODE_CTRL_POWER_SWITCH_AWJ_OFF         WJ_MODE_CTRL_POWER_SWITCH_AWJ_OFF_Msk
#define WJ_MODE_CTRL_POWER_SWITCH_AWJ_ON          WJ_MODE_CTRL_POWER_SWITCH_AWJ_ON_Msk

/* MEM_RET_CTRL, offset: 0x524 */
#define WJ_MEM_RET_CTRL_SMU0_PD_Pos               (0U)
#define WJ_MEM_RET_CTRL_SMU0_PD_Msk               (0x1U << WJ_MEM_RET_CTRL_SMU0_PD_Pos)
#define WJ_MEM_RET_CTRL_SMU0_PD                   WJ_MEM_RET_CTRL_SMU0_PD_Msk

#define WJ_MEM_RET_CTRL_SMU1_PD_Pos               (1U)
#define WJ_MEM_RET_CTRL_SMU1_PD_Msk               (0x1U << WJ_MEM_RET_CTRL_SMU1_PD_Pos)
#define WJ_MEM_RET_CTRL_SMU1_PD                   WJ_MEM_RET_CTRL_SMU1_PD_Msk

/*IP_CTRL   */
#define WJ_IP_CTRL_USBPHY_PLL_PD_Pos              (2U)
#define WJ_IP_CTRL_USBPHY_PLL_PD_Msk              (0x1U << WJ_IP_CTRL_USBPHY_PLL_PD_Pos)
#define WJ_IP_CTRL_USBPHY_PLL_PD                  WJ_IP_CTRL_USBPHY_PLL_PD_Msk

#define WJ_IP_CTRL_USBPHY_SUSPEND_Pos             (9U)
#define WJ_IP_CTRL_USBPHY_SUSPEND_Msk             (0x1U << WJ_IP_CTRL_USBPHY_SUSPEND_Pos)
#define WJ_IP_CTRL_USBPHY_SUSPEND                 WJ_IP_CTRL_USBPHY_SUSPEND_Msk

#define WJ_IP_CTRL_CODEC_Pos                      (10U)
#define WJ_IP_CTRL_CODEC_Msk                      (0x1U << WJ_IP_CTRL_CODEC_Pos)
#define WJ_IP_CTRL_CODEC                          WJ_IP_CTRL_CODEC_Msk


static inline uint32_t wj_get_apb0_div(wj_pmu_reg_t *pmu_base)
{
    return (uint32_t)((uint32_t)1U << ((uint32_t)(pmu_base->PDU_DCU_MODE & (uint32_t)WJ_PDU_DCU_MODE_CR1_DIV8) >> WJ_PDU_DCU_MODE_CR1_Pos));
}

static inline void wj_set_apb0_div(wj_pmu_reg_t *pmu_base, uint32_t div)
{
    pmu_base->PDU_DCU_MODE &= ~WJ_PDU_DCU_MODE_CR1_Msk;
    pmu_base->PDU_DCU_MODE |= (uint32_t)(div  << WJ_PDU_DCU_MODE_CR1_Pos);
}

static inline uint32_t wj_get_apb1_div(wj_pmu_reg_t *pmu_base)
{
    return (uint32_t)((uint32_t)1U << (uint32_t)((uint32_t)(pmu_base->PDU_DCU_MODE & (uint32_t)WJ_PDU_DCU_MODE_CR2_DIV8) >> WJ_PDU_DCU_MODE_CR2_Pos));
}

static inline void wj_set_apb1_div(wj_pmu_reg_t *pmu_base, uint32_t div)
{
    pmu_base->PDU_DCU_MODE &= ~WJ_PDU_DCU_MODE_CR2_Msk;
    pmu_base->PDU_DCU_MODE |= (uint32_t)(div << WJ_PDU_DCU_MODE_CR2_Pos);
}

static inline void wj_set_dfcc_enable(wj_pmu_reg_t *pmu_base)
{
    pmu_base->DFCC |= WJ_DFCC_DFC_EN;
}

static inline uint32_t wj_get_dfcc_state(wj_pmu_reg_t *pmu_base)
{
    return (pmu_base->DFCC & WJ_DFCC_DFC_EN);
}

static inline uint32_t wj_get_ils_clock_state(wj_pmu_reg_t *pmu_base)
{
    return (pmu_base->CLKSTBR & WJ_CLKSTBR_ILSSTB_STATE);
}

static inline uint32_t wj_get_ihs_clock_state(wj_pmu_reg_t *pmu_base)
{
    return (pmu_base->CLKSTBR & WJ_CLKSTBR_IHSSTB_STATE);
}

static inline uint32_t wj_get_els_clock_state(wj_pmu_reg_t *pmu_base)
{
    return (pmu_base->CLKSTBR & WJ_CLKSTBR_ELSSTB_STATE);
}

static inline uint32_t wj_get_ehs_clock_state(wj_pmu_reg_t *pmu_base)
{
    return (pmu_base->CLKSTBR & WJ_CLKSTBR_EHSSTB_STATE);
}

static inline uint32_t wj_get_pll_clock_state(wj_pmu_reg_t *pmu_base)
{
    return (pmu_base->CLKSTBR & WJ_CLKSTBR_PLLSTB_STATE);
}

static inline void wj_set_pll_clk_src_ihs(wj_pmu_reg_t *pmu_base)
{
    pmu_base->PCR &= ~WJ_PCR_PLLSRC_SEL_Msk;
    pmu_base->PCR |= WJ_PCR_PLLSRC_SEL_IHS;
}

static inline void wj_set_pll_clk_src_ehs(wj_pmu_reg_t *pmu_base)
{
    pmu_base->PCR &= ~WJ_PCR_PLLSRC_SEL_Msk;
    pmu_base->PCR |= WJ_PCR_PLLSRC_SEL_EHS;
}

static inline void wj_set_pll_clk_src_usbphy(wj_pmu_reg_t *pmu_base)
{
    pmu_base->PCR &= ~WJ_PCR_PLLSRC_SEL_Msk;
    pmu_base->PCR |= WJ_PCR_PLLSRC_SEL_USB_PHY;
}

static inline void wj_set_pll_clkin_div(wj_pmu_reg_t *pmu_base, uint32_t div)
{
    pmu_base->PCR &= ~WJ_PCR_PLLSRC_DIV_Msk;
    pmu_base->PCR |= (uint32_t)(div << WJ_PCR_PLLSRC_DIV_Pos);
}

static inline void wj_set_pdu_mode_change_enable(wj_pmu_reg_t *pmu_base)
{
    pmu_base->MODE_CHG_EN |= WJ_MODE_CHG_EN_DCU_MD_EN;
}

static inline uint32_t wj_get_pdu_mode_change_state(wj_pmu_reg_t *pmu_base)
{
    return (pmu_base->MODE_CHG_EN & WJ_MODE_CHG_EN_DCU_MD_EN);
}

static inline void wj_set_clk_change_enable(wj_pmu_reg_t *pmu_base)
{
    pmu_base->MODE_CHG_EN |= WJ_MODE_CHG_EN_SRC_CLK_EN;
}

static inline uint32_t wj_get_clk_change_state(wj_pmu_reg_t *pmu_base)
{
    return (pmu_base->MODE_CHG_EN & WJ_MODE_CHG_EN_SRC_CLK_EN);
}

static inline void wj_set_src_ldo_change_enable(wj_pmu_reg_t *pmu_base)
{
    pmu_base->MODE_CHG_EN |= WJ_MODE_CHG_EN_SRC_LDO_EN;
}

static inline uint32_t wj_get_src_ldo_change_state(wj_pmu_reg_t *pmu_base)
{
    return (pmu_base->MODE_CHG_EN & WJ_MODE_CHG_EN_SRC_LDO_EN);
}

static inline void wj_set_pll_fraction_div_enable(wj_pmu_reg_t *pmu_base)
{
    pmu_base->PCR |= WJ_PDU_EN_FRAC_EN;
}

static inline void wj_set_pll_fraction_div_disable(wj_pmu_reg_t *pmu_base)
{
    pmu_base->PCR &= ~WJ_PDU_EN_FRAC_EN;
}

static inline void wj_set_fn_ration(wj_pmu_reg_t *pmu_base, uint32_t ration)
{
    pmu_base->PCR &= ~WJ_PCR_FN_RATION_Msk;
    pmu_base->PCR |= (ration << WJ_PCR_FN_RATION_Pos);
}

static inline void wj_set_fd_ration(wj_pmu_reg_t *pmu_base, uint32_t ration)
{
    pmu_base->PCR &= ~WJ_PCR_FD_RATION_Msk;
    pmu_base->PCR |= (ration << WJ_PCR_FD_RATION_Pos) | WJ_PDU_EN_FRAC_EN;
}

static inline void wj_set_ls_ils(wj_pmu_reg_t *pmu_base)
{
    pmu_base->SRC_CLKMD &= ~WJ_SRC_CLKMD_LS_SEL;
}

static inline void wj_set_ls_els(wj_pmu_reg_t *pmu_base)
{
    pmu_base->SRC_CLKMD |= WJ_SRC_CLKMD_LS_SEL;
}

static inline void wj_set_clock0_ihs(wj_pmu_reg_t *pmu_base)
{
    pmu_base->SRC_CLKMD &= ~WJ_SRC_CLKMD_CLK_SEL0;
}

static inline void wj_set_clock0_usbphy(wj_pmu_reg_t *pmu_base)
{
    pmu_base->SRC_CLKMD |= WJ_SRC_CLKMD_CLK_SEL0;
}

static inline void wj_set_clock1_pll(wj_pmu_reg_t *pmu_base)
{
    pmu_base->SRC_CLKMD &= ~WJ_SRC_CLKMD_CLK_SEL1;
}

static inline void wj_set_clock1_ehs(wj_pmu_reg_t *pmu_base)
{
    pmu_base->SRC_CLKMD |= WJ_SRC_CLKMD_CLK_SEL1;
}

static inline void wj_set_sysclk_select_clock0(wj_pmu_reg_t *pmu_base)
{
    pmu_base->SRC_CLKMD &= ~WJ_SRC_CLKMD_CLK_SEL2;
}

static inline void wj_set_sysclk_select_clock1(wj_pmu_reg_t *pmu_base)
{
    pmu_base->SRC_CLKMD |= WJ_SRC_CLKMD_CLK_SEL2;
}

static inline void wj_set_ils_rc_control_enable(wj_pmu_reg_t *pmu_base)
{
    pmu_base->SRC_CLKMD |= WJ_SRC_CLKMD_ILS_EN;
}

static inline void wj_set_ils_rc_control_disable(wj_pmu_reg_t *pmu_base)
{
    pmu_base->SRC_CLKMD &= ~WJ_SRC_CLKMD_ILS_EN;
}

static inline void wj_set_ihs_rc_control_enable(wj_pmu_reg_t *pmu_base)
{
    pmu_base->SRC_CLKMD |= WJ_SRC_CLKMD_IHS_EN;
}

static inline void wj_set_ihs_rc_control_disable(wj_pmu_reg_t *pmu_base)
{
    pmu_base->SRC_CLKMD &= ~WJ_SRC_CLKMD_IHS_EN;
}

static inline void wj_set_els_output_enable(wj_pmu_reg_t *pmu_base)
{
    pmu_base->SRC_CLKMD |= WJ_SRC_CLKMD_ELS_OE;
}

static inline void wj_set_els_output_disable(wj_pmu_reg_t *pmu_base)
{
    pmu_base->SRC_CLKMD &= ~WJ_SRC_CLKMD_ELS_OE;
}

static inline void wj_set_els_input_enable(wj_pmu_reg_t *pmu_base)
{
    pmu_base->SRC_CLKMD |= WJ_SRC_CLKMD_ELS_IE;
}

static inline void wj_set_els_input_disable(wj_pmu_reg_t *pmu_base)
{
    pmu_base->SRC_CLKMD &= ~WJ_SRC_CLKMD_ELS_IE;
}

static inline void wj_set_ehs_output_enable(wj_pmu_reg_t *pmu_base)
{
    pmu_base->SRC_CLKMD |= WJ_SRC_CLKMD_EHS_OE;
}

static inline void wj_set_ehs_output_disable(wj_pmu_reg_t *pmu_base)
{
    pmu_base->SRC_CLKMD &= ~WJ_SRC_CLKMD_EHS_OE;
}

static inline void wj_set_ehs_input_enable(wj_pmu_reg_t *pmu_base)
{
    pmu_base->SRC_CLKMD |= WJ_SRC_CLKMD_EHS_IE;
}

static inline void wj_set_ehs_input_disable(wj_pmu_reg_t *pmu_base)
{
    pmu_base->SRC_CLKMD &= ~WJ_SRC_CLKMD_EHS_IE;
}

static inline void wj_set_pll_output_enable(wj_pmu_reg_t *pmu_base)
{
    pmu_base->SRC_CLKMD |= WJ_SRC_CLKMD_PLL_EN;
}

static inline void wj_set_pll_output_disable(wj_pmu_reg_t *pmu_base)
{
    pmu_base->SRC_CLKMD &= ~WJ_SRC_CLKMD_PLL_EN;
}

static inline void wj_set_usbphy_output_enable(wj_pmu_reg_t *pmu_base)
{
    pmu_base->SRC_CLKMD |= WJ_SRC_CLKMD_USBPHY_EN;
}

static inline void wj_set_usbphy_output_disable(wj_pmu_reg_t *pmu_base)
{
    pmu_base->SRC_CLKMD &= ~WJ_SRC_CLKMD_USBPHY_EN;
}

static inline void wj_set_mclk_dfcc_enable(wj_pmu_reg_t *pmu_base)
{
    pmu_base->MCLK_RATIO |= WJ_MCLK_RATIO_MCLK_DFCCEN;
}

static inline uint32_t wj_get_mclk_dfcc_state(wj_pmu_reg_t *pmu_base)
{
    return (pmu_base->MCLK_RATIO & WJ_MCLK_RATIO_MCLK_DFCCEN);
}

static inline void wj_set_mclk_div(wj_pmu_reg_t *pmu_base, uint32_t div)
{
    pmu_base->MCLK_RATIO &= ~WJ_MCLK_RATIO_MCLK_DIV_Msk;
    pmu_base->MCLK_RATIO |= (uint32_t)((div - 1U) << WJ_MCLK_RATIO_MCLK_DIV_Pos);
}

static inline uint32_t wj_get_mclk_div(wj_pmu_reg_t *pmu_base)
{
    return ((pmu_base->MCLK_RATIO & WJ_MCLK_RATIO_MCLK_DIV_Msk) + 1U);
}

static inline void wj_set_matrix_clock_gate_on(wj_pmu_reg_t *pmu_base, uint32_t inx)
{
    pmu_base->PDU_DCU_MCGCR = pmu_base->PDU_DCU_MCGSR | ((uint32_t)1U << inx);
}

static inline void wj_set_matrix_clock_gate_off(wj_pmu_reg_t *pmu_base, uint32_t inx)
{
    pmu_base->PDU_DCU_MCGCR = pmu_base->PDU_DCU_MCGSR & (~((uint32_t)1U << inx));
}

static inline void wj_set_matrix_peripheral_reset(wj_pmu_reg_t *pmu_base, uint32_t inx)
{
    pmu_base->PDU_DCU_MSWRST &= ~((uint32_t)1U << inx);
    pmu_base->PDU_DCU_MSWRST |= ((uint32_t)1U << inx);
}

static inline void wj_set_apb0_clock_gate_on(wj_pmu_reg_t *pmu_base, uint32_t inx)
{
    pmu_base->PDU_DCU_P0CGCR = pmu_base->PDU_DCU_P0CGSR | ((uint32_t)1U << inx);
}

static inline void wj_set_apb0_clock_gate_off(wj_pmu_reg_t *pmu_base, uint32_t inx)
{
    pmu_base->PDU_DCU_P0CGCR = pmu_base->PDU_DCU_P0CGSR & (~((uint32_t)1U << inx));
}

static inline void wj_set_apb0_peripheral_reset(wj_pmu_reg_t *pmu_base, uint32_t inx)
{
    pmu_base->PDU_DCU_P0SWRST &= ~((uint32_t)1U << inx);
    pmu_base->PDU_DCU_P0SWRST |= ((uint32_t)1U << inx);
}

static inline void wj_set_apb1_clock_gate_on(wj_pmu_reg_t *pmu_base, uint32_t inx)
{
    pmu_base->PDU_DCU_P1CGCR = pmu_base->PDU_DCU_P1CGSR | ((uint32_t)1U << inx);
}

static inline void wj_set_apb1_clock_gate_off(wj_pmu_reg_t *pmu_base, uint32_t inx)
{
    pmu_base->PDU_DCU_P1CGCR = pmu_base->PDU_DCU_P1CGSR & ~((uint32_t)1U << inx);
}

static inline void wj_set_apb1_peripheral_reset(wj_pmu_reg_t *pmu_base, uint32_t inx)
{
    pmu_base->PDU_DCU_P1SWRST &= ~((uint32_t)1U << inx);
    pmu_base->PDU_DCU_P1SWRST |= ((uint32_t)1U << inx);
}

/*PMU CONFIG*/
static inline void wj_pmu_lp_wait_clk_on_mode_enable(wj_pmu_reg_t *pmu_base)
{
    pmu_base->LPCR &= ~WJ_LPCR_LP_WAIT_CLK_ON;
}

static inline void wj_pmu_lp_wait_clk_off_mode_enable(wj_pmu_reg_t *pmu_base)
{
    pmu_base->LPCR |= WJ_LPCR_LP_WAIT_CLK_OFF;
}

static inline void wj_pmu_lp_stop_mode_enable(wj_pmu_reg_t *pmu_base)
{
    pmu_base->LPCR &= ~WJ_LPCR_LP_STOP_MODE;
}

static inline void wj_pmu_lp_standby_mode_enable(wj_pmu_reg_t *pmu_base)
{
    pmu_base->LPCR |= WJ_LPCR_LP_STANDBY_MODE;
}

static inline void wj_pmu_allow_lp_mode(wj_pmu_reg_t *pmu_base)
{
    pmu_base->LPCR |= WJ_LPCR_LOW_MODE_EN;
}

static inline void wj_pmu_unallow_lp_mode(wj_pmu_reg_t *pmu_base)
{
    pmu_base->LPCR &= ~WJ_LPCR_LOW_MODE_EN;
}

static inline void wj_set_wakeup_source0(wj_pmu_reg_t *pmu_base, uint32_t wakeupn)
{
    pmu_base->WIME0 &= ~((uint32_t)1U << wakeupn);
}

static inline void wj_clear_wakeup_source0(wj_pmu_reg_t *pmu_base, uint32_t wakeupn)
{
    pmu_base->WIME0 |= ((uint32_t)1U << wakeupn);
}

static inline void wj_isram0_power_on_in_stop_mode(wj_pmu_reg_t *pmu_base)
{
    pmu_base->MEM_RET_CTRL &= ~WJ_MEM_RET_CTRL_SMU0_PD;
}

static inline void wj_isram0_power_off_in_stop_mode(wj_pmu_reg_t *pmu_base)
{
    pmu_base->MEM_RET_CTRL |= WJ_MEM_RET_CTRL_SMU0_PD;
}

static inline void wj_isram1_power_on_in_stop_mode(wj_pmu_reg_t *pmu_base)
{
    pmu_base->MEM_RET_CTRL &= ~WJ_MEM_RET_CTRL_SMU1_PD;
}

static inline void wj_isram1_power_off_in_stop_mode(wj_pmu_reg_t *pmu_base)
{
    pmu_base->MEM_RET_CTRL |= WJ_MEM_RET_CTRL_SMU1_PD;
}

static inline void wj_codec_power_on(wj_pmu_reg_t *pmu_base)
{
    pmu_base->IP_CTRL |= WJ_IP_CTRL_CODEC;
}

static inline void wj_codec_power_off(wj_pmu_reg_t *pmu_base)
{
    pmu_base->IP_CTRL &= ~WJ_IP_CTRL_CODEC;
}

static inline void wj_usb_phy_power_on(wj_pmu_reg_t *pmu_base)
{
    pmu_base->IP_CTRL &= ~WJ_IP_CTRL_USBPHY_PLL_PD;
}

static inline void wj_usb_phy_power_off(wj_pmu_reg_t *pmu_base)
{
    pmu_base->IP_CTRL |= WJ_IP_CTRL_USBPHY_PLL_PD | WJ_IP_CTRL_USBPHY_SUSPEND;
}

static inline void wj_pmu_trim_pd_ldo_vtr12_set_1V25(wj_pmu_reg_t *pmu_base)
{
    pmu_base->PMU_TRIM &= ~WJ_PD_LDO_VTR12_TRIM_Msk;
    pmu_base->PMU_TRIM |= (uint32_t)(0x5U << WJ_PD_LDO_VTR12_TRIM_Pos);
}

static inline void wj_pmu_trim_pd_ldo_vtr12_set_1V3(wj_pmu_reg_t *pmu_base)
{
    pmu_base->PMU_TRIM &= ~WJ_PD_LDO_VTR12_TRIM_Msk;
    pmu_base->PMU_TRIM |= (uint32_t)(0x6U << WJ_PD_LDO_VTR12_TRIM_Pos);
}

static inline void wj_pmu_trim_pd_ldo_vtr12_set_1V35(wj_pmu_reg_t *pmu_base)
{
    pmu_base->PMU_TRIM &= ~WJ_PD_LDO_VTR12_TRIM_Msk;
    pmu_base->PMU_TRIM |= (uint32_t)(0x7U << WJ_PD_LDO_VTR12_TRIM_Pos);
}

#ifdef __cplusplus
}
#endif

#endif
