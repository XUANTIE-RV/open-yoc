/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     wj_jupiter_pmu.h
 * @brief    head file for jupiter pmu
 * @version  V1.0
 * @date     22. apr 2020
 ******************************************************************************/
#ifndef _WJ_JUPITER_PMU_H_
#define _WJ_JUPITER_PMU_H_

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
    __IOM uint32_t PCR1;              /* Offset: 0x030 (R/W) pll control register1 */
    uint32_t RESERVED1[9];
    __IOM uint32_t PMU_TRIM;          /* Offset: 0x058 (R/W) RC/POR/BOR trimming control register */
    uint32_t RESERVED2[9];
    __IOM uint32_t LC_WIME;           /* Offset: 0x080 (R/W) LC wake up source mask register */
    __IOM uint32_t BC_WIME;           /* Offset: 0x084 (R/W) BC wake up source mask register */
    uint32_t RESERVED3[30];
    __IOM uint32_t MODE_CHG_EN;       /* Offset: 0x100 (R/W) src/dcu mode change register */
    __IOM uint32_t SRC_CLKMD;         /* Offset: 0x104 (R/W) src clock mode register */
    __IOM uint32_t SRC_LDOMD;         /* Offset: 0x108 (R/W) src ldo mode register */
    uint32_t RESERVED4[9];
    __IOM uint32_t GNSS_DCU_MODE;     /* Offset: 0x130 (R/W) GNSS DCU clock gate control register */
    __IOM uint32_t GNSS_DCU_CGCR;     /* Offset: 0x134 (R/W) GNSS DCU Clock gate control register */
    uint32_t RESERVED5;
    __IOM uint32_t GNSS_DCU_SWRST;    /* Offset: 0x13c (R/W) GNSS DCU software Reset control register */
    __IOM uint32_t LC_CPU_DCU_MODE;   /* Offset: 0x140 (R/W) LC CPU DCU mode register */
    uint32_t RESERVED6[3];
    __IOM uint32_t BC_CPU_DCU_MODE;   /* Offset: 0x150 (R/W) BC CPU DCU mode register */
    uint32_t RESERVED7[3];
    __IOM uint32_t RETU_DCU_MODE;     /* Offset: 0x160 (R/W) RETU DCU mode register */
    uint32_t RESERVED8[11];
    __IOM uint32_t PDU_DCU_MODE;      /* Offset: 0x190 (R/W) PDU DCU mode register */
    __IOM uint32_t PDU_DCU_LC_MCGCR;  /* Offset: 0x194 (R/W) PDU DCU LC matrix clock gate control register */
    __IOM uint32_t PDU_DCU_LC_MSWRST; /* Offset: 0x198 (R/W) PDU DCU LC matrix software reset control register */
    __IOM uint32_t PDU_DCU_BC_MCGCR;   /* Offset: 0x19c (W) PDU DCU matrix clock gate control register */
    uint32_t RESERVED9;
    __IOM uint32_t PDU_DCU_BC_MSWRST; /* Offset: 0x1a4 (R/W) PDU DCU BC matrix software reset control register */
    __IOM uint32_t PDU_DCU_P0CGCR;     /* Offset: 0x1a8 (W) PDU DCU apb0 clock gate control register */
    uint32_t RESERVED10;
    __IOM uint32_t PDU_DCU_P0SWRST;   /* Offset: 0x1b0 (R/W) PDU DCU apb0 software reset control register */
    __IOM uint32_t PDU_DCU_P1CGCR;     /* Offset: 0x1b4 (R) PDU DCU apb1 clock gate status register */
    uint32_t RESERVED11;
    __IOM uint32_t PDU_DCU_P1SWRST;   /* Offset: 0x1bc (R/W) PDU DCU apb1 software reset control register */
    __IOM uint32_t ANA_DCU_MODE;      /* Offset: 0x1c0 (R/W) ANA DCU mode register */
    uint32_t RESERVED12[35];
    __IOM uint32_t AON_DCU_MODE;      /* Offset: 0x250 (R/W) AON DCU mode register */
    uint32_t RESERVED13[2];
    uint32_t AOAPB_DCU_CKGT;          /* Offset: 0x25c (R/W) AOAPB DCU clock gate control register */
    uint32_t RESERVED14;
    __IOM uint32_t AOAPB_DCU_SWRST;   /* Offset: 0x264 (R/W) AOAPB DCU software reset control register */
    uint32_t RESERVED15[101];
    __IM uint32_t SOC_ID;             /* Offset: 0x3fc (R) SOC_ID register */
    __IOM uint32_t USER_DEFINE[4];    /* Offset: 0x400-0x40c (R/W) save data in lowpower mode */
    uint32_t RESERVED16[60];
    __IOM uint32_t DCU_RST_DUR;       /* Offset: 0x500 (R/W) All DCU Reset Time of Duration Register */
    __IOM uint32_t DCU_ISO_DUR;       /* Offset: 0x504 (R/W) All DCU Isolation Time of Duration Register */
    __IOM uint32_t RET_CNT;           /* Offset: 0x508 (R/W) RET count register */
    __IOM uint32_t CLK_RATIO;         /* Offset: 0x50c (R/W) clock ratio register */
    __IOM uint32_t RESET_STATE;       /* Offset: 0x510 (R/W) RESET state register */
    __IOM uint32_t IP_CTRL;           /* Offset: 0x514 (R/W) IP control register */
    __IOM uint32_t DFC_CTRL;          /* Offset: 0x518 (R/W) DFC control register */
    __IOM uint32_t BC_RAM_DEKAY;      /* Offset: 0x51c (R/W) BC RAM delay time register */
    __IOM uint32_t MODE_CTRL;         /* Offset: 0x520 (R/W) MODE control register */
    __IOM uint32_t MEM_RET_CTRL;      /* Offset: 0x524 (R/W) MEM retention control register */
    __IOM uint32_t BC_CTRL;           /* Offset: 0x528 (R/W) BC CPU Control register */
    uint32_t RESERVED17;
    __IOM uint32_t APWR_CNT;          /* Offset: 0x530 (R/W) AON_PWR Stable Count register */
    __IOM uint32_t RLDO_CNT;          /* Offset: 0x534 (R/W) RLDO_PWR stable Count register */
    __IOM uint32_t MPWR_CNT;          /* Offset: 0x538 (R/W) MAIN_PWR stable Count register */
    __IOM uint32_t STROBE_CNT;        /* Offset: 0x53c (R/W) RAR strobe counter register */
    __IOM uint32_t RPWR_CNT;          /* Offset: 0x540 (R/W) RF_PWR stable count register */
    __IOM uint32_t RTC_ISO_CTRL;      /* Offset: 0x544 (R/W) RTC isolation control register */
    __IOM uint32_t RF_STATE;          /* Offset: 0x548 (R/W) RF state register */
} wj_pmu_reg_t;

/* LPCR, offset: 0x00 */
#define WJ_LPCR_LOW_MODE_Pos                      (2U)
#define WJ_LPCR_LOW_MODE_Msk                      (0x1U << WJ_LPCR_LOW_MODE_Pos)
#define WJ_LPCR_LOW_MODE_EN                       WJ_LPCR_LOW_MODE_Msk

#define WJ_LPCR_LP_STATE_Pos                      (5U)
#define WJ_LPCR_LP_STATE_Msk                      (0x1U << WJ_LPCR_LP_STATE_Pos)
#define WJ_LPCR_LP_STATE                          WJ_LPCR_LP_STATE_Msk

#define WJ_LPCR_CPU0_RUN_Pos                      (6U)
#define WJ_LPCR_CPU0_RUN_Msk                      (0x1U << WJ_LPCR_CPU0_RUN_Pos)
#define WJ_LPCR_CPU0_RUN                          WJ_LPCR_CPU0_RUN_Msk

#define WJ_LPCR_CPU1_RUN_Pos                      (7U)
#define WJ_LPCR_CPU1_RUN_Msk                      (0x1U << WJ_LPCR_CPU1_RUN_Pos)
#define WJ_LPCR_CPU1_RUN                          WJ_LPCR_CPU1_RUN_Msk

#define WJ_LPCR_BOOT_FROM_Pos                     (11U)
#define WJ_LPCR_BOOT_FROM_Msk                     (0x1U << WJ_LPCR_BOOT_FROM_Pos)
#define WJ_LPCR_BOOT_FROM                         WJ_LPCR_BOOT_FROM_Msk

#define WJ_LPCR_LP_MODE_SWITCH_Pos                (16U)
#define WJ_LPCR_LP_MODE_SWITCH_Msk                (0x7U << WJ_LPCR_LP_MODE_SWITCH_Pos)
#define WJ_LPCR_LP_MODE_WAIT_CLK_ON_MODE          (0x0U << WJ_LPCR_LP_MODE_SWITCH_Pos)
#define WJ_LPCR_LP_MODE_WAIT_CLK_OFF_MODE         (0x1U << WJ_LPCR_LP_MODE_SWITCH_Pos)
#define WJ_LPCR_LP_MODE_RETENTION_MODE            (0x2U << WJ_LPCR_LP_MODE_SWITCH_Pos)
#define WJ_LPCR_LP_MODE_BACKUP_MODE               (0x3U << WJ_LPCR_LP_MODE_SWITCH_Pos)
#define WJ_LPCR_LP_MODE_STANDBY_MODE              (0x4U << WJ_LPCR_LP_MODE_SWITCH_Pos)

/* CLKSTBR, offset: 0x18 */
#define WJ_CLKSTBR_ELSSTB_Pos                     (2U)
#define WJ_CLKSTBR_ELSSTB_Msk                     (0x1U << WJ_CLKSTBR_ELSSTB_Pos)
#define WJ_CLKSTBR_ELSSTB_STATE                   WJ_CLKSTBR_ELSSTB_Msk

#define WJ_CLKSTBR_PLLSTB_Pos                     (4U)
#define WJ_CLKSTBR_PLLSTB_Msk                     (0x1U << WJ_CLKSTBR_PLLSTB_Pos)
#define WJ_CLKSTBR_PLLSTB_STATE                   WJ_CLKSTBR_PLLSTB_Msk

/* CLKSTBST, offset: 0x1c */
#define WJ_CLKSTBST_ELSIS_Pos                     (2U)
#define WJ_CLKSTBST_ELSIS_Msk                     (0x1U << WJ_CLKSTBST_ELSIS_Pos)
#define WJ_ELS_INTERRUPT_EN                       WJ_CLKSTBST_ELSIS_Msk

#define WJ_CLKSTBST_PLLIS_Pos                     (4U)
#define WJ_CLKSTBST_PLLIS_Msk                     (0x1U << WJ_CLKSTBST_PLLIS_Pos)
#define WJ_PLL_INTERRUPT_EN                       WJ_CLKSTBST_PLLIS_Msk

/* CLKSTBMK, offset: 0x20 */
#define WJ_CLKSTBMK_ELSIM_Pos                     (2U)
#define WJ_CLKSTBMK_ELSIM_Msk                     (0x1U << WJ_CLKSTBMK_ELSIM_Pos)
#define WJ_ELS_INTERRUPT_MASK_EN                  WJ_CLKSTBMK_ELSIM_Msk

#define WJ_CLKSTBMK_PLLIM_Pos                     (4U)
#define WJ_CLKSTBMK_PLLIM_Msk                     (0x1U << WJ_CLKSTBMK_PLLIM_Pos)
#define WJ_PLL_INTERRUPT_MASK_EN                  WJ_CLKSTBMK_PLLIM_Msk

/* DFCC, offset: 0x28 */
#define WJ_DFCC_DFCEN_LC_MCLK_Pos                 (0U)
#define WJ_DFCC_DFCEN_LC_MCLK_Msk                 (0x1U << WJ_DFCC_DFCEN_LC_MCLK_Pos)
#define WJ_DFCC_DFC_LC_MCLK_EN                    WJ_DFCC_DFCEN_LC_MCLK_Msk

#define WJ_DFCC_DFCEN_LC_PCLK_Pos                 (1U)
#define WJ_DFCC_DFCEN_LC_PCLK_Msk                 (0x1U << WJ_DFCC_DFCEN_LC_PCLK_Pos)
#define WJ_DFCC_DFC_LC_PCLK_EN                    WJ_DFCC_DFCEN_LC_PCLK_Msk

#define WJ_DFCC_DFCEN_BC_MCLK_Pos                 (2U)
#define WJ_DFCC_DFCEN_BC_MCLK_Msk                 (0x1U << WJ_DFCC_DFCEN_BC_MCLK_Pos)
#define WJ_DFCC_DFC_BC_MCLK_EN                    WJ_DFCC_DFCEN_BC_MCLK_Msk

#define WJ_DFCC_DFCEN_BC_PCLK_Pos                 (3U)
#define WJ_DFCC_DFCEN_BC_PCLK_Msk                 (0x1U << WJ_DFCC_DFCEN_BC_PCLK_Pos)
#define WJ_DFCC_DFC_BC_PCLK_EN                    WJ_DFCC_DFCEN_BC_PCLK_Msk

#define WJ_DFCC_DFCEN_APB_REF_CLK_Pos             (4U)
#define WJ_DFCC_DFCEN_APB_REF_CLK_Msk             (0x1U << WJ_DFCC_DFCEN_APB_REF_CLK_Pos)
#define WJ_DFCC_DFC_APB_REF_CLK_EN                WJ_DFCC_DFCEN_APB_REF_CLK_Msk

/* PCR, offset: 0x2C */
#define WJ_PCR_VCODIVSEL_Pos                      (0U)
#define WJ_PCR_VCODIVSEL_Msk                      (0x1U << WJ_PCR_VCODIVSEL_Pos)
#define WJ_PCR_VCODIVSEL                          WJ_PCR_VCODIVSEL_Msk

#define WJ_PCR_FBDIV_Pos                          (1U)
#define WJ_PCR_FBDIV_Msk                          (0x3FFFU << WJ_PCR_FBDIV_Pos)

#define WJ_PCR_POSTDIV_Pos                        (15U)
#define WJ_PCR_POSTDIV_Msk                        (0xFU << WJ_PCR_POSTDIV_Pos)

#define WJ_PCR_FOUTBCOEN_Pos                      (19U)
#define WJ_PCR_FOUTBCOEN_Msk                      (0x1U << WJ_PCR_FOUTBCOEN_Pos)
#define WJ_PDU_FOUTBCOEN_EN                       WJ_PCR_FOUTBCOEN_Msk

#define WJ_PCR_FOUTPOSTDIVEN_Pos                  (20U)
#define WJ_PCR_FOUTPOSTDIVEN_Msk                  (0x1U << WJ_PCR_FOUTPOSTDIVEN_Pos)
#define WJ_PDU_FOUTPOSTDIVEN_EN                   WJ_PCR_FOUTPOSTDIVEN_Msk

#define WJ_PCR_PLL_MODE_Pos                       (21U)
#define WJ_PCR_PLL_MODE_Msk                       (0x1U << WJ_PCR_PLL_MODE_Pos)
#define WJ_PDU_PLL_MODE                           WJ_PCR_PLL_MODE_Msk

#define WJ_PCR_BYPASS_Pos                         (22U)
#define WJ_PCR_BYPASS_Msk                         (0x1U << WJ_PCR_BYPASS_Pos)
#define WJ_PDU_BYPASS                             WJ_PCR_BYPASS_Msk

/* PCR1, offset: 0x30 */
#define WJ_PCR_FCALIN_Pos                         (0U)
#define WJ_PCR_FCALIN_Msk                         (0xFFU << WJ_PCR_FCALIN_Pos)

#define WJ_PCR_FCALCNT_Pos                        (8U)
#define WJ_PCR_FCALCNT_Msk                        (0x3U << WJ_PCR_FCALCNT_Pos)

#define WJ_PCR_FCALIRES_Pos                       (10U)
#define WJ_PCR_FCALIRES_Msk                       (0x7U << WJ_PCR_FCALIRES_Pos)

#define WJ_PCR_FCALBYP_Pos                        (13U)
#define WJ_PCR_FCALBYP_Msk                        (0x1U << WJ_PCR_FCALBYP_Pos)
#define WJ_PDU_FCALBYP                            WJ_PCR_FCALBYP_Msk

#define WJ_PCR_FCALTEST_Pos                       (14U)
#define WJ_PCR_FCALTEST_Msk                       (0x1U << WJ_PCR_FCALTEST_Pos)
#define WJ_PDU_FCALTEST                           WJ_PCR_FCALTEST_Msk

#define WJ_PCR_FCALOUT_Pos                        (15U)
#define WJ_PCR_FCALOUT_Msk                        (0xFFU << WJ_PCR_FCALOUT_Pos)

/* PMU_TRIM, offset: 0x58 */
#define WJ_PMU_TRIM_IHS_RC_TF0_TRIM_Pos           (0U)
#define WJ_PMU_TRIM_IHS_RC_TF0_TRIM_Msk           (0x2U << WJ_PMU_TRIM_IHS_RC_TF0_TRIM_Pos)

#define WJ_PMU_TRIM_IHS_RC_TF1_TRIM_Pos           (2U)
#define WJ_PMU_TRIM_IHS_RC_TF1_TRIM_Msk           (0x1FU << WJ_PMU_TRIM_IHS_RC_TF1_TRIM_Pos)

#define WJ_PMU_TRIM_VSEL_BOR_Pos                  (12U)
#define WJ_PMU_TRIM_VSEL_BOR_Msk                  (0x7U << WJ_PMU_TRIM_VSEL_BOR_Pos)

#define WJ_PMU_TRIM_BOR_EN_Pos                    (17U)
#define WJ_PMU_TRIM_BOR_EN_Msk                    (0x1U << WJ_PMU_TRIM_BOR_EN_Pos)
#define WJ_PMU_TRIM_BOR_EN                        WJ_PMU_TRIM_BOR_EN_Msk

#define WJ_PMU_TRIM_MAIN_PWR_VSEL_Pos             (18U)
#define WJ_PMU_TRIM_MAIN_PWR_VSEL_Msk             (0x1FU << WJ_PMU_TRIM_MAIN_PWR_VSEL_Pos)

#define WJ_PMU_TRIM_RF_PWR_VSEL_Pos               (23U)
#define WJ_PMU_TRIM_RF_PWR_VSEL_Msk               (0x1FU << WJ_PMU_TRIM_RF_PWR_VSEL_Pos)

/* LC-WIME, offset: 0x80 */
#define WJ_LC_WIME_GPIO0_Pos                      (0U)
#define WJ_LC_WIME_GPIO0_Msk                      (0x1U << WJ_LC_WIME_GPIO0_Pos)
#define WJ_LC_WIME_GPIO0_MASK                     WJ_LC_WIME_GPIO0_Msk

#define WJ_LC_WIME_TIME1_0_Pos                    (1U)
#define WJ_LC_WIME_TIME1_0_Msk                    (0x1U << WJ_LC_WIME_TIME1_0_Pos)
#define WJ_LC_WIME_TIME1_0_MASK                   WJ_LC_WIME_TIME1_0_Msk

#define WJ_LC_WIME_TIME1_1_Pos                    (2U)
#define WJ_LC_WIME_TIME1_1_Msk                    (0x1U << WJ_LC_WIME_TIME1_1_Pos)
#define WJ_LC_WIME_TIME1_1_MASK                   WJ_LC_WIME_TIME1_1_Msk

#define WJ_LC_WIME_TIME3_0_Pos                    (3U)
#define WJ_LC_WIME_TIME3_0_Msk                    (0x1U << WJ_LC_WIME_TIME3_0_Pos)
#define WJ_LC_WIME_TIME3_0_MASK                   WJ_LC_WIME_TIME3_0_Msk

#define WJ_LC_WIME_TIME3_1_Pos                    (4U)
#define WJ_LC_WIME_TIME3_1_Msk                    (0x1U << WJ_LC_WIME_TIME3_1_Pos)
#define WJ_LC_WIME_TIME3_1_MASK                   WJ_LC_WIME_TIME3_1_Msk

#define WJ_LC_WIME_WDT1_Pos                       (5U)
#define WJ_LC_WIME_WDT1_Msk                       (0x1U << WJ_LC_WIME_WDT1_Pos)
#define WJ_LC_WIME_WDT1_MASK                      WJ_LC_WIME_WDT1_Msk

#define WJ_LC_WIME_RTC_Pos                        (6U)
#define WJ_LC_WIME_RTC_Msk                        (0x1U << WJ_LC_WIME_RTC_Pos)
#define WJ_LC_WIME_RTC_MASK                       WJ_LC_WIME_RTC_Msk

#define WJ_LC_WIME_UART1_Pos                      (7U)
#define WJ_LC_WIME_UART1_Msk                      (0x1U << WJ_LC_WIME_UART1_Pos)
#define WJ_LC_WIME_UART1_MASK                     WJ_LC_WIME_UART1_Msk

#define WJ_LC_WIME_I2C1_Pos                       (8U)
#define WJ_LC_WIME_I2C1_Msk                       (0x1U << WJ_LC_WIME_I2C1_Pos)
#define WJ_LC_WIME_I2C1_MASK                      WJ_LC_WIME_I2C1_Msk

#define WJ_LC_WIME_I2C3_Pos                       (9U)
#define WJ_LC_WIME_I2C3_Msk                       (0x1U << WJ_LC_WIME_I2C3_Pos)
#define WJ_LC_WIME_I2C3_MASK                      WJ_LC_WIME_I2C3_Msk

#define WJ_LC_WIME_SPI1_Pos                       (10U)
#define WJ_LC_WIME_SPI1_Msk                       (0x1U << WJ_LC_WIME_SPI1_Pos)
#define WJ_LC_WIME_SPI1_MASK                      WJ_LC_WIME_SPI1_Msk

#define WJ_LC_WIME_ADC_Pos                        (11U)
#define WJ_LC_WIME_ADC_Msk                        (0x1U << WJ_LC_WIME_ADC_Pos)
#define WJ_LC_WIME_ADC_MASK                       WJ_LC_WIME_ADC_Msk

#define WJ_LC_WIME_MAILBOX0_Pos                   (12U)
#define WJ_LC_WIME_MAILBOX0_Msk                   (0x1U << WJ_LC_WIME_MAILBOX0_Pos)
#define WJ_LC_WIME_MAILBOX0_MASK                  WJ_LC_WIME_MAILBOX0_Msk

#define WJ_LC_WIME_GNSS_LC_INTR_Pos               (13U)
#define WJ_LC_WIME_GNSS_LC_INTR_Msk               (0x1U << WJ_LC_WIME_GNSS_LC_INTR_Pos)
#define WJ_LC_WIME_GNSS_LC_INTR_MASK              WJ_LC_WIME_GNSS_LC_INTR_Msk

#define WJ_LC_WIME_PWM_INTR_Pos                   (14U)
#define WJ_LC_WIME_PWM_INTR_Msk                   (0x1U << WJ_LC_WIME_PWM_INTR_Pos)
#define WJ_LC_WIME_PWM_INTR_MASK                  WJ_LC_WIME_PWM_INTR_Msk

#define WJ_LC_WIME_EFUSE_INTR_Pos                 (15U)
#define WJ_LC_WIME_EFUSE_INTR_Msk                 (0x1U << WJ_LC_WIME_EFUSE_INTR_Pos)
#define WJ_LC_WIME_EFUSE_INTR_MASK                WJ_LC_WIME_EFUSE_INTR_Msk

#define WJ_LC_WIME_IOCTL_WK_Pos                   (16U)
#define WJ_LC_WIME_IOCTL_WK_Msk                   (0x1U << WJ_LC_WIME_IOCTL_WK_Pos)
#define WJ_LC_WIME_IOCTL_WK_MASK                  WJ_LC_WIME_IOCTL_WK_Msk

#define WJ_LC_WIME_MAILBOX1_Pos                   (17U)
#define WJ_LC_WIME_MAILBOX1_Msk                   (0x1U << WJ_LC_WIME_MAILBOX1_Pos)
#define WJ_LC_WIME_MAILBOX1_MASK                  WJ_LC_WIME_MAILBOX1_Msk

/* BC-WIME, offset: 0x80 */
#define WJ_BC_WIME_GPIO0_Pos                      (0U)
#define WJ_BC_WIME_GPIO0_Msk                      (0x1U << WJ_BC_WIME_GPIO0_Pos)
#define WJ_BC_WIME_GPIO0_MASK                     WJ_BC_WIME_GPIO0_Msk

#define WJ_BC_WIME_TIME0_0_Pos                    (1U)
#define WJ_BC_WIME_TIME0_0_Msk                    (0x1U << WJ_BC_WIME_TIME0_0_Pos)
#define WJ_BC_WIME_TIME0_0_MASK                   WJ_BC_WIME_TIME0_0_Msk

#define WJ_BC_WIME_TIME0_1_Pos                    (2U)
#define WJ_BC_WIME_TIME0_1_Msk                    (0x1U << WJ_BC_WIME_TIME0_1_Pos)
#define WJ_BC_WIME_TIME0_1_MASK                   WJ_BC_WIME_TIME0_1_Msk

#define WJ_BC_WIME_TIME2_0_Pos                    (3U)
#define WJ_BC_WIME_TIME2_0_Msk                    (0x1U << WJ_BC_WIME_TIME2_0_Pos)
#define WJ_BC_WIME_TIME2_0_MASK                   WJ_BC_WIME_TIME2_0_Msk

#define WJ_BC_WIME_TIME2_1_Pos                    (4U)
#define WJ_BC_WIME_TIME2_1_Msk                    (0x1U << WJ_BC_WIME_TIME2_1_Pos)
#define WJ_BC_WIME_TIME2_1_MASK                   WJ_BC_WIME_TIME2_1_Msk

#define WJ_BC_WIME_WDT0_Pos                       (5U)
#define WJ_BC_WIME_WDT0_Msk                       (0x1U << WJ_BC_WIME_WDT0_Pos)
#define WJ_BC_WIME_WDT0_MASK                      WJ_BC_WIME_WDT0_Msk

#define WJ_BC_WIME_UART0_Pos                      (6U)
#define WJ_BC_WIME_UART0_Msk                      (0x1U << WJ_BC_WIME_UART0_Pos)
#define WJ_BC_WIME_UART0_MASK                     WJ_BC_WIME_UART0_Msk

#define WJ_BC_WIME_UART2_Pos                      (7U)
#define WJ_BC_WIME_UART2_Msk                      (0x1U << WJ_BC_WIME_UART2_Pos)
#define WJ_BC_WIME_UART2_MASK                     WJ_BC_WIME_UART2_Msk

#define WJ_BC_WIME_UART4_Pos                      (8U)
#define WJ_BC_WIME_UART4_Msk                      (0x1U << WJ_BC_WIME_UART4_Pos)
#define WJ_BC_WIME_UART4_MASK                     WJ_BC_WIME_UART4_Msk

#define WJ_BC_WIME_I2C0_Pos                       (9U)
#define WJ_BC_WIME_I2C0_Msk                       (0x1U << WJ_BC_WIME_I2C0_Pos)
#define WJ_BC_WIME_I2C0_MASK                      WJ_BC_WIME_I2C0_Msk

#define WJ_BC_WIME_I2C1_Pos                       (10U)
#define WJ_BC_WIME_I2C1_Msk                       (0x1U << WJ_BC_WIME_I2C1_Pos)
#define WJ_BC_WIME_I2C1_MASK                      WJ_BC_WIME_I2C1_Msk

#define WJ_BC_WIME_SPI2_Pos                       (11U)
#define WJ_BC_WIME_SPI2_Msk                       (0x1U << WJ_BC_WIME_SPI2_Pos)
#define WJ_BC_WIME_SPI2_MASK                      WJ_BC_WIME_SPI2_Msk

#define WJ_BC_WIME_QSPIFLS_INTR_Pos               (12U)
#define WJ_BC_WIME_QSPIFLS_INTR_Msk               (0x1U << WJ_BC_WIME_QSPIFLS_INTR_Pos)
#define WJ_BC_WIME_QSPIFLS_INTR_MASK              WJ_BC_WIME_QSPIFLS_INTR_Msk

#define WJ_BC_WIME_DMAC0_Pos                      (13U)
#define WJ_BC_WIME_DMAC0_Msk                      (0x1U << WJ_BC_WIME_DMAC0_Pos)
#define WJ_BC_WIME_DMAC0_MASK                     WJ_BC_WIME_DMAC0_Msk

#define WJ_BC_WIME_MAILBOX0_Pos                   (14U)
#define WJ_BC_WIME_MAILBOX0_Msk                   (0x1U << WJ_BC_WIME_MAILBOX0_Pos)
#define WJ_BC_WIME_MAILBOX0_MASK                  WJ_BC_WIME_MAILBOX0_Msk

#define WJ_BC_WIME_GNSS_BC_INTR_Pos               (15U)
#define WJ_BC_WIME_GNSS_BC_INTR_Msk               (0x1U << WJ_BC_WIME_GNSS_BC_INTR_Pos)
#define WJ_BC_WIME_GNSS_BC_INTR_MASK              WJ_BC_WIME_GNSS_BC_INTR_Msk

#define WJ_BC_WIME_SPI0_Pos                       (16U)
#define WJ_BC_WIME_SPI0_Msk                       (0x1U << WJ_BC_WIME_SPI0_Pos)
#define WJ_BC_WIME_SPI0_MASK                      WJ_BC_WIME_SPI0_Msk

#define WJ_BC_WIME_MAILBOX1_Pos                   (17U)
#define WJ_BC_WIME_MAILBOX1_Msk                   (0x1U << WJ_BC_WIME_MAILBOX1_Pos)
#define WJ_BC_WIME_MAILBOX1_MASK                  WJ_BC_WIME_MAILBOX1_Msk

#define WJ_BC_WIME_SPI4_Pos                       (18U)
#define WJ_BC_WIME_SPI4_Msk                       (0x1U << WJ_BC_WIME_SPI4_Pos)
#define WJ_BC_WIME_SPI4_MASK                      WJ_BC_WIME_SPI4_Msk

#define WJ_BC_WIME_RTC_Pos                        (19U)
#define WJ_BC_WIME_RTC_Msk                        (0x1U << WJ_BC_WIME_RTC_Pos)
#define WJ_BC_WIME_RTC_MASK                       WJ_BC_WIME_RTC_Msk

#define WJ_BC_WIME_IOCTL_WK_Pos                   (20U)
#define WJ_BC_WIME_IOCTL_WK_Msk                   (0x1U << WJ_BC_WIME_IOCTL_WK_Pos)
#define WJ_BC_WIME_IOCTL_WK_MASK                  WJ_BC_WIME_IOCTL_WK_Msk

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
#define WJ_SRC_CLKMD_CLK_SEL0_Pos                 (2U)
#define WJ_SRC_CLKMD_CLK_SEL0_Msk                 (0x1U << WJ_SRC_CLKMD_CLK_SEL0_Pos)
#define WJ_SRC_CLKMD_CLK_SEL0                     WJ_SRC_CLKMD_CLK_SEL0_Msk

#define WJ_SRC_CLKMD_CLK_SEL1_Pos                 (3U)
#define WJ_SRC_CLKMD_CLK_SEL1_Msk                 (0x1U << WJ_SRC_CLKMD_CLK_SEL1_Pos)
#define WJ_SRC_CLKMD_CLK_SEL1                     WJ_SRC_CLKMD_CLK_SEL1_Msk

#define WJ_SRC_CLKMD_ELS_OE_Pos                   (10U)
#define WJ_SRC_CLKMD_ELS_OE_Msk                   (0x1U << WJ_SRC_CLKMD_ELS_OE_Pos)
#define WJ_SRC_CLKMD_ELS_OE                       WJ_SRC_CLKMD_ELS_OE_Msk

#define WJ_SRC_CLKMD_ELS_IE_Pos                   (11U)
#define WJ_SRC_CLKMD_ELS_IE_Msk                   (0x1U << WJ_SRC_CLKMD_ELS_IE_Pos)
#define WJ_SRC_CLKMD_ELS_IE                       WJ_SRC_CLKMD_ELS_IE_Msk

#define WJ_SRC_CLKMD_PLL_EN_Pos                   (14U)
#define WJ_SRC_CLKMD_PLL_EN_Msk                   (0x1U << WJ_SRC_CLKMD_PLL_EN_Pos)
#define WJ_SRC_CLKMD_PLL_EN                       WJ_SRC_CLKMD_PLL_EN_Msk

/* SRC_LDOMD, offset: 0x108 */
#define WJ_SRC_LDOMD_AON_PWR_SEL_Pos              (0U)
#define WJ_SRC_LDOMD_AON_PWR_SEL_Msk              (0x1U << WJ_SRC_LDOMD_AON_PWR_SEL_Pos)
#define WJ_SRC_LDOMD_AON_PWR_SEL_QLR               WJ_SRC_LDOMD_AON_PWR_SEL_Msk

#define WJ_SRC_LDOMD_AON_PWR_VSEL_Pos             (1U)
#define WJ_SRC_LDOMD_AON_PWR_VSEL_Msk             (0x1FU << WJ_SRC_LDOMD_AON_PWR_VSEL_Pos)

#define WJ_SRC_LDOMD_LP_RAM_LDO_Pos               (6U)
#define WJ_SRC_LDOMD_LP_RAM_LDO_Msk               (0x3U << WJ_SRC_LDOMD_LP_RAM_LDO_Pos)
#define WJ_SRC_LDOMD_LP_RAM_LDO_SHUTDOWN_MODE     (0x0U << WJ_SRC_LDOMD_LP_RAM_LDO_Pos)
#define WJ_SRC_LDOMD_LP_RAM_LDO_NORMAL_MODE       (0x1U << WJ_SRC_LDOMD_LP_RAM_LDO_Pos)
#define WJ_SRC_LDOMD_LP_RAM_LDO_HIZ_MODE          (0x3U << WJ_SRC_LDOMD_LP_RAM_LDO_Pos)

#define WJ_SRC_LDOMD_LP_RAM_LDO_VSEL_Pos          (8U)
#define WJ_SRC_LDOMD_LP_RAM_LDO_VSEL_Msk          (0x1FU << WJ_SRC_LDOMD_LP_RAM_LDO_VSEL_Pos)

#define WJ_SRC_LDOMD_MAIN_PWR_Pos                 (13U)
#define WJ_SRC_LDOMD_MAIN_PWR_Msk                 (0x3U << WJ_SRC_LDOMD_MAIN_PWR_Pos)
#define WJ_SRC_LDOMD_MAIN_PWR_SHUTDOWN_MODE       (0x0U << WJ_SRC_LDOMD_MAIN_PWR_Pos)
#define WJ_SRC_LDOMD_MAIN_PWR_NORMAL_MODE         (0x1U << WJ_SRC_LDOMD_MAIN_PWR_Pos)
#define WJ_SRC_LDOMD_MAIN_PWR_HIZ_MODE            (0x3U << WJ_SRC_LDOMD_MAIN_PWR_Pos)

#define WJ_SRC_LDOMD_MAIN_PWR_SEL_Pos             (15U)
#define WJ_SRC_LDOMD_MAIN_PWR_SEL_Msk             (0x1U << WJ_SRC_LDOMD_MAIN_PWR_SEL_Pos)
#define WJ_SRC_LDOMD_MAIN_PWR_SEL_ILR             WJ_SRC_LDOMD_MAIN_PWR_SEL_Msk

#define WJ_SRC_LDOMD_RF_PWR_DCDC_ULP_Pos           (17U)
#define WJ_SRC_LDOMD_RF_PWR_DCDC_ULP_Msk           (0x3U << WJ_SRC_LDOMD_RF_PWR_DCDC_ULP_Pos)
#define WJ_SRC_LDOMD_RF_PWR_DCDC_ULP_SHUTDOWN_MODE (0x0U << WJ_SRC_LDOMD_RF_PWR_DCDC_ULP_Pos)
#define WJ_SRC_LDOMD_RF_PWR_DCDC_ULP_NORMAL_MODE   (0x1U << WJ_SRC_LDOMD_RF_PWR_DCDC_ULP_Pos)
#define WJ_SRC_LDOMD_RF_PWR_DCDC_ULP_HIZ_MODE      (0x3U << WJ_SRC_LDOMD_RF_PWR_DCDC_ULP_Pos)

#define WJ_SRC_LDOMD_RF_PWR_Pos                   (19U)
#define WJ_SRC_LDOMD_RF_PWR_Msk                   (0x3U << WJ_SRC_LDOMD_RF_PWR_Pos)
#define WJ_SRC_LDOMD_RF_PWR_SHUTDOWN_MODE         (0x0U << WJ_SRC_LDOMD_RF_PWR_Pos)
#define WJ_SRC_LDOMD_RF_PWR_NORMAL_MODE           (0x1U << WJ_SRC_LDOMD_RF_PWR_Pos)
#define WJ_SRC_LDOMD_RF_PWR_HIZ_MODE              (0x3U << WJ_SRC_LDOMD_RF_PWR_Pos)

/* GNSS_DCU_MODE, offset: 0x130 */
#define WJ_GNSS_DCU_MODE_LP_MODE_Pos              (2U)
#define WJ_GNSS_DCU_MODE_LP_MODE_Msk              (0x3U << WJ_GNSS_DCU_MODE_LP_MODE_Pos)
#define WJ_GNSS_DCU_MODE_CKOFF_PON                (0x0U << WJ_GNSS_DCU_MODE_LP_MODE_Pos)
#define WJ_GNSS_DCU_MODE_CKOFF_POFF               (0x1U << WJ_GNSS_DCU_MODE_LP_MODE_Pos)
#define WJ_GNSS_DCU_MODE_PON                      (0x2U << WJ_GNSS_DCU_MODE_LP_MODE_Pos)

/* GNSS_DCU_CGCR, offset: 0x134 */
#define WJ_GNSS_DCU_CGCR_LC_GNSS_CLK_GATE_Pos     (0U)
#define WJ_GNSS_DCU_CGCR_LC_GNSS_CLK_GATE_Msk     (0x1U << WJ_GNSS_DCU_CGCR_LC_GNSS_CLK_GATE_Pos)
#define WJ_GNSS_DCU_CGCR_LC_GNSS_CLK_GATE_ON      WJ_GNSS_DCU_CGCR_LC_GNSS_CLK_GATE_Msk

#define WJ_GNSS_DCU_CGCR_BC_GNSS_CLK_GATE_Pos     (1U)
#define WJ_GNSS_DCU_CGCR_BC_GNSS_CLK_GATE_Msk     (0x1U << WJ_GNSS_DCU_CGCR_BC_GNSS_CLK_GATE_Pos)
#define WJ_GNSS_DCU_CGCR_BC_GNSS_CLK_GATE_ON      WJ_GNSS_DCU_CGCR_BC_GNSS_CLK_GATE_Msk

/* GNSS_DCU_SWRST, offset: 0x13c */
#define WJ_GNSS_DCU_SWRST_LC_GNSS_SWRST_Pos       (0U)
#define WJ_GNSS_DCU_SWRST_LC_GNSS_SWRST_Msk       (0x1U << WJ_GNSS_DCU_SWRST_LC_GNSS_SWRST_Pos)
#define WJ_GNSS_DCU_SWRST_LC_GNSS_SWRST_ON        WJ_GNSS_DCU_SWRST_LC_GNSS_SWRST_Msk

#define WJ_GNSS_DCU_SWRST_BC_GNSS_SWRST_Pos       (1U)
#define WJ_GNSS_DCU_SWRST_BC_GNSS_SWRST_Msk       (0x1U << WJ_GNSS_DCU_SWRST_BC_GNSS_SWRST_Pos)
#define WJ_GNSS_DCU_SWRST_BC_GNSS_SWRST_ON        WJ_GNSS_DCU_SWRST_BC_GNSS_SWRST_Msk

/* LC_CPU_CKGT, offset: 0x140 */
#define WJ_LC_CPU_CKGT_Pos                        (3U)
#define WJ_LC_CPU_CKGT_Msk                        (0x1U << WJ_LC_CPU_CKGT_Pos)
#define WJ_LC_CPU_CKGT_CKON                       WJ_LC_CPU_CKGT_Msk

/* BC_CPU_CKGT, offset: 0x150 */
#define WJ_BC_CPU_CKGT_Pos                        (3U)
#define WJ_BC_CPU_CKGT_Msk                        (0x1U << WJ_BC_CPU_CKGT_Pos)
#define WJ_BC_CPU_CKGT_CKON                       WJ_BC_CPU_CKGT_Msk

/* RETU_DCU_MODE, offset: 0x160 */
#define WJ_RETU_DCU_MODE_LP_MODE_Pos              (2U)
#define WJ_RETU_DCU_MODE_LP_MODE_Msk              (0x3U << WJ_RETU_DCU_MODE_LP_MODE_Pos)
#define WJ_RETU_DCU_MODE_CKOFF_PON                (0x0U << WJ_RETU_DCU_MODE_LP_MODE_Pos)
#define WJ_RETU_DCU_MODE_CKOFF_POFF               (0x1U << WJ_RETU_DCU_MODE_LP_MODE_Pos)
#define WJ_RETU_DCU_MODE_PON                      (0x2U << WJ_RETU_DCU_MODE_LP_MODE_Pos)

/* PDU_DCU_MODE, offset: 0x190 */
#define WJ_PDU_DCU_MODE_LP_MODE_Pos               (2U)
#define WJ_PDU_DCU_MODE_LP_MODE_Msk               (0x3U << WJ_PDU_DCU_MODE_LP_MODE_Pos)
#define WJ_PDU_DCU_MODE_CKOFF_PON                 (0x0U << WJ_PDU_DCU_MODE_LP_MODE_Pos)
#define WJ_PDU_DCU_MODE_CKOFF_POFF                (0x1U << WJ_PDU_DCU_MODE_LP_MODE_Pos)
#define WJ_PDU_DCU_MODE_PON                       (0x2U << WJ_PDU_DCU_MODE_LP_MODE_Pos)

/* PDU_DCU_LC_MCGCR, offset: 0x194 */
#define WJ_PDU_DCU_LC_MCGCR_LC_SRAM_CLK_GATE_Pos  (0U)
#define WJ_PDU_DCU_LC_MCGCR_LC_SRAM_CLK_GATE_Msk  (0x1U << WJ_PDU_DCU_LC_MCGCR_LC_SRAM_CLK_GATE_Pos)
#define WJ_PDU_DCU_LC_MCGCR_LC_SRAM_CLK_GATE_ON   WJ_PDU_DCU_LC_MCGCR_LC_SRAM_CLK_GATE_Msk

#define WJ_PDU_DCU_LC_MCGCR_LC_ROM_CLK_GATE_Pos   (1U)
#define WJ_PDU_DCU_LC_MCGCR_LC_ROM_CLK_GATE_Msk   (0x1U << WJ_PDU_DCU_LC_MCGCR_LC_ROM_CLK_GATE_Pos)
#define WJ_PDU_DCU_LC_MCGCR_LC_ROM_CLK_GATE_ON    WJ_PDU_DCU_LC_MCGCR_LC_ROM_CLK_GATE_Msk

#define WJ_PDU_DCU_LC_MCGCR_APB1_BRIDGE_HCLK_GATE_Pos  (2U)
#define WJ_PDU_DCU_LC_MCGCR_APB1_BRIDGE_HCLK_GATE_Msk  (0x1U << WJ_PDU_DCU_LC_MCGCR_APB1_BRIDGE_HCLK_GATE_Pos)
#define WJ_PDU_DCU_LC_MCGCR_APB1_BRIDGE_HCLK_GATE_ON   WJ_PDU_DCU_LC_MCGCR_APB1_BRIDGE_HCLK_GATE_Msk

#define WJ_PDU_DCU_LC_MCGCR_LC_SYNCBUS_CLK_GATE_Pos    (3U)
#define WJ_PDU_DCU_LC_MCGCR_LC_SYNCBUS_CLK_GATE_Msk    (0x1U << WJ_PDU_DCU_LC_MCGCR_LC_SYNCBUS_CLK_GATE_Pos)
#define WJ_PDU_DCU_LC_MCGCR_LC_SYNCBUS_CLK_GATE_ON     WJ_PDU_DCU_LC_MCGCR_LC_SYNCBUS_CLK_GATE_Msk

#define WJ_PDU_DCU_LC_MCGCR_LC_MBUS_CLK_GATE_Pos       (4U)
#define WJ_PDU_DCU_LC_MCGCR_LC_MBUS_CLK_GATE_Msk       (0x1U << WJ_PDU_DCU_LC_MCGCR_LC_MBUS_CLK_GATE_Pos)
#define WJ_PDU_DCU_LC_MCGCR_LC_MBUS_CLK_GATE_ON        WJ_PDU_DCU_LC_MCGCR_LC_MBUS_CLK_GATE_Msk

#define WJ_PDU_DCU_LC_MCGCR_LC_PMU_SYNC_BRIDGE_CLK_GATE_Pos  (5U)
#define WJ_PDU_DCU_LC_MCGCR_LC_PMU_SYNC_BRIDGE_CLK_GATE_Msk  (0x1U << WJ_PDU_DCU_LC_MCGCR_LC_MBUS_CLK_GATE_Pos)
#define WJ_PDU_DCU_LC_MCGCR_LC_PMU_SYNC_BRIDGE_CLK_GATE_ON   WJ_PDU_DCU_LC_MCGCR_LC_MBUS_CLK_GATE_Msk

/* PDU_DCU_LC_MSWRST, offset: 0x198 */
#define WJ_PDU_DCU_LC_MCGCR_LC_SRAM_SWRST_Pos  (0U)
#define WJ_PDU_DCU_LC_MCGCR_LC_SRAM_SWRST_Msk  (0x1U << WJ_PDU_DCU_LC_MCGCR_LC_SRAM_SWRST_Pos)
#define WJ_PDU_DCU_LC_MCGCR_LC_SRAM_SWRST_ON   WJ_PDU_DCU_LC_MCGCR_LC_SRAM_SWRST_Msk

#define WJ_PDU_DCU_LC_MCGCR_LC_ROM_SWRST_Pos   (1U)
#define WJ_PDU_DCU_LC_MCGCR_LC_ROM_SWRST_Msk   (0x1U << WJ_PDU_DCU_LC_MCGCR_LC_ROM_SWRST_Pos)
#define WJ_PDU_DCU_LC_MCGCR_LC_ROM_SWRST_ON    WJ_PDU_DCU_LC_MCGCR_LC_ROM_SWRST_Msk

#define WJ_PDU_DCU_LC_MCGCR_APB1_BRIDGE_HSWRST_Pos  (2U)
#define WJ_PDU_DCU_LC_MCGCR_APB1_BRIDGE_HSWRST_Msk  (0x1U << WJ_PDU_DCU_LC_MCGCR_APB1_BRIDGE_HSWRST_Pos)
#define WJ_PDU_DCU_LC_MCGCR_APB1_BRIDGE_HSWRST_ON   WJ_PDU_DCU_LC_MCGCR_APB1_BRIDGE_HSWRST_Msk

#define WJ_PDU_DCU_LC_MCGCR_LC_SYNCBUS_SWRST_Pos    (3U)
#define WJ_PDU_DCU_LC_MCGCR_LC_SYNCBUS_SWRST_Msk    (0x1U << WJ_PDU_DCU_LC_MCGCR_LC_SYNCBUS_SWRST_Pos)
#define WJ_PDU_DCU_LC_MCGCR_LC_SYNCBUS_SWRST_ON     WJ_PDU_DCU_LC_MCGCR_LC_SYNCBUS_SWRST_Msk

#define WJ_PDU_DCU_LC_MCGCR_LC_MBUS_SWRST_Pos       (4U)
#define WJ_PDU_DCU_LC_MCGCR_LC_MBUS_SWRST_Msk       (0x1U << WJ_PDU_DCU_LC_MCGCR_LC_MBUS_SWRST_Pos)
#define WJ_PDU_DCU_LC_MCGCR_LC_MBUS_SWRST_ON        WJ_PDU_DCU_LC_MCGCR_LC_MBUS_SWRST_Msk

#define WJ_PDU_DCU_LC_MCGCR_LC_PMU_SYNC_BRIDGE_SWRST_Pos  (5U)
#define WJ_PDU_DCU_LC_MCGCR_LC_PMU_SYNC_BRIDGE_SWRST_Msk  (0x1U << WJ_PDU_DCU_LC_MCGCR_LC_PMU_SYNC_BRIDGE_SWRST_Pos)
#define WJ_PDU_DCU_LC_MCGCR_LC_PMU_SYNC_BRIDGE_SWRST_ON   WJ_PDU_DCU_LC_MCGCR_LC_PMU_SYNC_BRIDGE_SWRST_Msk

/* PDU_DCU_BC_MCGCR, offset: 0x19C */
#define WJ_PDU_DCU_BC_MCGCR_BC_SRAM_CLK_Pos       (0U)
#define WJ_PDU_DCU_BC_MCGCR_BC_SRAM_CLK_Msk       (0x1U << WJ_PDU_DCU_BC_MCGCR_BC_SRAM_CLK_Pos)
#define WJ_PDU_DCU_BC_MCGCR_BC_SRAM_CLK_ON        WJ_PDU_DCU_BC_MCGCR_BC_SRAM_CLK_Msk

#define WJ_PDU_DCU_BC_MCGCR_DMA0_CLK_Pos          (1U)
#define WJ_PDU_DCU_BC_MCGCR_DMA0_CLK_Msk          (0x1U << WJ_PDU_DCU_BC_MCGCR_DMA0_CLK_Pos)
#define WJ_PDU_DCU_BC_MCGCR_DMA0_CLK_ON           WJ_PDU_DCU_BC_MCGCR_DMA0_CLK_Msk

#define WJ_PDU_DCU_BC_MCGCR_MBOX0_CLK_Pos         (2U)
#define WJ_PDU_DCU_BC_MCGCR_MBOX0_CLK_Msk         (0x1U << WJ_PDU_DCU_BC_MCGCR_MBOX0_CLK_Pos)
#define WJ_PDU_DCU_BC_MCGCR_MBOX0_CLK_ON          WJ_PDU_DCU_BC_MCGCR_MBOX0_CLK_Msk

#define WJ_PDU_DCU_BC_MCGCR_BC_ROM_CLK_Pos        (3U)
#define WJ_PDU_DCU_BC_MCGCR_BC_ROM_CLK_Msk        (0x1U << WJ_PDU_DCU_BC_MCGCR_BC_ROM_CLK_Pos)
#define WJ_PDU_DCU_BC_MCGCR_BC_ROM_CLK_ON         WJ_PDU_DCU_BC_MCGCR_BC_ROM_CLK_Msk

#define WJ_PDU_DCU_BC_MCGCR_MBOX1_CLK_Pos         (4U)
#define WJ_PDU_DCU_BC_MCGCR_MBOX1_CLK_Msk         (0x1U << WJ_PDU_DCU_BC_MCGCR_MBOX1_CLK_Pos)
#define WJ_PDU_DCU_BC_MCGCR_MBOX1_CLK_ON          WJ_PDU_DCU_BC_MCGCR_MBOX1_CLK_Msk

#define WJ_PDU_DCU_BC_MCGCR_MDUMMY0_CLK_Pos       (5U)
#define WJ_PDU_DCU_BC_MCGCR_MDUMMY0_CLK_Msk       (0x1U << WJ_PDU_DCU_BC_MCGCR_MDUMMY0_CLK_Pos)
#define WJ_PDU_DCU_BC_MCGCR_MDUMMY0_CLK_ON        WJ_PDU_DCU_BC_MCGCR_MDUMMY0_CLK_Msk

#define WJ_PDU_DCU_BC_MCGCR_QSPIFLS_AHB_CLK_Pos   (6U)
#define WJ_PDU_DCU_BC_MCGCR_QSPIFLS_AHB_CLK_Msk   (0x1U << WJ_PDU_DCU_BC_MCGCR_QSPIFLS_AHB_CLK_Pos)
#define WJ_PDU_DCU_BC_MCGCR_QSPIFLS_AHB_CLK_ON    WJ_PDU_DCU_BC_MCGCR_QSPIFLS_AHB_CLK_Msk

#define WJ_PDU_DCU_BC_MCGCR_ETB_WORK_CLK_Pos      (7U)
#define WJ_PDU_DCU_BC_MCGCR_ETB_WORK_CLK_Msk      (0x1U << WJ_PDU_DCU_BC_MCGCR_ETB_WORK_CLK_Pos)
#define WJ_PDU_DCU_BC_MCGCR_ETB_WORK_CLK_ON       WJ_PDU_DCU_BC_MCGCR_ETB_WORK_CLK_Msk

#define WJ_PDU_DCU_BC_MCGCR_APB0_BRIDGE_HCLK_Pos   (8U)
#define WJ_PDU_DCU_BC_MCGCR_APB0_BRIDGE_HCLK_Msk   (0x1U << WJ_PDU_DCU_BC_MCGCR_APB0_BRIDGE_HCLK_Pos)
#define WJ_PDU_DCU_BC_MCGCR_APB0_BRIDGE_HCLK_ON    WJ_PDU_DCU_BC_MCGCR_APB0_BRIDGE_HCLK_Msk

#define WJ_PDU_DCU_BC_MCGCR_BC_SYNCBUS_CLK_Pos    (9U)
#define WJ_PDU_DCU_BC_MCGCR_BC_SYNCBUS_CLK_Msk    (0x1U << WJ_PDU_DCU_BC_MCGCR_BC_SYNCBUS_CLK_Pos)
#define WJ_PDU_DCU_BC_MCGCR_BC_SYNCBUS_CLK_ON     WJ_PDU_DCU_BC_MCGCR_BC_SYNCBUS_CLK_Msk

#define WJ_PDU_DCU_BC_MCGCR_BC_MBUS_CLK_Pos       (10U)
#define WJ_PDU_DCU_BC_MCGCR_BC_MBUS_CLK_Msk       (0x1U << WJ_PDU_DCU_BC_MCGCR_BC_MBUS_CLK_Pos)
#define WJ_PDU_DCU_BC_MCGCR_BC_MBUS_CLK_ON        WJ_PDU_DCU_BC_MCGCR_BC_MBUS_CLK_Msk

/* PDU_DCU_BC_MCGCR, offset: 0x1A4 */
#define WJ_PDU_DCU_BC_MCGCR_BC_SRAM_SWRST_Pos     (0U)
#define WJ_PDU_DCU_BC_MCGCR_BC_SRAM_SWRST_Msk     (0x1U << WJ_PDU_DCU_BC_MCGCR_BC_SRAM_SWRST_Pos)
#define WJ_PDU_DCU_BC_MCGCR_BC_SRAM_SWRST_ON      WJ_PDU_DCU_BC_MCGCR_BC_SRAM_SWRST_Msk

#define WJ_PDU_DCU_BC_MCGCR_DMA0_SWRST_Pos        (1U)
#define WJ_PDU_DCU_BC_MCGCR_DMA0_SWRST_Msk        (0x1U << WJ_PDU_DCU_BC_MCGCR_DMA0_SWRST_Pos)
#define WJ_PDU_DCU_BC_MCGCR_DMA0_SWRST_ON         WJ_PDU_DCU_BC_MCGCR_DMA0_SWRST_Msk

#define WJ_PDU_DCU_BC_MCGCR_MBOX0_SWRST_Pos       (2U)
#define WJ_PDU_DCU_BC_MCGCR_MBOX0_SWRST_Msk       (0x1U << WJ_PDU_DCU_BC_MCGCR_MBOX0_SWRST_Pos)
#define WJ_PDU_DCU_BC_MCGCR_MBOX0_SWRST_ON        WJ_PDU_DCU_BC_MCGCR_MBOX0_SWRST_Msk

#define WJ_PDU_DCU_BC_MCGCR_BC_ROM_SWRST_Pos      (3U)
#define WJ_PDU_DCU_BC_MCGCR_BC_ROM_SWRST_Msk      (0x1U << WJ_PDU_DCU_BC_MCGCR_BC_ROM_SWRST_Pos)
#define WJ_PDU_DCU_BC_MCGCR_BC_ROM_SWRST_ON       WJ_PDU_DCU_BC_MCGCR_BC_ROM_SWRST_Msk

#define WJ_PDU_DCU_BC_MCGCR_MBOX1_SWRST_Pos       (4U)
#define WJ_PDU_DCU_BC_MCGCR_MBOX1_SWRST_Msk       (0x1U << WJ_PDU_DCU_BC_MCGCR_MBOX1_SWRST_Pos)
#define WJ_PDU_DCU_BC_MCGCR_MBOX1_SWRST_ON        WJ_PDU_DCU_BC_MCGCR_MBOX1_SWRST_Msk

#define WJ_PDU_DCU_BC_MCGCR_MDUMMY0_SWRST_Pos     (5U)
#define WJ_PDU_DCU_BC_MCGCR_MDUMMY0_SWRST_Msk     (0x1U << WJ_PDU_DCU_BC_MCGCR_MDUMMY0_SWRST_Pos)
#define WJ_PDU_DCU_BC_MCGCR_MDUMMY0_SWRST_ON      WJ_PDU_DCU_BC_MCGCR_MDUMMY0_SWRST_Msk

#define WJ_PDU_DCU_BC_MCGCR_QSPIFLS_AHB_SWRST_Pos (6U)
#define WJ_PDU_DCU_BC_MCGCR_QSPIFLS_AHB_SWRST_Msk (0x1U << WJ_PDU_DCU_BC_MCGCR_QSPIFLS_AHB_SWRST_Pos)
#define WJ_PDU_DCU_BC_MCGCR_QSPIFLS_AHB_SWRST_ON  WJ_PDU_DCU_BC_MCGCR_QSPIFLS_AHB_SWRST_Msk

#define WJ_PDU_DCU_BC_MCGCR_ETB_WORK_SWRST_Pos    (7U)
#define WJ_PDU_DCU_BC_MCGCR_ETB_WORK_SWRST_Msk    (0x1U << WJ_PDU_DCU_BC_MCGCR_ETB_WORK_SWRST_Pos)
#define WJ_PDU_DCU_BC_MCGCR_ETB_WORK_SWRST_ON     WJ_PDU_DCU_BC_MCGCR_ETB_WORK_SWRST_Msk

#define WJ_PDU_DCU_BC_MCGCR_APB_BRIDGE_HSWRST_Pos (8U)
#define WJ_PDU_DCU_BC_MCGCR_APB_BRIDGE_HSWRST_Msk (0x1U << WJ_PDU_DCU_BC_MCGCR_APB_BRIDGE_HSWRST_Pos)
#define WJ_PDU_DCU_BC_MCGCR_APB_BRIDGE_HSWRST_ON  WJ_PDU_DCU_BC_MCGCR_APB_BRIDGE_HSWRST_Msk

#define WJ_PDU_DCU_BC_MCGCR_BC_SYNCBUS_SWRST_Pos  (9U)
#define WJ_PDU_DCU_BC_MCGCR_BC_SYNCBUS_SWRST_Msk  (0x1U << WJ_PDU_DCU_BC_MCGCR_BC_SYNCBUS_SWRST_Pos)
#define WJ_PDU_DCU_BC_MCGCR_BC_SYNCBUS_SWRST_ON   WJ_PDU_DCU_BC_MCGCR_BC_SYNCBUS_SWRST_Msk

#define WJ_PDU_DCU_BC_MCGCR_BC_MBUS_SWRST_Pos     (10U)
#define WJ_PDU_DCU_BC_MCGCR_BC_MBUS_SWRST_Msk     (0x1U << WJ_PDU_DCU_BC_MCGCR_BC_MBUS_SWRST_Pos)
#define WJ_PDU_DCU_BC_MCGCR_BC_MBUS_SWRST_ON      WJ_PDU_DCU_BC_MCGCR_BC_MBUS_SWRST_Msk

/* ANA_DCU_MODE, offset: 0x1C0 */
#define WJ_ANA_DCU_MODE_LPMODE_Pos                (2U)
#define WJ_ANA_DCU_MODE_LPMODE_Msk                (0x3U << WJ_ANA_DCU_MODE_LPMODE_Pos)
#define WJ_ANA_DCU_MODE_CKOFF_PON                 (0x0U << WJ_ANA_DCU_MODE_LPMODE_Pos)
#define WJ_ANA_DCU_MODE_CKOFF_POFF                (0x1U << WJ_ANA_DCU_MODE_LPMODE_Pos)
#define WJ_ANA_DCU_MODE_PON                       (0x2U << WJ_ANA_DCU_MODE_LPMODE_Pos)

/* AON_DCU_MODE, offset: 0x250 */
#define WJ_AON_DCU_MODE_LPMODE_Pos                (3U)
#define WJ_AON_DCU_MODE_LPMODE_Msk                (0x1U << WJ_AON_DCU_MODE_LPMODE_Pos)
#define WJ_AON_DCU_MODE_CKOFF                     WJ_AON_DCU_MODE_LPMODE_Msk

/* AOAPB_DCU_CGCR, offset: 0x25C */
#define WJ_AOAPB_DCU_CGCR_IOCTL_CLK_Pos           (0U)
#define WJ_AOAPB_DCU_CGCR_IOCTL_CLK_Msk           (0x1U << WJ_AOAPB_DCU_CGCR_IOCTL_CLK_Pos)
#define WJ_AOAPB_DCU_CGCR_IOCTL_CLK_ON            WJ_AOAPB_DCU_CGCR_IOCTL_CLK_Msk

#define WJ_AOAPB_DCU_CGCR_RTC_CLK_Pos             (1U)
#define WJ_AOAPB_DCU_CGCR_RTC_CLK_Msk             (0x1U << WJ_AOAPB_DCU_CGCR_RTC_CLK_Pos)
#define WJ_AOAPB_DCU_CGCR_RTC_CLK_ON              WJ_AOAPB_DCU_CGCR_RTC_CLK_Msk

/* AOAPB_DCU_SWRST, offset: 0x264 */
#define WJ_AOAPB_DCU_SWRST_IOCTL_SWRST_Pos        (0U)
#define WJ_AOAPB_DCU_SWRST_IOCTL_SWRST_Msk        (0x1U << WJ_AOAPB_DCU_SWRST_IOCTL_SWRST_Pos)
#define WJ_AOAPB_DCU_SWRST_IOCTL_SWRST_ON         WJ_AOAPB_DCU_SWRST_IOCTL_SWRST_Msk

#define WJ_AOAPB_DCU_SWRST_RTC_SWRST_Pos          (1U)
#define WJ_AOAPB_DCU_SWRST_RTC_SWRST_Msk          (0x1U << WJ_AOAPB_DCU_SWRST_RTC_SWRST_Pos)
#define WJ_AOAPB_DCU_SWRST_RTC_SWRST_ON           WJ_AOAPB_DCU_SWRST_RTC_SWRST_Msk

/* DCU_RST_CNT, offset: 0x500 */
#define WJ_DCU_RST_CNT_RST_EN_DUR_Pos             (0U)
#define WJ_DCU_RST_CNT_RST_EN_DUR_Msk             (0xFFFU << WJ_DCU_RST_CNT_RST_EN_DUR_Pos)

#define WJ_DCU_RST_CNT_RST_DIS_DUR_Pos            (16U)
#define WJ_DCU_RST_CNT_RST_DIS_DUR_Msk            (0xFFFU << WJ_DCU_RST_CNT_RST_EN_DUR_Pos)

/* DCU_RST_CNT, offset: 0x504 */
#define WJ_DCU_ISO_CNT_ISO_EN_DUR_Pos             (0U)
#define WJ_DCU_ISO_CNT_ISO_EN_DUR_Msk             (0xFFFU << WJ_DCU_ISO_CNT_ISO_EN_DUR_Pos)

#define WJ_DCU_ISO_CNT_ISO_DIS_DUR_Pos            (16U)
#define WJ_DCU_ISO_CNT_ISO_DIS_DUR_Msk            (0xFFFU << WJ_DCU_ISO_CNT_ISO_DIS_DUR_Pos)

/* CLK_RATIO, offset: 0x50c */
#define WJ_CLK_RATIO_BC_MCLK_DIV_Pos              (0U)
#define WJ_CLK_RATIO_BC_MCLK_DIV_Msk              (0xFU << WJ_CLK_RATIO_BC_MCLK_DIV_Pos)

#define WJ_CLK_RATIO_LC_MCLK_DIV_Pos              (4U)
#define WJ_CLK_RATIO_LC_MCLK_DIV_Msk              (0xFU << WJ_CLK_RATIO_LC_MCLK_DIV_Pos)

#define WJ_CLK_RATIO_BC_PCLK_DIV_Pos              (8U)
#define WJ_CLK_RATIO_BC_PCLK_DIV_Msk              (0xFU << WJ_CLK_RATIO_BC_PCLK_DIV_Pos)

#define WJ_CLK_RATIO_LC_PCLK_DIV_Pos              (12U)
#define WJ_CLK_RATIO_LC_PCLK_DIV_Msk              (0xFU << WJ_CLK_RATIO_LC_PCLK_DIV_Pos)

#define WJ_CLK_RATIO_APB_REF_CLK_RATIO_Pos        (16U)
#define WJ_CLK_RATIO_APB_REF_CLK_RATIO_Msk        (0xFU << WJ_CLK_RATIO_APB_REF_CLK_RATIO_Pos)

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

#define WJ_RESET_STATE_WDT0_Pos                   (3U)
#define WJ_RESET_STATE_WDT0_Msk                   (0x1U << WJ_RESET_STATE_WDT0_Pos)
#define WJ_RESET_STATE_WDT0_RESET                 WJ_RESET_STATE_WDT0_Msk

#define WJ_RESET_STATE_WDT1_Pos                   (4U)
#define WJ_RESET_STATE_WDT1_Msk                   (0x1U << WJ_RESET_STATE_WDT1_Pos)
#define WJ_RESET_STATE_WDT1_RESET                 WJ_RESET_STATE_WDT1_Msk

#define WJ_RESET_STATE_CPU0_Pos                   (5U)
#define WJ_RESET_STATE_CPU0_Msk                   (0x1U << WJ_RESET_STATE_CPU0_Pos)
#define WJ_RESET_STATE_CPU0_RESET                 WJ_RESET_STATE_CPU0_Msk

#define WJ_RESET_STATE_CPU1_Pos                   (6U)
#define WJ_RESET_STATE_CPU1_Msk                   (0x1U << WJ_RESET_STATE_CPU1_Pos)
#define WJ_RESET_STATE_CPU1_RESET                 WJ_RESET_STATE_CPU1_Msk

/* DFC_CTRL, offset: 0x518 */
#define WJ_DFC_CTRL_DFC_CNT_Pos                   (0U)
#define WJ_DFC_CTRL_DFC_CNT_Msk                   (0xFFU << WJ_DFC_CTRL_DFC_CNT_Pos)

#define WJ_DFC_CTRL_CLK_SW_MODE_Pos               (8U)
#define WJ_DFC_CTRL_CLK_SW_MODE_Msk               (0x1U << WJ_DFC_CTRL_CLK_SW_MODE_Pos)
#define WJ_DFC_CTRL_WAIT_CPU_QSPI_IDLE            WJ_DFC_CTRL_CLK_SW_MODE_Msk

/* RAM_DELAY, offset: 0x51c */
#define WJ_RAM_DELAY_BC_ISRAM0_Pos                (0U)
#define WJ_RAM_DELAY_BC_ISRAM0_Msk                (0x3U << WJ_RAM_DELAY_BC_ISRAM0_Pos)
#define WJ_RAM_DELAY_BC_ISRAM0_0_CYCLE_DELAY      (0x0U << WJ_RAM_DELAY_BC_ISRAM0_Pos)
#define WJ_RAM_DELAY_BC_ISRAM0_1_CYCLE_DELAY      (0x1U << WJ_RAM_DELAY_BC_ISRAM0_Pos)
#define WJ_RAM_DELAY_BC_ISRAM0_2_CYCLE_DELAY      (0x2U << WJ_RAM_DELAY_BC_ISRAM0_Pos)

#define WJ_RAM_DELAY_BC_ISRAM1_Pos                (2U)
#define WJ_RAM_DELAY_BC_ISRAM1_Msk                (0x3U << WJ_RAM_DELAY_BC_ISRAM1_Pos)
#define WJ_RAM_DELAY_BC_ISRAM1_0_CYCLE_DELAY      (0x0U << WJ_RAM_DELAY_BC_ISRAM1_Pos)
#define WJ_RAM_DELAY_BC_ISRAM1_1_CYCLE_DELAY      (0x1U << WJ_RAM_DELAY_BC_ISRAM1_Pos)
#define WJ_RAM_DELAY_BC_ISRAM1_2_CYCLE_DELAY      (0x2U << WJ_RAM_DELAY_BC_ISRAM1_Pos)

#define WJ_RAM_DELAY_BC_DSRAM0_Pos                (4U)
#define WJ_RAM_DELAY_BC_DSRAM0_Msk                (0x3U << WJ_RAM_DELAY_BC_DSRAM0_Pos)
#define WJ_RAM_DELAY_BC_DSRAM0_0_CYCLE_DELAY      (0x0U << WJ_RAM_DELAY_BC_DSRAM0_Pos)
#define WJ_RAM_DELAY_BC_DSRAM0_1_CYCLE_DELAY      (0x1U << WJ_RAM_DELAY_BC_DSRAM0_Pos)
#define WJ_RAM_DELAY_BC_DSRAM0_2_CYCLE_DELAY      (0x2U << WJ_RAM_DELAY_BC_DSRAM0_Pos)

#define WJ_RAM_DELAY_BC_DSRAM1_Pos                (6U)
#define WJ_RAM_DELAY_BC_DSRAM1_Msk                (0x3U << WJ_RAM_DELAY_BC_DSRAM1_Pos)
#define WJ_RAM_DELAY_BC_DSRAM1_0_CYCLE_DELAY      (0x0U << WJ_RAM_DELAY_BC_DSRAM1_Pos)
#define WJ_RAM_DELAY_BC_DSRAM1_1_CYCLE_DELAY      (0x1U << WJ_RAM_DELAY_BC_DSRAM1_Pos)
#define WJ_RAM_DELAY_BC_DSRAM1_2_CYCLE_DELAY      (0x2U << WJ_RAM_DELAY_BC_DSRAM1_Pos)

#define WJ_RAM_DELAY_BC_DSRAM2_Pos                (8U)
#define WJ_RAM_DELAY_BC_DSRAM2_Msk                (0x3U << WJ_RAM_DELAY_BC_DSRAM2_Pos)
#define WJ_RAM_DELAY_BC_DSRAM2_0_CYCLE_DELAY      (0x0U << WJ_RAM_DELAY_BC_DSRAM2_Pos)
#define WJ_RAM_DELAY_BC_DSRAM2_1_CYCLE_DELAY      (0x1U << WJ_RAM_DELAY_BC_DSRAM2_Pos)
#define WJ_RAM_DELAY_BC_DSRAM2_2_CYCLE_DELAY      (0x2U << WJ_RAM_DELAY_BC_DSRAM2_Pos)

/* MODE_CTRL, offset: 0x520 */
#define WJ_MODE_CTRL_SCLK_HOLD_Pos                (1U)
#define WJ_MODE_CTRL_SCLK_HOLD_Msk                (0x1U << WJ_MODE_CTRL_SCLK_HOLD_Pos)
#define WJ_MODE_CTRL_SCLK_HOLD                    WJ_MODE_CTRL_SCLK_HOLD_Msk

#define WJ_MODE_CTRL_AON_PWR_HOLD_Pos             (2U)
#define WJ_MODE_CTRL_AON_PWR_HOLD_Msk             (0x1U << WJ_MODE_CTRL_AON_PWR_HOLD_Pos)
#define WJ_MODE_CTRL_AON_PWR_HOLD                 WJ_MODE_CTRL_AON_PWR_HOLD_Msk

#define WJ_MODE_CTRL_LP_RAM_HOLD_Pos              (3U)
#define WJ_MODE_CTRL_LP_RAM_HOLD_Msk              (0x1U << WJ_MODE_CTRL_LP_RAM_HOLD_Pos)
#define WJ_MODE_CTRL_LP_RAM_HOLD                  WJ_MODE_CTRL_LP_RAM_HOLD_Msk

#define WJ_MODE_CTRL_MAIN_PWR_HOLD_Pos            (4U)
#define WJ_MODE_CTRL_MAIN_PWR_HOLD_Msk            (0x1U << WJ_MODE_CTRL_MAIN_PWR_HOLD_Pos)
#define WJ_MODE_CTRL_MAIN_PWR_HOLD                WJ_MODE_CTRL_MAIN_PWR_HOLD_Msk

#define WJ_MODE_CTRL_RF_PWR_HOLD_Pos              (5U)
#define WJ_MODE_CTRL_RF_PWR_HOLD_Msk              (0x1U << WJ_MODE_CTRL_RF_PWR_HOLD_Pos)
#define WJ_MODE_CTRL_RF_PWR_HOLD                  WJ_MODE_CTRL_RF_PWR_HOLD_Msk

#define WJ_MODE_CTRL_PDU_DCU_HOLD_Pos             (6U)
#define WJ_MODE_CTRL_PDU_DCU_HOLD_Msk             (0x1U << WJ_MODE_CTRL_PDU_DCU_HOLD_Pos)
#define WJ_MODE_CTRL_PDU_DCU_HOLD                 WJ_MODE_CTRL_PDU_DCU_HOLD_Msk

#define WJ_MODE_CTRL_ANA_DCU_HOLD_Pos             (7U)
#define WJ_MODE_CTRL_ANA_DCU_HOLD_Msk             (0x1U << WJ_MODE_CTRL_ANA_DCU_HOLD_Pos)
#define WJ_MODE_CTRL_ANA_DCU_HOLD                 WJ_MODE_CTRL_ANA_DCU_HOLD_Msk

#define WJ_MODE_CTRL_RETU_DCU_HOLD_Pos            (8U)
#define WJ_MODE_CTRL_RETU_DCU_HOLD_Msk            (0x1U << WJ_MODE_CTRL_RETU_DCU_HOLD_Pos)
#define WJ_MODE_CTRL_RETU_DCU_HOLD                WJ_MODE_CTRL_RETU_DCU_HOLD_Msk

#define WJ_MODE_CTRL_GNSS_DCU_HOLD_Pos            (9U)
#define WJ_MODE_CTRL_GNSS_DCU_HOLD_Msk            (0x1U << WJ_MODE_CTRL_GNSS_DCU_HOLD_Pos)
#define WJ_MODE_CTRL_GNSS_DCU_HOLD                WJ_MODE_CTRL_GNSS_DCU_HOLD_Msk

/* MEM_RET_CTRL, offset: 0x524 */
#define WJ_MEM_RET_CTRL_BKUP_SRAM_Pos             (0U)
#define WJ_MEM_RET_CTRL_BKUP_SRAM_Msk             (0x1U << WJ_MEM_RET_CTRL_BKUP_SRAM_Pos)
#define WJ_MEM_RET_CTRL_BKUP_SRAM                 WJ_MEM_RET_CTRL_BKUP_SRAM_Msk

#define WJ_MEM_RET_CTRL_I_D_SRAM_Pos              (1U)
#define WJ_MEM_RET_CTRL_I_D_SRAM_Msk              (0x1U << WJ_MEM_RET_CTRL_I_D_SRAM_Pos)
#define WJ_MEM_RET_CTRL_I_D_SRAM                  WJ_MEM_RET_CTRL_I_D_SRAM_Msk

#define WJ_MEM_RET_CTRL_LP_V_SEL_Pos              (2U)
#define WJ_MEM_RET_CTRL_LP_V_SEL_Msk              (0x1FU << WJ_MEM_RET_CTRL_I_D_SRAM_Pos)

/* BC_CTRL, offset: 0x528 */
#define WJ_BC_CTRL_BC_CPU_RST_Pos                 (0U)
#define WJ_BC_CTRL_BC_CPU_RST_Msk                 (0x1U << WJ_BC_CTRL_BC_CPU_RST_Pos)
#define WJ_BC_CTRL_BC_CPU_RST                     WJ_BC_CTRL_BC_CPU_RST_Msk

#define WJ_BC_CTRL_BC_CPU_HALT_Pos                (1U)
#define WJ_BC_CTRL_BC_CPU_HALT_Msk                (0x1U << WJ_BC_CTRL_BC_CPU_HALT_Pos)
#define WJ_BC_CTRL_BC_CPU_HALT                    WJ_BC_CTRL_BC_CPU_HALT_Msk

#define WJ_BC_CTRL_BC_CPU_DFS_ACK_Pos             (2U)
#define WJ_BC_CTRL_BC_CPU_DFS_ACK_Msk             (0x1U << WJ_BC_CTRL_BC_CPU_DFS_ACK_Pos)
#define WJ_BC_CTRL_BC_CPU_DFS_ACK                 WJ_BC_CTRL_BC_CPU_DFS_ACK_Msk

/* APWR_CNT, offset: 0x528 */
#define WJ_APWR_CNT_AON_PWR_ON_Pos                (0U)
#define WJ_APWR_CNT_AON_PWR_ON_Msk                (0xFFFFU << WJ_APWR_CNT_AON_PWR_ON_Pos)

#define WJ_APWR_CNT_AON_PWR_OFF_Pos               (16U)
#define WJ_APWR_CNT_AON_PWR_OFF_Msk               (0xFFFFU << WJ_APWR_CNT_AON_PWR_OFF_Pos)

/* RLDO_CNT, offset: 0x534 */
#define WJ_RLDO_CNT_RAM_LDO_ON_Pos                (0U)
#define WJ_RLDO_CNT_RAM_LDO_ON_Msk                (0xFFFFU << WJ_RLDO_CNT_RAM_LDO_ON_Pos)

#define WJ_RLDO_CNT_RAM_LDO_OFF_Pos               (16U)
#define WJ_RLDO_CNT_RAM_LDO_OFF_Msk               (0xFFFFU << WJ_RLDO_CNT_RAM_LDO_OFF_Pos)

/* MPWR_CNT, offset: 0x538 */
#define WJ_MPWR_CNT_MAIN_PWR_ON_Pos               (0U)
#define WJ_MPWR_CNT_MAIN_PWR_ON_Msk               (0xFFFFU << WJ_MPWR_CNT_MAIN_PWR_ON_Pos)

#define WJ_MPWR_CNT_MAIN_PWR_OFF_Pos              (16U)
#define WJ_MPWR_CNT_MAIN_PWR_OFF_Msk              (0xFFFFU << WJ_MPWR_CNT_MAIN_PWR_OFF_Pos)

/* LDO_STROBE_CNT, offset: 0x53c */
#define WJ_LDO_STROBE_CNT_tSET_Pos                (0U)
#define WJ_LDO_STROBE_CNT_tSET_Msk                (0xFFU << WJ_LDO_STROBE_CNT_tSET_Pos)

#define WJ_LDO_STROBE_tPW_Pos                     (8U)
#define WJ_LDO_STROBE_tPW_Msk                     (0xFFU << WJ_LDO_STROBE_tPW_Pos)

/* RPWR_CNT, offset: 0x540 */
#define WJ_RPWR_CNT_RF_PWR_Pos                    (0U)
#define WJ_RPWR_CNT_RF_PWR_Msk                    (0xFFU << WJ_RPWR_CNT_RF_PWR_Pos)

/* RTC_ISO_CTRL, offset: 0x544 */
#define WJ_RTC_ISO_DATA_Pos                       (0U)
#define WJ_RTC_ISO_DATA_Msk                       (0xFFU << WJ_RTC_ISO_DATA_Pos)

#define WJ_RTC_ISO_WKEN_Pos                       (8U)
#define WJ_RTC_ISO_WKEN_Msk                       (0x1U << WJ_RTC_ISO_WKEN_Pos)

/*PMU CONFIG*/
static inline void wj_pmu_set_rtc_iso_ctrl_data(wj_pmu_reg_t *pmu_base, uint8_t data)
{
    pmu_base->RTC_ISO_CTRL &= ~WJ_RTC_ISO_DATA_Msk;
    pmu_base->RTC_ISO_CTRL |= (data << WJ_RTC_ISO_DATA_Pos);
}

static inline void wj_pmu_set_rtc_iso_ctrl_wken(wj_pmu_reg_t *pmu_base)
{
    pmu_base->RTC_ISO_CTRL |= WJ_RTC_ISO_WKEN_Msk;
}

static inline void wj_pmu_clear_rtc_iso_ctrl_wken(wj_pmu_reg_t *pmu_base)
{
    pmu_base->RTC_ISO_CTRL &= ~WJ_RTC_ISO_WKEN_Msk;
}

static inline void wj_pmu_lp_wait_clk_on_mode_enable(wj_pmu_reg_t *pmu_base)
{
    pmu_base->LPCR &= ~WJ_LPCR_LP_MODE_WAIT_CLK_ON_MODE;
}

static inline void wj_pmu_lp_wait_clk_off_mode_enable(wj_pmu_reg_t *pmu_base)
{
    pmu_base->LPCR |= WJ_LPCR_LP_MODE_WAIT_CLK_OFF_MODE;
}

static inline void wj_pmu_lp_retention_mode_enable(wj_pmu_reg_t *pmu_base)
{
    pmu_base->LPCR |= WJ_LPCR_LP_MODE_RETENTION_MODE;
}

static inline void wj_pmu_lp_backup_mode_enable(wj_pmu_reg_t *pmu_base)
{
    pmu_base->LPCR |= WJ_LPCR_LP_MODE_BACKUP_MODE;
}

static inline void wj_pmu_lp_standby_mode_enable(wj_pmu_reg_t *pmu_base)
{
    pmu_base->LPCR |= WJ_LPCR_LP_MODE_STANDBY_MODE;
}

static inline uint32_t wj_get_soc_boot_from(wj_pmu_reg_t *pmu_base)
{
    return (pmu_base->LPCR & WJ_LPCR_BOOT_FROM);
}

static inline uint32_t ck_get_cpu0_run_mode(wj_pmu_reg_t *pmu_base)
{
    return (pmu_base->LPCR & WJ_LPCR_CPU0_RUN);
}

static inline uint32_t ck_get_cpu1_run_mode(wj_pmu_reg_t *pmu_base)
{
    return (pmu_base->LPCR & WJ_LPCR_CPU1_RUN);
}

static inline uint32_t ck_get_lp_mode_state(wj_pmu_reg_t *pmu_base)
{
    return (pmu_base->LPCR & WJ_LPCR_LP_STATE);
}

static inline void wj_pmu_allow_lp_mode(wj_pmu_reg_t *pmu_base)
{
    pmu_base->LPCR |= WJ_LPCR_LOW_MODE_EN;
}

static inline void wj_pmu_unallow_lp_mode(wj_pmu_reg_t *pmu_base)
{
    pmu_base->LPCR &= ~WJ_LPCR_LOW_MODE_EN;
}

static inline uint32_t wj_get_els_clock_state(wj_pmu_reg_t *pmu_base)
{
    return (pmu_base->CLKSTBR & WJ_CLKSTBR_ELSSTB_STATE);
}

static inline uint32_t wj_get_pll_clock_state(wj_pmu_reg_t *pmu_base)
{
    return (pmu_base->CLKSTBR & WJ_CLKSTBR_PLLSTB_STATE);
}

static inline uint32_t wj_get_els_clock_irq_state(wj_pmu_reg_t *pmu_base)
{
    return (pmu_base->CLKSTBST & WJ_ELS_INTERRUPT_EN);
}

static inline uint32_t wj_get_pll_clock_irq_state(wj_pmu_reg_t *pmu_base)
{
    return (pmu_base->CLKSTBST & WJ_PLL_INTERRUPT_EN);
}

static inline void wj_set_els_clock_irq_mask(wj_pmu_reg_t *pmu_base)
{
    pmu_base->CLKSTBMK |= WJ_ELS_INTERRUPT_MASK_EN;
}

static inline void wj_set_els_clock_irq_unmask(wj_pmu_reg_t *pmu_base)
{
    pmu_base->CLKSTBMK &= ~WJ_ELS_INTERRUPT_MASK_EN;
}

static inline void wj_set_pll_clock_irq_mask(wj_pmu_reg_t *pmu_base)
{
    pmu_base->CLKSTBMK |= WJ_PLL_INTERRUPT_MASK_EN;
}

static inline void wj_set_pll_clock_irq_unmask(wj_pmu_reg_t *pmu_base)
{
    pmu_base->CLKSTBMK &= ~WJ_PLL_INTERRUPT_MASK_EN;
}

static inline uint32_t wj_get_clock_sources_stable_counter(wj_pmu_reg_t *pmu_base)
{
    return pmu_base->CSSCR;
}

static inline void wj_set_lc_mclk_dfcc_enable(wj_pmu_reg_t *pmu_base)
{
    pmu_base->DFCC |= WJ_DFCC_DFC_LC_MCLK_EN;
}

static inline void wj_set_lc_pclk_dfcc_enable(wj_pmu_reg_t *pmu_base)
{
    pmu_base->DFCC |= WJ_DFCC_DFC_LC_PCLK_EN;
}

static inline void wj_set_bc_mclk_dfcc_enable(wj_pmu_reg_t *pmu_base)
{
    pmu_base->DFCC |= WJ_DFCC_DFC_BC_MCLK_EN;
}

static inline void wj_set_bc_pclk_dfcc_enable(wj_pmu_reg_t *pmu_base)
{
    pmu_base->DFCC |= WJ_DFCC_DFC_BC_PCLK_EN;
}

static inline void wj_set_apb_ref_clk_dfcc_enable(wj_pmu_reg_t *pmu_base)
{
    pmu_base->DFCC |= WJ_DFCC_DFC_APB_REF_CLK_EN;
}

static inline uint32_t wj_get_lc_mclk_dfcc_state(wj_pmu_reg_t *pmu_base)
{
    return (pmu_base->DFCC & WJ_DFCC_DFCEN_LC_MCLK_Msk);
}

static inline uint32_t wj_get_lc_pclk_dfcc_state(wj_pmu_reg_t *pmu_base)
{
    return (pmu_base->DFCC & WJ_DFCC_DFCEN_LC_PCLK_Msk);
}

static inline uint32_t wj_get_bc_mclk_dfcc_state(wj_pmu_reg_t *pmu_base)
{
    return (pmu_base->DFCC & WJ_DFCC_DFCEN_BC_MCLK_Msk);
}

static inline uint32_t wj_get_bc_pclk_dfcc_state(wj_pmu_reg_t *pmu_base)
{
    return (pmu_base->DFCC & WJ_DFCC_DFCEN_BC_PCLK_Msk);
}

static inline uint32_t wj_get_apb_ref_clk_dfcc_state(wj_pmu_reg_t *pmu_base)
{
    return (pmu_base->DFCC & WJ_DFCC_DFCEN_APB_REF_CLK_Msk);
}

static inline void wj_set_pll_vcodivsel(wj_pmu_reg_t *pmu_base, uint32_t val)
{
    pmu_base->PCR &= ~WJ_PCR_VCODIVSEL_Msk;
    pmu_base->PCR |= val << WJ_PCR_VCODIVSEL_Pos;
}

static inline void wj_set_pll_fbdiv_divide(wj_pmu_reg_t *pmu_base, uint32_t val)
{
    pmu_base->PCR &= ~WJ_PCR_FBDIV_Msk;
    pmu_base->PCR |= val << WJ_PCR_FBDIV_Pos;
}

static inline void wj_set_pll_postdiv_divide(wj_pmu_reg_t *pmu_base, uint32_t val)
{
    pmu_base->PCR &= ~WJ_PCR_POSTDIV_Msk;
    pmu_base->PCR |= val << WJ_PCR_POSTDIV_Pos;
}

static inline void wj_pll_foutvcoen_enable(wj_pmu_reg_t *pmu_base)
{
    pmu_base->PCR |= WJ_PDU_FOUTBCOEN_EN;
}

static inline void wj_pll_foutvcoen_disable(wj_pmu_reg_t *pmu_base)
{
    pmu_base->PCR &= ~WJ_PDU_FOUTBCOEN_EN;
}

static inline void wj_pll_foutpostdiven_enable(wj_pmu_reg_t *pmu_base)
{
    pmu_base->PCR |= WJ_PDU_FOUTPOSTDIVEN_EN;
}

static inline void wj_pll_foutpostdiven_disable(wj_pmu_reg_t *pmu_base)
{
    pmu_base->PCR &= ~WJ_PDU_FOUTPOSTDIVEN_EN;
}

static inline void wj_set_pll_phase_correction_mode(wj_pmu_reg_t *pmu_base)
{
    pmu_base->PCR |= WJ_PDU_PLL_MODE;
}

static inline void wj_set_pll_normal_mode(wj_pmu_reg_t *pmu_base)
{
    pmu_base->PCR &= ~WJ_PDU_PLL_MODE;
}

static inline void wj_set_pll_fref_bypass(wj_pmu_reg_t *pmu_base)
{
    pmu_base->PCR |= WJ_PDU_BYPASS;
}

static inline void wj_set_pll_fvco_bypass(wj_pmu_reg_t *pmu_base)
{
    pmu_base->PCR &= ~WJ_PDU_BYPASS;
}

static inline void wj_set_pll_fcalires(wj_pmu_reg_t *pmu_base, uint32_t val)
{
    pmu_base->PCR1 &= ~WJ_PCR_FCALIRES_Msk;
    pmu_base->PCR1 |= val << WJ_PCR_FCALIRES_Pos;
}

static inline void wj_set_lc_wakeup_source(wj_pmu_reg_t *pmu_base, uint32_t wakeupn)
{
    pmu_base->LC_WIME &= ~((uint32_t)1U << wakeupn);
}

static inline void wj_clear_lc_wakeup_source(wj_pmu_reg_t *pmu_base, uint32_t wakeupn)
{
    pmu_base->LC_WIME |= ((uint32_t)1U << wakeupn);
}

static inline void wj_set_dcu_mode_change_enable(wj_pmu_reg_t *pmu_base)
{
    pmu_base->MODE_CHG_EN |= WJ_MODE_CHG_EN_DCU_MD_EN;
}

static inline uint32_t wj_get_dcu_mode_change_state(wj_pmu_reg_t *pmu_base)
{
    return (pmu_base->MODE_CHG_EN & WJ_MODE_CHG_EN_DCU_MD_EN);
}

static inline void wj_set_src_clk_change_enable(wj_pmu_reg_t *pmu_base)
{
    pmu_base->MODE_CHG_EN |= WJ_MODE_CHG_EN_SRC_CLK_EN;
}

static inline uint32_t wj_get_src_clk_change_state(wj_pmu_reg_t *pmu_base)
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

static inline void wj_set_clk0_select_pll_hs_clock(wj_pmu_reg_t *pmu_base)
{
    pmu_base->SRC_CLKMD |= WJ_SRC_CLKMD_CLK_SEL0;
}

static inline void wj_set_clk0_select_pll_ls_clock(wj_pmu_reg_t *pmu_base)
{
    pmu_base->SRC_CLKMD &= ~WJ_SRC_CLKMD_CLK_SEL0;
}

static inline void wj_set_clk1_select_ihs_clock(wj_pmu_reg_t *pmu_base)
{
    pmu_base->SRC_CLKMD &= ~WJ_SRC_CLKMD_CLK_SEL1;
}

static inline void wj_set_clk1_select_clk0_clock(wj_pmu_reg_t *pmu_base)
{
    pmu_base->SRC_CLKMD |= WJ_SRC_CLKMD_CLK_SEL1;
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

static inline void wj_set_pll_output_enable(wj_pmu_reg_t *pmu_base)
{
    pmu_base->SRC_CLKMD |= WJ_SRC_CLKMD_PLL_EN;
}

static inline void wj_set_pll_output_disable(wj_pmu_reg_t *pmu_base)
{
    pmu_base->SRC_CLKMD &= ~WJ_SRC_CLKMD_PLL_EN;
}

static inline void wj_aon_pwr_output_supply_select_ilr(wj_pmu_reg_t *pmu_base)
{
    pmu_base->SRC_LDOMD &= ~WJ_SRC_LDOMD_AON_PWR_SEL_QLR;
}

static inline void wj_aon_pwr_output_supply_select_qlr(wj_pmu_reg_t *pmu_base)
{
    pmu_base->SRC_LDOMD |= WJ_SRC_LDOMD_AON_PWR_SEL_QLR;
}

static inline void wj_set_aon_pwr_vsel(wj_pmu_reg_t *pmu_base, uint32_t vreg)
{
    pmu_base->SRC_LDOMD &= ~WJ_SRC_LDOMD_AON_PWR_VSEL_Msk;
    pmu_base->SRC_LDOMD |= vreg << WJ_SRC_LDOMD_AON_PWR_VSEL_Pos;
}

static inline void wj_lp_ram_ldo_shutdown_mode(wj_pmu_reg_t *pmu_base)
{
    pmu_base->SRC_LDOMD |= WJ_SRC_LDOMD_LP_RAM_LDO_SHUTDOWN_MODE;
}

static inline void wj_lp_ram_ldo_normal_mode(wj_pmu_reg_t *pmu_base)
{
    pmu_base->SRC_LDOMD |= WJ_SRC_LDOMD_LP_RAM_LDO_NORMAL_MODE;
}

static inline void wj_lp_ram_ldo_hiz_mode(wj_pmu_reg_t *pmu_base)
{
    pmu_base->SRC_LDOMD |= WJ_SRC_LDOMD_LP_RAM_LDO_HIZ_MODE;
}

static inline void wj_set_lp_ram_pwr_vsel(wj_pmu_reg_t *pmu_base, uint32_t vreg)
{
    pmu_base->SRC_LDOMD &= ~WJ_SRC_LDOMD_LP_RAM_LDO_VSEL_Msk;
    pmu_base->SRC_LDOMD |= vreg << WJ_SRC_LDOMD_LP_RAM_LDO_VSEL_Pos;
}

static inline void wj_main_pwr_shutdown_mode(wj_pmu_reg_t *pmu_base)
{
    pmu_base->SRC_LDOMD |= WJ_SRC_LDOMD_MAIN_PWR_SHUTDOWN_MODE;
}

static inline void wj_main_pwr_normal_mode(wj_pmu_reg_t *pmu_base)
{
    pmu_base->SRC_LDOMD |= WJ_SRC_LDOMD_MAIN_PWR_NORMAL_MODE;
}

static inline void wj_main_pwr_hiz_mode(wj_pmu_reg_t *pmu_base)
{
    pmu_base->SRC_LDOMD |= WJ_SRC_LDOMD_MAIN_PWR_HIZ_MODE;
}

static inline void wj_main_pwr_sel_ilr(wj_pmu_reg_t *pmu_base)
{
    pmu_base->SRC_LDOMD |= WJ_SRC_LDOMD_MAIN_PWR_SEL_ILR;
}

static inline void wj_main_pwr_sel_dcdc_ulp(wj_pmu_reg_t *pmu_base)
{
    pmu_base->SRC_LDOMD &= ~WJ_SRC_LDOMD_MAIN_PWR_SEL_ILR;
}

static inline void wj_set_ihs_rc_tf0_trim(wj_pmu_reg_t *pmu_base, uint32_t vreg)
{
    pmu_base->PMU_TRIM &= ~WJ_PMU_TRIM_IHS_RC_TF0_TRIM_Msk;
    pmu_base->PMU_TRIM |= WJ_PMU_TRIM_IHS_RC_TF0_TRIM_Msk & (vreg << WJ_PMU_TRIM_IHS_RC_TF0_TRIM_Pos);
}

static inline uint32_t wj_get_ihs_rc_tf0_trim(wj_pmu_reg_t *pmu_base, uint32_t vreg)
{
    return ((pmu_base->PMU_TRIM & WJ_PMU_TRIM_IHS_RC_TF0_TRIM_Msk) >> WJ_PMU_TRIM_IHS_RC_TF0_TRIM_Pos);
}

static inline void wj_set_ihs_rc_tf1_trim(wj_pmu_reg_t *pmu_base, uint32_t vreg)
{
    pmu_base->PMU_TRIM &= ~WJ_PMU_TRIM_IHS_RC_TF1_TRIM_Msk;
    pmu_base->PMU_TRIM |= WJ_PMU_TRIM_IHS_RC_TF1_TRIM_Msk & (vreg << WJ_PMU_TRIM_IHS_RC_TF1_TRIM_Pos);
}

static inline uint32_t wj_get_ihs_rc_tf1_trim(wj_pmu_reg_t *pmu_base, uint32_t vreg)
{
    return ((pmu_base->PMU_TRIM & WJ_PMU_TRIM_IHS_RC_TF1_TRIM_Msk) >> WJ_PMU_TRIM_IHS_RC_TF1_TRIM_Pos);
}

static inline void wj_set_vsel_bor(wj_pmu_reg_t *pmu_base, uint32_t vreg)
{
    pmu_base->PMU_TRIM &= ~WJ_PMU_TRIM_VSEL_BOR_Msk;
    pmu_base->PMU_TRIM |= WJ_PMU_TRIM_VSEL_BOR_Msk & (vreg << WJ_PMU_TRIM_VSEL_BOR_Pos);
}

static inline uint32_t wj_get_vsel_bor(wj_pmu_reg_t *pmu_base, uint32_t vreg)
{
    return ((pmu_base->PMU_TRIM & WJ_PMU_TRIM_VSEL_BOR_Msk) >> WJ_PMU_TRIM_VSEL_BOR_Pos);
}

static inline void wj_set_en_bor(wj_pmu_reg_t *pmu_base, uint32_t vreg)
{
    pmu_base->PMU_TRIM &= ~WJ_PMU_TRIM_BOR_EN_Msk;
    pmu_base->PMU_TRIM |= WJ_PMU_TRIM_BOR_EN_Msk & (vreg << WJ_PMU_TRIM_BOR_EN_Pos);
}

static inline uint32_t wj_get_en_bor(wj_pmu_reg_t *pmu_base, uint32_t vreg)
{
    return ((pmu_base->PMU_TRIM & WJ_PMU_TRIM_BOR_EN_Msk) >> WJ_PMU_TRIM_BOR_EN_Pos);
}

static inline void wj_set_main_pwr_vsel(wj_pmu_reg_t *pmu_base, uint32_t vreg)
{
    pmu_base->PMU_TRIM &= ~WJ_PMU_TRIM_MAIN_PWR_VSEL_Msk;
    pmu_base->PMU_TRIM |= WJ_PMU_TRIM_MAIN_PWR_VSEL_Msk & (vreg << WJ_PMU_TRIM_MAIN_PWR_VSEL_Pos);
}

static inline uint32_t wj_get_main_pwr_vsel(wj_pmu_reg_t *pmu_base, uint32_t vreg)
{
    return ((pmu_base->PMU_TRIM & WJ_PMU_TRIM_MAIN_PWR_VSEL_Msk) >> WJ_PMU_TRIM_MAIN_PWR_VSEL_Pos);
}

static inline void wj_set_rf_pwr_vsel(wj_pmu_reg_t *pmu_base, uint32_t vreg)
{
    pmu_base->PMU_TRIM &= ~WJ_PMU_TRIM_RF_PWR_VSEL_Msk;
    pmu_base->PMU_TRIM |= WJ_PMU_TRIM_RF_PWR_VSEL_Msk & (vreg << WJ_PMU_TRIM_RF_PWR_VSEL_Pos);
}

static inline uint32_t wj_get_rf_pwr_vsel(wj_pmu_reg_t *pmu_base, uint32_t vreg)
{
    return ((pmu_base->PMU_TRIM & WJ_PMU_TRIM_RF_PWR_VSEL_Msk) >> WJ_PMU_TRIM_RF_PWR_VSEL_Pos);
}

static inline void wj_rf_pwr_shutdown_mode(wj_pmu_reg_t *pmu_base)
{
    pmu_base->SRC_LDOMD |= WJ_SRC_LDOMD_RF_PWR_SHUTDOWN_MODE;
}

static inline void wj_rf_pwr_normal_mode(wj_pmu_reg_t *pmu_base)
{
    pmu_base->SRC_LDOMD |= WJ_SRC_LDOMD_RF_PWR_NORMAL_MODE;
}

static inline void wj_rf_pwr_hiz_mode(wj_pmu_reg_t *pmu_base)
{
    pmu_base->SRC_LDOMD |= WJ_SRC_LDOMD_RF_PWR_HIZ_MODE;
}


static inline void wj_set_gnss_dcu_lp_pon_mode(wj_pmu_reg_t *pmu_base)
{
    pmu_base->GNSS_DCU_MODE &= ~WJ_GNSS_DCU_MODE_LP_MODE_Msk;
    pmu_base->GNSS_DCU_MODE |= WJ_GNSS_DCU_MODE_PON;
}

static inline void wj_set_gnss_ckoff_pon_mode(wj_pmu_reg_t *pmu_base)
{
    pmu_base->GNSS_DCU_MODE &= ~WJ_GNSS_DCU_MODE_LP_MODE_Msk;
    pmu_base->GNSS_DCU_MODE |= WJ_GNSS_DCU_MODE_CKOFF_PON;
}

static inline void wj_set_gnss_ckoff_poff_mode(wj_pmu_reg_t *pmu_base)
{
    pmu_base->GNSS_DCU_MODE &= ~WJ_GNSS_DCU_MODE_LP_MODE_Msk;
    pmu_base->GNSS_DCU_MODE |= WJ_GNSS_DCU_MODE_CKOFF_POFF;
}

static inline void wj_set_gnss_dcu_clock_bc_gnss_clk_on(wj_pmu_reg_t *pmu_base)
{
    pmu_base->GNSS_DCU_CGCR |= WJ_GNSS_DCU_CGCR_BC_GNSS_CLK_GATE_ON;
}

static inline void wj_set_gnss_dcu_clock_bc_gnss_clk_off(wj_pmu_reg_t *pmu_base)
{
    pmu_base->GNSS_DCU_CGCR &= ~WJ_GNSS_DCU_CGCR_BC_GNSS_CLK_GATE_ON;
}

static inline void wj_set_gnss_dcu_clock_lc_gnss_clk_on(wj_pmu_reg_t *pmu_base)
{
    pmu_base->GNSS_DCU_CGCR |= WJ_GNSS_DCU_CGCR_LC_GNSS_CLK_GATE_ON;
}

static inline void wj_set_gnss_dcu_clock_lc_gnss_clk_off(wj_pmu_reg_t *pmu_base)
{
    pmu_base->GNSS_DCU_CGCR &= ~WJ_GNSS_DCU_CGCR_LC_GNSS_CLK_GATE_ON;
}

static inline void wj_set_gnss_dcu_clock_lc_gnss_swrst(wj_pmu_reg_t *pmu_base)
{
    pmu_base->GNSS_DCU_SWRST &= ~WJ_GNSS_DCU_SWRST_LC_GNSS_SWRST_ON;
    pmu_base->GNSS_DCU_SWRST |= WJ_GNSS_DCU_SWRST_LC_GNSS_SWRST_ON;
}

static inline void wj_set_gnss_dcu_clock_bc_gnss_swrst(wj_pmu_reg_t *pmu_base)
{
    pmu_base->GNSS_DCU_SWRST &= ~WJ_GNSS_DCU_SWRST_BC_GNSS_SWRST_ON;
    pmu_base->GNSS_DCU_SWRST |= WJ_GNSS_DCU_SWRST_BC_GNSS_SWRST_ON;
}

static inline void wj_set_lc_cpu_ckon(wj_pmu_reg_t *pmu_base)
{
    pmu_base->LC_CPU_DCU_MODE |= WJ_LC_CPU_CKGT_CKON;
}

static inline void wj_set_lc_cpu_ckoff(wj_pmu_reg_t *pmu_base)
{
    pmu_base->LC_CPU_DCU_MODE &= ~WJ_LC_CPU_CKGT_CKON;
}

static inline void wj_set_bc_cpu_ckon(wj_pmu_reg_t *pmu_base)
{
    pmu_base->BC_CPU_DCU_MODE |= WJ_BC_CPU_CKGT_CKON;
}

static inline void wj_set_bc_cpu_ckoff(wj_pmu_reg_t *pmu_base)
{
    pmu_base->BC_CPU_DCU_MODE &= ~WJ_BC_CPU_CKGT_CKON;
}

static inline void wj_set_retu_dcu_lp_pon_mode(wj_pmu_reg_t *pmu_base)
{
    pmu_base->RETU_DCU_MODE &= ~WJ_RETU_DCU_MODE_LP_MODE_Msk;
    pmu_base->RETU_DCU_MODE |= WJ_RETU_DCU_MODE_CKOFF_PON;
}

static inline void wj_set_retu_ckoff_pon_mode(wj_pmu_reg_t *pmu_base)
{
    pmu_base->RETU_DCU_MODE &= ~WJ_RETU_DCU_MODE_LP_MODE_Msk;
    pmu_base->RETU_DCU_MODE |= WJ_RETU_DCU_MODE_PON;
}

static inline void wj_set_retu_ckoff_poff_mode(wj_pmu_reg_t *pmu_base)
{
    pmu_base->RETU_DCU_MODE &= ~WJ_RETU_DCU_MODE_LP_MODE_Msk;
    pmu_base->RETU_DCU_MODE |= WJ_RETU_DCU_MODE_CKOFF_POFF;
}

static inline void wj_set_pdu_dcu_lp_pon_mode(wj_pmu_reg_t *pmu_base)
{
    pmu_base->PDU_DCU_MODE &= ~WJ_PDU_DCU_MODE_LP_MODE_Msk;
    pmu_base->PDU_DCU_MODE |= WJ_PDU_DCU_MODE_CKOFF_PON;
}

static inline void wj_set_pdu_ckoff_pon_mode(wj_pmu_reg_t *pmu_base)
{
    pmu_base->PDU_DCU_MODE &= ~WJ_PDU_DCU_MODE_LP_MODE_Msk;
    pmu_base->PDU_DCU_MODE |= WJ_PDU_DCU_MODE_CKOFF_POFF;
}

static inline void wj_set_pdu_ckoff_poff_mode(wj_pmu_reg_t *pmu_base)
{
    pmu_base->PDU_DCU_MODE &= ~WJ_PDU_DCU_MODE_LP_MODE_Msk;
    pmu_base->PDU_DCU_MODE |= WJ_PDU_DCU_MODE_PON;
}

static inline void wj_pdu_dcu_lc_matrix_clock_on(wj_pmu_reg_t *pmu_base, uint32_t ch)
{
    pmu_base->PDU_DCU_LC_MCGCR |= (1U << ch);
}

static inline void wj_pdu_dcu_lc_matrix_clock_off(wj_pmu_reg_t *pmu_base, uint32_t ch)
{
    pmu_base->PDU_DCU_LC_MCGCR &= ~(1U << ch);
}

static inline void wj_pdu_dcu_bc_matrix_clock_on(wj_pmu_reg_t *pmu_base, uint32_t ch)
{
    pmu_base->PDU_DCU_BC_MCGCR |= (1U << ch);
}

static inline void wj_pdu_dcu_bc_matrix_clock_off(wj_pmu_reg_t *pmu_base, uint32_t ch)
{
    pmu_base->PDU_DCU_BC_MCGCR &= ~(1U << ch);
}

static inline void wj_pdu_dcu_bc_matrix_reset(wj_pmu_reg_t *pmu_base, uint32_t ch)
{
    pmu_base->PDU_DCU_BC_MSWRST |= (1U << ch);
    pmu_base->PDU_DCU_BC_MSWRST &= ~(1U << ch);
}

static inline void wj_pdu_dcu_bc_apb_clock_on(wj_pmu_reg_t *pmu_base, uint32_t ch)
{
    pmu_base->PDU_DCU_P0CGCR |= (1U << ch);
}

static inline void wj_pdu_dcu_bc_apb_clock_off(wj_pmu_reg_t *pmu_base, uint32_t ch)
{
    pmu_base->PDU_DCU_P0CGCR &= ~(1U << ch);
}

static inline void wj_pdu_dcu_bc_apb_reset(wj_pmu_reg_t *pmu_base, uint32_t ch)
{
    pmu_base->PDU_DCU_P0SWRST |= (1U << ch);
    pmu_base->PDU_DCU_P0SWRST &= ~(1U << ch);
}

static inline void wj_pdu_dcu_lc_apb_clock_on(wj_pmu_reg_t *pmu_base, uint32_t ch)
{
    pmu_base->PDU_DCU_P1CGCR |= (1U << ch);
}

static inline void wj_pdu_dcu_lc_apb_clock_off(wj_pmu_reg_t *pmu_base, uint32_t ch)
{
    pmu_base->PDU_DCU_P1CGCR &= ~(1U << ch);
}

static inline void wj_pdu_dcu_lc_apb_reset(wj_pmu_reg_t *pmu_base, uint32_t ch)
{
    pmu_base->PDU_DCU_P1SWRST |= (1U << ch);
    pmu_base->PDU_DCU_P1SWRST &= ~(1U << ch);
}

static inline void wj_set_ana_dcu_lp_pon_mode(wj_pmu_reg_t *pmu_base)
{
    pmu_base->ANA_DCU_MODE &= ~WJ_ANA_DCU_MODE_LPMODE_Msk;
    pmu_base->ANA_DCU_MODE |= WJ_ANA_DCU_MODE_PON;
}

static inline void wj_set_ana_dcu_lp_ckoff_pon_mode(wj_pmu_reg_t *pmu_base)
{
    pmu_base->ANA_DCU_MODE &= ~WJ_ANA_DCU_MODE_LPMODE_Msk;
    pmu_base->ANA_DCU_MODE |= WJ_ANA_DCU_MODE_CKOFF_PON;
}

static inline void wj_set_ana_dcu_lp_ckoff_poff_mode(wj_pmu_reg_t *pmu_base)
{
    pmu_base->ANA_DCU_MODE &= ~WJ_ANA_DCU_MODE_LPMODE_Msk;
    pmu_base->ANA_DCU_MODE |= WJ_ANA_DCU_MODE_CKOFF_POFF;
}

static inline void wj_set_aon_dcu_ckoff_mode(wj_pmu_reg_t *pmu_base)
{
    pmu_base->ANA_DCU_MODE &= ~WJ_AON_DCU_MODE_CKOFF;
}

static inline void wj_set_aon_dcu_ckon_mode(wj_pmu_reg_t *pmu_base)
{
    pmu_base->ANA_DCU_MODE |= WJ_AON_DCU_MODE_CKOFF;
}

static inline void wj_set_aoapb_dcu_clock_on(wj_pmu_reg_t *pmu_base, uint32_t ch)
{
    pmu_base->AOAPB_DCU_CKGT |= (1U << ch);
}

static inline void wj_set_aoapb_dcu_clock_off(wj_pmu_reg_t *pmu_base, uint32_t ch)
{
    pmu_base->AOAPB_DCU_CKGT &= ~(1U << ch);
}

static inline void wj_set_aoapb_dcu_rtc_clk_reset(wj_pmu_reg_t *pmu_base)
{
    pmu_base->AOAPB_DCU_SWRST |= WJ_AOAPB_DCU_SWRST_RTC_SWRST_ON;
    pmu_base->AOAPB_DCU_SWRST &= ~WJ_AOAPB_DCU_SWRST_RTC_SWRST_ON;
}

static inline void wj_set_aoapb_dcu_loctl_clk_reset(wj_pmu_reg_t *pmu_base)
{
    pmu_base->AOAPB_DCU_SWRST |= WJ_AOAPB_DCU_SWRST_IOCTL_SWRST_ON;
    pmu_base->AOAPB_DCU_SWRST &= ~WJ_AOAPB_DCU_SWRST_IOCTL_SWRST_ON;
}

static inline uint32_t wj_get_soc_id(wj_pmu_reg_t *pmu_base)
{
    return pmu_base->SOC_ID;
}

static inline uint32_t wj_get_rst_dis_dur(wj_pmu_reg_t *pmu_base)
{
    return (pmu_base->DCU_RST_DUR & WJ_DCU_RST_CNT_RST_DIS_DUR_Msk);
}

static inline uint32_t wj_get_rst_en_dur(wj_pmu_reg_t *pmu_base)
{
    return (pmu_base->DCU_RST_DUR & WJ_DCU_RST_CNT_RST_EN_DUR_Msk);
}

static inline uint32_t wj_get_iso_dis_dur(wj_pmu_reg_t *pmu_base)
{
    return (pmu_base->DCU_ISO_DUR & WJ_DCU_ISO_CNT_ISO_DIS_DUR_Msk);
}

static inline uint32_t wj_get_iso_en_dur(wj_pmu_reg_t *pmu_base)
{
    return (pmu_base->DCU_ISO_DUR & WJ_DCU_ISO_CNT_ISO_EN_DUR_Msk);
}

static inline void wj_set_clk_ratio_bc_mclk(wj_pmu_reg_t *pmu_base, uint32_t ratio)
{
    pmu_base->CLK_RATIO &= ~(WJ_CLK_RATIO_BC_MCLK_DIV_Msk);
    pmu_base->CLK_RATIO |= ((ratio << WJ_CLK_RATIO_BC_MCLK_DIV_Pos) & WJ_CLK_RATIO_BC_MCLK_DIV_Msk);
}

static inline uint32_t wj_get_clk_ratio_bc_mclk(wj_pmu_reg_t *pmu_base)
{
    return (pmu_base->CLK_RATIO  & WJ_CLK_RATIO_BC_MCLK_DIV_Msk);
}

static inline void wj_set_clk_ratio_lc_mclk(wj_pmu_reg_t *pmu_base, uint32_t ratio)
{
    pmu_base->CLK_RATIO &= ~(WJ_CLK_RATIO_LC_MCLK_DIV_Msk);
    pmu_base->CLK_RATIO |= ((ratio << WJ_CLK_RATIO_LC_MCLK_DIV_Pos) & WJ_CLK_RATIO_LC_MCLK_DIV_Msk);
}

static inline uint32_t wj_get_clk_ratio_lc_mclk(wj_pmu_reg_t *pmu_base)
{
    return (pmu_base->CLK_RATIO  & WJ_CLK_RATIO_LC_MCLK_DIV_Msk);
}

static inline void wj_set_clk_ratio_bc_pclk(wj_pmu_reg_t *pmu_base, uint32_t ratio)
{
    pmu_base->CLK_RATIO &= ~(WJ_CLK_RATIO_BC_PCLK_DIV_Msk);
    pmu_base->CLK_RATIO |= ((ratio << WJ_CLK_RATIO_BC_PCLK_DIV_Pos) & WJ_CLK_RATIO_BC_PCLK_DIV_Msk);
}

static inline uint32_t wj_get_clk_ratio_bc_pclk(wj_pmu_reg_t *pmu_base)
{
    return (pmu_base->CLK_RATIO  & WJ_CLK_RATIO_BC_PCLK_DIV_Msk);
}

static inline void wj_set_clk_ratio_lc_pclk(wj_pmu_reg_t *pmu_base, uint32_t ratio)
{
    pmu_base->CLK_RATIO &= ~(WJ_CLK_RATIO_LC_PCLK_DIV_Msk);
    pmu_base->CLK_RATIO |= ((ratio << WJ_CLK_RATIO_LC_PCLK_DIV_Pos) & WJ_CLK_RATIO_LC_PCLK_DIV_Msk);
}

static inline uint32_t wj_get_clk_ratio_lc_pclk(wj_pmu_reg_t *pmu_base)
{
    return (pmu_base->CLK_RATIO  & WJ_CLK_RATIO_LC_PCLK_DIV_Msk);
}

static inline void wj_set_clk_ratio_apb_ref_clk(wj_pmu_reg_t *pmu_base, uint32_t ratio)
{
    pmu_base->CLK_RATIO &= ~(WJ_CLK_RATIO_APB_REF_CLK_RATIO_Msk);
    pmu_base->CLK_RATIO |= ((ratio << WJ_CLK_RATIO_APB_REF_CLK_RATIO_Pos) & WJ_CLK_RATIO_APB_REF_CLK_RATIO_Msk);
}

static inline uint32_t wj_get_clk_ratio_apb_ref_clk(wj_pmu_reg_t *pmu_base)
{
    return (pmu_base->CLK_RATIO  & WJ_CLK_RATIO_APB_REF_CLK_RATIO_Msk);
}

static inline void wj_set_clock_switch_mode_wait_cpu_qspi_idie(wj_pmu_reg_t *pmu_base)
{
    pmu_base->DFC_CTRL |= WJ_DFC_CTRL_WAIT_CPU_QSPI_IDLE;
}

static inline void wj_set_clock_switch_mode_can_not_wait_cpu_qspi_idie(wj_pmu_reg_t *pmu_base)
{
    pmu_base->DFC_CTRL &= ~WJ_DFC_CTRL_WAIT_CPU_QSPI_IDLE;
}

static inline uint32_t wj_get_dfc_cnt(wj_pmu_reg_t *pmu_base)
{
    return (pmu_base->DFC_CTRL  & WJ_DFC_CTRL_DFC_CNT_Msk);
}

static inline void wj_set_bc_isram0_no_delay(wj_pmu_reg_t *pmu_base)
{
    pmu_base->BC_RAM_DEKAY &= ~WJ_RAM_DELAY_BC_ISRAM0_Msk;
    pmu_base->BC_RAM_DEKAY |= WJ_RAM_DELAY_BC_ISRAM0_0_CYCLE_DELAY;
}

static inline void wj_set_bc_isram0_1_cycle_delay(wj_pmu_reg_t *pmu_base)
{
    pmu_base->BC_RAM_DEKAY &= ~WJ_RAM_DELAY_BC_ISRAM0_Msk;
    pmu_base->BC_RAM_DEKAY |= WJ_RAM_DELAY_BC_ISRAM0_1_CYCLE_DELAY;
}

static inline void wj_set_bc_isram0_2_cycle_delay(wj_pmu_reg_t *pmu_base)
{
    pmu_base->BC_RAM_DEKAY &= ~WJ_RAM_DELAY_BC_ISRAM0_Msk;
    pmu_base->BC_RAM_DEKAY |= WJ_RAM_DELAY_BC_ISRAM0_2_CYCLE_DELAY;
}

static inline void wj_set_bc_isram1_no_delay(wj_pmu_reg_t *pmu_base)
{
    pmu_base->BC_RAM_DEKAY &= ~WJ_RAM_DELAY_BC_ISRAM1_Msk;
    pmu_base->BC_RAM_DEKAY |= WJ_RAM_DELAY_BC_ISRAM1_0_CYCLE_DELAY;
}

static inline void wj_set_bc_isram1_1_cycle_delay(wj_pmu_reg_t *pmu_base)
{
    pmu_base->BC_RAM_DEKAY &= ~WJ_RAM_DELAY_BC_ISRAM1_Msk;
    pmu_base->BC_RAM_DEKAY |= WJ_RAM_DELAY_BC_ISRAM1_1_CYCLE_DELAY;
}

static inline void wj_set_bc_isram1_2_cycle_delay(wj_pmu_reg_t *pmu_base)
{
    pmu_base->BC_RAM_DEKAY &= ~WJ_RAM_DELAY_BC_ISRAM1_Msk;
    pmu_base->BC_RAM_DEKAY |= WJ_RAM_DELAY_BC_ISRAM1_2_CYCLE_DELAY;
}

static inline void wj_set_bc_dsram0_no_delay(wj_pmu_reg_t *pmu_base)
{
    pmu_base->BC_RAM_DEKAY &= ~WJ_RAM_DELAY_BC_DSRAM0_Msk;
    pmu_base->BC_RAM_DEKAY |= WJ_RAM_DELAY_BC_DSRAM0_0_CYCLE_DELAY;
}

static inline void wj_set_bc_dsram0_1_cycle_delay(wj_pmu_reg_t *pmu_base)
{
    pmu_base->BC_RAM_DEKAY &= ~WJ_RAM_DELAY_BC_DSRAM0_Msk;
    pmu_base->BC_RAM_DEKAY |= WJ_RAM_DELAY_BC_DSRAM0_1_CYCLE_DELAY;
}

static inline void wj_set_bc_dsram0_2_cycle_delay(wj_pmu_reg_t *pmu_base)
{
    pmu_base->BC_RAM_DEKAY &= ~WJ_RAM_DELAY_BC_DSRAM0_Msk;
    pmu_base->BC_RAM_DEKAY |= WJ_RAM_DELAY_BC_DSRAM0_2_CYCLE_DELAY;
}

static inline void wj_set_bc_dsram1_no_delay(wj_pmu_reg_t *pmu_base)
{
    pmu_base->BC_RAM_DEKAY &= ~WJ_RAM_DELAY_BC_DSRAM1_Msk;
    pmu_base->BC_RAM_DEKAY |= WJ_RAM_DELAY_BC_DSRAM1_0_CYCLE_DELAY;
}

static inline void wj_set_bc_dsram1_1_cycle_delay(wj_pmu_reg_t *pmu_base)
{
    pmu_base->BC_RAM_DEKAY &= ~WJ_RAM_DELAY_BC_DSRAM1_Msk;
    pmu_base->BC_RAM_DEKAY |= WJ_RAM_DELAY_BC_DSRAM1_1_CYCLE_DELAY;
}

static inline void wj_set_bc_dsram1_2_cycle_delay(wj_pmu_reg_t *pmu_base)
{
    pmu_base->BC_RAM_DEKAY &= ~WJ_RAM_DELAY_BC_DSRAM1_Msk;
    pmu_base->BC_RAM_DEKAY |= WJ_RAM_DELAY_BC_DSRAM1_2_CYCLE_DELAY;
}

static inline void wj_set_bc_dsram2_no_delay(wj_pmu_reg_t *pmu_base)
{
    pmu_base->BC_RAM_DEKAY &= ~WJ_RAM_DELAY_BC_DSRAM2_Msk;
    pmu_base->BC_RAM_DEKAY |= WJ_RAM_DELAY_BC_DSRAM2_0_CYCLE_DELAY;
}

static inline void wj_set_bc_dsram2_1_cycle_delay(wj_pmu_reg_t *pmu_base)
{
    pmu_base->BC_RAM_DEKAY &= ~WJ_RAM_DELAY_BC_DSRAM2_Msk;
    pmu_base->BC_RAM_DEKAY |= WJ_RAM_DELAY_BC_DSRAM2_1_CYCLE_DELAY;
}

static inline void wj_set_bc_dsram2_2_cycle_delay(wj_pmu_reg_t *pmu_base)
{
    pmu_base->BC_RAM_DEKAY &= ~WJ_RAM_DELAY_BC_DSRAM2_Msk;
    pmu_base->BC_RAM_DEKAY |= WJ_RAM_DELAY_BC_DSRAM2_2_CYCLE_DELAY;
}

static inline void wj_set_mode_ctrl_hold(wj_pmu_reg_t *pmu_base, uint32_t ch)
{
    pmu_base->MODE_CTRL |= (1U << ch);
}

static inline void wj_set_mode_ctrl_unhold(wj_pmu_reg_t *pmu_base, uint32_t ch)
{
    pmu_base->MODE_CTRL &= ~(0U << ch);
}

static inline uint32_t wj_get_mode_ctrl_hold(wj_pmu_reg_t *pmu_base)
{
    return pmu_base->MODE_CTRL;
}

static inline void wj_set_bkup_sram_ret1n_hardware(wj_pmu_reg_t *pmu_base)
{
    pmu_base->MEM_RET_CTRL &= ~WJ_MEM_RET_CTRL_BKUP_SRAM;
}

static inline void wj_set_bkup_sram_ret1n_bypass(wj_pmu_reg_t *pmu_base)
{
    pmu_base->MEM_RET_CTRL |= WJ_MEM_RET_CTRL_BKUP_SRAM;
}

static inline void wj_set_isram_dsram_ret1n_hardware(wj_pmu_reg_t *pmu_base)
{
    pmu_base->MEM_RET_CTRL &= ~WJ_MEM_RET_CTRL_I_D_SRAM;
}

static inline void wj_set_isram_dsram_ret1n_bypass(wj_pmu_reg_t *pmu_base)
{
    pmu_base->MEM_RET_CTRL |= WJ_MEM_RET_CTRL_I_D_SRAM;
}

static inline void wj_set_lp_voltage_sel(wj_pmu_reg_t *pmu_base, uint32_t val)
{
    pmu_base->MEM_RET_CTRL |= ((val << WJ_MEM_RET_CTRL_BKUP_SRAM_Pos) & WJ_MEM_RET_CTRL_BKUP_SRAM);
}

static inline void wj_set_bc_cpu_rst(wj_pmu_reg_t *pmu_base)
{
    pmu_base->BC_CTRL &= ~WJ_BC_CTRL_BC_CPU_RST;
    pmu_base->BC_CTRL |= WJ_BC_CTRL_BC_CPU_RST;
}

static inline void wj_set_bc_cpu_halt(wj_pmu_reg_t *pmu_base)
{
    pmu_base->BC_CTRL |= WJ_BC_CTRL_BC_CPU_HALT;
}

static inline void wj_set_bc_cpu_release(wj_pmu_reg_t *pmu_base)
{
    pmu_base->BC_CTRL &= ~WJ_BC_CTRL_BC_CPU_HALT;
}

static inline uint32_t wj_read_bc_cpu_work_mode(wj_pmu_reg_t *pmu_base)
{
    return (pmu_base->BC_CTRL & WJ_BC_CTRL_BC_CPU_DFS_ACK);
}

static inline uint32_t wj_get_aon_pwr_on_stable_counter(wj_pmu_reg_t *pmu_base)
{
    return (pmu_base->APWR_CNT & WJ_APWR_CNT_AON_PWR_ON_Msk);
}

static inline uint32_t wj_get_aon_pwr_off_stable_counter(wj_pmu_reg_t *pmu_base)
{
    return (pmu_base->APWR_CNT & WJ_APWR_CNT_AON_PWR_OFF_Msk);
}

static inline uint32_t wj_get_ram_ldo_on_stable_counter(wj_pmu_reg_t *pmu_base)
{
    return (pmu_base->RLDO_CNT & WJ_RLDO_CNT_RAM_LDO_ON_Msk);
}

static inline uint32_t wj_get_ram_ldo_off_stable_counter(wj_pmu_reg_t *pmu_base)
{
    return (pmu_base->RLDO_CNT & WJ_RLDO_CNT_RAM_LDO_OFF_Msk);
}

static inline uint32_t wj_get_main_pwr_on_stable_counter(wj_pmu_reg_t *pmu_base)
{
    return (pmu_base->MPWR_CNT & WJ_MPWR_CNT_MAIN_PWR_ON_Msk);
}

static inline uint32_t wj_get_main_pwr_off_stable_counter(wj_pmu_reg_t *pmu_base)
{
    return (pmu_base->MPWR_CNT & WJ_MPWR_CNT_MAIN_PWR_OFF_Msk);
}

static inline uint32_t wj_get_tset_ldo_strobe_duration_counter(wj_pmu_reg_t *pmu_base)
{
    return (pmu_base->STROBE_CNT & WJ_LDO_STROBE_CNT_tSET_Msk);
}

static inline uint32_t wj_get_tpw_ldo_strobe_duration_counter(wj_pmu_reg_t *pmu_base)
{
    return (pmu_base->STROBE_CNT & WJ_LDO_STROBE_tPW_Msk);
}

static inline uint32_t wj_get_rf_pwr_stable_counter(wj_pmu_reg_t *pmu_base)
{
    return (pmu_base->RPWR_CNT & WJ_RPWR_CNT_RF_PWR_Msk);
}


static inline uint32_t wj_get_user_define_value(wj_pmu_reg_t *pmu_base, uint32_t idx)
{
    if (idx < 4) {
        return pmu_base->USER_DEFINE[idx];
    } else {
        return  0;
    }
}

static inline void wj_set_user_define_value(wj_pmu_reg_t *pmu_base, uint32_t idx, uint32_t value)
{
    if (idx < 4) {
        pmu_base->USER_DEFINE[idx] = value;
    }
}

#ifdef __cplusplus
}
#endif

#endif
