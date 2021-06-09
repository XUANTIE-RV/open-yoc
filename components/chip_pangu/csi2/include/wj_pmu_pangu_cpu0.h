
/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     wj_pangu_pmu.h
 * @brief    head file for pangu pmu
 * @version  V1.0
 * @date     7. August 2020
 ******************************************************************************/
#ifndef _WJ_PANGU_PMU_H_
#define _WJ_PANGU_PMU_H_

#include <soc.h>
#include <csi_core.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    __IOM uint32_t PMU_CLKSRCSEL;                 /* Offset: 0x000 (W/R) 32K clock source selection register                   */
    __IOM uint32_t PMU_SYSCLKDIVEN;               /* Offset: 0x004 (W/R) System clock divider control register                 */
    __IOM uint32_t PMU_CPU12SWRST;                /* Offset: 0x008 (W/R) CPU1, CPU2 reset control register                     */
    __IOM uint32_t PMU_PLLCTRL;                   /* Offset: 0x00C (W/R) System PLL control register                           */
    __IOM uint32_t PMU_PLLCTRL_FRAC;              /* Offset: 0x010 (W/R) Fractional PLL control register                       */
    __IOM uint32_t PMU_LPCR;                      /* Offset: 0x014 (W/R) Low power control register                            */
    __IOM uint32_t PMU_WKUPMASK;                  /* Offset: 0x018 (W/R) Wake-up mask register                                 */
    __IOM uint32_t PMU_RSTSTA;                    /* Offset: 0x01C (W/R) Reset status register                                 */
    __IOM uint32_t PMU_RCCTRL;                    /* Offset: 0x020 (W/R) Crystal Control Register                              */
    __IM  uint32_t PMU_LPSTA;                     /* Offset: 0x024 ( /R) Low power status register                             */
    __IM  uint32_t PMU_LVMD;                      /* Offset: 0x028 ( /R) Reserve                                               */
    __IOM uint32_t PMU_REQMDTRIG;                 /* Offset: 0x02C (W/R) Reserve                                               */
    __IM  uint32_t PMU_PERIERRSTA;                /* Offset: 0x030 ( /R) Peripheral error register                             */
    __IOM uint32_t PMU_PLLINTRCTRL;               /* Offset: 0x034 (W/R) PLL interrupt control register                        */
    __IOM uint32_t PMU_FRACPLLFRAC;               /* Offset: 0x038 (W/R) Fractional PLL, FRAC parameter configuration register */
    uint32_t RESERVED0;                           /* Offset: 0x03C (W/R) Reserve                                               */
    __IOM uint32_t PMU_BOOTADDR_804_0;            /* Offset: 0x040 (W/R) CPU0 boot address register                            */
    __IOM uint32_t PMU_BOOTADDR_805;              /* Offset: 0x044 (W/R) CPU2 boot address register                            */
    __IOM uint32_t PMU_BOOTADDR_BYPASS;           /* Offset: 0x048 (W/R) CPU boot address mask register                        */
    __IOM uint32_t PMU_AONTIMCTRL;                /* Offset: 0x04C (W/R) Low-power counter control register                    */
    uint32_t RESERVED1;                           /* Offset: 0x050 (W/R) Reserve                                               */
    __IOM uint32_t PMU_AONTIMRST;                 /* Offset: 0x054 (W/R) Low power counter reset register                      */
    uint32_t RESERVED2;                           /* Offset: 0x058 (W/R) Reserve                                               */
    __IOM uint32_t PMU_AONRTCRST;                 /* Offset: 0x05C (W/R) RTC reset register                                    */
    uint32_t RESERVED3;                           /* Offset: 0x060 (W/R) Reserve                                               */
    __IOM uint32_t PMU_AONGPIORST;                /* Offset: 0x064 (W/R) GPIO0 reset register                                  */
    __IOM uint32_t PMU_LPBOOTADDR_804_0;          /* Offset: 0x068 (W/R) CPU0 low power boot register                          */
    __IOM uint32_t PMU_LPBOOTADDR_805;            /* Offset: 0x06C (W/R) CPU2 low power boot register                          */
    __IOM uint32_t PMU_QSPI_CNT;                  /* Offset: 0x070 (W/R) Wait for QSPI prefetch end counter                    */
    __IOM uint32_t PMU_WAIT_12M;                  /* Offset: 0x074 (W/R) Wait for 12M crystal oscillator to stabilize counter  */
    __IOM uint32_t PMU_RTCCLKCTRL;                /* Offset: 0x078 (W/R) RTC clock control register                            */
    uint32_t RESERVED4;                           /* Offset: 0x07C (W/R) Reserve                                               */
    __IOM uint32_t PMU_BOOTADDR_804_1;            /* Offset: 0x080 (W/R) CPU1 boot address register                            */
    __IOM uint32_t PMU_LP_BOOTADDR_804_1;         /* Offset: 0x084 (W/R) CPU1 low power boot register                          */
    uint32_t RESERVED5[94];                       /* Offset: 0x088 (W/R) Reserve                                               */
    __IOM uint32_t DLC_PCTRL;                     /* Offset: 0x200 (W/R) DLC bus control register                              */
    __IOM uint32_t DLC_PRDATA;                    /* Offset: 0x204 (W/R) DLC bus read data register                            */
    __IOM uint32_t DLC_SR;                        /* Offset: 0x208 (W/R) LC bus status register                                */
    __IOM uint32_t DLC_IMR;                       /* Offset: 0x20C (W/R) MAESTRO interrupt mask register                       */
    __IOM uint32_t DLC_IFR;                       /* Offset: 0x210 (W/R) MAESTRO interrupt status register                     */
    __IOM uint32_t DLC_IOIFR;                     /* Offset: 0x214 (W/R) ICU_OK status register                                */
    __IOM uint32_t DLC_IDIFR;                     /* Offset: 0x218 (W/R) ICU_DELAY status register                             */
    __IOM uint32_t DLC_IMCIFR;                    /* Offset: 0x21C (W/R) ICU_MODE_CHANGED status register                      */
} wj_pmu_reg_cpr0_t;

typedef struct {
    __IOM uint32_t CPR1_SYSCLK0_SEL_REG;           /* Offset: 0x000 (W/R) sys_clk_0 system clock source selection configuration register    */
    __IOM uint32_t CPR1_SYSCLK_DIV_CTL0_REG;       /* Offset: 0x004 (W/R) System clock divider configuration register 0                     */
    uint32_t RESERVED0;                            /* Offset: 0x008 (W/R) Reserve                                                           */
    __IOM uint32_t CPR1_FFT_CTL_REG;               /* Offset: 0x00C (W/R) FFT clock control register                                        */
    __IOM uint32_t CPR1_TIM1_CTL_REG;              /* Offset: 0x010 (W/R) Timer 1 clock control register                                    */
    __IOM uint32_t CPR1_FMC_AHB0_CTL_REG;          /* Offset: 0x014 (W/R) FMC ahb0 clock control register                                   */
    __IOM uint32_t CPR1_ROM_CTL_REG;               /* Offset: 0x018 (W/R) ROM clock control register                                        */
    __IOM uint32_t CPR1_GPIO0_CTL_REG;             /* Offset: 0x01C (W/R) GPIO0 clock control register                                      */
    __IOM uint32_t CPR1_QSPI_CTL_REG;              /* Offset: 0x020 (W/R) QSPI clock control register                                       */
    __IOM uint32_t CPR1_USI0_CTL_REG;              /* Offset: 0x024 (W/R) USI 0 clock control register                                      */
    __IOM uint32_t CPR1_USI1_CTL_REG;              /* Offset: 0x028 (W/R) USI 1 clock control register                                      */
    __IOM uint32_t CPR1_UART0_CTL_REG;             /* Offset: 0x02C (W/R) UART 0 clock control register                                     */
    __IOM uint32_t CPR1_I2S0_CLK_CTL_REG;          /* Offset: 0x030 (W/R) I2S0 clock control register                                       */
    __IOM uint32_t CPR1_I2S1_CLK_CTL_REG;          /* Offset: 0x034 (W/R) I2S1 clock control register                                       */
    __IOM uint32_t CPR1_EFUSE_CTL_REG;             /* Offset: 0x038 (W/R) APB0 eFUSE clock control register                                 */
    __IOM uint32_t CPR1_SASC_CTL_REG;              /* Offset: 0x03C (W/R) SASC clock control register                                       */
    __IOM uint32_t CPR1_TIPC_CTL_REG;              /* Offset: 0x040 (W/R) TIPC clock control register                                       */
    __IOM uint32_t CPR1_SDIO_CLK_CTL_REG;          /* Offset: 0x044 (W/R) SDIO clock control register                                       */
    __IOM uint32_t CPR1_SDMMC_CLK_CTL_REG;         /* Offset: 0x048 (W/R) SDMMC clock control register                                      */
    __IOM uint32_t CPR1_AES_CLK_CTL_REG;           /* Offset: 0x04C (W/R) AES clock control register                                        */
    __IOM uint32_t CPR1_RSA_CLK_CTL_REG;           /* Offset: 0x050 (W/R) RSA clock control register                                        */
    __IOM uint32_t CPR1_SHA_CLK_CTL_REG;           /* Offset: 0x054 (W/R) SHA clock control register                                        */
    __IOM uint32_t CPR1_TRNG_CLK_CTL_REG;          /* Offset: 0x058 (W/R) TRNG clock control register                                       */
    __IOM uint32_t CPR1_DMA0_CLK_CTL_REG;          /* Offset: 0x05C (W/R) DMA0 clock control register                                       */
    __IOM uint32_t CPR1_WDT_CLK_CTL_REG;           /* Offset: 0x060 (W/R) WDT clock control register                                        */
    __IOM uint32_t CPR1_PWM0_CLK_CTL_REG;          /* Offset: 0x064 (W/R) PWM0 clock control register                                       */
    __IOM uint32_t CPR1_MAINSWRST;                 /* Offset: 0x068 (W/R) AHB0 master software reset register                               */
    __IOM uint32_t CPR1_AHB0SUB0SWRST;             /* Offset: 0x06C (W/R) ahb0 sub0 software reset register                                 */
    __IOM uint32_t CPR1_AHB0SUB1SWRST;             /* Offset: 0x070 (W/R) ahb0 sub1 Software reset register                                 */
    __IOM uint32_t CPR1_AHB0SUB2SWRST;             /* Offset: 0x074 (W/R) ahb0 sub2 Software reset register                                 */
    __IOM uint32_t CPR1_APB0SWRST;                 /* Offset: 0x078 (W/R) apb0 software reset register                                      */
    __IOM uint32_t CPR1_UART1_CTL_REG;             /* Offset: 0x07c (W/R) UART1 clock control register                                      */
    __IOM uint32_t CPR1_CPU0_DAHBL_BASE_REG;       /* Offset: 0x080 (W/R) CPU0 DTCM space base address configuration register               */
    __IOM uint32_t CPR1_CPU0_DAHBL_MASK_REG;       /* Offset: 0x084 (W/R) CPU0 DTCM space MASK                                              */
    __IOM uint32_t CPR1_CPU0_IAHBL_BASE_REG;       /* Offset: 0x088 (W/R) CPU0 ITCM space base address configuration register               */
    __IOM uint32_t CPR1_CPU0_IAHBL_MASK_REG;       /* Offset: 0x08c (W/R) CPU0 ITCM space MASK                                              */
    __IOM uint32_t CPR1_CPU1_DAHBL_BASE_REG;       /* Offset: 0x090 (W/R) CPU1 DTCM space base address configuration register               */
    __IOM uint32_t CPR1_CPU1_DAHBL_MASK_REG;       /* Offset: 0x094 (W/R) CPU1 DTCM space MASK                                              */
    __IOM uint32_t CPR1_CPU1_IAHBL_BASE_REG;       /* Offset: 0x098 (W/R) CPU1 ITCM space base address configuration register               */
    __IOM uint32_t CPR1_CPU1_IAHBL_MASK_REG;       /* Offset: 0x09c (W/R) CPU1 ITCM space MASK                                              */
} wj_pmu_reg_cpr1_t;

typedef struct {
    __IOM uint32_t CPR2_SYSCLK_SEL_REG;            /* Offset: 0x000 (W/R) CPU2 system clock source selection configuration register */
    __IOM uint32_t CPR2_SYSCLK_DIV_CTL1_REG;       /* Offset: 0x004 (W/R) System clock divider configuration register 1             */
    __IOM uint32_t CPR2_TIM2_CTL_REG;              /* Offset: 0x008 (W/R) Timer 2 clock control register                            */
    __IOM uint32_t CPR2_TIM3_CTL_REG;              /* Offset: 0x00C (W/R) Timer 3 clock control register                            */
    __IOM uint32_t CPR2_GPIO1_CTL_REG;             /* Offset: 0x010 (W/R) GPIO 1 clock control register                             */
    __IOM uint32_t CPR2_USI2_CTL_REG;              /* Offset: 0x014 (W/R) USI 2 clock control register                              */
    __IOM uint32_t CPR2_USI3_CTL_REG;              /* Offset: 0x018 (W/R) USI 3 clock control register                              */
    __IOM uint32_t CPR2_UART2_CLK_CTL_REG;         /* Offset: 0x01C (W/R) UART2 clock control register                              */
    __IOM uint32_t CPR2_UART3_CLK_CTL_REG;         /* Offset: 0x020 (W/R) UART3 clock control register                              */
    __IOM uint32_t CPR2_I2S2_CTL_REG;              /* Offset: 0x024 (W/R) I2S 2 clock control register                              */
    __IOM uint32_t CPR2_I2S3_CTL_REG;              /* Offset: 0x028 (W/R) I2S 3 clock control register                              */
    __IOM uint32_t CPR2_SPDIF_CTL_REG;             /* Offset: 0x02C (W/R) SPDIF clock control register                              */
    __IOM uint32_t CPR2_FMC_AHB1_CTL_REG;          /* Offset: 0x030 (W/R) FSMC clock control register                               */
    __IOM uint32_t CPR2_TDM_CTL_REG;               /* Offset: 0x034 (W/R) TDM clock control register                                */
    __IOM uint32_t CPR2_PDM_CTL_REG;               /* Offset: 0x038 (W/R) PDM clock control register                                */
    __IOM uint32_t CPR2_DMA1_CLK_CTL_REG;          /* Offset: 0x03C (W/R) DMA1 clock control register                               */
    __IOM uint32_t CPR2_PWM1_CLK_CTL_REG;          /* Offset: 0x040 (W/R) PWM1 clock control register                               */
    __IOM uint32_t CPR2_DMA_CH8_SEL_REG;           /* Offset: 0x044 (W/R) DMA1 channel8 MUX selection controller                    */
    __IOM uint32_t CPR2_DMA_CH9_SEL_REG;           /* Offset: 0x048 (W/R) DMA1 channel9 MUX selection controller                    */
    __IOM uint32_t CPR2_DMA_CH10_SEL_REG;          /* Offset: 0x04C (W/R) DMA1 channel10 MUX selection controller                   */
    __IOM uint32_t CPR2_DMA_CH11_SEL_REG;          /* Offset: 0x050 (W/R) DMA1 channel11 MUX selection controller                   */
    __IOM uint32_t CPR2_DMA_CH12_SEL_REG;          /* Offset: 0x054 (W/R) DMA1 channel12 MUX selection controller                   */
    __IOM uint32_t CPR2_DMA_CH13_SEL_REG;          /* Offset: 0x058 (W/R) DMA1 channel13 MUX selection controller                   */
    __IOM uint32_t CPR2_DMA_CH14_SEL_REG;          /* Offset: 0x05C (W/R) DMA1 channel14 MUX selection controller                   */
    __IOM uint32_t CPR2_DMA_CH15_SEL_REG;          /* Offset: 0x060 (W/R) DMA1 channel15 MUX selection controller                   */
    __IOM uint32_t CPR2_MS1SWRST;                  /* Offset: 0x064 (W/R) AHB1 master software reset register                       */
    __IOM uint32_t CPR2_APB1SWRST;                 /* Offset: 0x068 (W/R) APB1 IP software reset register                           */
    __IOM uint32_t CPR2_CODEC_CLK_CTL_REG;         /* Offset: 0x06C (W/R) CODEC_GASKET clock control register                       */
} wj_pmu_reg_cpr2_t;


/* CPR0 PMU_CLKSRCSEL, Offset: 0x0 */
#define PMU_CLKSRCSEL_EILS_SEL_Pos                      0U
#define PMU_CLKSRCSEL_EILS_SEL_Msk                      (0x1U << PMU_CLKSRCSEL_EILS_SEL_Pos)
#define PMU_CLKSRCSEL_EILS_SEL_ILS                      PMU_CLKSRCSEL_EILS_SEL_Msk

/* CPR0 PMU_SYSCLKDIVEN, Offset: 0x4 */
#define PMU_SYSCLKDIVEN_D0_EN_Pos                       0U
#define PMU_SYSCLKDIVEN_D0_EN_Msk                       (0x1U << PMU_SYSCLKDIVEN_D0_EN_Pos)
#define PMU_SYSCLKDIVEN_D0_EN                           PMU_SYSCLKDIVEN_D0_EN_Msk
#define PMU_SYSCLKDIVEN_D0_DIV_Pos                      4U
#define PMU_SYSCLKDIVEN_D0_DIV_Msk                      (0xFU << PMU_SYSCLKDIVEN_D0_DIV_Pos)
#define PMU_SYSCLKDIVEN_D0_DIV(n)                       (((n) - 2) << PMU_SYSCLKDIVEN_D0_DIV_Pos)
#define PMU_SYSCLKDIVEN_D1_EN_Pos                       8U
#define PMU_SYSCLKDIVEN_D1_EN_Msk                       (0x1U << PMU_SYSCLKDIVEN_D1_EN_Pos)
#define PMU_SYSCLKDIVEN_D1_EN                           PMU_SYSCLKDIVEN_D1_EN_Msk
#define PMU_SYSCLKDIVEN_D1_DIV_Pos                      12U
#define PMU_SYSCLKDIVEN_D1_DIV_Msk                      (0xFU << PMU_SYSCLKDIVEN_D1_DIV_Pos)
#define PMU_SYSCLKDIVEN_D1_DIV(n)                       (((n) - 2) << PMU_SYSCLKDIVEN_D1_DIV_Pos)
#define PMU_SYSCLKDIVEN_D2_EN_Pos                       16U
#define PMU_SYSCLKDIVEN_D2_EN_Msk                       (0x1U << PMU_SYSCLKDIVEN_D2_EN_Pos)
#define PMU_SYSCLKDIVEN_D2_EN                           PMU_SYSCLKDIVEN_D2_EN_Msk
#define PMU_SYSCLKDIVEN_D2_DIV_Pos                      20U
#define PMU_SYSCLKDIVEN_D2_DIV_Msk                      (0x7U << PMU_SYSCLKDIVEN_D2_DIV_Pos)
#define PMU_SYSCLKDIVEN_D2_DIV(n)                       (((n) - 2) << PMU_SYSCLKDIVEN_D2_DIV_Pos)

/* CPR0 PMU_CPU12SWRST, Offset: 0x8 */
#define PMU_CPU12SWRST_CPU1_RST_EN_Pos                  0U
#define PMU_CPU12SWRST_CPU1_RST_EN_Msk                  (0x1U << PMU_CPU12SWRST_CPU1_RST_EN_Pos)
#define PMU_CPU12SWRST_CPU1_RST_EN                      PMU_CPU12SWRST_CPU1_RST_EN_Msk
#define PMU_CPU12SWRST_CPU2_RST_EN_Pos                  1U
#define PMU_CPU12SWRST_CPU2_RST_EN_Msk                  (0x1U << PMU_CPU12SWRST_CPU2_RST_EN_Pos)
#define PMU_CPU12SWRST_CPU2_RST_EN                      PMU_CPU12SWRST_CPU2_RST_EN_Msk

/* CPR0 PMU_PLLCTRL, Offset: 0xC */
#define PMU_PLLCTRL_PLL_LOCK_STA_Pos                    0U
#define PMU_PLLCTRL_PLL_LOCK_STA_Msk                    (0x1U << PMU_PLLCTRL_PLL_LOCK_STA_Pos)
#define PMU_PLLCTRL_PLL_LOCK_STA_LOCK                   PMU_PLLCTRL_PLL_LOCK_STA_Msk
#define PMU_PLLCTRL_PLL_BYPASS_Pos                      2U
#define PMU_PLLCTRL_PLL_BYPASS_Msk                      (0x1U << PMU_PLLCTRL_PLL_BYPASS_Pos)
#define PMU_PLLCTRL_PLL_BYPASS_EN                       PMU_PLLCTRL_PLL_BYPASS_Msk
#define PMU_PLLCTRL_PLL_DACPD_Pos                       3U
#define PMU_PLLCTRL_PLL_DACPD_Msk                       (0x1U << PMU_PLLCTRL_PLL_DACPD_Pos)
#define PMU_PLLCTRL_PLL_DACPD_EN                        PMU_PLLCTRL_PLL_DACPD_Msk
#define PMU_PLLCTRL_PLL_POSTDIV1_Pos                    4U
#define PMU_PLLCTRL_PLL_POSTDIV1_Msk                    (0x7U << PMU_PLLCTRL_PLL_POSTDIV1_Pos)
#define PMU_PLLCTRL_PLL_POSTDIV1(n)                     ((n) << PMU_PLLCTRL_PLL_POSTDIV1_Pos)
#define PMU_PLLCTRL_PLL_FOUT4PHASEPD_Pos                7U
#define PMU_PLLCTRL_PLL_FOUT4PHASEPD_Msk                (0x1U << PMU_PLLCTRL_PLL_FOUT4PHASEPD_Pos)
#define PMU_PLLCTRL_PLL_FOUT4PHASEPD_EN                 PMU_PLLCTRL_PLL_FOUT4PHASEPD_Msk
#define PMU_PLLCTRL_PLL_POSTDIV2_Pos                    8U
#define PMU_PLLCTRL_PLL_POSTDIV2_Msk                    (0x7U << PMU_PLLCTRL_PLL_POSTDIV2_Pos)
#define PMU_PLLCTRL_PLL_POSTDIV2(n)                     ((n) << PMU_PLLCTRL_PLL_POSTDIV2_Pos)
#define PMU_PLLCTRL_PLL_FOUTPOSTDIVPD_Pos               11U
#define PMU_PLLCTRL_PLL_FOUTPOSTDIVPD_Msk               (0x1U << PMU_PLLCTRL_PLL_FOUTPOSTDIVPD_Pos)
#define PMU_PLLCTRL_PLL_FOUTPOSTDIVPD_EN                PMU_PLLCTRL_PLL_FOUTPOSTDIVPD_Msk
#define PMU_PLLCTRL_PLL_REFDIV_Pos                      12U
#define PMU_PLLCTRL_PLL_REFDIV_Msk                      (0x3FU << PMU_PLLCTRL_PLL_REFDIV_Pos)
#define PMU_PLLCTRL_PLL_REFDIV_(n)                      ((n) << PMU_PLLCTRL_PLL_REFDIV_Pos)
#define PMU_PLLCTRL_PLL_FOUTVCOPD_Pos                   18U
#define PMU_PLLCTRL_PLL_FOUTVCOPD_Msk                   (0x1U << PMU_PLLCTRL_PLL_FOUTVCOPD_Pos)
#define PMU_PLLCTRL_PLL_FOUTVCOPD_EN                    PMU_PLLCTRL_PLL_FOUTVCOPD_Msk
#define PMU_PLLCTRL_PLL_FBDIV_Pos                       20U
#define PMU_PLLCTRL_PLL_FBDIV_Msk                       (0xFFFU << PMU_PLLCTRL_PLL_FBDIV_Pos)
#define PMU_PLLCTRL_PLL_FBDIV(n)                        ((n) << PMU_PLLCTRL_PLL_FBDIV_Pos)

/* CPR0 PMU_PLLCTRL_FRAC, Offset: 0x10 */
#define PMU_PLLCTRL_FRAC_PLL_LOCK_STA_Pos               0U
#define PMU_PLLCTRL_FRAC_PLL_LOCK_STA_Msk               (0x1U << PMU_PLLCTRL_FRAC_PLL_LOCK_STA_Pos)
#define PMU_PLLCTRL_FRAC_PLL_LOCK_STA_LOCK              PMU_PLLCTRL_FRAC_PLL_LOCK_STA_Msk
#define PMU_PLLCTRL_FRAC_PLL_BYPASS_Pos                 2U
#define PMU_PLLCTRL_FRAC_PLL_BYPASS_Msk                 (0x1U << PMU_PLLCTRL_FRAC_PLL_BYPASS_Pos)
#define PMU_PLLCTRL_FRAC_PLL_BYPASS_EN                  PMU_PLLCTRL_FRAC_PLL_BYPASS_Msk
#define PMU_PLLCTRL_FRAC_PLL_DACPD_Pos                  3U
#define PMU_PLLCTRL_FRAC_PLL_DACPD_Msk                  (0x1U << PMU_PLLCTRL_FRAC_PLL_DACPD_Pos)
#define PMU_PLLCTRL_FRAC_PLL_DACPD_EN                   PMU_PLLCTRL_FRAC_PLL_DACPD_Msk
#define PMU_PLLCTRL_FRAC_PLL_POSTDIV1_Pos               4U
#define PMU_PLLCTRL_FRAC_PLL_POSTDIV1_Msk               (0x7U << PMU_PLLCTRL_FRAC_PLL_POSTDIV1_Pos)
#define PMU_PLLCTRL_FRAC_PLL_POSTDIV1(n)                ((n) << PMU_PLLCTRL_FRAC_PLL_POSTDIV1_Pos)
#define PMU_PLLCTRL_FRAC_PLL_FOUT4PHASEPD_Pos           7U
#define PMU_PLLCTRL_FRAC_PLL_FOUT4PHASEPD_Msk           (0x1U << PMU_PLLCTRL_FRAC_PLL_FOUT4PHASEPD_Pos)
#define PMU_PLLCTRL_FRAC_PLL_FOUT4PHASEPD_EN            PMU_PLLCTRL_FRAC_PLL_FOUT4PHASEPD_Msk
#define PMU_PLLCTRL_FRAC_PLL_POSTDIV2_Pos               8U
#define PMU_PLLCTRL_FRAC_PLL_POSTDIV2_Msk               (0x7U << PMU_PLLCTRL_FRAC_PLL_POSTDIV2_Pos)
#define PMU_PLLCTRL_FRAC_PLL_POSTDIV2(n)                ((n) << PMU_PLLCTRL_FRAC_PLL_POSTDIV2_Pos)
#define PMU_PLLCTRL_FRAC_PLL_FOUTPOSTDIVPD_Pos          11U
#define PMU_PLLCTRL_FRAC_PLL_FOUTPOSTDIVPD_Msk          (0x1U << PMU_PLLCTRL_FRAC_PLL_FOUTPOSTDIVPD_Pos)
#define PMU_PLLCTRL_FRAC_PLL_FOUTPOSTDIVPD_EN           PMU_PLLCTRL_FRAC_PLL_FOUTPOSTDIVPD_Msk
#define PMU_PLLCTRL_FRAC_PLL_REFDIV_Pos                 12U
#define PMU_PLLCTRL_FRAC_PLL_REFDIV_Msk                 (0x3FU << PMU_PLLCTRL_FRAC_PLL_REFDIV_Pos)
#define PMU_PLLCTRL_FRAC_PLL_REFDIV(n)                  ((n) << PMU_PLLCTRL_FRAC_PLL_REFDIV_Pos)
#define PMU_PLLCTRL_FRAC_PLL_FOUTVCOPD_Pos              18U
#define PMU_PLLCTRL_FRAC_PLL_FOUTVCOPD_Msk              (0x1U << PMU_PLLCTRL_FRAC_PLL_FOUTVCOPD_Pos)
#define PMU_PLLCTRL_FRAC_PLL_FOUTVCOPD_EN               PMU_PLLCTRL_FRAC_PLL_FOUTVCOPD_Msk
#define PMU_PLLCTRL_FRAC_PLL_FBDIV_Pos                  20U
#define PMU_PLLCTRL_FRAC_PLL_FBDIV_Msk                  (0xFFFU << PMU_PLLCTRL_FRAC_PLL_FBDIV_Pos)
#define PMU_PLLCTRL_FRAC_PLL_FBDIV(n)                   ((n) << PMU_PLLCTRL_FRAC_PLL_FBDIV_Pos)

/* CPR0 PMU_LPCR, Offset: 0x14 */
#define PMU_LPCR_LPEN_Pos                               0U
#define PMU_LPCR_LPEN_Msk                               (0x1U << PMU_LPCR_LPEN_Pos)
#define PMU_LPCR_LPEN                                   PMU_LPCR_LPEN_Msk
#define PMU_LPCR_CHANGE_VOLT_Pos                        1U
#define PMU_LPCR_CHANGE_VOLT_Msk                        (0x1U << PMU_LPCR_CHANGE_VOLT_Pos)
#define PMU_LPCR_CHANGE_VOLT_ENTER_HW_VAD1              PMU_LPCR_CHANGE_VOLT_Msk
#define PMU_LPCR_SW_TRIGGER_Pos                         2U
#define PMU_LPCR_SW_TRIGGER_Msk                         (0x1U << PMU_LPCR_SW_TRIGGER_Pos)
#define PMU_LPCR_SW_TRIGGER                             PMU_LPCR_SW_TRIGGER_Msk
#define PMU_LPCR_BOOT_FROM_LP_Pos                       3U
#define PMU_LPCR_BOOT_FROM_LP_Msk                       (0x1U << PMU_LPCR_BOOT_FROM_LP_Pos)
#define PMU_LPCR_BOOT_FROM_LP                           PMU_LPCR_BOOT_FROM_LP_Msk

/* CPR0 PMU_WKUPMASK, Offset: 0x18 */
#define PMU_WKUPMASK_GPIO_MASK_EN_Pos                   0U
#define PMU_WKUPMASK_GPIO_MASK_EN_Msk                   (0x1 << PMU_WKUPMASK_GPIO_MASK_EN_Pos)
#define PMU_WKUPMASK_GPIO_MASK_EN                       PMU_WKUPMASK_GPIO_MASK_EN_Msk
#define PMU_WKUPMASK_AONTIM_MASK_EN_Pos                 1U
#define PMU_WKUPMASK_AONTIM_MASK_EN_Msk                 (0x1U << PMU_WKUPMASK_AONTIM_MASK_EN_Pos)
#define PMU_WKUPMASK_AONTIM_MASK_EN                     PMU_WKUPMASK_AONTIM_MASK_EN_Msk
#define PMU_WKUPMASK_RTC_MASK_EN_Pos                    2U
#define PMU_WKUPMASK_RTC_MASK_EN_Msk                    (0x1U << PMU_WKUPMASK_RTC_MASK_EN_Pos)
#define PMU_WKUPMASK_RTC_MASK_EN                        PMU_WKUPMASK_RTC_MASK_EN_Msk
#define PMU_WKUPMASK_CODEC_WT_MASK_EN_Pos               3U
#define PMU_WKUPMASK_CODEC_WT_MASK_EN_Msk               (0x1U << PMU_WKUPMASK_CODEC_WT_MASK_EN_Pos)
#define PMU_WKUPMASK_CODEC_WT_MASK_EN                   PMU_WKUPMASK_CODEC_WT_MASK_EN_Msk

/* CPR0 PMU_RSTSTA, Offset: 0x1C */
#define PMU_RSTSTA_POR_RST_FLAG_Pos                     0U
#define PMU_RSTSTA_POR_RST_FLAG_Msk                     (0x1U << PMU_RSTSTA_POR_RST_FLAG_Pos)
#define PMU_RSTSTA_POR_RST                              PMU_RSTSTA_POR_RST_FLAG_Msk
#define PMU_RSTSTA_MCU_RST_FLAG_Pos                     1U
#define PMU_RSTSTA_MCU_RST_FLAG_Msk                     (0x1U << PMU_RSTSTA_MCU_RST_FLAG_Pos)
#define PMU_RSTSTA_MCU_RST                              PMU_RSTSTA_MCU_RST_FLAG_Msk
#define PMU_RSTSTA_WDG_RST_FLAG_Pos                     2U
#define PMU_RSTSTA_WDG_RST_FLAG_Msk                     (0x1U << PMU_RSTSTA_WDG_RST_FLAG_Pos)
#define PMU_RSTSTA_WDG_RST                              PMU_RSTSTA_WDG_RST_FLAG_Msk
#define PMU_RSTSTA_CPU0_RST_FLAG_Pos                    3U
#define PMU_RSTSTA_CPU0_RST_FLAG_Msk                    (0x1U << PMU_RSTSTA_CPU0_RST_FLAG_Pos)
#define PMU_RSTSTA_CPU0_RST                             PMU_RSTSTA_CPU0_RST_FLAG_Msk

/* CPR0 PMU_RCCTRL, Offset: 0x20 */
#define PMU_RCCTRL_ILS_RC_TRIM_Pos                      0U
#define PMU_RCCTRL_ILS_RC_TRIM_Msk                      (0x3FU << PMU_RCCTRL_ILS_RC_TRIM_Pos)
#define PMU_RCCTRL_ILS_RC_TRIM(n)                       ((n) << PMU_RCCTRL_ILS_RC_TRIM_Pos)
#define PMU_RCCTRL_ILS_CLK_EN_Pos                       16U
#define PMU_RCCTRL_ILS_CLK_EN_Msk                       (0x3U << PMU_RCCTRL_ILS_CLK_EN_Pos)
#define PMU_RCCTRL_ILS_CLK_EN                           PMU_RCCTRL_ILS_CLK_EN_Msk
#define PMU_RCCTRL_ILS_CLK_DIS_FAST                     (0x2U << PMU_RCCTRL_ILS_CLK_EN_Pos)
#define PMU_RCCTRL_ILS_CLK_DIS_SLOW                     (0x0U << PMU_RCCTRL_ILS_CLK_EN_Pos)
#define PMU_RCCTRL_ELS_CLK_EN_Pos                       19U
#define PMU_RCCTRL_ELS_CLK_EN_Msk                       (0x1U << PMU_RCCTRL_ELS_CLK_EN_Pos)
#define PMU_RCCTRL_ELS_CLK_EN                           PMU_RCCTRL_ELS_CLK_EN_Msk
#define PMU_RCCTRL_ELS_CLK_BYPASS_Pos                   20U
#define PMU_RCCTRL_ELS_CLK_BYPASS_Msk                   (0x1U << PMU_RCCTRL_ELS_CLK_BYPASS_Pos)
#define PMU_RCCTRL_ELS_CLK_BYPASS                       PMU_RCCTRL_ELS_CLK_BYPASS_Msk

/* CPR0 PMU_LPSTA, Offset: 0x24 */
#define PMU_LPSTA_CPU0_LP_STA_Pos                       0U
#define PMU_LPSTA_CPU0_LP_STA_Msk                       (0x3U << PMU_LPSTA_CPU0_LP_STA_Pos)
#define PMU_LPSTA_CPU1_LP_STA_Pos                       2U
#define PMU_LPSTA_CPU1_LP_STA_Msk                       (0x3U << PMU_LPSTA_CPU0_LP_STA_Pos)
#define PMU_LPSTA_CPU2_LP_STA_Pos                       4U
#define PMU_LPSTA_CPU2_LP_STA_Msk                       (0x3U << PMU_LPSTA_CPU0_LP_STA_Pos)

/* CPR0 PMU_LVMD, Offset: 0x28 */
#define PMU_LVMD_LV_MD_INTR_Pos                         0U
#define PMU_LVMD_LV_MD_INTR_Msk                         (0x1U << PMU_LVMD_LV_MD_INTR_Pos)

/* CPR0 PMU_REQMDTRIG, Offset: 0x2C */
#define PMU_REQMDTRIG_CHANGEMODE_REQ_Pos                0U
#define PMU_REQMDTRIG_CHANGEMODE_REQ_Msk                (0x1U << PMU_REQMDTRIG_CHANGEMODE_REQ_Pos)
#define PMU_REQMDTRIG_CHANGEMODE_REQ                    PMU_REQMDTRIG_CHANGEMODE_REQ_Msk

/* CPR0 PMU_PERIERRSTA, Offset: 0x30 */
#define PMU_PERIERRSTA_USI0_ERR_FLAG_Pos                0U
#define PMU_PERIERRSTA_USI0_ERR_FLAG_Msk                (0x1U << PMU_PERIERRSTA_USI0_ERR_FLAG_Pos)
#define PMU_PERIERRSTA_USI0_ERR_FLAG                    PMU_PERIERRSTA_USI0_ERR_FLAG_Msk
#define PMU_PERIERRSTA_USI1_ERR_FLAG_Pos                1U
#define PMU_PERIERRSTA_USI1_ERR_FLAG_Msk                (0x1U << PMU_PERIERRSTA_USI1_ERR_FLAG_Pos)
#define PMU_PERIERRSTA_USI1_ERR_FLAG                    PMU_PERIERRSTA_USI1_ERR_FLAG_Msk
#define PMU_PERIERRSTA_USI2_ERR_FLAG_Pos                2U
#define PMU_PERIERRSTA_USI2_ERR_FLAG_Msk                (0x1U << PMU_PERIERRSTA_USI2_ERR_FLAG_Pos)
#define PMU_PERIERRSTA_USI2_ERR_FLAG                    PMU_PERIERRSTA_USI2_ERR_FLAG_Msk
#define PMU_PERIERRSTA_USI3_ERR_FLAG_Pos                3U
#define PMU_PERIERRSTA_USI3_ERR_FLAG_Msk                (0x1U << PMU_PERIERRSTA_USI3_ERR_FLAG_Pos)
#define PMU_PERIERRSTA_USI3_ERR_FLAG                    PMU_PERIERRSTA_USI3_ERR_FLAG_Msk
#define PMU_PERIERRSTA_I2S0_ERR_FLAG_Pos                4U
#define PMU_PERIERRSTA_I2S0_ERR_FLAG_Msk                (0x1U << PMU_PERIERRSTA_I2S0_ERR_FLAG_Pos)
#define PMU_PERIERRSTA_I2S0_ERR_FLAG                    PMU_PERIERRSTA_I2S0_ERR_FLAG_Msk
#define PMU_PERIERRSTA_I2S1_ERR_FLAG_Pos                5U
#define PMU_PERIERRSTA_I2S1_ERR_FLAG_Msk                (0x1U << PMU_PERIERRSTA_I2S1_ERR_FLAG_Pos)
#define PMU_PERIERRSTA_I2S1_ERR_FLAG                    PMU_PERIERRSTA_I2S1_ERR_FLAG_Msk
#define PMU_PERIERRSTA_I2S2_ERR_FLAG_Pos                6U
#define PMU_PERIERRSTA_I2S2_ERR_FLAG_Msk                (0x1U << PMU_PERIERRSTA_I2S2_ERR_FLAG_Pos)
#define PMU_PERIERRSTA_I2S2_ERR_FLAG                    PMU_PERIERRSTA_I2S2_ERR_FLAG_Msk
#define PMU_PERIERRSTA_I2S3_ERR_FLAG_Pos                7U
#define PMU_PERIERRSTA_I2S3_ERR_FLAG_Msk                (0x1U << PMU_PERIERRSTA_I2S3_ERR_FLAG_Pos)
#define PMU_PERIERRSTA_I2S3_ERR_FLAG                    PMU_PERIERRSTA_I2S3_ERR_FLAG_Msk
#define PMU_PERIERRSTA_PDM_ERR_FLAG_Pos                 8U
#define PMU_PERIERRSTA_PDM_ERR_FLAG_Msk                 (0x1U << PMU_PERIERRSTA_PDM_ERR_FLAG_Pos)
#define PMU_PERIERRSTA_PDM_ERR_FLAG                     PMU_PERIERRSTA_PDM_ERR_FLAG_Msk
#define PMU_PERIERRSTA_TDM_ERR_FLAG_Pos                 9U
#define PMU_PERIERRSTA_TDM_ERR_FLAG_Msk                 (0x1U << PMU_PERIERRSTA_TDM_ERR_FLAG_Pos)
#define PMU_PERIERRSTA_TDM_ERR_FLAG                     PMU_PERIERRSTA_TDM_ERR_FLAG_Msk
#define PMU_PERIERRSTA_SPDIF_ERR_FLAG_Pos               10U
#define PMU_PERIERRSTA_SPDIF_ERR_FLAG_Msk               (0x1U << PMU_PERIERRSTA_SPDIF_ERR_FLAG_Pos)
#define PMU_PERIERRSTA_SPDIF_ERR_FLAG                   PMU_PERIERRSTA_SPDIF_ERR_FLAG_Msk
#define PMU_PERIERRSTA_MCA_ERR_FLAG_Pos                 11U
#define PMU_PERIERRSTA_MCA_ERR_FLAG_Msk                 (0x1U << PMU_PERIERRSTA_MCA_ERR_FLAG_Pos)
#define PMU_PERIERRSTA_MCA_ERR_FLAG                     PMU_PERIERRSTA_MCA_ERR_FLAG_Msk
#define PMU_PERIERRSTA_GSK_ERR_FLAG_Pos                 12U
#define PMU_PERIERRSTA_GSK_ERR_FLAG_Msk                 (0x1U << PMU_PERIERRSTA_GSK_ERR_FLAG_Pos)
#define PMU_PERIERRSTA_GSK_ERR_FLAG                     PMU_PERIERRSTA_GSK_ERR_FLAG_Msk
/* CPR0 PMU_PLLINTRCTRL, Offset: 0x34 */
#define PMU_PLLINTRCTRL_PLL_LOSELOCK_EN_Pos             0U
#define PMU_PLLINTRCTRL_PLL_LOSELOCK_EN_Msk             (0x1U << PMU_PLLINTRCTRL_PLL_LOSELOCK_EN_Pos)
#define PMU_PLLINTRCTRL_PLL_LOSELOCK_EN                 PMU_PLLINTRCTRL_PLL_LOSELOCK_EN_Msk
#define PMU_PLLINTRCTRL_PLL_LOSELOCK_EN_FRAC_Pos        1U
#define PMU_PLLINTRCTRL_PLL_LOSELOCK_EN_FRAC_Msk        (0x1U << PMU_PLLINTRCTRL_PLL_LOSELOCK_EN_FRAC_Pos)
#define PMU_PLLINTRCTRL_PLL_LOSELOCK_EN_FRAC            PMU_PLLINTRCTRL_PLL_LOSELOCK_EN_FRAC_Msk
#define PMU_PLLINTRCTRL_PLL_LOSELOCK_CLR_Pos            2U
#define PMU_PLLINTRCTRL_PLL_LOSELOCK_CLR_Msk            (0x1U << PMU_PLLINTRCTRL_PLL_LOSELOCK_CLR_Pos)
#define PMU_PLLINTRCTRL_PLL_LOSELOCK_CLR                PMU_PLLINTRCTRL_PLL_LOSELOCK_CLR_Msk
#define PMU_PLLINTRCTRL_PLL_LOSELOCK_CLR_FRAC_Pos       3U
#define PMU_PLLINTRCTRL_PLL_LOSELOCK_CLR_FRAC_Msk       (0x1U << PMU_PLLINTRCTRL_PLL_LOSELOCK_CLR_FRAC_Pos)
#define PMU_PLLINTRCTRL_PLL_LOSELOCK_CLR_FRAC           PMU_PLLINTRCTRL_PLL_LOSELOCK_CLR_FRAC_Msk
#define PMU_PLLINTRCTRL_PLL_LOCK_EN_Pos                 4U
#define PMU_PLLINTRCTRL_PLL_LOCK_EN_Msk                 (0x1U << PMU_PLLINTRCTRL_PLL_LOCK_EN_Pos)
#define PMU_PLLINTRCTRL_PLL_LOCK_EN                     PMU_PLLINTRCTRL_PLL_LOCK_EN_Msk
#define PMU_PLLINTRCTRL_PLL_LOCK_EN_FRAC_Pos            5U
#define PMU_PLLINTRCTRL_PLL_LOCK_EN_FRAC_Msk            (0x1U << PMU_PLLINTRCTRL_PLL_LOCK_EN_FRAC_Pos)
#define PMU_PLLINTRCTRL_PLL_LOCK_EN_FRAC                PMU_PLLINTRCTRL_PLL_LOCK_EN_FRAC_Msk
#define PMU_PLLINTRCTRL_PLL_LOCK_CLR_Pos                6U
#define PMU_PLLINTRCTRL_PLL_LOCK_CLR_Msk                (0x1U << PMU_PLLINTRCTRL_PLL_LOCK_CLR_Pos)
#define PMU_PLLINTRCTRL_PLL_LOCK_CLR                    PMU_PLLINTRCTRL_PLL_LOCK_CLR_Msk
#define PMU_PLLINTRCTRL_PLL_LOCK_CLR_FRAC_Pos           7U
#define PMU_PLLINTRCTRL_PLL_LOCK_CLR_FRAC_Msk           (0x1U << PMU_PLLINTRCTRL_PLL_LOCK_CLR_FRAC_Pos)
#define PMU_PLLINTRCTRL_PLL_LOCK_CLR_FRAC               PMU_PLLINTRCTRL_PLL_LOCK_CLR_FRAC_Msk

/* CPR0 PMU_FRACPLLFRAC, Offset: 0x38 */
#define PMU_FRACPLLFRAC_FRAC_PLL_FRAC_Pos               0U
#define PMU_FRACPLLFRAC_FRAC_PLL_FRAC_Msk               (0xFFFFFFU << PMU_FRACPLLFRAC_FRAC_PLL_FRAC_Pos)
#define PMU_FRACPLLFRAC_FRAC_PLL_FRAC(n)                ((n) << PMU_FRACPLLFRAC_FRAC_PLL_FRAC_Pos)

/* CPR0 PMU_BOOTADDR_804_0, Offset: 0x40 */
#define PMU_BOOTADDR_804_0_Pos                          0U
#define PMU_BOOTADDR_804_0_Msk                          (0xFFFFFFFFU << PMU_BOOTADDR_804_0_Pos)

/* CPR0 PMU_BOOTADDR_805, Offset: 0x44 */
#define PMU_BOOTADDR_805_Pos                            27U
#define PMU_BOOTADDR_805_Msk                            (0x1FU << PMU_BOOTADDR_805_Pos)

/* CPR0 PMU_BOOTADDR_BYPASS, Offset: 0x48 */
#define PMU_BOOTADDR_BYPASS_Pos                         0U
#define PMU_BOOTADDR_BYPASS_Msk                         (0x1U << PMU_BOOTADDR_BYPASS_Pos)
#define PMU_BOOTADDR_BYPASS_EN                          PMU_BOOTADDR_BYPASS_Msk

/* CPR0 PMU_AONTIMCTRL, Offset: 0x4C */
#define PMU_AONTIMCTRL_PCLK_CG_EN_Pos                   0U
#define PMU_AONTIMCTRL_PCLK_CG_EN_Msk                   (0x1U << PMU_AONTIMCTRL_PCLK_CG_EN_Pos)
#define PMU_AONTIMCTRL_PCLK_CG_EN                       PMU_AONTIMCTRL_PCLK_CG_EN_Msk
#define PMU_AONTIMCTRL_CNT_CG_EN_Pos                    4U
#define PMU_AONTIMCTRL_CNT_CG_EN_Msk                    (0x1U << PMU_AONTIMCTRL_CNT_CG_EN_Pos)
#define PMU_AONTIMCTRL_CNT_CG_EN                        PMU_AONTIMCTRL_CNT_CG_EN_Msk
#define PMU_AONTIMCTRL_SERIE_SEL_Pos                    8U
#define PMU_AONTIMCTRL_SERIE_SEL_Msk                    (0x1U << PMU_AONTIMCTRL_SERIE_SEL_Pos)
#define PMU_AONTIMCTRL_SERIE_SEL                        PMU_AONTIMCTRL_SERIE_SEL_Msk

/* CPR0 PMU_AONTIMRST, Offset: 0x54 */
#define PMU_AONTIMRST_CNT0_RST_EN_Pos                   0U
#define PMU_AONTIMRST_CNT0_RST_EN_Msk                   (0x1U << PMU_AONTIMRST_CNT0_RST_EN_Pos)
#define PMU_AONTIMRST_CNT0_RST_EN                       PMU_AONTIMRST_CNT0_RST_EN_Msk
#define PMU_AONTIMRST_CNT1_RST_EN_Pos                   1U
#define PMU_AONTIMRST_CNT1_RST_EN_Msk                   (0x1U << PMU_AONTIMRST_CNT1_RST_EN_Pos)
#define PMU_AONTIMRST_CNT1_RST_EN                       PMU_AONTIMRST_CNT1_RST_EN_Msk
#define PMU_AONTIMRST_PCLK_RST_EN_Pos                   2U
#define PMU_AONTIMRST_PCLK_RST_EN_Msk                   (0x1U << PMU_AONTIMRST_PCLK_RST_EN_Pos)
#define PMU_AONTIMRST_PCLK_RST_EN                       PMU_AONTIMRST_PCLK_RST_EN_Msk

/* CPR0 PMU_AONRTCRST, Offset: 0x5C */
#define PMU_AONRTCRST_RTC_RST_EN_Pos                    0U
#define PMU_AONRTCRST_RTC_RST_EN_Msk                    (0x1U << PMU_AONRTCRST_RTC_RST_EN_Pos)
#define PMU_AONRTCRST_RTC_RST_EN                        PMU_AONRTCRST_RTC_RST_EN_Msk

/* CPR0 PMU_AONGPIORST, Offset: 0x64 */
#define PMU_AONGPIORST_AONGPIO_RST_EN_Pos               0U
#define PMU_AONGPIORST_AONGPIO_RST_EN_Msk               (0x1U << PMU_AONGPIORST_AONGPIO_RST_EN_Pos)
#define PMU_AONGPIORST_AONGPIO_RST_EN                   PMU_AONGPIORST_AONGPIO_RST_EN_Msk

/* CPR0 PMU_RTCCLKCTRL, Offset: 0x78 */
#define PMU_RTCCLKCTRL_RTC_32K_CG_EN_Pos                0U
#define PMU_RTCCLKCTRL_RTC_32K_CG_EN_Msk                (0x1U << PMU_RTCCLKCTRL_RTC_32K_CG_EN_Pos)
#define PMU_RTCCLKCTRL_RTC_32K_CG_EN                    PMU_RTCCLKCTRL_RTC_32K_CG_EN_Msk
#define PMU_RTCCLKCTRL_RTC_PCLK_CG_EN_Pos               4U
#define PMU_RTCCLKCTRL_RTC_PCLK_CG_EN_Msk               (0x1U << PMU_RTCCLKCTRL_RTC_PCLK_CG_EN_Pos)
#define PMU_RTCCLKCTRL_RTC_PCLK_CG_EN                   PMU_RTCCLKCTRL_RTC_PCLK_CG_EN_Msk

/* DLC_PCTRL, Offset: 0x200 */
#define PMU_DLC_PCTRL_START_Pos                         0U
#define PMU_DLC_PCTRL_START_Msk                         (1U << PMU_DLC_PCTRL_START_Pos)
#define PMU_DLC_PCTRL_START_EN                          PMU_DLC_PCTRL_START_Msk
#define PMU_DLC_PCTRL_PADDR_Pos                         1U
#define PMU_DLC_PCTRL_PADDR_Msk                         (0x3FFFU << PMU_DLC_PCTRL_PADDR_Pos)
#define PMU_DLC_PCTRL_PADDR(n)                          ((n) << PMU_DLC_PCTRL_PADDR_Pos)
#define PMU_DLC_PCTRL_DIR_Pos                           15U
#define PMU_DLC_PCTRL_DIR_Msk                           (1U << PMU_DLC_PCTRL_DIR_Pos)
#define PMU_DLC_PCTRL_DIR_EN                            PMU_DLC_PCTRL_DIR_Msk

/* DLC_PRDATA, Offset: 0x204 */
#define PMU_DLC_PRDATA_PRDATA_Pos                        0U
#define PMU_DLC_PRDATA_PRDATA_Msk                        (0xFFU << PMU_DLC_PRDATA_PRDATA_Pos)
#define PMU_DLC_PRDATA_PRDATA(n)                         ((n) << PMU_DLC_PRDATA_PRDATA_Pos)

/* DLC_SR, Offset: 0x208 */
#define PMU_DLC_SR_PICL_BUSY_Pos                         0U
#define PMU_DLC_SR_PICL_BUSY_Msk                         (1U << PMU_DLC_SR_PICL_BUSY_Pos)
#define PMU_DLC_SR_PICL_BUSY_EN                          PMU_DLC_SR_PICL_BUSY_Msk
#define PMU_DLC_SR_SCU_BUSY_Pos                          1U
#define PMU_DLC_SR_SCU_BUSY_Msk                          (1U << PMU_DLC_SR_SCU_BUSY_Pos)
#define PMU_DLC_SR_SCU_BUSY_EN                           PMU_DLC_SR_SCU_BUSY_Msk

/* DLC_IMR, Offset: 0x20C */
#define PMU_DLC_IMR_ICU_OK_MASK_Pos                      0U
#define PMU_DLC_IMR_ICU_OK_MASK_Msk                      (1U << PMU_DLC_IMR_ICU_OK_MASK_Pos)
#define PMU_DLC_IMR_ICU_OK_MASK_EN                       PMU_DLC_IMR_ICU_OK_MASK_Msk
#define PMU_DLC_IMR_ICU_DELAYED_MASK_Pos                 1U
#define PMU_DLC_IMR_ICU_DELAYED_MASK_Msk                 (1U << PMU_DLC_IMR_ICU_DELAYED_MASK_Pos)
#define PMU_DLC_IMR_ICU_DELAYED_MASK_EN                  PMU_DLC_IMR_ICU_DELAYED_MASK_Msk
#define PMU_DLC_IMR_ICU_MODE_CHANGED_MASK_Pos            2U
#define PMU_DLC_IMR_ICU_MODE_CHANGED_MASK_Msk            (1U << PMU_DLC_IMR_ICU_MODE_CHANGED_MASK_Pos)
#define PMU_DLC_IMR_ICU_MODE_CHANGED_MASK_EN             PMU_DLC_IMR_ICU_MODE_CHANGED_MASK_Msk
#define PMU_DLC_IMR_PICL_OK_MASK_Pos                     3U
#define PMU_DLC_IMR_PICL_OK_MASK_Msk                     (1U << PMU_DLC_IMR_PICL_OK_MASK_Pos)
#define PMU_DLC_IMR_PICL_OK_MASK_EN                      PMU_DLC_IMR_PICL_OK_MASK_Msk
#define PMU_DLC_IMR_SCU_OK_MASK_Pos                      4U
#define PMU_DLC_IMR_SCU_OK_MASK_Msk                      (1U << PMU_DLC_IMR_SCU_OK_MASK_Pos)
#define PMU_DLC_IMR_SCU_OK_MASK_EN                       PMU_DLC_IMR_SCU_OK_MASK_Msk

/* DLC_IFR, Offset: 0x210 */
#define PMU_DLC_IFR_ICU_OK_FLAG_Pos                      0U
#define PMU_DLC_IFR_ICU_OK_FLAG_Msk                      (1U << PMU_DLC_IFR_ICU_OK_FLAG_Pos)
#define PMU_DLC_IFR_ICU_OK_FLAG_EN                       PMU_DLC_IFR_ICU_OK_FLAG_Msk
#define PMU_DLC_IFR_ICU_DELAYED_FLAG_Pos                 1U
#define PMU_DLC_IFR_ICU_DELAYED_FLAG_Msk                 (1U << PMU_DLC_IFR_ICU_DELAYED_FLAG_Pos)
#define PMU_DLC_IFR_ICU_DELAYED_FLAG_EN                  PMU_DLC_IFR_ICU_DELAYED_FLAG_Msk
#define PMU_DLC_IFR_ICU_MODE_CHANGED_FLAG_Pos            2U
#define PMU_DLC_IFR_ICU_MODE_CHANGED_FLAG_Msk            (1U << PMU_DLC_IFR_ICU_MODE_CHANGED_FLAG_Pos)
#define PMU_DLC_IFR_ICU_MODE_CHANGED_FLAG_EN             PMU_DLC_IFR_ICU_MODE_CHANGED_FLAG_Msk
#define PMU_DLC_IFR_PICL_OK_FLAG_Pos                     3U
#define PMU_DLC_IFR_PICL_OK_FLAG_Msk                     (1U << PMU_DLC_IFR_PICL_OK_FLAG_Pos)
#define PMU_DLC_IFR_PICL_OK_FLAG_EN                      PMU_DLC_IFR_PICL_OK_FLAG_Msk
#define PMU_DLC_IFR_SCU_OK_FLAG_Pos                      4U
#define PMU_DLC_IFR_SCU_OK_FLAG_Msk                      (1U << PMU_DLC_IFR_SCU_OK_FLAG_Pos)
#define PMU_DLC_IFR_SCU_OK_FLAG_EN                       PMU_DLC_IFR_SCU_OK_FLAG_Msk

/* DLC_IOIFR, Offset: 0x214 */
#define PMU_DLC_IOIFR_ICU_OK_FLAGS_Pos                   1U
#define PMU_DLC_IOIFR_ICU_OK_FLAGS_Msk                   (0x7FFFFFFFU << PMU_DLC_IOIFR_ICU_OK_FLAGS_Pos)
#define PMU_DLC_IOIFR_ICU_OK_FLAGS(n)                    ((n) << PMU_DLC_IOIFR_ICU_OK_FLAGS_Pos)

/* DLC_IDIFR, Offset: 0x218 */
#define PMU_DLC_IDIFR_ICU_DELAYED_FLAGS_Pos              1U
#define PMU_DLC_IDIFR_ICU_DELAYED_FLAGS_Msk              (0x7FFFFFFFU << PMU_DLC_IDIFR_ICU_DELAYED_FLAGS_Pos)
#define PMU_DLC_IDIFR_ICU_DELAYED_FLAGS(n)               ((n) << PMU_DLC_IDIFR_ICU_DELAYED_FLAGS_Pos)

/* DLC_IMCIFR, Offset: 0x21C */
#define PMU_DLC_IMCIFR_ICU_MODE_CHANGED_FLAGS_Pos        1U
#define PMU_DLC_IMCIFR_ICU_MODE_CHANGED_FLAGS_Msk        (0x7FFFFFFFU << PMU_DLC_IMCIFR_ICU_MODE_CHANGED_FLAGS_Pos)
#define PMU_DLC_IMCIFR_ICU_MODE_CHANGED_FLAGS(n)         ((n) << PMU_DLC_IMCIFR_ICU_MODE_CHANGED_FLAGS_Pos)


/**
 * PMU_CLKSRCSEL
 * Offset: 0x000 (W/R) 32K clock source selection register
 * '1': Internal crystal
 * '0': external crystal
*/
static inline uint32_t wj_pmu_read_clksrc_sel_sta(wj_pmu_reg_cpr0_t *pmu_base)
{
    return (pmu_base->PMU_CLKSRCSEL & PMU_CLKSRCSEL_EILS_SEL_ILS);
}
static inline void wj_pmu_write_clksrc_sel_els(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->PMU_CLKSRCSEL &= ~PMU_CLKSRCSEL_EILS_SEL_ILS;
}
static inline void wj_pmu_write_clksrc_sel_ils(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->PMU_CLKSRCSEL |= PMU_CLKSRCSEL_EILS_SEL_ILS;
}

/**
 * PMU_SYSCLKDIVEN
 * Offset: 0x004 (W/R) System clock divider control register
*/
static inline uint32_t wj_pmu_read_sysclk_d0_div(wj_pmu_reg_cpr0_t *pmu_base)
{
    return (((pmu_base->PMU_SYSCLKDIVEN & PMU_SYSCLKDIVEN_D0_DIV_Msk) >> PMU_SYSCLKDIVEN_D0_DIV_Pos) + 2U);
}
static inline uint32_t wj_pmu_read_sysclk_d1_div(wj_pmu_reg_cpr0_t *pmu_base)
{
    return (((pmu_base->PMU_SYSCLKDIVEN & PMU_SYSCLKDIVEN_D1_DIV_Msk) >> PMU_SYSCLKDIVEN_D1_DIV_Pos) + 2U);
}
static inline uint32_t wj_pmu_read_sysclk_d2_div(wj_pmu_reg_cpr0_t *pmu_base)
{
    return (((pmu_base->PMU_SYSCLKDIVEN & PMU_SYSCLKDIVEN_D2_DIV_Msk) >> PMU_SYSCLKDIVEN_D2_DIV_Pos) + 2U);
}

static inline void wj_pmu_write_sysclk_pll_div(wj_pmu_reg_cpr0_t *pmu_base, uint32_t div)
{
    pmu_base->PMU_SYSCLKDIVEN = div;
}

/**
 * PMU_CPU12SWRST
 * Offset: 0x008 (W/R) CPU1, CPU2 reset control register
*/
static inline void wj_pmu_write_soft_reset(wj_pmu_reg_cpr0_t *pmu_base, uint32_t cpu, uint32_t val)
{
    if (1U == cpu) {
        pmu_base->PMU_CPU12SWRST = (val ? PMU_CPU12SWRST_CPU1_RST_EN : ~PMU_CPU12SWRST_CPU1_RST_EN);
    } else if (2U == cpu) {
        pmu_base->PMU_CPU12SWRST = (val ? PMU_CPU12SWRST_CPU2_RST_EN : ~PMU_CPU12SWRST_CPU2_RST_EN);
    }
}
static inline uint32_t wj_pmu_read_soft_reset(wj_pmu_reg_cpr0_t *pmu_base, uint32_t cpu)
{
    return (1U == cpu ? (pmu_base->PMU_CPU12SWRST & PMU_CPU12SWRST_CPU1_RST_EN_Msk) >> PMU_CPU12SWRST_CPU1_RST_EN_Pos : \
            (pmu_base->PMU_CPU12SWRST & PMU_CPU12SWRST_CPU2_RST_EN_Msk) >> PMU_CPU12SWRST_CPU2_RST_EN_Pos);
}

/**
 * PMU_PLLCTRL
 * Offset: 0x00C (W/R) System PLL control register
*/
static inline uint32_t wj_pmu_read_pll_lock_sta_sta(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->PMU_PLLCTRL & PMU_PLLCTRL_PLL_LOCK_STA_Msk) >> PMU_PLLCTRL_PLL_LOCK_STA_Pos);
}
static inline void wj_pmu_write_pll_bypass_en(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->PMU_PLLCTRL |= PMU_PLLCTRL_PLL_BYPASS_EN;
}
static inline void wj_pmu_write_pll_bypass_dis(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->PMU_PLLCTRL &= ~PMU_PLLCTRL_PLL_BYPASS_EN;
}
static inline uint32_t wj_pmu_read_pll_bypass_sta(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->PMU_PLLCTRL & PMU_PLLCTRL_PLL_BYPASS_Msk) >> PMU_PLLCTRL_PLL_BYPASS_Pos);
}
static inline void wj_pmu_write_pll_dacpd_en(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->PMU_PLLCTRL |= PMU_PLLCTRL_PLL_DACPD_EN;
}
static inline void wj_pmu_write_pll_dacpd_dis(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->PMU_PLLCTRL &= ~PMU_PLLCTRL_PLL_DACPD_EN;
}
static inline uint32_t wj_pmu_read_pll_dacpd_sta(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->PMU_PLLCTRL & PMU_PLLCTRL_PLL_DACPD_Msk) >> PMU_PLLCTRL_PLL_DACPD_Pos);
}
static inline void wj_pmu_write_pll_postdiv1(wj_pmu_reg_cpr0_t *pmu_base, uint32_t div)
{
    pmu_base->PMU_PLLCTRL &= ~PMU_PLLCTRL_PLL_POSTDIV1_Msk;
    pmu_base->PMU_PLLCTRL |= PMU_PLLCTRL_PLL_POSTDIV1(div);
}
static inline uint32_t wj_pmu_read_pll_postdiv1(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->PMU_PLLCTRL & PMU_PLLCTRL_PLL_POSTDIV1_Msk) >> PMU_PLLCTRL_PLL_POSTDIV1_Pos);
}
static inline void wj_pmu_write_pll_fout4phasepd_en(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->PMU_PLLCTRL |= PMU_PLLCTRL_PLL_FOUT4PHASEPD_EN;
}
static inline void wj_pmu_write_pll_fout4phasepd_dis(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->PMU_PLLCTRL &= ~PMU_PLLCTRL_PLL_FOUT4PHASEPD_EN;
}
static inline uint32_t wj_pmu_read_pll_fout4phasepd_sta(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->PMU_PLLCTRL & PMU_PLLCTRL_PLL_FOUT4PHASEPD_Msk) >> PMU_PLLCTRL_PLL_FOUT4PHASEPD_Pos);
}
static inline void wj_pmu_write_pll_postdiv2(wj_pmu_reg_cpr0_t *pmu_base, uint32_t div)
{
    pmu_base->PMU_PLLCTRL &= ~PMU_PLLCTRL_PLL_POSTDIV2_Msk;
    pmu_base->PMU_PLLCTRL |= PMU_PLLCTRL_PLL_POSTDIV2(div);
}
static inline uint32_t wj_pmu_read_pll_postdiv2(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->PMU_PLLCTRL & PMU_PLLCTRL_PLL_POSTDIV2_Msk) >> PMU_PLLCTRL_PLL_POSTDIV2_Pos);
}
static inline void wj_pmu_write_pll_foutpostdivpd_en(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->PMU_PLLCTRL |= PMU_PLLCTRL_PLL_FOUTPOSTDIVPD_EN;
}
static inline void wj_pmu_write_pll_foutpostdivpd_dis(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->PMU_PLLCTRL &= ~PMU_PLLCTRL_PLL_FOUTPOSTDIVPD_EN;
}
static inline uint32_t wj_pmu_read_pll_foutpostdivpd_sta(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->PMU_PLLCTRL & PMU_PLLCTRL_PLL_FOUTPOSTDIVPD_Msk) >> PMU_PLLCTRL_PLL_FOUTPOSTDIVPD_Pos);
}
static inline void wj_pmu_write_pll_refdiv(wj_pmu_reg_cpr0_t *pmu_base, uint32_t div)
{
    pmu_base->PMU_PLLCTRL &= ~PMU_PLLCTRL_PLL_REFDIV_Msk;
    pmu_base->PMU_PLLCTRL |= PMU_PLLCTRL_PLL_REFDIV_(div);
}
static inline uint32_t wj_pmu_read_pll_refdiv(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->PMU_PLLCTRL & PMU_PLLCTRL_PLL_REFDIV_Msk) >> PMU_PLLCTRL_PLL_REFDIV_Pos);
}
static inline void wj_pmu_write_pll_foutvcopd_en(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->PMU_PLLCTRL |= PMU_PLLCTRL_PLL_FOUTVCOPD_EN;
}
static inline void wj_pmu_write_pll_foutvcopd_dis(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->PMU_PLLCTRL &= ~PMU_PLLCTRL_PLL_FOUTVCOPD_EN;
}
static inline uint32_t wj_pmu_read_pll_foutvcopd_sta(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->PMU_PLLCTRL & PMU_PLLCTRL_PLL_FOUTVCOPD_Msk) >> PMU_PLLCTRL_PLL_FOUTVCOPD_Pos);
}
static inline void wj_pmu_write_pll_fbdiv(wj_pmu_reg_cpr0_t *pmu_base, uint32_t div)
{
    pmu_base->PMU_PLLCTRL &= ~PMU_PLLCTRL_PLL_FBDIV_Msk;
    pmu_base->PMU_PLLCTRL |= PMU_PLLCTRL_PLL_FBDIV(div);
}
static inline uint32_t wj_pmu_read_pll_fbdiv(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->PMU_PLLCTRL & PMU_PLLCTRL_PLL_FBDIV_Msk) >> PMU_PLLCTRL_PLL_FBDIV_Pos);
}
/**
 * PMU_PLLCTRL_FRAC
 * Offset: 0x010 (W/R) Fractional PLL control register
*/
static inline uint32_t wj_pmu_read_pll_lock_sta_frac_sta(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->PMU_PLLCTRL_FRAC & PMU_PLLCTRL_FRAC_PLL_LOCK_STA_Msk) >> PMU_PLLCTRL_FRAC_PLL_LOCK_STA_Pos);
}
static inline void wj_pmu_write_pll_bypass_frac_en(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->PMU_PLLCTRL_FRAC |= PMU_PLLCTRL_FRAC_PLL_BYPASS_EN;
}
static inline void wj_pmu_write_pll_bypass_frac_dis(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->PMU_PLLCTRL_FRAC &= ~PMU_PLLCTRL_FRAC_PLL_BYPASS_EN;
}
static inline uint32_t wj_pmu_read_pll_bypass_frac_sta(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->PMU_PLLCTRL_FRAC & PMU_PLLCTRL_FRAC_PLL_BYPASS_Msk) >> PMU_PLLCTRL_FRAC_PLL_BYPASS_Pos);
}
static inline void wj_pmu_write_pll_dacpd_frac_en(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->PMU_PLLCTRL_FRAC |= PMU_PLLCTRL_FRAC_PLL_DACPD_EN;
}
static inline void wj_pmu_write_pll_dacpd_frac_dis(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->PMU_PLLCTRL_FRAC &= ~PMU_PLLCTRL_FRAC_PLL_DACPD_EN;
}
static inline uint32_t wj_pmu_read_pll_dacpd_frac_sta(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->PMU_PLLCTRL_FRAC & PMU_PLLCTRL_FRAC_PLL_DACPD_Msk) >> PMU_PLLCTRL_FRAC_PLL_DACPD_Pos);
}
static inline void wj_pmu_write_pll_postdiv1_frac(wj_pmu_reg_cpr0_t *pmu_base, uint32_t div)
{
    pmu_base->PMU_PLLCTRL_FRAC &= ~PMU_PLLCTRL_FRAC_PLL_POSTDIV1_Msk;
    pmu_base->PMU_PLLCTRL_FRAC |= PMU_PLLCTRL_FRAC_PLL_POSTDIV1(div);
}
static inline uint32_t wj_pmu_read_pll_postdiv1_frac(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->PMU_PLLCTRL_FRAC & PMU_PLLCTRL_FRAC_PLL_POSTDIV1_Msk) >> PMU_PLLCTRL_FRAC_PLL_POSTDIV1_Pos);
}
static inline void wj_pmu_write_pll_fout4phasepd_frac_en(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->PMU_PLLCTRL_FRAC |= PMU_PLLCTRL_FRAC_PLL_FOUT4PHASEPD_EN;
}
static inline void wj_pmu_write_pll_fout4phasepd_frac_dis(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->PMU_PLLCTRL_FRAC &= ~PMU_PLLCTRL_FRAC_PLL_FOUT4PHASEPD_EN;
}
static inline uint32_t wj_pmu_read_pll_fout4phasepd_frac_sta(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->PMU_PLLCTRL_FRAC & PMU_PLLCTRL_FRAC_PLL_FOUT4PHASEPD_Msk) >> PMU_PLLCTRL_FRAC_PLL_FOUT4PHASEPD_Pos);
}
static inline void wj_pmu_write_pll_postdiv2_frac(wj_pmu_reg_cpr0_t *pmu_base, uint32_t div)
{
    pmu_base->PMU_PLLCTRL_FRAC &= ~PMU_PLLCTRL_FRAC_PLL_POSTDIV2_Msk;
    pmu_base->PMU_PLLCTRL_FRAC |= PMU_PLLCTRL_FRAC_PLL_POSTDIV2(div);
}
static inline uint32_t wj_pmu_read_pll_postdiv2_frac(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->PMU_PLLCTRL_FRAC & PMU_PLLCTRL_FRAC_PLL_POSTDIV2_Msk) >> PMU_PLLCTRL_FRAC_PLL_POSTDIV2_Pos);
}
static inline void wj_pmu_write_pll_foutpostdivpd_frac_en(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->PMU_PLLCTRL_FRAC |= PMU_PLLCTRL_FRAC_PLL_FOUTPOSTDIVPD_EN;
}
static inline void wj_pmu_write_pll_foutpostdivpd_frac_dis(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->PMU_PLLCTRL_FRAC &= ~PMU_PLLCTRL_FRAC_PLL_FOUTPOSTDIVPD_EN;
}
static inline uint32_t wj_pmu_read_pll_foutpostdivpd_frac_sta(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->PMU_PLLCTRL_FRAC & PMU_PLLCTRL_FRAC_PLL_FOUTPOSTDIVPD_Msk) >> PMU_PLLCTRL_FRAC_PLL_FOUTPOSTDIVPD_Pos);
}
static inline void wj_pmu_write_pll_refdiv_frac(wj_pmu_reg_cpr0_t *pmu_base, uint32_t div)
{
    pmu_base->PMU_PLLCTRL_FRAC &= ~PMU_PLLCTRL_FRAC_PLL_REFDIV_Msk;
    pmu_base->PMU_PLLCTRL_FRAC |= PMU_PLLCTRL_FRAC_PLL_REFDIV(div);
}
static inline uint32_t wj_pmu_read_pll_refdiv_frac(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->PMU_PLLCTRL_FRAC & PMU_PLLCTRL_FRAC_PLL_REFDIV_Msk) >> PMU_PLLCTRL_FRAC_PLL_REFDIV_Pos);
}
static inline void wj_pmu_write_pll_foutvcopd_frac_en(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->PMU_PLLCTRL_FRAC |= PMU_PLLCTRL_FRAC_PLL_FOUTVCOPD_EN;
}
static inline void wj_pmu_write_pll_foutvcopd_frac_dis(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->PMU_PLLCTRL_FRAC &= ~PMU_PLLCTRL_FRAC_PLL_FOUTVCOPD_EN;
}
static inline uint32_t wj_pmu_read_pll_foutvcopd_frac_sta(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->PMU_PLLCTRL_FRAC & PMU_PLLCTRL_FRAC_PLL_FOUTVCOPD_Msk) >> PMU_PLLCTRL_FRAC_PLL_FOUTVCOPD_Pos);
}
static inline void wj_pmu_write_pll_fbdiv_frac(wj_pmu_reg_cpr0_t *pmu_base, uint32_t div)
{
    pmu_base->PMU_PLLCTRL_FRAC &= ~PMU_PLLCTRL_FRAC_PLL_FBDIV_Msk;
    pmu_base->PMU_PLLCTRL_FRAC |= PMU_PLLCTRL_FRAC_PLL_FBDIV(div);
}
static inline uint32_t wj_pmu_read_pll_fbdiv_frac(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->PMU_PLLCTRL_FRAC & PMU_PLLCTRL_FRAC_PLL_FBDIV_Msk) >> PMU_PLLCTRL_FRAC_PLL_FBDIV_Pos);
}
/**
 * PMU_LPCR
 * Offset: 0x014 (W/R) Low power control register
*/
static inline void wj_pmu_write_lpen_en(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->PMU_LPCR |= PMU_LPCR_LPEN;
}
static inline void wj_pmu_write_lpen_dis(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->PMU_LPCR &= ~PMU_LPCR_LPEN;
}
static inline uint32_t wj_pmu_read_lpen_sta(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->PMU_LPCR & PMU_LPCR_LPEN_Msk) >> PMU_LPCR_LPEN_Pos);
}
static inline void wj_pmu_write_change_volt_en(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->PMU_LPCR |= PMU_LPCR_CHANGE_VOLT_ENTER_HW_VAD1;
}
static inline void wj_pmu_write_change_volt_dis(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->PMU_LPCR &= ~PMU_LPCR_CHANGE_VOLT_ENTER_HW_VAD1;
}
static inline uint32_t wj_pmu_read_change_volt_sta(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->PMU_LPCR & PMU_LPCR_CHANGE_VOLT_Msk) >> PMU_LPCR_CHANGE_VOLT_Pos);
}
static inline void wj_pmu_write_sw_trigger_en(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->PMU_LPCR |= PMU_LPCR_SW_TRIGGER;
}
static inline void wj_pmu_write_sw_trigger_dis(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->PMU_LPCR &= ~PMU_LPCR_SW_TRIGGER;
}
static inline uint32_t wj_pmu_read_sw_trigger_sta(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->PMU_LPCR & PMU_LPCR_SW_TRIGGER_Msk) >> PMU_LPCR_SW_TRIGGER_Pos);
}
static inline uint32_t wj_pmu_read_boot_from_lp_sta(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->PMU_LPCR & PMU_LPCR_BOOT_FROM_LP_Msk) >> PMU_LPCR_BOOT_FROM_LP_Pos);
}
/**
 * PMU_WKUPMASK
 * Offset: 0x018 (W/R) Wake-up mask register
*/
static inline void wj_pmu_write_gpio_mask_en(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->PMU_WKUPMASK |= PMU_WKUPMASK_GPIO_MASK_EN;
}
static inline void wj_pmu_write_gpio_mask_dis(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->PMU_WKUPMASK &= ~PMU_WKUPMASK_GPIO_MASK_EN;
}
static inline uint32_t wj_pmu_read_gpio_mask_sta(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->PMU_WKUPMASK & PMU_WKUPMASK_GPIO_MASK_EN_Msk) >> PMU_WKUPMASK_GPIO_MASK_EN_Pos);
}
static inline void wj_pmu_write_aontim_mask_en(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->PMU_WKUPMASK |= PMU_WKUPMASK_AONTIM_MASK_EN;
}
static inline void wj_pmu_write_aontim_mask_dis(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->PMU_WKUPMASK &= ~PMU_WKUPMASK_AONTIM_MASK_EN;
}
static inline uint32_t wj_pmu_read_aontim_mask_sta(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->PMU_WKUPMASK & PMU_WKUPMASK_AONTIM_MASK_EN_Msk) >> PMU_WKUPMASK_AONTIM_MASK_EN_Pos);
}
static inline void wj_pmu_write_rtc_mask_en(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->PMU_WKUPMASK |= PMU_WKUPMASK_RTC_MASK_EN;
}
static inline void wj_pmu_write_rtc_mask_dis(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->PMU_WKUPMASK &= ~PMU_WKUPMASK_RTC_MASK_EN;
}
static inline uint32_t wj_pmu_read_rtc_mask_sta(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->PMU_WKUPMASK & PMU_WKUPMASK_RTC_MASK_EN_Msk) >> PMU_WKUPMASK_RTC_MASK_EN_Pos);
}
static inline void wj_pmu_write_codec_wt_mask_en(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->PMU_WKUPMASK |= PMU_WKUPMASK_CODEC_WT_MASK_EN;
}
static inline void wj_pmu_write_codec_wt_mask_dis(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->PMU_WKUPMASK &= ~PMU_WKUPMASK_CODEC_WT_MASK_EN;
}
static inline uint32_t wj_pmu_read_codec_wt_mask_sta(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->PMU_WKUPMASK & PMU_WKUPMASK_CODEC_WT_MASK_EN_Msk) >> PMU_WKUPMASK_CODEC_WT_MASK_EN_Pos);
}
/**
 * PMU_RSTSTA
 * Offset: 0x01C (W/R) Reset status register
*/
static inline void wj_pmu_write_por_flag_en(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->PMU_RSTSTA |= PMU_RSTSTA_POR_RST;
}
static inline void wj_pmu_write_por_flag_dis(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->PMU_RSTSTA &= ~PMU_RSTSTA_POR_RST;
}
static inline uint32_t wj_pmu_read_por_flag_sta(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->PMU_RSTSTA & PMU_RSTSTA_POR_RST_FLAG_Msk) >> PMU_RSTSTA_POR_RST_FLAG_Pos);
}
static inline void wj_pmu_write_mcu_rst_flag_en(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->PMU_RSTSTA |= PMU_RSTSTA_MCU_RST;
}
static inline void wj_pmu_write_mcu_rst_flag_dis(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->PMU_RSTSTA &= ~PMU_RSTSTA_MCU_RST;
}
static inline uint32_t wj_pmu_read_mcu_rst_flag_sta(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->PMU_RSTSTA & PMU_RSTSTA_MCU_RST_FLAG_Msk) >> PMU_RSTSTA_MCU_RST_FLAG_Pos);
}
static inline void wj_pmu_write_wdg_rst_flag_en(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->PMU_RSTSTA |= PMU_RSTSTA_WDG_RST;
}
static inline void wj_pmu_write_wdg_rst_flag_dis(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->PMU_RSTSTA &= ~PMU_RSTSTA_WDG_RST;
}
static inline uint32_t wj_pmu_read_wdg_rst_flag_sta(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->PMU_RSTSTA & PMU_RSTSTA_WDG_RST_FLAG_Msk) >> PMU_RSTSTA_WDG_RST_FLAG_Pos);
}
static inline void wj_pmu_write_cpu0_srst_flag_en(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->PMU_RSTSTA |= PMU_RSTSTA_CPU0_RST;
}
static inline void wj_pmu_write_cpu0_srst_flag_dis(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->PMU_RSTSTA &= ~PMU_RSTSTA_CPU0_RST;
}
static inline uint32_t wj_pmu_read_cpu0_srst_flag_sta(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->PMU_RSTSTA & PMU_RSTSTA_CPU0_RST_FLAG_Msk) >> PMU_RSTSTA_CPU0_RST_FLAG_Pos);
}
/**
 * PMU_RCCTRL
 * Offset: 0x020 (W/R) Crystal Control Register
*/
static inline void wj_pmu_write_ils_rc_trim(wj_pmu_reg_cpr0_t *pmu_base, uint32_t div)
{
    pmu_base->PMU_RCCTRL &= ~PMU_RCCTRL_ILS_RC_TRIM_Msk;
    pmu_base->PMU_RCCTRL |= PMU_RCCTRL_ILS_RC_TRIM(div);
}
static inline uint32_t wj_pmu_read_ils_rc_trim(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->PMU_RCCTRL & PMU_RCCTRL_ILS_RC_TRIM_Msk) >> PMU_RCCTRL_ILS_RC_TRIM_Pos);
}
static inline void wj_pmu_write_ils_clk_en(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->PMU_RCCTRL &= ~PMU_RCCTRL_ILS_CLK_EN_Msk;
    pmu_base->PMU_RCCTRL |= PMU_RCCTRL_ILS_CLK_EN;
}
static inline void wj_pmu_write_ils_clk_dis_fast(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->PMU_RCCTRL &= ~PMU_RCCTRL_ILS_CLK_EN_Msk;
    pmu_base->PMU_RCCTRL |= PMU_RCCTRL_ILS_CLK_DIS_FAST;
}
static inline void wj_pmu_write_ils_clk_dis_slow(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->PMU_RCCTRL &= ~PMU_RCCTRL_ILS_CLK_EN_Msk;
    pmu_base->PMU_RCCTRL |= PMU_RCCTRL_ILS_CLK_DIS_SLOW;
}
static inline uint32_t wj_pmu_read_ils_clk_en(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->PMU_RCCTRL & PMU_RCCTRL_ILS_CLK_EN_Msk) >> PMU_RCCTRL_ILS_CLK_EN_Pos);
}
static inline void wj_pmu_write_els_clk_en(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->PMU_RCCTRL |= PMU_RCCTRL_ELS_CLK_EN;
}
static inline void wj_pmu_write_els_clk_dis(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->PMU_RCCTRL &= ~PMU_RCCTRL_ELS_CLK_EN;
}
static inline uint32_t wj_pmu_read_els_clk_sta(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->PMU_RCCTRL & PMU_RCCTRL_ELS_CLK_EN_Msk) >> PMU_RCCTRL_ELS_CLK_EN_Pos);
}
static inline void wj_pmu_write_els_clk_bypass_en(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->PMU_RCCTRL |= PMU_RCCTRL_ELS_CLK_BYPASS;
}
static inline void wj_pmu_write_els_clk_bypass_dis(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->PMU_RCCTRL &= ~PMU_RCCTRL_ELS_CLK_BYPASS;
}
static inline uint32_t wj_pmu_read_els_clk_bypass_sta(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->PMU_RCCTRL & PMU_RCCTRL_ELS_CLK_BYPASS_Msk) >> PMU_RCCTRL_ELS_CLK_BYPASS_Pos);
}
/**
 * PMU_LPSTA
 * Offset: 0x024 ( /R) Low power status register
*/
static inline uint32_t wj_pmu_read_cpu0_lp_sta(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->PMU_LPSTA & PMU_LPSTA_CPU0_LP_STA_Msk) >> PMU_LPSTA_CPU0_LP_STA_Pos);
}
static inline uint32_t wj_pmu_read_cpu1_lp_sta(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->PMU_LPSTA & PMU_LPSTA_CPU1_LP_STA_Msk) >> PMU_LPSTA_CPU1_LP_STA_Pos);
}
static inline uint32_t wj_pmu_read_cpu2_lp_sta(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->PMU_LPSTA & PMU_LPSTA_CPU2_LP_STA_Msk) >> PMU_LPSTA_CPU2_LP_STA_Pos);
}
/**
 * PMU_PERIERRSTA
 * Offset: 0x030 ( /R) Peripheral error register
*/
static inline uint32_t wj_pmu_read_usi0_err_flag(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->PMU_PERIERRSTA & PMU_PERIERRSTA_USI0_ERR_FLAG_Msk) >> PMU_PERIERRSTA_USI0_ERR_FLAG_Pos);
}
static inline uint32_t wj_pmu_read_usi1_err_flag(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->PMU_PERIERRSTA & PMU_PERIERRSTA_USI1_ERR_FLAG_Msk) >> PMU_PERIERRSTA_USI1_ERR_FLAG_Pos);
}
static inline uint32_t wj_pmu_read_usi2_err_flag(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->PMU_PERIERRSTA & PMU_PERIERRSTA_USI2_ERR_FLAG_Msk) >> PMU_PERIERRSTA_USI2_ERR_FLAG_Pos);
}
static inline uint32_t wj_pmu_read_usi3_err_flag(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->PMU_PERIERRSTA & PMU_PERIERRSTA_USI3_ERR_FLAG_Msk) >> PMU_PERIERRSTA_USI3_ERR_FLAG_Pos);
}
static inline uint32_t wj_pmu_read_i2s0_err_flag(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->PMU_PERIERRSTA & PMU_PERIERRSTA_I2S0_ERR_FLAG_Msk) >> PMU_PERIERRSTA_I2S0_ERR_FLAG_Pos);
}
static inline uint32_t wj_pmu_read_i2s1_err_flag(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->PMU_PERIERRSTA & PMU_PERIERRSTA_I2S1_ERR_FLAG_Msk) >> PMU_PERIERRSTA_I2S1_ERR_FLAG_Pos);
}
static inline uint32_t wj_pmu_read_i2s2_err_flag(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->PMU_PERIERRSTA & PMU_PERIERRSTA_I2S2_ERR_FLAG_Msk) >> PMU_PERIERRSTA_I2S2_ERR_FLAG_Pos);
}
static inline uint32_t wj_pmu_read_i2s3_err_flag(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->PMU_PERIERRSTA & PMU_PERIERRSTA_I2S3_ERR_FLAG_Msk) >> PMU_PERIERRSTA_I2S3_ERR_FLAG_Pos);
}
static inline uint32_t wj_pmu_read_pdm_err_flag(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->PMU_PERIERRSTA & PMU_PERIERRSTA_PDM_ERR_FLAG_Msk) >> PMU_PERIERRSTA_PDM_ERR_FLAG_Pos);
}
static inline uint32_t wj_pmu_read_tdm_err_flag(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->PMU_PERIERRSTA & PMU_PERIERRSTA_TDM_ERR_FLAG_Msk) >> PMU_PERIERRSTA_TDM_ERR_FLAG_Pos);
}
static inline uint32_t wj_pmu_read_spdif_err_flag(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->PMU_PERIERRSTA & PMU_PERIERRSTA_SPDIF_ERR_FLAG_Msk) >> PMU_PERIERRSTA_SPDIF_ERR_FLAG_Pos);
}
static inline uint32_t wj_pmu_read_mca_err_flag(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->PMU_PERIERRSTA & PMU_PERIERRSTA_MCA_ERR_FLAG_Msk) >> PMU_PERIERRSTA_MCA_ERR_FLAG_Pos);
}
static inline uint32_t wj_pmu_read_gsk_err_flag(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->PMU_PERIERRSTA & PMU_PERIERRSTA_GSK_ERR_FLAG_Msk) >> PMU_PERIERRSTA_GSK_ERR_FLAG_Pos);
}
/**
 * PMU_PLLINTRCTRL
 * Offset: 0x034 (W/R) PLL interrupt control register
*/
static inline void wj_pmu_write_pll_loselock_en(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->PMU_PLLINTRCTRL |= PMU_PLLINTRCTRL_PLL_LOSELOCK_EN;
}
static inline void wj_pmu_write_pll_loselock_dis(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->PMU_PLLINTRCTRL &= ~PMU_PLLINTRCTRL_PLL_LOSELOCK_EN;
}
static inline uint32_t wj_pmu_read_pll_loselock_sta(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->PMU_PLLINTRCTRL & PMU_PLLINTRCTRL_PLL_LOSELOCK_EN_Msk) >> PMU_PLLINTRCTRL_PLL_LOSELOCK_EN_Pos);
}
static inline void wj_pmu_write_pll_loselock_en_frac(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->PMU_PLLINTRCTRL |= PMU_PLLINTRCTRL_PLL_LOSELOCK_EN_FRAC;
}
static inline void wj_pmu_write_pll_loselock_dis_frac(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->PMU_PLLINTRCTRL &= ~PMU_PLLINTRCTRL_PLL_LOSELOCK_EN_FRAC;
}
static inline uint32_t wj_pmu_read_pll_loselock_en_frac_sta(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->PMU_PLLINTRCTRL & PMU_PLLINTRCTRL_PLL_LOSELOCK_EN_FRAC_Msk) >> PMU_PLLINTRCTRL_PLL_LOSELOCK_EN_FRAC_Pos);
}
static inline void wj_pmu_write_pll_loselock_clr_en(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->PMU_PLLINTRCTRL |= PMU_PLLINTRCTRL_PLL_LOSELOCK_CLR;
}
static inline void wj_pmu_write_pll_loselock_clr_dis(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->PMU_PLLINTRCTRL &= ~PMU_PLLINTRCTRL_PLL_LOSELOCK_CLR;
}
static inline uint32_t wj_pmu_read_pll_loselock_clr_sta(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->PMU_PLLINTRCTRL & PMU_PLLINTRCTRL_PLL_LOSELOCK_CLR_Msk) >> PMU_PLLINTRCTRL_PLL_LOSELOCK_CLR_Pos);
}
static inline void wj_pmu_write_pll_loselock_clr_frac_en(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->PMU_PLLINTRCTRL |= PMU_PLLINTRCTRL_PLL_LOSELOCK_CLR_FRAC;
}
static inline void wj_pmu_write_pll_loselock_clr_frac_dis(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->PMU_PLLINTRCTRL &= ~PMU_PLLINTRCTRL_PLL_LOSELOCK_CLR_FRAC;
}
static inline uint32_t wj_pmu_read_pll_loselock_clr_frac_sta(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->PMU_PLLINTRCTRL & PMU_PLLINTRCTRL_PLL_LOSELOCK_CLR_FRAC_Msk) >> PMU_PLLINTRCTRL_PLL_LOSELOCK_CLR_FRAC_Pos);
}
static inline void wj_pmu_write_pll_lock_en(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->PMU_PLLINTRCTRL |= PMU_PLLINTRCTRL_PLL_LOCK_EN;
}
static inline void wj_pmu_write_pll_lock_dis(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->PMU_PLLINTRCTRL &= ~PMU_PLLINTRCTRL_PLL_LOCK_EN;
}
static inline uint32_t wj_pmu_read_pll_lock_sta(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->PMU_PLLINTRCTRL & PMU_PLLINTRCTRL_PLL_LOCK_EN_Msk) >> PMU_PLLINTRCTRL_PLL_LOCK_EN_Pos);
}
static inline void wj_pmu_write_pll_lock_en_frac(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->PMU_PLLINTRCTRL |= PMU_PLLINTRCTRL_PLL_LOCK_EN_FRAC;
}
static inline void wj_pmu_write_pll_lock_dis_frac(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->PMU_PLLINTRCTRL &= ~PMU_PLLINTRCTRL_PLL_LOCK_EN_FRAC;
}
static inline uint32_t wj_pmu_read_pll_lock_en_frac_sta(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->PMU_PLLINTRCTRL & PMU_PLLINTRCTRL_PLL_LOCK_EN_FRAC_Msk) >> PMU_PLLINTRCTRL_PLL_LOCK_EN_FRAC_Pos);
}
static inline void wj_pmu_write_pll_lock_clr_en(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->PMU_PLLINTRCTRL |= PMU_PLLINTRCTRL_PLL_LOCK_CLR;
}
static inline void wj_pmu_write_pll_lock_clr_dis(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->PMU_PLLINTRCTRL &= ~PMU_PLLINTRCTRL_PLL_LOCK_CLR;
}
static inline uint32_t wj_pmu_read_pll_lock_clr_sta(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->PMU_PLLINTRCTRL & PMU_PLLINTRCTRL_PLL_LOCK_CLR_Msk) >> PMU_PLLINTRCTRL_PLL_LOCK_CLR_Pos);
}
static inline void wj_pmu_write_pll_lock_clr_frac_en(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->PMU_PLLINTRCTRL |= PMU_PLLINTRCTRL_PLL_LOCK_CLR_FRAC;
}
static inline void wj_pmu_write_pll_lock_clr_frac_dis(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->PMU_PLLINTRCTRL &= ~PMU_PLLINTRCTRL_PLL_LOCK_CLR_FRAC;
}
static inline uint32_t wj_pmu_read_pll_lock_clr_frac_sta(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->PMU_PLLINTRCTRL & PMU_PLLINTRCTRL_PLL_LOCK_CLR_FRAC_Msk) >> PMU_PLLINTRCTRL_PLL_LOCK_CLR_FRAC_Pos);
}
/**
 * PMU_FRACPLLFRAC
 * Offset:0x038 (W/R) Fractional PLL, FRAC parameter configuration register
*/
static inline void wj_pmu_write_frac_pll_frac(wj_pmu_reg_cpr0_t *pmu_base, uint32_t para)
{
    pmu_base->PMU_FRACPLLFRAC &= ~PMU_FRACPLLFRAC_FRAC_PLL_FRAC_Msk;
    pmu_base->PMU_FRACPLLFRAC |= PMU_FRACPLLFRAC_FRAC_PLL_FRAC(para);
}
static inline uint32_t wj_pmu_read_frac_pll_frac(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->PMU_FRACPLLFRAC & PMU_FRACPLLFRAC_FRAC_PLL_FRAC_Msk) >> PMU_FRACPLLFRAC_FRAC_PLL_FRAC_Pos);
}
/**
 * PMU_BOOTADDR_804_0
 * Offset: 0x040 (W/R) CPU0 boot address register
*/
static inline void wj_pmu_write_booaddr_804_0(wj_pmu_reg_cpr0_t *pmu_base, uint32_t addr)
{
    pmu_base->PMU_BOOTADDR_804_0 = addr;
}
static inline uint32_t wj_pmu_read_bootaddr_804_0(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->PMU_BOOTADDR_804_0 & PMU_BOOTADDR_804_0_Msk) >> PMU_BOOTADDR_804_0_Pos);
}
/**
 * PMU_BOOTADDR_805
 * Offset: 0x044 (W/R) CPU2 boot address register
*/
static inline void wj_pmu_write_booaddr_805(wj_pmu_reg_cpr0_t *pmu_base, uint32_t addr)
{
    pmu_base->PMU_BOOTADDR_805 = addr;
}

static inline uint32_t wj_pmu_read_bootaddr_805(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->PMU_BOOTADDR_805 & PMU_BOOTADDR_805_Msk) >> PMU_BOOTADDR_805_Pos);
}
/**
 * PMU_BOOTADDR_BYPASS
 * Offset: 0x048 (W/R) CPU boot address mask register
*/
static inline void wj_pmu_write_bootaddr_bypass_en(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->PMU_BOOTADDR_BYPASS |= PMU_BOOTADDR_BYPASS_EN;
}
static inline void wj_pmu_write_bootaddr_bypass_dis(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->PMU_BOOTADDR_BYPASS &= ~PMU_BOOTADDR_BYPASS_EN;
}
static inline uint32_t wj_pmu_read_bootaddr_bypass_sta(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->PMU_BOOTADDR_BYPASS & PMU_BOOTADDR_BYPASS_Msk) >> PMU_BOOTADDR_BYPASS_Pos);
}
/**
 * PMU_AONTIMCTRL
 * Offset: 0x04C (W/R) Low-power counter control register
*/
static inline void wj_pmu_write_lptim_pclk_cg_en(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->PMU_AONTIMCTRL |= PMU_AONTIMCTRL_PCLK_CG_EN;
}
static inline void wj_pmu_write_lptim_pclk_cg_dis(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->PMU_AONTIMCTRL &= ~PMU_AONTIMCTRL_PCLK_CG_EN;
}
static inline uint32_t wj_pmu_read_lptim_pclk_cg_sta(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->PMU_AONTIMCTRL & PMU_AONTIMCTRL_PCLK_CG_EN_Msk) >> PMU_AONTIMCTRL_PCLK_CG_EN_Pos);
}
static inline void wj_pmu_write_lptim_cnt_cg_en(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->PMU_AONTIMCTRL |= PMU_AONTIMCTRL_CNT_CG_EN;
}
static inline void wj_pmu_write_ptim_cnt_cg_dis(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->PMU_AONTIMCTRL &= ~PMU_AONTIMCTRL_CNT_CG_EN;
}
static inline uint32_t wj_pmu_read_ptim_cnt_cg_sta(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->PMU_AONTIMCTRL & PMU_AONTIMCTRL_CNT_CG_EN_Msk) >> PMU_AONTIMCTRL_CNT_CG_EN_Pos);
}
static inline void wj_pmu_write_lptim_serie_sel_en(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->PMU_AONTIMCTRL |= PMU_AONTIMCTRL_SERIE_SEL;
}
static inline void wj_pmu_write_lptim_serie_sel_dis(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->PMU_AONTIMCTRL &= ~PMU_AONTIMCTRL_SERIE_SEL;
}
static inline uint32_t wj_pmu_read_lptim_serie_sel_sta(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->PMU_AONTIMCTRL & PMU_AONTIMCTRL_SERIE_SEL_Msk) >> PMU_AONTIMCTRL_SERIE_SEL_Pos);
}
/**
 * PMU_AONTIMRST
 * Offset: 0x054 (W/R) Low power counter reset register
*/
static inline void wj_pmu_write_lptimcnt0_rst_en(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->PMU_AONTIMRST |= PMU_AONTIMRST_CNT0_RST_EN;
}
static inline void wj_pmu_write_lptimcnt0_rst_dis(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->PMU_AONTIMRST &= ~PMU_AONTIMRST_CNT0_RST_EN;
}
static inline uint32_t wj_pmu_read_lptimcnt0_rst_sta(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->PMU_AONTIMRST & PMU_AONTIMRST_CNT0_RST_EN_Msk) >> PMU_AONTIMRST_CNT0_RST_EN_Pos);
}
static inline void wj_pmu_write_lptimcnt1_rst_en(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->PMU_AONTIMRST |= PMU_AONTIMRST_CNT1_RST_EN;
}
static inline void wj_pmu_write_lptimcnt1_rst_dis(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->PMU_AONTIMRST &= ~PMU_AONTIMRST_CNT1_RST_EN;
}
static inline uint32_t wj_pmu_read_lptimcnt1_rst_sta(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->PMU_AONTIMRST & PMU_AONTIMRST_CNT1_RST_EN_Msk) >> PMU_AONTIMRST_CNT1_RST_EN_Pos);
}
static inline void wj_pmu_write_lptim_pclk_rst_en(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->PMU_AONTIMRST |= PMU_AONTIMRST_PCLK_RST_EN;
}
static inline void wj_pmu_write_lptim_pclk_rst_dis(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->PMU_AONTIMRST &= ~PMU_AONTIMRST_PCLK_RST_EN;
}
static inline uint32_t wj_pmu_read_lptim_pclk_rst_sta(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->PMU_AONTIMRST & PMU_AONTIMRST_PCLK_RST_EN_Msk) >> PMU_AONTIMRST_PCLK_RST_EN_Pos);
}
/**
 * PMU_AONRTCRST
 * Offset: 0x05C (W/R) RTC reset register
*/
static inline void wj_pmu_write_rtc_rst_en(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->PMU_AONRTCRST |= PMU_AONRTCRST_RTC_RST_EN;
}
static inline void wj_pmu_write_rtc_rst_dis(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->PMU_AONRTCRST &= ~PMU_AONRTCRST_RTC_RST_EN;
}
static inline uint32_t wj_pmu_read_rtc_rst_sta(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->PMU_AONRTCRST & PMU_AONRTCRST_RTC_RST_EN_Msk) >> PMU_AONRTCRST_RTC_RST_EN_Pos);
}
/**
 * PMU_AONGPIORST
 * Offset: 0x064 (W/R) GPIO0 reset register
*/
static inline void wj_pmu_write_aongpio_rst_en(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->PMU_AONGPIORST |= PMU_AONGPIORST_AONGPIO_RST_EN;
}
static inline void wj_pmu_write_aongpio_rst_dis(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->PMU_AONGPIORST &= ~PMU_AONGPIORST_AONGPIO_RST_EN;
}
static inline uint32_t wj_pmu_read_aongpio_rst_sta(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->PMU_AONGPIORST & PMU_AONGPIORST_AONGPIO_RST_EN_Msk) >> PMU_AONGPIORST_AONGPIO_RST_EN_Pos);
}
/**
 * PMU_LPBOOTADDR_804_0
 * Offset: 0x068 (W/R) CPU0 low power boot register
*/
static inline void wj_pmu_write_lpbootaddr_804_0(wj_pmu_reg_cpr0_t *pmu_base, uint32_t addr)
{
    pmu_base->PMU_LPBOOTADDR_804_0 = addr;
}
static inline uint32_t wj_pmu_read_lpbootaddr_804_0(wj_pmu_reg_cpr0_t *pmu_base)
{
    return (pmu_base->PMU_LPBOOTADDR_804_0);
}
/**
 * PMU_LPBOOTADDR_805
 * Offset: 0x06C (W/R) CPU2 low power boot register
*/
static inline void wj_pmu_write_lpbootaddr_805(wj_pmu_reg_cpr0_t *pmu_base, uint32_t addr)
{
    pmu_base->PMU_LPBOOTADDR_805 = addr;
}
static inline uint32_t wj_pmu_read_lpbootaddr_805(wj_pmu_reg_cpr0_t *pmu_base)
{
    return (pmu_base->PMU_LPBOOTADDR_805);
}
/**
 * PMU_QSPI_CNT
 * Offset: 0x070 (W/R) Wait for QSPI prefetch end counter
*/
static inline void wj_pmu_write_qspi_cnt_end(wj_pmu_reg_cpr0_t *pmu_base, uint32_t cnt)
{
    pmu_base->PMU_QSPI_CNT = cnt;
}
static inline uint32_t wj_pmu_read_qspi_cnt_end(wj_pmu_reg_cpr0_t *pmu_base)
{
    return (pmu_base->PMU_QSPI_CNT);
}
/**
 * PMU_WAIT_12M
 * Offset: 0x074 (W/R) Wait for 12M crystal oscillator to stabilize counter
*/
static inline void wj_pmu_write_value_10ms(wj_pmu_reg_cpr0_t *pmu_base, uint32_t delay)
{
    pmu_base->PMU_WAIT_12M = delay;
}
static inline uint32_t wj_pmu_read_value_10ms(wj_pmu_reg_cpr0_t *pmu_base)
{
    return (pmu_base->PMU_WAIT_12M);
}
/**
 * PMU_RTCCLKCTRL
 * Offset: 0x078 (W/R) RTC clock control register
*/
static inline void wj_pmu_write_rtc_32k_cg_en(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->PMU_RTCCLKCTRL |= PMU_RTCCLKCTRL_RTC_32K_CG_EN;
}
static inline void wj_pmu_write_rtc_32k_cg_dis(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->PMU_RTCCLKCTRL &= ~PMU_RTCCLKCTRL_RTC_32K_CG_EN;
}
static inline uint32_t wj_pmu_read_rtc_32k_cg_sta(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->PMU_RTCCLKCTRL & PMU_RTCCLKCTRL_RTC_32K_CG_EN_Msk) >> PMU_RTCCLKCTRL_RTC_32K_CG_EN_Pos);
}
static inline void wj_pmu_write_rtc_pclk_cg_en(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->PMU_RTCCLKCTRL |= PMU_RTCCLKCTRL_RTC_PCLK_CG_EN;
}
static inline void wj_pmu_write_rtc_pclk_cg_dis(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->PMU_RTCCLKCTRL &= ~PMU_RTCCLKCTRL_RTC_PCLK_CG_EN;
}
static inline uint32_t wj_pmu_read_rtc_pclk_cg_sta(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->PMU_RTCCLKCTRL & PMU_RTCCLKCTRL_RTC_PCLK_CG_EN_Msk) >> PMU_RTCCLKCTRL_RTC_PCLK_CG_EN_Pos);
}
/**
 * PMU_BOOTADDR_804_1
 * Offset: 0x080 (W/R) CPU1 boot address register
*/
static inline void wj_pmu_write_booaddr_804_1(wj_pmu_reg_cpr0_t *pmu_base, uint32_t addr)
{
    pmu_base->PMU_BOOTADDR_804_1 = addr;
}
static inline uint32_t wj_pmu_read_booaddr_804_1(wj_pmu_reg_cpr0_t *pmu_base)
{
    return (pmu_base->PMU_BOOTADDR_804_1);
}
/**
 * PMU_LP_BOOTADDR_804_1
 * Offset: 0x084 (W/R) CPU1 low power boot register
*/
static inline void wj_pmu_write_lpbootaddr_804_1(wj_pmu_reg_cpr0_t *pmu_base, uint32_t addr)
{
    pmu_base->PMU_LP_BOOTADDR_804_1 = addr;
}
static inline uint32_t wj_pmu_read_lpbootaddr_804_1(wj_pmu_reg_cpr0_t *pmu_base)
{
    return (pmu_base->PMU_LP_BOOTADDR_804_1);
}
/**
 * DLC_PCTRL
 * Offset: 0x200 (W/R) DLC bus control register
*/
static inline void wj_pmu_write_dlc_pctrl_start_en(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->DLC_PCTRL |= PMU_DLC_PCTRL_START_EN;
}
static inline void wj_pmu_write_dlc_pctrl_start_dis(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->DLC_PCTRL &= ~PMU_DLC_PCTRL_START_EN;
}
static inline uint32_t wj_pmu_read_dlc_pctrl_start_sta(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->DLC_PCTRL & PMU_DLC_PCTRL_START_Msk) >> PMU_DLC_PCTRL_START_Pos);
}
static inline void wj_pmu_write_dlc_pctrl_paddr(wj_pmu_reg_cpr0_t *pmu_base, uint32_t addr)
{
    pmu_base->DLC_PCTRL &= ~PMU_DLC_PCTRL_PADDR_Msk;
    pmu_base->DLC_PCTRL |= PMU_DLC_PCTRL_PADDR(addr);
}
static inline uint32_t wj_pmu_read_dlc_pctrl_paddr(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->DLC_PCTRL & PMU_DLC_PCTRL_PADDR_Msk) >> PMU_DLC_PCTRL_PADDR_Pos);
}
static inline void wj_pmu_write_dlc_pctrl_dir_en(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->DLC_PCTRL |= PMU_DLC_PCTRL_DIR_EN;
}
static inline void wj_pmu_write_dlc_pctrl_dir_dis(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->DLC_PCTRL &= ~PMU_DLC_PCTRL_DIR_EN;
}
static inline uint32_t wj_pmu_read_dlc_pctrl_dir_sta(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->DLC_PCTRL & PMU_DLC_PCTRL_DIR_Msk) >> PMU_DLC_PCTRL_DIR_Pos);
}
/**
 * DLC_PRDATA
 * Offset: 0x204 (W/R) DLC bus read data register
*/
static inline void wj_pmu_write_dlc_prdata_paddr(wj_pmu_reg_cpr0_t *pmu_base, uint32_t addr)
{
    pmu_base->DLC_PRDATA &= ~PMU_DLC_PRDATA_PRDATA_Msk;
    pmu_base->DLC_PRDATA |= PMU_DLC_PRDATA_PRDATA(addr);
}
static inline uint32_t wj_pmu_read_dlc_prdata_paddr(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->DLC_PRDATA & PMU_DLC_PRDATA_PRDATA_Msk) >> PMU_DLC_PRDATA_PRDATA_Pos);
}
/**
 * DLC_SR
 * Offset: 0x208 (W/R) LC bus status register
*/
static inline uint32_t wj_pmu_read_picl_busy_sta(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->DLC_SR & PMU_DLC_SR_PICL_BUSY_Msk) >> PMU_DLC_SR_PICL_BUSY_Pos);
}
static inline uint32_t wj_pmu_read_scu_busy_sta(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->DLC_SR & PMU_DLC_SR_SCU_BUSY_Msk) >> PMU_DLC_SR_SCU_BUSY_Pos);
}
/**
 * DLC_IMR
 * Offset: 0x20C (W/R) MAESTRO interrupt mask register
*/
static inline void wj_pmu_write_icu_ok_mask_en(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->DLC_IMR |= PMU_DLC_IMR_ICU_OK_MASK_EN;
}
static inline void wj_pmu_write_icu_ok_mask_dis(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->DLC_IMR &= ~PMU_DLC_IMR_ICU_OK_MASK_EN;
}
static inline uint32_t wj_pmu_read_icu_ok_mask_sta(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->DLC_IMR & PMU_DLC_IMR_ICU_OK_MASK_Msk) >> PMU_DLC_IMR_ICU_OK_MASK_Pos);
}
static inline void wj_pmu_write_icu_delayed_mask_en(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->DLC_IMR |= PMU_DLC_IMR_ICU_DELAYED_MASK_EN;
}
static inline void wj_pmu_write_icu_delayed_mask_dis(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->DLC_IMR &= ~PMU_DLC_IMR_ICU_DELAYED_MASK_EN;
}
static inline uint32_t wj_pmu_read_icu_delayed_mask_sta(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->DLC_IMR & PMU_DLC_IMR_ICU_DELAYED_MASK_Msk) >> PMU_DLC_IMR_ICU_DELAYED_MASK_Pos);
}
static inline void wj_pmu_write_icu_mode_changed_mask_en(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->DLC_IMR |= PMU_DLC_IMR_ICU_MODE_CHANGED_MASK_EN;
}
static inline void wj_pmu_write_icu_mode_changed_mask_dis(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->DLC_IMR &= ~PMU_DLC_IMR_ICU_MODE_CHANGED_MASK_EN;
}
static inline uint32_t wj_pmu_read_icu_mode_changed_mask_sta(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->DLC_IMR & PMU_DLC_IMR_ICU_MODE_CHANGED_MASK_Msk) >> PMU_DLC_IMR_ICU_MODE_CHANGED_MASK_Pos);
}
static inline void wj_pmu_write_picl_ok_mask_en(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->DLC_IMR |= PMU_DLC_IMR_PICL_OK_MASK_EN;
}
static inline void wj_pmu_write_picl_ok_mask_dis(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->DLC_IMR &= ~PMU_DLC_IMR_PICL_OK_MASK_EN;
}
static inline uint32_t wj_pmu_read_picl_ok_mask_sta(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->DLC_IMR & PMU_DLC_IMR_PICL_OK_MASK_Msk) >> PMU_DLC_IMR_PICL_OK_MASK_Pos);
}
static inline void wj_pmu_write_ok_mask_en(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->DLC_IMR |= PMU_DLC_IMR_SCU_OK_MASK_EN;
}
static inline void wj_pmu_write_ok_mask_dis(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->DLC_IMR &= ~PMU_DLC_IMR_SCU_OK_MASK_EN;
}
static inline uint32_t wj_pmu_read_ok_mask_sta(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->DLC_IMR & PMU_DLC_IMR_SCU_OK_MASK_Msk) >> PMU_DLC_IMR_SCU_OK_MASK_Pos);
}

/**
 * DLC_IFR
 * Offset: 0x210 (W/R) MAESTRO interrupt status register
*/
static inline uint32_t wj_pmu_read_icu_ok_flag_sta(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->DLC_IFR & PMU_DLC_IFR_ICU_OK_FLAG_Msk) >> PMU_DLC_IFR_ICU_OK_FLAG_Pos);
}
static inline uint32_t wj_pmu_read_icu_delayed_flag_sta(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->DLC_IFR & PMU_DLC_IFR_ICU_DELAYED_FLAG_Msk) >> PMU_DLC_IFR_ICU_DELAYED_FLAG_Pos);
}
static inline uint32_t wj_pmu_read_icu_mode_changed_flag_sta(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->DLC_IFR & PMU_DLC_IFR_ICU_MODE_CHANGED_FLAG_Msk) >> PMU_DLC_IFR_ICU_MODE_CHANGED_FLAG_Pos);
}
static inline void wj_pmu_write_picl_ok_flag_en(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->DLC_IFR |= PMU_DLC_IFR_PICL_OK_FLAG_EN;
}
static inline void wj_pmu_write_picl_ok_flag_dis(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->DLC_IFR &= ~PMU_DLC_IFR_PICL_OK_FLAG_EN;
}
static inline uint32_t wj_pmu_read_picl_ok_flag_sta(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->DLC_IFR & PMU_DLC_IFR_PICL_OK_FLAG_Msk) >> PMU_DLC_IFR_PICL_OK_FLAG_Pos);
}
static inline void wj_pmu_write_ok_flag_en(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->DLC_IFR |= PMU_DLC_IFR_SCU_OK_FLAG_EN;
}
static inline void wj_pmu_write_scu_ok_flag_dis(wj_pmu_reg_cpr0_t *pmu_base)
{
    pmu_base->DLC_IFR &= ~PMU_DLC_IFR_SCU_OK_FLAG_EN;
}
static inline uint32_t wj_pmu_read_scu_ok_flag_sta(wj_pmu_reg_cpr0_t *pmu_base)
{
    return ((pmu_base->DLC_IFR & PMU_DLC_IFR_SCU_OK_FLAG_Msk) >> PMU_DLC_IFR_SCU_OK_FLAG_Pos);
}

/**
 * DLC_IOIFR
 * Offset: 0x214 (W/R) ICU_OK status register
*/
static inline uint32_t wj_pmu_read_icu_ok_flags_sta(wj_pmu_reg_cpr0_t *pmu_base, uint32_t bits)
{
    return ((pmu_base->DLC_IFR & PMU_DLC_IOIFR_ICU_OK_FLAGS(bits)) >> PMU_DLC_IOIFR_ICU_OK_FLAGS_Pos);
}

/**
 * DLC_IDIFR
 * Offset: 0x218 (W/R) ICU_DELAY status register
*/
static inline uint32_t wj_pmu_read_icu_delayed_flags_sta(wj_pmu_reg_cpr0_t *pmu_base, uint32_t bits)
{
    return ((pmu_base->DLC_IDIFR & PMU_DLC_IDIFR_ICU_DELAYED_FLAGS(bits)) >> PMU_DLC_IDIFR_ICU_DELAYED_FLAGS_Pos);
}

/**
 * DLC_IMCIFR
 * Offset: 0x21C (W/R) ICU_MODE_CHANGED status register
*/
static inline uint32_t wj_pmu_read_icu_mode_changed_flags_sta(wj_pmu_reg_cpr0_t *pmu_base, uint32_t bits)
{
    return ((pmu_base->DLC_IDIFR & PMU_DLC_IMCIFR_ICU_MODE_CHANGED_FLAGS(bits)) >> PMU_DLC_IMCIFR_ICU_MODE_CHANGED_FLAGS_Pos);
}


/* CPR1 PMU_SYSCLK0_SEL_REG, Offset: 0x0 */
#define CPR1_SYSCLK0_SEL_Pos                0U
#define CPR1_SYSCLK0_SEL_Msk                (0x1U << CPR1_SYSCLK0_SEL_Pos)
#define CPR1_SYSCLK0_SEL_12M                CPR1_SYSCLK0_SEL_Msk
#define CPR1_SYSCLK0_SEL_D0                 (~CPR1_SYSCLK0_SEL_Msk)

/* CPR1 SYSCLK0_DIV_CTL_REG, Offset: 0x4 */
#define CPR1_SYS_CLK_1_DIV_Pos              0U
#define CPR1_SYS_CLK_1_DIV_Msk              (0x3U << CPR1_SYS_CLK_1_DIV_Pos)
#define CPR1_SYS_CLK_1_DIV(n)               ((n / 2U) << CPR1_SYS_CLK_1_DIV_Pos)
#define CPR1_SYS_CLK_2_DIV_Pos              4U
#define CPR1_SYS_CLK_2_DIV_Msk              (0x3U << CPR1_SYS_CLK_2_DIV_Pos)
#define CPR1_SYS_CLK_2_DIV(n)               ((n / 2U) << CPR1_SYS_CLK_2_DIV_Pos)
#define CPR1_APB0_CLK_DIV_Pos               8U
#define CPR1_APB0_CLK_DIV_Msk               (0xFU << CPR1_APB0_CLK_DIV_Pos)
#define CPR1_APB0_CLK_DIV(n)                ((n - 1U) << CPR1_APB0_CLK_DIV_Pos)

/* CPR1 FFT_CTL_REG, Offset: 0xC */
#define CPR1_FFT_CG_Pos                     0U
#define CPR1_FFT_CG_Msk                     (0x1U << CPR1_FFT_CG_Pos)
#define CPR1_FFT_CG_EN                      CPR1_FFT_CG_Msk
#define CPR1_FFT_BUSY_Pos                   4U
#define CPR1_FFT_BUSY_Msk                   (0x1U << CPR1_FFT_BUSY_Pos)

/* CPR1 TIM1_CTL_REG, Offset: 0x10 */
#define CPR1_TIM1_CG_Pos                     0U
#define CPR1_TIM1_CG_Msk                     (0x1U << CPR1_TIM1_CG_Pos)
#define CPR1_TIM1_CG_EN                      CPR1_TIM1_CG_Msk
#define CPR1_TIM1_SERIE_SEL_Pos              4U
#define CPR1_TIM1_SERIE_SEL_Msk              (0x1U << CPR1_TIM1_SERIE_SEL_Pos)
#define CPR1_TIM1_SERIE_SEL_EN               CPR1_TIM1_SERIE_SEL_Msk
#define CPR1_TIM1_BUSY_Pos                   8U
#define CPR1_TIM1_BUSY_Msk                   (0x1U << CPR1_TIM1_BUSY_Pos)

/* CPR1 FMC_AHB0_CTL_REG, Offset: 0x14 */
#define CPR1_FMC_AHB0_CG_Pos                 0U
#define CPR1_FMC_AHB0_CG_Msk                 (0x1U << CPR1_FMC_AHB0_CG_Pos)
#define CPR1_FMC_AHB0_CG_EN                  CPR1_FMC_AHB0_CG_Msk
#define CPR1_FMC_AHB0_BUSY_Pos               4U
#define CPR1_FMC_AHB0_BUSY_Msk               (0x1U << CPR1_FMC_AHB0_BUSY_Pos)

/* CPR1 ROM_CTL_REG, Offset: 0x18 */
#define CPR1_ROM_CG_Pos                      0U
#define CPR1_ROM_CG_Msk                      (0x1U << CPR1_ROM_CG_Pos)
#define CPR1_ROM_CG_EN                       CPR1_ROM_CG_Msk
#define CPR1_ROM_BUSY_Pos                    4U
#define CPR1_ROM_BUSY_Msk                    (0x1U << CPR1_ROM_BUSY_Pos)

/* CPR1 GPIO0_CTL_REG, Offset: 0x1c */
#define CPR1_GPIO0_CG_Pos                    0U
#define CPR1_GPIO0_CG_Msk                    (0x1U << CPR1_GPIO0_CG_Pos)
#define CPR1_GPIO0_CG_EN                     CPR1_GPIO0_CG_Msk
#define CPR1_GPIO0_BUSY_Pos                  4U
#define CPR1_GPIO0_BUSY_Msk                  (0x1U << CPR1_GPIO0_BUSY_Pos)

/* CPR1 QSPI_CTL_REG, Offset: 0x20 */
#define CPR1_QSPI_CG_Pos                     0U
#define CPR1_QSPI_CG_Msk                     (0x1U << CPR1_QSPI_CG_Pos)
#define CPR1_QSPI_CG_EN                      CPR1_QSPI_CG_Msk
#define CPR1_QSPI_BUSY_Pos                   4U
#define CPR1_QSPI_BUSY_Msk                   (0x1U << CPR1_QSPI_BUSY_Pos)

/* CPR1 USI0_CTL_REG, Offset: 0x24 */
#define CPR1_USI0_CG_Pos                     0U
#define CPR1_USI0_CG_Msk                     (0x1U << CPR1_USI0_CG_Pos)
#define CPR1_USI0_CG_EN                      CPR1_USI0_CG_Msk
#define CPR1_USI0_BUSY_Pos                   4U
#define CPR1_USI0_BUSY_Msk                   (0x1U << CPR1_USI0_BUSY_Pos)

/* CPR1 USI1_CTL_REG, Offset: 0x28 */
#define CPR1_USI1_CG_Pos                     0U
#define CPR1_USI1_CG_Msk                     (0x1U << CPR1_USI1_CG_Pos)
#define CPR1_USI1_CG_EN                      CPR1_USI1_CG_Msk
#define CPR1_USI1_BUSY_Pos                   4U
#define CPR1_USI1_BUSY_Msk                   (0x1U << CPR1_USI1_BUSY_Pos)

/* CPR1 UART0_CTL_REG, Offset: 0x2c */
#define CPR1_UART0_CG_Pos                     0U
#define CPR1_UART0_CG_Msk                     (0x1U << CPR1_UART0_CG_Pos)
#define CPR1_UART0_CG_EN                      CPR1_UART0_CG_Msk
#define CPR1_UART0_BUSY_Pos                   4U
#define CPR1_UART0_BUSY_Msk                   (0x1U << CPR1_UART0_BUSY_Pos)

/* CPR1 I2S0_CTL_REG, Offset: 0x30 */
#define CPR1_I2S0_CG_Pos                     0U
#define CPR1_I2S0_CG_Msk                     (0x1U << CPR1_I2S0_CG_Pos)
#define CPR1_I2S0_CG_EN                      CPR1_I2S0_CG_Msk
#define CPR1_I2S0_BUSY_Pos                   4U
#define CPR1_I2S0_BUSY_Msk                   (0x1U << CPR1_I2S0_BUSY_Pos)
#define CPR1_I2S01_Pos                       8U
#define CPR1_I2S01_Msk                       (0x1U << CPR1_I2S0_CG_Pos)
#define CPR1_I2S01_EN                        CPR1_I2S0_CG_Msk

/* CPR1 I2S1_CTL_REG, Offset: 0x34 */
#define CPR1_I2S1_CG_Pos                     0U
#define CPR1_I2S1_CG_Msk                     (0x1U << CPR1_I2S1_CG_Pos)
#define CPR1_I2S1_CG_EN                      CPR1_I2S1_CG_Msk
#define CPR1_I2S1_BUSY_Pos                   4U
#define CPR1_I2S1_BUSY_Msk                   (0x1U << CPR1_I2S1_BUSY_Pos)

/* CPR1 EFUSE_CTL_REG, Offset: 0x38 */
#define CPR1_EFUSE_CG_Pos                     0U
#define CPR1_EFUSE_CG_Msk                     (0x1U << CPR1_EFUSE_CG_Pos)
#define CPR1_EFUSE_CG_EN                      CPR1_EFUSE_CG_Msk
#define CPR1_EFUSE_BUSY_Pos                   4U
#define CPR1_EFUSE_BUSY_Msk                   (0x1U << CPR1_EFUSE_BUSY_Pos)

/* CPR1 SASC_CTL_REG, Offset: 0x3c */
#define CPR1_SASC_CG_Pos                     0U
#define CPR1_SASC_CG_Msk                     (0x1U << CPR1_SASC_CG_Pos)
#define CPR1_SASC_CG_EN                      CPR1_SASC_CG_Msk
#define CPR1_SASC_BUSY_Pos                   4U
#define CPR1_SASC_BUSY_Msk                   (0x1U << CPR1_SASC_BUSY_Pos)

/* CPR1 TIPC_CTL_REG, Offset: 0x40 */
#define CPR1_TIPC_CG_Pos                     0U
#define CPR1_TIPC_CG_Msk                     (0x1U << CPR1_TIPC_CG_Pos)
#define CPR1_TIPC_CG_EN                      CPR1_TIPC_CG_Msk
#define CPR1_TIPC_BUSY_Pos                   4U
#define CPR1_TIPC_BUSY_Msk                   (0x1U << CPR1_TIPC_BUSY_Pos)

/* CPR1 SDIO_CLK_CTL_REG, Offset: 0x44 */
#define CPR1_SDIO_CG_Pos                     0U
#define CPR1_SDIO_CG_Msk                     (0x1U << CPR1_SDIO_CG_Pos)
#define CPR1_SDIO_CG_EN                      CPR1_SDIO_CG_Msk
#define CPR1_SDIO_SEL_Pos                    4U
#define CPR1_SDIO_SEL_Msk                    (0x1U << CPR1_SDIO_SEL_Pos)
#define CPR1_SDIO_SEL_12M                    CPR1_SDIO_SEL_Msk
#define CPR1_SDIO_SEL_D3                     (~CPR1_SDIO_SEL_Msk)
#define CPR1_SYS_D3_DIV_Pos                  8U
#define CPR1_SYS_D3_DIV_Msk                  (0xFU << CPR1_SYS_D3_DIV_Pos)
#define CPR1_SYS_D3_DIV(n)                   (((n) - 1U) << CPR1_SYS_D3_DIV_Pos)
#define CPR1_SDIO_BUSY_Pos                   12U
#define CPR1_SDIO_BUSY_Msk                   (0x1U << CPR1_SDIO_BUSY_Pos)

/* CPR1 SDMMC_CTL_REG, Offset: 0x48 */
#define CPR1_SDMMC_CG_Pos                    0U
#define CPR1_SDMMC_CG_Msk                    (0x1U << CPR1_SDMMC_CG_Pos)
#define CPR1_SDMMC_CG_EN                     CPR1_SDMMC_CG_Msk
#define CPR1_SDMMC_SEL_Pos                   4U
#define CPR1_SDMMC_SEL_Msk                   (0x1U << CPR1_SDMMC_SEL_Pos)
#define CPR1_SDMMC_SEL_12M                   CPR1_SDMMC_SEL_Msk
#define CPR1_SDMMC_SEL_D3                    (~CPR1_SDMMC_SEL_Msk)
#define CPR1_SDMMC_BUSY_Pos                  8U
#define CPR1_SDMMC_BUSY_Msk                  (0x1U << CPR1_SDMMC_BUSY_Pos)

/* CPR1 AES_CTL_REG, Offset: 0x4C */
#define CPR1_AES_CG_Pos                     0U
#define CPR1_AES_CG_Msk                     (0x1U << CPR1_AES_CG_Pos)
#define CPR1_AES_CG_EN                      CPR1_AES_CG_Msk
#define CPR1_AES_BUSY_Pos                   4U
#define CPR1_AES_BUSY_Msk                   (0x1U << CPR1_AES_BUSY_Pos)

/* CPR1 RSA_CTL_REG, Offset: 0x50 */
#define CPR1_RSA_CG_Pos                     0U
#define CPR1_RSA_CG_Msk                     (0x1U << CPR1_RSA_CG_Pos)
#define CPR1_RSA_CG_EN                      CPR1_RSA_CG_Msk
#define CPR1_RSA_BUSY_Pos                   4U
#define CPR1_RSA_BUSY_Msk                   (0x1U << CPR1_RSA_BUSY_Pos)

/* CPR1 SHA_CTL_REG, Offset: 0x54 */
#define CPR1_SHA_CG_Pos                     0U
#define CPR1_SHA_CG_Msk                     (0x1U << CPR1_SHA_CG_Pos)
#define CPR1_SHA_CG_EN                      CPR1_SHA_CG_Msk
#define CPR1_SHA_BUSY_Pos                   4U
#define CPR1_SHA_BUSY_Msk                   (0x1U << CPR1_SHA_BUSY_Pos)

/* CPR1 TRNG_CTL_REG, Offset: 0x58 */
#define CPR1_TRNG_CG_Pos                    0U
#define CPR1_TRNG_CG_Msk                    (0x1U << CPR1_TRNG_CG_Pos)
#define CPR1_TRNG_CG_EN                     CPR1_TRNG_CG_Msk
#define CPR1_TRNG_BUSY_Pos                  4U
#define CPR1_TRNG_BUSY_Msk                  (0x1U << CPR1_TRNG_BUSY_Pos)

/* CPR1 DMA0_CTL_REG, Offset: 0x5C */
#define CPR1_DMA0_CG_Pos                    0U
#define CPR1_DMA0_CG_Msk                    (0x1U << CPR1_DMA0_CG_Pos)
#define CPR1_DMA0_CG_EN                     CPR1_DMA0_CG_Msk
#define CPR1_DMA0_BUSY_Pos                  4U
#define CPR1_DMA0_BUSY_Msk                  (0x1U << CPR1_DMA0_BUSY_Pos)

/* CPR1 WDT_CTL_REG, Offset: 0x60 */
#define CPR1_WDT_CG_Pos                     0U
#define CPR1_WDT_CG_Msk                     (0x1U << CPR1_WDT_CG_Pos)
#define CPR1_WDT_CG_EN                      CPR1_WDT_CG_Msk
#define CPR1_WDT_BUSY_Pos                   4U
#define CPR1_WDT_BUSY_Msk                   (0x1U << CPR1_WDT_BUSY_Pos)

/* CPR1 PWM0_CTL_REG, Offset: 0x64 */
#define CPR1_PWM0_CG_Pos                    0U
#define CPR1_PWM0_CG_Msk                    (0x1U << CPR1_PWM0_CG_Pos)
#define CPR1_PWM0_CG_EN                     CPR1_PWM0_CG_Msk
#define CPR1_PWM0_BUSY_Pos                  4U
#define CPR1_PWM0_BUSY_Msk                  (0x1U << CPR1_PWM0_BUSY_Pos)

/* CPR1 MAINSWRST, Offset: 0x68 */
#define CPR1_ROM_RST_Pos                    0U
#define CPR1_ROM_RST_Msk                    (0x1U << CPR1_ROM_RST_Pos)
#define CPR1_ROM_RST_EN                     CPR1_ROM_RST_Msk
#define CPR1_FMC0_RST_Pos                   1U
#define CPR1_FMC0_RST_Msk                   (0x1U << CPR1_FMC0_RST_Pos)
#define CPR1_FMC0_RST_EN                    CPR1_FMC0_RST_Msk
#define CPR1_QSPI_RST_Pos                   2U
#define CPR1_QSPI_RST_Msk                   (0x1U << CPR1_QSPI_RST_Pos)
#define CPR1_QSPI_RST_EN                    CPR1_QSPI_RST_Msk
#define CPR1_DMA0_RST_Pos                   3U
#define CPR1_DMA0_RST_Msk                   (0x1U << CPR1_DMA0_RST_Pos)
#define CPR1_DMA0_RST_EN                    CPR1_DMA0_RST_Msk

/* CPR1 AHB0SUB0SWRST, Offset: 0x6C */
#define CPR1_SASC_RST_Pos                   0U
#define CPR1_SASC_RST_Msk                   (0x1U << CPR1_SASC_RST_Pos)
#define CPR1_SASC_RST_EN                    CPR1_SASC_RST_Msk
#define CPR1_SDMMC_RST_Pos                  1U
#define CPR1_SDMMC_RST_Msk                  (0x1U << CPR1_SDMMC_RST_Pos)
#define CPR1_SDMMC_RST_EN                   CPR1_SDMMC_RST_Msk
#define CPR1_SDIO_RST_Pos                   2U
#define CPR1_SDIO_RST_Msk                   (0x1U << CPR1_SDIO_RST_Pos)
#define CPR1_SDIO_RST_EN                    CPR1_SDIO_RST_Msk

/* CPR1 AHB0SUB1SWRST, Offset: 0x70 */
#define CPR1_FFT_M_RST_Pos                  0U
#define CPR1_FFT_M_RST_Msk                  (0x1U << CPR1_FFT_M_RST_Pos)
#define CPR1_FFT_M_RST_EN                   CPR1_FFT_M_RST_Msk
#define CPR1_FFT_S_RST_Pos                  1U
#define CPR1_FFT_S_RST_Msk                  (0x1U << CPR1_FFT_S_RST_Pos)
#define CPR1_FFT_S_RST_EN                   CPR1_FFT_S_RST_Msk
#define CPR1_SHA_RST_Pos                    2U
#define CPR1_SHA_RST_Msk                    (0x1U << CPR1_SHA_RST_Pos)
#define CPR1_SHA_RST_EN                     CPR1_SHA_RST_Msk
#define CPR1_AES_RST_Pos                    3U
#define CPR1_AES_RST_Msk                    (0x1U << CPR1_AES_RST_Pos)
#define CPR1_AES_RST_EN                     CPR1_AES_RST_Msk

/* CPR1 AHB0SUB2SWRST, Offset: 0x74 */
#define CPR1_MCC0_RST_Pos                   0U
#define CPR1_MCC0_RST_Msk                   (0x1U << CPR1_SASC_RST_Pos)
#define CPR1_MCC0_RST_EN                    CPR1_SASC_RST_Msk
#define CPR1_RSA_RST_Pos                    1U
#define CPR1_RSA_RST_Msk                    (0x1U << CPR1_SDMMC_RST_Pos)
#define CPR1_RSA_RST_EN                     CPR1_SDMMC_RST_Msk
#define CPR1_TRNG_RST_Pos                   2U
#define CPR1_TRNG_RST_Msk                   (0x1U << CPR1_SDIO_RST_Pos)
#define CPR1_TRNG_RST_EN                    CPR1_SDIO_RST_Msk

/* CPR1 APB0SWRST, Offset: 0x78 */
#define CPR1_QSPIC_RST_Pos                  0U
#define CPR1_QSPIC_RST_Msk                  (0x1U << CPR1_QSPIC_RST_Pos)
#define CPR1_QSPIC_RST_EN                   CPR1_QSPIC_RST_Msk
#define CPR1_WDT_IN_RST_Pos                 1U
#define CPR1_WDT_IN_RST_Msk                 (0x1U << CPR1_WDT_IN_RST_Pos)
#define CPR1_WDT_IN_RST_EN                  CPR1_WDT_IN_RST_Msk
#define CPR1_TIM1_RST_Pos                   2U
#define CPR1_TIM1_RST_Msk                   (0x1U << CPR1_TIM1_RST_Pos)
#define CPR1_TIM1_RST_EN                    CPR1_TIM1_RST_Msk
#define CPR1_TIPC_RST_Pos                   3U
#define CPR1_TIPC_RST_Msk                   (0x1U << CPR1_TIPC_RST_Pos)
#define CPR1_TIPC_RST_EN                    CPR1_TIPC_RST_Msk
#define CPR1_PWM0_RST_Pos                   4U
#define CPR1_PWM0_RST_Msk                   (0x1U << CPR1_PWM0_RST_Pos)
#define CPR1_PWM0_RST_EN                    CPR1_PWM0_RST_Msk
#define CPR1_I2S0_RST_Pos                   5U
#define CPR1_I2S0_RST_Msk                   (0x1U << CPR1_I2S0_RST_Pos)
#define CPR1_I2S0_RST_EN                    CPR1_I2S0_RST_Msk
#define CPR1_I2S1_RST_Pos                   6U
#define CPR1_I2S1_RST_Msk                   (0x1U << CPR1_I2S1_RST_Pos)
#define CPR1_I2S1_RST_EN                    CPR1_I2S1_RST_Msk
#define CPR1_USI0_RST_Pos                   7U
#define CPR1_USI0_RST_Msk                   (0x1U << CPR1_USI0_RST_Pos)
#define CPR1_USI0_RST_EN                    CPR1_USI0_RST_Msk
#define CPR1_USI1_RST_Pos                   8U
#define CPR1_USI1_RST_Msk                   (0x1U << CPR1_USI1_RST_Pos)
#define CPR1_USI1_RST_EN                    CPR1_USI1_RST_Msk
#define CPR1_UART0_RST_Pos                  9U
#define CPR1_UART0_RST_Msk                  (0x1U << CPR1_UART0_RST_Pos)
#define CPR1_UART0_RST_EN                   CPR1_UART0_RST_Msk
#define CPR1_UART1_RST_Pos                  10U
#define CPR1_UART1_RST_Msk                  (0x1U << CPR1_UART1_RST_Pos)
#define CPR1_UART1_RST_EN                   CPR1_UART1_RST_Msk

/* CPR1 UART1_CTL_REG, Offset: 0x7C */
#define CPR1_UART1_CG_Pos                   0U
#define CPR1_UART1_CG_Msk                   (0x1U << CPR1_PWM0_CG_Pos)
#define CPR1_UART1_CG_EN                    CPR1_PWM0_CG_Msk
#define CPR1_UART1_BUSY_Pos                 4U
#define CPR1_UART1_BUSY_Msk                 (0x1U << CPR1_PWM0_BUSY_Pos)

/* CPR1 CPU0_DTCM_BASE, Offset: 0x80 */

/* CPR1 CPU0_DTCM_MASK, Offset: 0x84 */

/* CPR1 CPU0_ITCM_BASE, Offset: 0x88 */

/* CPR1 CPU0_ITCM_MASK, Offset: 0x8c */

/* CPR1 CPU1_DTCM_BASE, Offset: 0x90 */

/* CPR1 CPU1_DTCM_MASK, Offset: 0x94 */

/* CPR1 CPU1_ITCM_BASE, Offset: 0x98 */

/* CPR1 CPU1_ITCM_MASK, Offset: 0x9c */

/* CPR1 PMU_SYSCLK0_SEL_REG, Offset: 0x0 */
static inline void wj_pmu_write_sysclk0_sel_12m(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_SYSCLK0_SEL_REG |= CPR1_SYSCLK0_SEL_12M;
}
static inline void wj_pmu_write_sysclk0_sel_d0(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_SYSCLK0_SEL_REG &= CPR1_SYSCLK0_SEL_D0;
}
static inline uint32_t wj_pmu_read_sysclk0_sta(wj_pmu_reg_cpr1_t *pmu_base)
{
    return ((pmu_base->CPR1_SYSCLK0_SEL_REG & CPR1_SYSCLK0_SEL_Msk) >> CPR1_SYSCLK0_SEL_Pos);
}

/* CPR1 SYSCLK0_DIV_CTL_REG, Offset: 0x4 */
static inline void wj_pmu_write_sysclk_1_div(wj_pmu_reg_cpr1_t *pmu_base, uint32_t div)
{
    pmu_base->CPR1_SYSCLK_DIV_CTL0_REG &= ~CPR1_SYS_CLK_1_DIV_Msk;
    pmu_base->CPR1_SYSCLK_DIV_CTL0_REG |= CPR1_SYS_CLK_1_DIV(div);
}
static inline uint32_t wj_pmu_read_sysclk_1_div_sta(wj_pmu_reg_cpr1_t *pmu_base)
{
    return (((pmu_base->CPR1_SYSCLK_DIV_CTL0_REG & CPR1_SYS_CLK_1_DIV_Msk) >> CPR1_SYS_CLK_1_DIV_Pos) * 2U);
}
static inline void wj_pmu_write_sysclk_2_div(wj_pmu_reg_cpr1_t *pmu_base, uint32_t div)
{
    pmu_base->CPR1_SYSCLK_DIV_CTL0_REG &= ~CPR1_SYS_CLK_2_DIV_Msk;
    pmu_base->CPR1_SYSCLK_DIV_CTL0_REG |= CPR1_SYS_CLK_2_DIV(div);
}
static inline uint32_t wj_pmu_read_sysclk_2_div_sta(wj_pmu_reg_cpr1_t *pmu_base)
{
    return (((pmu_base->CPR1_SYSCLK_DIV_CTL0_REG & CPR1_SYS_CLK_2_DIV_Msk) >> CPR1_SYS_CLK_2_DIV_Pos) * 2U);
}
static inline void wj_pmu_write_apb0_div(wj_pmu_reg_cpr1_t *pmu_base, uint32_t div)
{
    pmu_base->CPR1_SYSCLK_DIV_CTL0_REG &= ~CPR1_APB0_CLK_DIV_Msk;
    pmu_base->CPR1_SYSCLK_DIV_CTL0_REG |= CPR1_APB0_CLK_DIV(div);
}
static inline uint32_t wj_pmu_read_apb0_sta(wj_pmu_reg_cpr1_t *pmu_base)
{
    return (((pmu_base->CPR1_SYSCLK_DIV_CTL0_REG & CPR1_APB0_CLK_DIV_Msk) >> CPR1_APB0_CLK_DIV_Pos) + 1U);
}

/* CPR1 FFT_CTL_REG, Offset: 0xC */
static inline void wj_pmu_write_fft_cg_en(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_FFT_CTL_REG |= CPR1_FFT_CG_EN;
}
static inline void wj_pmu_write_fft_cg_dis(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_FFT_CTL_REG &= ~CPR1_FFT_CG_EN;
}
static inline uint32_t wj_pmu_read_fft_cg_sta(wj_pmu_reg_cpr1_t *pmu_base)
{
    return ((pmu_base->CPR1_FFT_CTL_REG & CPR1_FFT_BUSY_Msk) >> CPR1_FFT_BUSY_Pos);
}

/* CPR1 TIM1_CTL_REG, Offset: 0x10 */
static inline void wj_pmu_write_tim1_cg_en(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_TIM1_CTL_REG |= CPR1_TIM1_CG_EN;
}
static inline void wj_pmu_write_tim1_cg_dis(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_TIM1_CTL_REG &= ~CPR1_TIM1_CG_EN;
}
static inline void wj_pmu_write_tim1_serie_en(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_TIM1_CTL_REG |= CPR1_TIM1_SERIE_SEL_EN;
}
static inline void wj_pmu_write_tim1_serie_dis(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_TIM1_CTL_REG &= ~CPR1_TIM1_SERIE_SEL_EN;
}
static inline uint32_t wj_pmu_read_tim1_cg_sta(wj_pmu_reg_cpr1_t *pmu_base)
{
    return ((pmu_base->CPR1_TIM1_CTL_REG & CPR1_TIM1_BUSY_Msk) >> CPR1_TIM1_BUSY_Pos);
}

/* CPR1 FMC_AHB0_CTL_REG, Offset: 0x14 */
static inline void wj_pmu_write_ahp0_cg_en(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_FMC_AHB0_CTL_REG |= CPR1_FMC_AHB0_CG_EN;
}
static inline void wj_pmu_write_ahp0_cg_dis(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_FMC_AHB0_CTL_REG &= ~CPR1_FMC_AHB0_CG_EN;
}
static inline uint32_t wj_pmu_read_ahp0_cg_sta(wj_pmu_reg_cpr1_t *pmu_base)
{
    return ((pmu_base->CPR1_FMC_AHB0_CTL_REG & CPR1_FMC_AHB0_BUSY_Msk) >> CPR1_FMC_AHB0_BUSY_Pos);
}

/* CPR1 ROM_CTL_REG, Offset: 0x18 */
static inline void wj_pmu_write_rom_cg_en(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_ROM_CTL_REG |= CPR1_ROM_CG_EN;
}
static inline void wj_pmu_write_rom_cg_dis(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_ROM_CTL_REG &= ~CPR1_ROM_CG_EN;
}
static inline uint32_t wj_pmu_read_rom_cg_sta(wj_pmu_reg_cpr1_t *pmu_base)
{
    return ((pmu_base->CPR1_ROM_CTL_REG & CPR1_ROM_BUSY_Msk) >> CPR1_ROM_BUSY_Pos);
}

/* CPR1 GPIO0_CTL_REG, Offset: 0x1c */
static inline void wj_pmu_write_gpio0_cg_en(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_GPIO0_CTL_REG |= CPR1_GPIO0_CG_EN;
}
static inline void wj_pmu_write_gpio0_cg_dis(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_GPIO0_CTL_REG &= ~CPR1_GPIO0_CG_EN;
}
static inline uint32_t wj_pmu_read_gpio0_cg_sta(wj_pmu_reg_cpr1_t *pmu_base)
{
    return ((pmu_base->CPR1_GPIO0_CTL_REG & CPR1_GPIO0_BUSY_Msk) >> CPR1_GPIO0_BUSY_Pos);
}

/* CPR1 QSPI_CTL_REG, Offset: 0x20 */
static inline void wj_pmu_write_qspi_cg_en(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_QSPI_CTL_REG |= CPR1_QSPI_CG_EN;
}
static inline void wj_pmu_write_qspi_cg_dis(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_QSPI_CTL_REG &= ~CPR1_QSPI_CG_EN;
}
static inline uint32_t wj_pmu_read_qspi_cg_sta(wj_pmu_reg_cpr1_t *pmu_base)
{
    return ((pmu_base->CPR1_QSPI_CTL_REG & CPR1_QSPI_BUSY_Msk) >> CPR1_QSPI_BUSY_Pos);
}

/* CPR1 USI0_CTL_REG, Offset: 0x24 */
static inline void wj_pmu_write_usi0_cg_en(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_USI0_CTL_REG |= CPR1_USI0_CG_EN;
}
static inline void wj_pmu_write_usi0_cg_dis(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_USI0_CTL_REG &= ~CPR1_USI0_CG_EN;
}
static inline uint32_t wj_pmu_read_usi0_cg_sta(wj_pmu_reg_cpr1_t *pmu_base)
{
    return ((pmu_base->CPR1_USI0_CTL_REG & CPR1_USI0_BUSY_Msk) >> CPR1_USI0_BUSY_Pos);
}

/* CPR1 USI1_CTL_REG, Offset: 0x28 */
static inline void wj_pmu_write_usi1_cg_en(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_USI1_CTL_REG |= CPR1_USI1_CG_EN;
}
static inline void wj_pmu_write_usi1_cg_dis(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_USI1_CTL_REG &= ~CPR1_USI1_CG_EN;
}
static inline uint32_t wj_pmu_read_usi1_cg_sta(wj_pmu_reg_cpr1_t *pmu_base)
{
    return ((pmu_base->CPR1_USI1_CTL_REG & CPR1_USI1_BUSY_Msk) >> CPR1_USI1_BUSY_Pos);
}

/* CPR1 UART0_CTL_REG, Offset: 0x2c */
static inline void wj_pmu_write_uart0_cg_en(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_UART0_CTL_REG |= CPR1_UART0_CG_EN;
}
static inline void wj_pmu_write_uart0_cg_dis(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_UART0_CTL_REG &= ~CPR1_UART0_CG_EN;
}
static inline uint32_t wj_pmu_read_uart0_cg_sta(wj_pmu_reg_cpr1_t *pmu_base)
{
    return ((pmu_base->CPR1_UART0_CTL_REG & CPR1_UART0_BUSY_Msk) >> CPR1_UART0_BUSY_Pos);
}

/* CPR1 I2S0_CTL_REG, Offset: 0x30 */
static inline void wj_pmu_write_i2s0_cg_en(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_I2S0_CLK_CTL_REG |= CPR1_I2S0_CG_EN;
}
static inline void wj_pmu_write_i2s0_cg_dis(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_I2S0_CLK_CTL_REG &= ~CPR1_I2S0_CG_EN;
}
static inline uint32_t wj_pmu_read_i2s0_cg_sta(wj_pmu_reg_cpr1_t *pmu_base)
{
    return ((pmu_base->CPR1_I2S0_CLK_CTL_REG & CPR1_I2S0_BUSY_Msk) >> CPR1_I2S0_BUSY_Pos);
}
static inline void wj_pmu_write_i2s01_en(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_I2S0_CLK_CTL_REG |= CPR1_I2S01_EN;
}
static inline void wj_pmu_write_i2s01_dis(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_I2S0_CLK_CTL_REG &= ~CPR1_I2S01_EN;
}

/* CPR1 I2S1_CTL_REG, Offset: 0x34 */
static inline void wj_pmu_write_i2s1_cg_en(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_I2S1_CLK_CTL_REG |= CPR1_I2S1_CG_EN;
}
static inline void wj_pmu_write_i2s1_cg_dis(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_I2S1_CLK_CTL_REG &= ~CPR1_I2S1_CG_EN;
}
static inline uint32_t wj_pmu_read_i2s1_cg_sta(wj_pmu_reg_cpr1_t *pmu_base)
{
    return ((pmu_base->CPR1_I2S1_CLK_CTL_REG & CPR1_I2S1_BUSY_Msk) >> CPR1_I2S1_BUSY_Pos);
}

/* CPR1 EFUSE_CTL_REG, Offset: 0x38 */
static inline void wj_pmu_write_efuse_cg_en(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_EFUSE_CTL_REG |= CPR1_EFUSE_CG_EN;
}
static inline void wj_pmu_write_efuse_cg_dis(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_EFUSE_CTL_REG &= ~CPR1_EFUSE_CG_EN;
}
static inline uint32_t wj_pmu_read_efuse_cg_sta(wj_pmu_reg_cpr1_t *pmu_base)
{
    return ((pmu_base->CPR1_EFUSE_CTL_REG & CPR1_EFUSE_BUSY_Msk) >> CPR1_EFUSE_BUSY_Pos);
}

/* CPR1 SASC_CTL_REG, Offset: 0x3c */
static inline void wj_pmu_write_sasc_cg_en(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_SASC_CTL_REG |= CPR1_SASC_CG_EN;
}
static inline void wj_pmu_write_sasc_cg_dis(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_SASC_CTL_REG &= ~CPR1_SASC_CG_EN;
}
static inline uint32_t wj_pmu_read_sasc_cg_sta(wj_pmu_reg_cpr1_t *pmu_base)
{
    return ((pmu_base->CPR1_SASC_CTL_REG & CPR1_SASC_BUSY_Msk) >> CPR1_SASC_BUSY_Pos);
}

/* CPR1 TIPC_CTL_REG, Offset: 0x40 */
static inline void wj_pmu_write_tipc_cg_en(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_TIPC_CTL_REG |= CPR1_TIPC_CG_EN;
}
static inline void wj_pmu_write_tipc_cg_dis(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_TIPC_CTL_REG &= ~CPR1_TIPC_CG_EN;
}
static inline uint32_t wj_pmu_read_tipc_cg_sta(wj_pmu_reg_cpr1_t *pmu_base)
{
    return ((pmu_base->CPR1_TIPC_CTL_REG & CPR1_TIPC_BUSY_Msk) >> CPR1_TIPC_BUSY_Pos);
}

/* CPR1 SDIO_CLK_CTL_REG, Offset: 0x44 */
static inline void wj_pmu_write_sdio_cg_en(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_SDIO_CLK_CTL_REG |= CPR1_SDIO_CG_EN;
}
static inline void wj_pmu_write_sdio_cg_dis(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_SDIO_CLK_CTL_REG &= ~CPR1_SDIO_CG_EN;
}
static inline void wj_pmu_write_sdio_sel_12m(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_SDIO_CLK_CTL_REG |= CPR1_SDIO_SEL_12M;
}
static inline void wj_pmu_write_sdio_sel_d3(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_SDIO_CLK_CTL_REG &= CPR1_SDIO_SEL_D3;
}
static inline uint32_t wj_pmu_write_sdio_sel_sta(wj_pmu_reg_cpr1_t *pmu_base)
{
    return ((pmu_base->CPR1_SDIO_CLK_CTL_REG & CPR1_SDIO_SEL_Msk) >> CPR1_SDIO_SEL_Pos);
}
static inline void wj_pmu_write_sdio_div(wj_pmu_reg_cpr1_t *pmu_base, uint32_t div)
{
    pmu_base->CPR1_SDIO_CLK_CTL_REG &= ~CPR1_SYS_D3_DIV_Msk;
    pmu_base->CPR1_SDIO_CLK_CTL_REG |= CPR1_SYS_D3_DIV(div);
}
static inline uint32_t wj_pmu_read_sdio_div_sta(wj_pmu_reg_cpr1_t *pmu_base)
{
    return (((pmu_base->CPR1_SDIO_CLK_CTL_REG & CPR1_SYS_D3_DIV_Msk) >> CPR1_SYS_D3_DIV_Pos) + 1U);
}
static inline uint32_t wj_pmu_read_sdio_cg_sta(wj_pmu_reg_cpr1_t *pmu_base)
{
    return ((pmu_base->CPR1_SDIO_CLK_CTL_REG & CPR1_SDIO_BUSY_Msk) >> CPR1_SDIO_BUSY_Pos);
}

/* CPR1 SDMMC_CTL_REG, Offset: 0x48 */
static inline void wj_pmu_write_sdmmc_cg_en(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_SDMMC_CLK_CTL_REG |= CPR1_SDMMC_CG_EN;
}
static inline void wj_pmu_write_sdmmc_cg_dis(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_SDMMC_CLK_CTL_REG &= ~CPR1_SDMMC_CG_EN;
}
static inline void wj_pmu_write_sdmmc_sel_12m(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_SDMMC_CLK_CTL_REG |= CPR1_SDMMC_SEL_12M;
}
static inline void wj_pmu_write_sdmmc_sel_d3(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_SDMMC_CLK_CTL_REG &= CPR1_SDMMC_SEL_D3;
}
static inline uint32_t wj_pmu_write_sdmmc_sel_sta(wj_pmu_reg_cpr1_t *pmu_base)
{
    return ((pmu_base->CPR1_SDMMC_CLK_CTL_REG & CPR1_SDMMC_SEL_Msk) >> CPR1_SDMMC_SEL_Pos);
}
static inline uint32_t wj_pmu_read_sdmmc_cg_sta(wj_pmu_reg_cpr1_t *pmu_base)
{
    return ((pmu_base->CPR1_SDMMC_CLK_CTL_REG & CPR1_SDMMC_BUSY_Msk) >> CPR1_SDMMC_BUSY_Pos);
}

/* CPR1 AES_CTL_REG, Offset: 0x4C */
static inline void wj_pmu_write_aes_cg_en(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_AES_CLK_CTL_REG |= CPR1_AES_CG_EN;
}
static inline void wj_pmu_write_aes_cg_dis(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_AES_CLK_CTL_REG &= ~CPR1_AES_CG_EN;
}
static inline uint32_t wj_pmu_read_aes_cg_sta(wj_pmu_reg_cpr1_t *pmu_base)
{
    return ((pmu_base->CPR1_AES_CLK_CTL_REG & CPR1_AES_BUSY_Msk) >> CPR1_AES_BUSY_Pos);
}

/* CPR1 RSA_CTL_REG, Offset: 0x50 */
static inline void wj_pmu_write_rsa_cg_en(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_RSA_CLK_CTL_REG |= CPR1_RSA_CG_EN;
}
static inline void wj_pmu_write_rsa_cg_dis(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_RSA_CLK_CTL_REG &= ~CPR1_RSA_CG_EN;
}
static inline uint32_t wj_pmu_read_rsa_cg_sta(wj_pmu_reg_cpr1_t *pmu_base)
{
    return ((pmu_base->CPR1_RSA_CLK_CTL_REG & CPR1_RSA_BUSY_Msk) >> CPR1_RSA_BUSY_Pos);
}

/* CPR1 SHA_CTL_REG, Offset: 0x54 */
static inline void wj_pmu_write_sha_cg_en(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_SHA_CLK_CTL_REG |= CPR1_SHA_CG_EN;
}
static inline void wj_pmu_write_sha_cg_dis(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_SHA_CLK_CTL_REG &= ~CPR1_SHA_CG_EN;
}
static inline uint32_t wj_pmu_read_sha_cg_sta(wj_pmu_reg_cpr1_t *pmu_base)
{
    return ((pmu_base->CPR1_SHA_CLK_CTL_REG & CPR1_SHA_BUSY_Msk) >> CPR1_SHA_BUSY_Pos);
}

/* CPR1 TRNG_CTL_REG, Offset: 0x58 */
static inline void wj_pmu_write_trng_cg_en(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_TRNG_CLK_CTL_REG |= CPR1_TRNG_CG_EN;
}
static inline void wj_pmu_write_trng_cg_dis(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_TRNG_CLK_CTL_REG &= ~CPR1_TRNG_CG_EN;
}
static inline uint32_t wj_pmu_read_trng_cg_sta(wj_pmu_reg_cpr1_t *pmu_base)
{
    return ((pmu_base->CPR1_TRNG_CLK_CTL_REG & CPR1_TRNG_BUSY_Msk) >> CPR1_TRNG_BUSY_Pos);
}

/* CPR1 DMA0_CTL_REG, Offset: 0x5C */
static inline void wj_pmu_write_dma0_cg_en(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_DMA0_CLK_CTL_REG |= CPR1_DMA0_CG_EN;
}
static inline void wj_pmu_write_dma0_cg_dis(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_DMA0_CLK_CTL_REG &= ~CPR1_DMA0_CG_EN;
}
static inline uint32_t wj_pmu_read_dma0_cg_sta(wj_pmu_reg_cpr1_t *pmu_base)
{
    return ((pmu_base->CPR1_DMA0_CLK_CTL_REG & CPR1_DMA0_BUSY_Msk) >> CPR1_DMA0_BUSY_Pos);
}

/* CPR1 WDT_CTL_REG, Offset: 0x60 */
static inline void wj_pmu_write_wdt_cg_en(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_WDT_CLK_CTL_REG |= CPR1_WDT_CG_EN;
}
static inline void wj_pmu_write_wdt_cg_dis(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_WDT_CLK_CTL_REG &= ~CPR1_WDT_CG_EN;
}
static inline uint32_t wj_pmu_read_wdt_cg_sta(wj_pmu_reg_cpr1_t *pmu_base)
{
    return ((pmu_base->CPR1_WDT_CLK_CTL_REG & CPR1_WDT_BUSY_Msk) >> CPR1_WDT_BUSY_Pos);
}

/* CPR1 PWM0_CTL_REG, Offset: 0x60 */
static inline void wj_pmu_write_pwm0_cg_en(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_PWM0_CLK_CTL_REG |= CPR1_PWM0_CG_EN;
}
static inline void wj_pmu_write_pwm0_cg_dis(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_PWM0_CLK_CTL_REG &= ~CPR1_PWM0_CG_EN;
}
static inline uint32_t wj_pmu_read_pwm0_cg_sta(wj_pmu_reg_cpr1_t *pmu_base)
{
    return ((pmu_base->CPR1_PWM0_CLK_CTL_REG & CPR1_PWM0_BUSY_Msk) >> CPR1_PWM0_BUSY_Pos);
}

/* CPR1 MAINSWRST, Offset: 0x68 */
static inline void wj_pmu_write_rom_rst_en(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_MAINSWRST |= CPR1_ROM_RST_EN;
}
static inline void wj_pmu_write_rom_rst_dis(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_MAINSWRST &= ~CPR1_ROM_RST_EN;
}
static inline uint32_t wj_pmu_read_rom_rst_sta(wj_pmu_reg_cpr1_t *pmu_base)
{
    return ((pmu_base->CPR1_MAINSWRST & CPR1_ROM_RST_Msk) >> CPR1_ROM_RST_Pos);
}
static inline void wj_pmu_write_fmc0_rst_en(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_MAINSWRST |= CPR1_FMC0_RST_EN;
}
static inline void wj_pmu_write_fmc0_rst_dis(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_MAINSWRST &= ~CPR1_FMC0_RST_EN;
}
static inline uint32_t wj_pmu_read_fmc0_rst_sta(wj_pmu_reg_cpr1_t *pmu_base)
{
    return ((pmu_base->CPR1_MAINSWRST & CPR1_FMC0_RST_Msk) >> CPR1_FMC0_RST_Pos);
}
static inline void wj_pmu_write_qspi_rst_en(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_MAINSWRST |= CPR1_QSPI_RST_EN;
}
static inline void wj_pmu_write_qspi_rst_dis(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_MAINSWRST &= ~CPR1_QSPI_RST_EN;
}
static inline uint32_t wj_pmu_read_qspi_rst_sta(wj_pmu_reg_cpr1_t *pmu_base)
{
    return ((pmu_base->CPR1_MAINSWRST & CPR1_QSPI_RST_Msk) >> CPR1_QSPI_RST_Pos);
}
static inline void wj_pmu_write_dma0_rst_en(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_MAINSWRST |= CPR1_DMA0_RST_EN;
}
static inline void wj_pmu_write_dma0_rst_dis(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_MAINSWRST &= ~CPR1_DMA0_RST_EN;
}
static inline uint32_t wj_pmu_read_dma0_rst_sta(wj_pmu_reg_cpr1_t *pmu_base)
{
    return ((pmu_base->CPR1_MAINSWRST & CPR1_DMA0_RST_Msk) >> CPR1_DMA0_RST_Pos);
}

/* CPR1 AHB0SUB0SWRST, Offset: 0x6C */
static inline void wj_pmu_write_sasc_rst_en(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_AHB0SUB0SWRST |= CPR1_SASC_RST_EN;
}
static inline void wj_pmu_write_sasc_rst_dis(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_AHB0SUB0SWRST &= ~CPR1_SASC_RST_EN;
}
static inline uint32_t wj_pmu_read_sasc_rst_sta(wj_pmu_reg_cpr1_t *pmu_base)
{
    return ((pmu_base->CPR1_AHB0SUB0SWRST & CPR1_SASC_RST_Msk) >> CPR1_SASC_RST_Pos);
}
static inline void wj_pmu_write_sdmmc_rst_en(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_AHB0SUB0SWRST |= CPR1_SDMMC_RST_EN;
}
static inline void wj_pmu_write_sdmmc_rst_dis(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_AHB0SUB0SWRST &= ~CPR1_SDMMC_RST_EN;
}
static inline uint32_t wj_pmu_read_sdmmc_rst_sta(wj_pmu_reg_cpr1_t *pmu_base)
{
    return ((pmu_base->CPR1_AHB0SUB0SWRST & CPR1_SDMMC_RST_Msk) >> CPR1_SDMMC_RST_Pos);
}
static inline void wj_pmu_write_sdio_rst_en(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_AHB0SUB0SWRST |= CPR1_SDIO_RST_EN;
}
static inline void wj_pmu_write_sdio_rst_dis(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_AHB0SUB0SWRST &= ~CPR1_SDIO_RST_EN;
}
static inline uint32_t wj_pmu_read_sdio_rst_sta(wj_pmu_reg_cpr1_t *pmu_base)
{
    return ((pmu_base->CPR1_AHB0SUB0SWRST & CPR1_SDIO_RST_Msk) >> CPR1_SDIO_RST_Pos);
}

/* CPR1 AHB0SUB1SWRST, Offset: 0x70 */
static inline void wj_pmu_write_fft_m_rst_en(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_AHB0SUB1SWRST |= CPR1_FFT_M_RST_EN;
}
static inline void wj_pmu_write_fft_m_rst_dis(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_AHB0SUB1SWRST &= ~CPR1_FFT_M_RST_EN;
}
static inline uint32_t wj_pmu_read_fft_m_rst_sta(wj_pmu_reg_cpr1_t *pmu_base)
{
    return ((pmu_base->CPR1_AHB0SUB1SWRST & CPR1_FFT_M_RST_Msk) >> CPR1_FFT_M_RST_Pos);
}
static inline void wj_pmu_write_fft_s_rst_en(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_AHB0SUB1SWRST |= CPR1_FFT_S_RST_EN;
}
static inline void wj_pmu_write_fft_s_rst_dis(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_AHB0SUB1SWRST &= ~CPR1_FFT_S_RST_EN;
}
static inline uint32_t wj_pmu_read_fft_s_rst_sta(wj_pmu_reg_cpr1_t *pmu_base)
{
    return ((pmu_base->CPR1_AHB0SUB1SWRST & CPR1_FFT_S_RST_Msk) >> CPR1_FFT_S_RST_Pos);
}
static inline void wj_pmu_write_sha_rst_en(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_AHB0SUB1SWRST |= CPR1_SHA_RST_EN;
}
static inline void wj_pmu_write_sha_rst_dis(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_AHB0SUB1SWRST &= ~CPR1_SHA_RST_EN;
}
static inline uint32_t wj_pmu_read_sha_rst_sta(wj_pmu_reg_cpr1_t *pmu_base)
{
    return ((pmu_base->CPR1_AHB0SUB1SWRST & CPR1_SHA_RST_Msk) >> CPR1_SHA_RST_Pos);
}
static inline void wj_pmu_write_aes_rst_en(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_AHB0SUB1SWRST |= CPR1_AES_RST_EN;
}
static inline void wj_pmu_write_aes_rst_dis(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_AHB0SUB1SWRST &= ~CPR1_AES_RST_EN;
}
static inline uint32_t wj_pmu_read_aes_rst_sta(wj_pmu_reg_cpr1_t *pmu_base)
{
    return ((pmu_base->CPR1_AHB0SUB1SWRST & CPR1_AES_RST_Msk) >> CPR1_AES_RST_Pos);
}

/* CPR1 AHB0SUB2SWRST, Offset: 0x74 */
static inline void wj_pmu_write_mcc0_rst_en(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_AHB0SUB2SWRST |= CPR1_MCC0_RST_EN;
}
static inline void wj_pmu_write_mcc0_rst_dis(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_AHB0SUB2SWRST &= ~CPR1_MCC0_RST_EN;
}
static inline uint32_t wj_pmu_read_mcc0_rst_sta(wj_pmu_reg_cpr1_t *pmu_base)
{
    return ((pmu_base->CPR1_AHB0SUB2SWRST & CPR1_MCC0_RST_Msk) >> CPR1_MCC0_RST_Pos);
}
static inline void wj_pmu_write_rsa_rst_en(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_AHB0SUB2SWRST |= CPR1_RSA_RST_EN;
}
static inline void wj_pmu_write_rsa_rst_dis(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_AHB0SUB2SWRST &= ~CPR1_RSA_RST_EN;
}
static inline uint32_t wj_pmu_read_rsa_rst_sta(wj_pmu_reg_cpr1_t *pmu_base)
{
    return ((pmu_base->CPR1_AHB0SUB2SWRST & CPR1_RSA_RST_Msk) >> CPR1_RSA_RST_Pos);
}
static inline void wj_pmu_write_trng_rst_en(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_AHB0SUB2SWRST |= CPR1_TRNG_RST_EN;
}
static inline void wj_pmu_write_trng_rst_dis(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_AHB0SUB2SWRST &= ~CPR1_TRNG_RST_EN;
}
static inline uint32_t wj_pmu_read_trng_rst_sta(wj_pmu_reg_cpr1_t *pmu_base)
{
    return ((pmu_base->CPR1_AHB0SUB2SWRST & CPR1_TRNG_RST_Msk) >> CPR1_TRNG_RST_Pos);
}

/* CPR1 APB0SWRST, Offset: 0x78 */
static inline void wj_pmu_write_qspic_rst_en(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_APB0SWRST |= CPR1_QSPIC_RST_EN;
}
static inline void wj_pmu_write_qspic_rst_dis(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_APB0SWRST &= ~CPR1_QSPIC_RST_EN;
}
static inline uint32_t wj_pmu_read_qspic_rst_sta(wj_pmu_reg_cpr1_t *pmu_base)
{
    return ((pmu_base->CPR1_APB0SWRST & CPR1_QSPIC_RST_Msk) >> CPR1_QSPIC_RST_Pos);
}
static inline void wj_pmu_write_wdt_rst_en(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_APB0SWRST |= CPR1_WDT_IN_RST_EN;
}
static inline void wj_pmu_write_wdt_rst_dis(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_APB0SWRST &= ~CPR1_WDT_IN_RST_EN;
}
static inline uint32_t wj_pmu_read_wdt_rst_sta(wj_pmu_reg_cpr1_t *pmu_base)
{
    return ((pmu_base->CPR1_APB0SWRST & CPR1_WDT_IN_RST_Msk) >> CPR1_WDT_IN_RST_Pos);
}
static inline void wj_pmu_write_tim1_rst_en(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_APB0SWRST |= CPR1_TIM1_RST_EN;
}
static inline void wj_pmu_write_tim1_rst_dis(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_APB0SWRST &= ~CPR1_TIM1_RST_EN;
}
static inline uint32_t wj_pmu_read_tim1_rst_sta(wj_pmu_reg_cpr1_t *pmu_base)
{
    return ((pmu_base->CPR1_APB0SWRST & CPR1_TIM1_RST_Msk) >> CPR1_TIM1_RST_Pos);
}
static inline void wj_pmu_write_tipc_rst_en(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_APB0SWRST |= CPR1_TIPC_RST_EN;
}
static inline void wj_pmu_write_tipc_rst_dis(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_APB0SWRST &= ~CPR1_TIPC_RST_EN;
}
static inline uint32_t wj_pmu_read_tipc_rst_sta(wj_pmu_reg_cpr1_t *pmu_base)
{
    return ((pmu_base->CPR1_APB0SWRST & CPR1_TIPC_RST_Msk) >> CPR1_TIPC_RST_Pos);
}
static inline void wj_pmu_write_pwm0_rst_en(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_APB0SWRST |= CPR1_PWM0_RST_EN;
}
static inline void wj_pmu_write_pwm0_rst_dis(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_APB0SWRST &= ~CPR1_PWM0_RST_EN;
}
static inline uint32_t wj_pmu_read_pwm0_rst_sta(wj_pmu_reg_cpr1_t *pmu_base)
{
    return ((pmu_base->CPR1_APB0SWRST & CPR1_PWM0_RST_Msk) >> CPR1_PWM0_RST_Pos);
}
static inline void wj_pmu_write_i2s0_rst_en(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_APB0SWRST |= CPR1_I2S0_RST_EN;
}
static inline void wj_pmu_write_i2s0_rst_dis(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_APB0SWRST &= ~CPR1_I2S0_RST_EN;
}
static inline uint32_t wj_pmu_read_i2s0_rst_sta(wj_pmu_reg_cpr1_t *pmu_base)
{
    return ((pmu_base->CPR1_APB0SWRST & CPR1_I2S0_RST_Msk) >> CPR1_I2S0_RST_Pos);
}
static inline void wj_pmu_write_i2s1_rst_en(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_APB0SWRST |= CPR1_I2S1_RST_EN;
}
static inline void wj_pmu_write_i2s1_rst_dis(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_APB0SWRST &= ~CPR1_I2S1_RST_EN;
}
static inline uint32_t wj_pmu_read_i2s1_rst_sta(wj_pmu_reg_cpr1_t *pmu_base)
{
    return ((pmu_base->CPR1_APB0SWRST & CPR1_I2S1_RST_Msk) >> CPR1_I2S1_RST_Pos);
}
static inline void wj_pmu_write_usi0_rst_en(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_APB0SWRST |= CPR1_USI0_RST_EN;
}
static inline void wj_pmu_write_usi0_rst_dis(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_APB0SWRST &= ~CPR1_USI0_RST_EN;
}
static inline uint32_t wj_pmu_read_usi0_rst_sta(wj_pmu_reg_cpr1_t *pmu_base)
{
    return ((pmu_base->CPR1_APB0SWRST & CPR1_USI0_RST_Msk) >> CPR1_USI0_RST_Pos);
}
static inline void wj_pmu_write_usi1_rst_en(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_APB0SWRST |= CPR1_USI1_RST_EN;
}
static inline void wj_pmu_write_usi1_rst_dis(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_APB0SWRST &= ~CPR1_USI1_RST_EN;
}
static inline uint32_t wj_pmu_read_usi1_rst_sta(wj_pmu_reg_cpr1_t *pmu_base)
{
    return ((pmu_base->CPR1_APB0SWRST & CPR1_USI1_RST_Msk) >> CPR1_USI1_RST_Pos);
}
static inline void wj_pmu_write_uart0_rst_en(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_APB0SWRST |= CPR1_UART0_RST_EN;
}
static inline void wj_pmu_write_uart0_rst_dis(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_APB0SWRST &= ~CPR1_UART0_RST_EN;
}
static inline uint32_t wj_pmu_read_uart0_rst_sta(wj_pmu_reg_cpr1_t *pmu_base)
{
    return ((pmu_base->CPR1_APB0SWRST & CPR1_UART0_RST_Msk) >> CPR1_UART0_RST_Pos);
}
static inline void wj_pmu_write_uart1_rst_en(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_APB0SWRST |= CPR1_UART1_RST_EN;
}
static inline void wj_pmu_write_uart1_rst_dis(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_APB0SWRST &= ~CPR1_UART1_RST_EN;
}
static inline uint32_t wj_pmu_read_uart1_rst_sta(wj_pmu_reg_cpr1_t *pmu_base)
{
    return ((pmu_base->CPR1_APB0SWRST & CPR1_UART1_RST_Msk) >> CPR1_UART1_RST_Pos);
}

/* CPR1 UART1_CTL_REG, Offset: 0x7C */
static inline void wj_pmu_write_uart1_cg_en(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_UART1_CTL_REG |= CPR1_UART1_CG_EN;
}
static inline void wj_pmu_write_uart1_cg_dis(wj_pmu_reg_cpr1_t *pmu_base)
{
    pmu_base->CPR1_UART1_CTL_REG &= ~CPR1_UART1_CG_EN;
}
static inline uint32_t wj_pmu_read_uart1_cg_sta(wj_pmu_reg_cpr1_t *pmu_base)
{
    return ((pmu_base->CPR1_UART1_CTL_REG & CPR1_UART1_BUSY_Msk) >> CPR1_UART1_BUSY_Pos);
}

/* CPR1 CPU0_DTCM_BASE, Offset: 0x80 */
static inline void wj_pmu_write_dtcm_baseaddr_804_0(wj_pmu_reg_cpr1_t *pmu_base, uint32_t addr)
{
    pmu_base->CPR1_CPU0_DAHBL_BASE_REG = addr;
}

static inline uint32_t wj_pmu_read_dtcm_baseaddr_804_0(wj_pmu_reg_cpr1_t *pmu_base)
{
    return (pmu_base->CPR1_CPU0_DAHBL_BASE_REG);
}
/* CPR1 CPU0_DTCM_MASK, Offset: 0x84 */
static inline void wj_pmu_write_dtcm_maskaddr_804_0(wj_pmu_reg_cpr1_t *pmu_base, uint32_t addr)
{
    pmu_base->CPR1_CPU0_DAHBL_MASK_REG = addr;
}

static inline uint32_t wj_pmu_read_dtcm_maskaddr_804_0(wj_pmu_reg_cpr1_t *pmu_base)
{
    return (pmu_base->CPR1_CPU0_DAHBL_MASK_REG);
}
/* CPR1 CPU0_ITCM_BASE, Offset: 0x88 */
static inline void wj_pmu_write_itcm_baseaddr_804_0(wj_pmu_reg_cpr1_t *pmu_base, uint32_t addr)
{
    pmu_base->CPR1_CPU0_DAHBL_BASE_REG = addr;
}

static inline uint32_t wj_pmu_read_itcm_baseaddr_804_0(wj_pmu_reg_cpr1_t *pmu_base)
{
    return (pmu_base->CPR1_CPU0_DAHBL_BASE_REG);
}
/* CPR1 CPU0_ITCM_MASK, Offset: 0x8c */
static inline void wj_pmu_write_itcm_maskaddr_804_0(wj_pmu_reg_cpr1_t *pmu_base, uint32_t addr)
{
    pmu_base->CPR1_CPU0_DAHBL_MASK_REG = addr;
}

static inline uint32_t wj_pmu_read_itcm_maskaddr_804_0(wj_pmu_reg_cpr1_t *pmu_base)
{
    return (pmu_base->CPR1_CPU0_DAHBL_MASK_REG);
}

/* CPR1 CPU1_DTCM_BASE, Offset: 0x90 */
static inline void wj_pmu_write_dtcm_baseaddr_804_1(wj_pmu_reg_cpr1_t *pmu_base, uint32_t addr)
{
    pmu_base->CPR1_CPU1_DAHBL_BASE_REG = addr;
}

static inline uint32_t wj_pmu_read_dtcm_baseaddr_804_1(wj_pmu_reg_cpr1_t *pmu_base)
{
    return (pmu_base->CPR1_CPU1_DAHBL_BASE_REG);
}
/* CPR1 CPU1_DTCM_MASK, Offset: 0x94 */
static inline void wj_pmu_write_dtcm_maskaddr_804_1(wj_pmu_reg_cpr1_t *pmu_base, uint32_t addr)
{
    pmu_base->CPR1_CPU1_DAHBL_MASK_REG = addr;
}

static inline uint32_t wj_pmu_read_dtcm_maskaddr_804_1(wj_pmu_reg_cpr1_t *pmu_base)
{
    return (pmu_base->CPR1_CPU1_DAHBL_MASK_REG);
}
/* CPR1 CPU1_ITCM_BASE, Offset: 0x98 */
static inline void wj_pmu_write_itcm_baseaddr_804_1(wj_pmu_reg_cpr1_t *pmu_base, uint32_t addr)
{
    pmu_base->CPR1_CPU1_DAHBL_BASE_REG = addr;
}

static inline uint32_t wj_pmu_read_itcm_baseaddr_804_1(wj_pmu_reg_cpr1_t *pmu_base)
{
    return (pmu_base->CPR1_CPU1_DAHBL_BASE_REG);
}
/* CPR1 CPU1_ITCM_MASK, Offset: 0x9c */
static inline void wj_pmu_write_itcm_maskaddr_804_1(wj_pmu_reg_cpr1_t *pmu_base, uint32_t addr)
{
    pmu_base->CPR1_CPU1_DAHBL_MASK_REG = addr;
}

static inline uint32_t wj_pmu_read_itcm_maskaddr_804_1(wj_pmu_reg_cpr1_t *pmu_base)
{
    return (pmu_base->CPR1_CPU1_DAHBL_MASK_REG);
}

/* CPR2 SYSCLK3_SEL_REG, Offset: 0x0 */
#define CPR2_SYSCLK3_SEL_Pos                        0U
#define CPR2_SYSCLK3_SEL_Msk                        (0x1U << CPR2_SYSCLK3_SEL_Pos)
#define CPR2_SYSCLK3_SEL_12M                        CPR2_SYSCLK3_SEL_Msk
#define CPR2_SYSCLK3_SEL_D1                         (~CPR2_SYSCLK3_SEL_Msk)

/* CPR2 CPR2_SYSCLK_DIV_CTL1_REG, Offset: 0x4 */
#define CPR2_APB1_CLK_DIV_Pos                       0U
#define CPR2_APB1_CLK_DIV_Msk                       (0xFU << CPR2_APB1_CLK_DIV_Pos)
#define CPR2_APB1_CLK_DIV(n)                        (((n) - 1) << CPR2_APB1_CLK_DIV_Pos)

/* CPR2 CPR2_TIM2_CTL_REG, Offset: 0x8 */
#define CPR2_TIM2_CG_Pos                            0U
#define CPR2_TIM2_CG_Msk                            (0x1U << CPR2_TIM2_CG_Pos)
#define CPR2_TIM2_CG_EN                             CPR2_TIM2_CG_Msk
#define CPR2_TIM2_SERIE_SEL_Pos                     4U
#define CPR2_TIM2_SERIE_SEL_Msk                     (0x1U << CPR2_TIM2_SERIE_SEL_Pos)
#define CPR2_TIM2_SERIE_SEL_EN                      CPR2_TIM2_SERIE_SEL_Msk
#define CPR2_TIM2_BUSY_Pos                          8U
#define CPR2_TIM2_BUSY_Msk                          (0x1U << CPR2_TIM2_BUSY_Pos)

/* CPR2 CPR2_TIM3_CTL_REG, Offset: 0xc */
#define CPR2_TIM3_CG_Pos                            0U
#define CPR2_TIM3_CG_Msk                            (0x1U << CPR2_TIM3_CG_Pos)
#define CPR2_TIM3_CG_EN                             CPR2_TIM3_CG_Msk
#define CPR2_TIM3_SERIE_SEL_Pos                     4U
#define CPR2_TIM3_SERIE_SEL_Msk                     (0x1U << CPR2_TIM3_SERIE_SEL_Pos)
#define CPR2_TIM3_SERIE_SEL_EN                      CPR2_TIM3_SERIE_SEL_Msk
#define CPR2_TIM3_BUSY_Pos                          8U
#define CPR2_TIM3_BUSY_Msk                          (0x1U << CPR2_TIM3_BUSY_Pos)

/* CPR2 GPIO1_CLK_CTL_REG, Offset: 0x10 */
#define CPR2_GPIO1_CG_Pos                            0U
#define CPR2_GPIO1_CG_Msk                            (0x1U << CPR2_GPIO1_CG_Pos)
#define CPR2_GPIO1_CG_EN                             CPR2_GPIO1_CG_Msk
#define CPR2_GPIO1_BUSY_Pos                          4U
#define CPR2_GPIO1_BUSY_Msk                          (0x1U << CPR2_GPIO1_BUSY_Pos)

/* CPR2 USI2_CLK_CTL_REG, Offset: 0x14 */
#define CPR2_USI2_CG_Pos                             0U
#define CPR2_USI2_CG_Msk                             (0x1U << CPR2_USI2_CG_Pos)
#define CPR2_USI2_CG_EN                              CPR2_USI2_CG_Msk
#define CPR2_USI2_BUSY_Pos                           4U
#define CPR2_USI2_BUSY_Msk                           (0x1U << CPR2_USI2_BUSY_Pos)

/* CPR2 USI3_CLK_CTL_REG, Offset: 0x18 */
#define CPR2_USI3_CG_Pos                             0U
#define CPR2_USI3_CG_Msk                             (0x1U << CPR2_USI3_CG_Pos)
#define CPR2_USI3_CG_EN                              CPR2_USI3_CG_Msk
#define CPR2_USI3_BUSY_Pos                           4U
#define CPR2_USI3_BUSY_Msk                           (0x1U << CPR2_USI3_BUSY_Pos)

/* CPR2 UART2_CLK_CTL_REG, Offset: 0x1C */
#define CPR2_UART2_CG_Pos                            0U
#define CPR2_UART2_CG_Msk                            (0x1U << CPR2_UART2_CG_Pos)
#define CPR2_UART2_CG_EN                             CPR2_UART2_CG_Msk
#define CPR2_UART2_BUSY_Pos                          4U
#define CPR2_UART2_BUSY_Msk                          (0x1U << CPR2_UART2_BUSY_Pos)

/* CPR2 UART3_CLK_CTL_REG, Offset: 0x20 */
#define CPR2_UART3_CG_Pos                            0U
#define CPR2_UART3_CG_Msk                            (0x1U << CPR2_UART3_CG_Pos)
#define CPR2_UART3_CG_EN                             CPR2_UART3_CG_Msk
#define CPR2_UART3_BUSY_Pos                          4U
#define CPR2_UART3_BUSY_Msk                          (0x1U << CPR2_UART3_BUSY_Pos)

/* CPR2 I2S2_CLK_CTL_REG, Offset: 0x24 */
#define CPR2_I2S2_CG_Pos                             0U
#define CPR2_I2S2_CG_Msk                             (0x1U << CPR2_I2S2_CG_Pos)
#define CPR2_I2S2_CG_EN                              CPR2_I2S2_CG_Msk
#define CPR2_I2S2_BUSY_Pos                           4U
#define CPR2_I2S2_BUSY_Msk                           (0x1U << CPR2_I2S2_BUSY_Pos)
#define CPR2_I2S23_CG_Pos                            8U
#define CPR2_I2S23_CG_Msk                            (0x1U << CPR2_I2S23_CG_Pos)
#define CPR2_I2S23_CG_EN                             CPR2_I2S23_CG_Msk

/* CPR2 I2S3_CLK_CTL_REG, Offset: 0x28 */
#define CPR2_I2S3_CG_Pos                             0U
#define CPR2_I2S3_CG_Msk                             (0x1U << CPR2_I2S3_CG_Pos)
#define CPR2_I2S3_CG_EN                              CPR2_I2S3_CG_Msk
#define CPR2_I2S3_BUSY_Pos                           4U
#define CPR2_I2S3_BUSY_Msk                           (0x1U << CPR2_I2S3_BUSY_Pos)

/* CPR2 SPDIF_CLK_CTL_REG, Offset: 0x2C */
#define CPR2_SPDIF_CG_Pos                             0U
#define CPR2_SPDIF_CG_Msk                             (0x1U << CPR2_SPDIF_CG_Pos)
#define CPR2_SPDIF_CG_EN                              CPR2_SPDIF_CG_Msk
#define CPR2_SPDIF_BUSY_Pos                           4U
#define CPR2_SPDIF_BUSY_Msk                           (0x1U << CPR2_SPDIF_BUSY_Pos)

/* CPR2 FMC_AHB1_CLK_CTL_REG, Offset: 0x30 */
#define CPR2_FMC_AHB1_CG_Pos                          0U
#define CPR2_FMC_AHB1_CG_Msk                          (0x1U << CPR2_FMC_AHB1_CG_Pos)
#define CPR2_FMC_AHB1_CG_EN                           CPR2_FMC_AHB1_CG_Msk
#define CPR2_FMC_AHB1_BUSY_Pos                        4U
#define CPR2_FMC_AHB1_BUSY_Msk                        (0x1U << CPR2_FMC_AHB1_BUSY_Pos)

/* CPR2 TDM_CLK_CTL_REG, Offset: 0x34 */
#define CPR2_TDM_CG_Pos                               0U
#define CPR2_TDM_CG_Msk                               (0x1U << CPR2_TDM_CG_Pos)
#define CPR2_TDM_CG_EN                                CPR2_TDM_CG_Msk
#define CPR2_TDM_BUSY_Pos                             4U
#define CPR2_TDM_BUSY_Msk                             (0x1U << CPR2_TDM_BUSY_Pos)

/* CPR2 PDM_CLK_CTL_REG, Offset: 0x38 */
#define CPR2_PDM_CG_Pos                               0U
#define CPR2_PDM_CG_Msk                               (0x1U << CPR2_PDM_CG_Pos)
#define CPR2_PDM_CG_EN                                CPR2_PDM_CG_Msk
#define CPR2_PDM_BUSY_Pos                             4U
#define CPR2_PDM_BUSY_Msk                             (0x1U << CPR2_PDM_BUSY_Pos)

/* CPR2 DMA1_CLK_CTL_REG, Offset: 0x3c */
#define CPR2_DMA1_CG_Pos                              0U
#define CPR2_DMA1_CG_Msk                              (0x1U << CPR2_DMA1_CG_Pos)
#define CPR2_DMA1_CG_EN                               CPR2_DMA1_CG_Msk
#define CPR2_DMA1_BUSY_Pos                            4U
#define CPR2_DMA1_BUSY_Msk                            (0x1U << CPR2_DMA1_BUSY_Pos)

/* CPR2 PWM1_CLK_CTL_REG, Offset: 0x40 */
#define CPR2_PWM1_CG_Pos                              0U
#define CPR2_PWM1_CG_Msk                              (0x1U << CPR2_PWM1_CG_Pos)
#define CPR2_PWM1_CG_EN                               CPR2_PWM1_CG_Msk
#define CPR2_PWM1_BUSY_Pos                            4U
#define CPR2_PWM1_BUSY_Msk                            (0x1U << CPR2_PWM1_BUSY_Pos)

/* CPR2 DMA_CH8_SEL_REG, Offset: 0x44 */
#define CPR2_DMA1_CH8_SEL_Pos                         0U
#define CPR2_DMA1_CH8_SEL_Msk                         (0x1FU << CPR2_DMA1_CH8_SEL_Pos)
#define CPR2_DMA1_CH8_SEL(n)                          ((n) << CPR2_DMA1_CH8_SEL_Pos)

/* CPR2 DMA_CH9_SEL_REG, Offset: 0x48 */
#define CPR2_DMA1_CH9_SEL_Pos                         0U
#define CPR2_DMA1_CH9_SEL_Msk                         (0x1FU << CPR2_DMA1_CH9_SEL_Pos)
#define CPR2_DMA1_CH9_SEL(n)                          ((n) << CPR2_DMA1_CH9_SEL_Pos)

/* CPR2 DMA_CH10_SEL_REG, Offset: 0x4c */
#define CPR2_DMA1_CH10_SEL_Pos                        0U
#define CPR2_DMA1_CH10_SEL_Msk                        (0x1FU << CPR2_DMA1_CH10_SEL_Pos)
#define CPR2_DMA1_CH10_SEL(n)                         ((n) << CPR2_DMA1_CH10_SEL_Pos)

/* CPR2 DMA_CH11_SEL_REG, Offset: 0x50 */
#define CPR2_DMA1_CH11_SEL_Pos                        0U
#define CPR2_DMA1_CH11_SEL_Msk                        (0x1FU << CPR2_DMA1_CH11_SEL_Pos)
#define CPR2_DMA1_CH11_SEL(n)                         ((n) << CPR2_DMA1_CH11_SEL_Pos)

/* CPR2 DMA_CH12_SEL_REG, Offset: 0x54 */
#define CPR2_DMA1_CH12_SEL_Pos                        0U
#define CPR2_DMA1_CH12_SEL_Msk                        (0x1FU << CPR2_DMA1_CH12_SEL_Pos)
#define CPR2_DMA1_CH12_SEL(n)                         ((n) << CPR2_DMA1_CH12_SEL_Pos)

/* CPR2 DMA_CH13_SEL_REG, Offset: 0x58 */
#define CPR2_DMA1_CH13_SEL_Pos                        0U
#define CPR2_DMA1_CH13_SEL_Msk                        (0x1FU << CPR2_DMA1_CH13_SEL_Pos)
#define CPR2_DMA1_CH13_SEL(n)                         ((n) << CPR2_DMA1_CH13_SEL_Pos)

/* CPR2 DMA_CH14_SEL_REG, Offset: 0x5c */
#define CPR2_DMA1_CH14_SEL_Pos                        0U
#define CPR2_DMA1_CH14_SEL_Msk                        (0x1FU << CPR2_DMA1_CH14_SEL_Pos)
#define CPR2_DMA1_CH14_SEL(n)                         ((n) << CPR2_DMA1_CH14_SEL_Pos)

/* CPR2 DMA_CH15_SEL_REG, Offset: 0x60 */
#define CPR2_DMA1_CH15_SEL_Pos                        0U
#define CPR2_DMA1_CH15_SEL_Msk                        (0x1FU << CPR2_DMA1_CH15_SEL_Pos)
#define CPR2_DMA1_CH15_SEL(n)                         ((n) << CPR2_DMA1_CH15_SEL_Pos)

/* CPR2 MS1SWRST, Offset: 0x64 */
#define CPR2_FMC1_RST_Pos                             0U
#define CPR2_FMC1_RST_Msk                             (0x1U << CPR2_FMC1_RST_Pos)
#define CPR2_FMC1_RST_EN                              CPR2_FMC1_RST_Msk
#define CPR2_FMC_SUB_RST_Pos                          1U
#define CPR2_FMC_SUB_RST_Msk                          (0x1U << CPR2_FMC_SUB_RST_Pos)
#define CPR2_FMC_SUB_RST_EN                           CPR2_FMC_SUB_RST_Msk
#define CPR2_MCC1_RST_Pos                             2U
#define CPR2_MCC1_RST_Msk                             (0x1U << CPR2_MCC1_RST_Pos)
#define CPR2_MCC1_RST_EN                              CPR2_MCC1_RST_Msk
#define CPR2_DMA1_RST_Pos                             3U
#define CPR2_DMA1_RST_Msk                             (0x1U << CPR2_DMA1_RST_Pos)
#define CPR2_DMA1_RST_EN                              CPR2_DMA1_RST_Msk

/* CPR2 APB1SWRST, Offset: 0x68 */
#define CPR2_CODEC_RST_Pos                            0U
#define CPR2_CODEC_RST_Msk                            (0x1U << CPR2_CODEC_RST_Pos)
#define CPR2_CODEC_RST_EN                             CPR2_CODEC_RST_Msk
#define CPR2_SPDIF_RST_Pos                            1U
#define CPR2_SPDIF_RST_Msk                            (0x1U << CPR2_SPDIF_RST_Pos)
#define CPR2_SPDIF_RST_EN                             CPR2_SPDIF_RST_Msk
#define CPR2_I2S2_RST_Pos                             2U
#define CPR2_I2S2_RST_Msk                             (0x1U << CPR2_I2S2_RST_Pos)
#define CPR2_I2S2_RST_EN                              CPR2_I2S2_RST_Msk
#define CPR2_I2S3_RST_Pos                             3U
#define CPR2_I2S3_RST_Msk                             (0x1U << CPR2_I2S3_RST_Pos)
#define CPR2_I2S3_RST_EN                              CPR2_I2S3_RST_Msk
#define CPR2_TDM_RST_Pos                              4U
#define CPR2_TDM_RST_Msk                              (0x1U << CPR2_TDM_RST_Pos)
#define CPR2_TDM_RST_EN                               CPR2_TDM_RST_Msk
#define CPR2_PDM_SUB_RST_Pos                          5U
#define CPR2_PDM_SUB_RST_Msk                          (0x1U << CPR2_PDM_SUB_RST_Pos)
#define CPR2_PDM_SUB_RST_EN                           CPR2_PDM_SUB_RST_Msk
#define CPR2_USI2_RST_Pos                             6U
#define CPR2_USI2_RST_Msk                             (0x1U << CPR2_USI2_RST_Pos)
#define CPR2_USI2_RST_EN                              CPR2_USI2_RST_Msk
#define CPR2_USI3_RST_Pos                             7U
#define CPR2_USI3_RST_Msk                             (0x1U << CPR2_USI3_RST_Pos)
#define CPR2_USI3_RST_EN                              CPR2_USI3_RST_Msk
#define CPR2_GPIO1_RST_Pos                            8U
#define CPR2_GPIO1_RST_Msk                            (0x1U << CPR2_GPIO1_RST_Pos)
#define CPR2_GPIO1_RST_EN                             CPR2_GPIO1_RST_Msk
#define CPR2_PWM1_RST_Pos                             9U
#define CPR2_PWM1_RST_Msk                             (0x1U << CPR2_PWM1_RST_Pos)
#define CPR2_PWM1_RST_EN                              CPR2_PWM1_RST_Msk
#define CPR2_TIM2_RST_Pos                             10U
#define CPR2_TIM2_RST_Msk                             (0x1U << CPR2_TIM2_RST_Pos)
#define CPR2_TIM2_RST_EN                              CPR2_TIM2_RST_Msk
#define CPR2_TIM3_RST_Pos                             11U
#define CPR2_TIM3_RST_Msk                             (0x1U << CPR2_TIM3_RST_Pos)
#define CPR2_TIM3_RST_EN                              CPR2_TIM3_RST_Msk
#define CPR2_UART2_RST_Pos                            12U
#define CPR2_UART2_RST_Msk                            (0x1U << CPR2_UART2_RST_Pos)
#define CPR2_UART2_RST_EN                             CPR2_UART2_RST_Msk
#define CPR2_UART3_RST_Pos                            13U
#define CPR2_UART3_RST_Msk                            (0x1U << CPR2_UART3_RST_Pos)
#define CPR2_UART3_RST_EN                             CPR2_UART3_RST_Msk

/* CPR2 CODEC _CLK _CTL_REG, Offset: 0x6C */
#define CPR2_CODEC_CG_Pos                             0U
#define CPR2_CODEC_CG_Msk                             (0x1U << CPR2_CODEC_CG_Pos)
#define CPR2_CODEC_CG_EN                              CPR2_CODEC_CG_Msk

/* CPR2 SYSCLK3_SEL_REG, Offset: 0x0 */
static inline void wj_pmu_write_sysclk3_sel_12m(wj_pmu_reg_cpr2_t *pmu_base)
{
    pmu_base->CPR2_SYSCLK_SEL_REG |= CPR2_SYSCLK3_SEL_12M;
}
static inline void wj_pmu_write_sysclk3_sel_d1(wj_pmu_reg_cpr2_t *pmu_base)
{
    pmu_base->CPR2_SYSCLK_SEL_REG &= CPR2_SYSCLK3_SEL_D1;
}
static inline uint32_t wj_pmu_read_sysclk3_sta(wj_pmu_reg_cpr2_t *pmu_base)
{
    return ((pmu_base->CPR2_SYSCLK_SEL_REG & CPR2_SYSCLK3_SEL_Msk) >> CPR2_SYSCLK3_SEL_Pos);
}

/* CPR2 CPR2_SYSCLK_DIV_CTL1_REG, Offset: 0x4 */
static inline void wj_pmu_write_apb1_div(wj_pmu_reg_cpr2_t *pmu_base, uint32_t div)
{
    pmu_base->CPR2_SYSCLK_DIV_CTL1_REG &= ~CPR2_APB1_CLK_DIV_Msk;
    pmu_base->CPR2_SYSCLK_DIV_CTL1_REG |= CPR2_APB1_CLK_DIV(div);
}
static inline uint32_t wj_pmu_read_apb1_sta(wj_pmu_reg_cpr2_t *pmu_base)
{
    return (((pmu_base->CPR2_SYSCLK_DIV_CTL1_REG & CPR2_APB1_CLK_DIV_Msk) >> CPR2_APB1_CLK_DIV_Pos) + 1U);
}

/* CPR2 CPR2_TIM2_CTL_REG, Offset: 0x8 */
static inline void wj_pmu_write_tim2_cg_en(wj_pmu_reg_cpr2_t *pmu_base)
{
    pmu_base->CPR2_TIM2_CTL_REG |= CPR2_TIM2_CG_EN;
}
static inline void wj_pmu_write_tim2_cg_dis(wj_pmu_reg_cpr2_t *pmu_base)
{
    pmu_base->CPR2_TIM2_CTL_REG &= ~CPR2_TIM2_CG_EN;
}
static inline void wj_pmu_write_tim2_serie_en(wj_pmu_reg_cpr2_t *pmu_base)
{
    pmu_base->CPR2_TIM2_CTL_REG |= CPR2_TIM2_SERIE_SEL_EN;
}
static inline void wj_pmu_write_tim2_serie_dis(wj_pmu_reg_cpr2_t *pmu_base)
{
    pmu_base->CPR2_TIM2_CTL_REG &= ~CPR2_TIM2_SERIE_SEL_EN;
}
static inline uint32_t wj_pmu_read_tim2_cg_sta(wj_pmu_reg_cpr2_t *pmu_base)
{
    return ((pmu_base->CPR2_TIM2_CTL_REG & CPR2_TIM2_BUSY_Msk) >> CPR2_TIM2_BUSY_Pos);
}

/* CPR2 CPR2_TIM3_CTL_REG, Offset: 0xc */
static inline void wj_pmu_write_tim3_cg_en(wj_pmu_reg_cpr2_t *pmu_base)
{
    pmu_base->CPR2_TIM3_CTL_REG |= CPR2_TIM3_CG_EN;
}
static inline void wj_pmu_write_tim3_cg_dis(wj_pmu_reg_cpr2_t *pmu_base)
{
    pmu_base->CPR2_TIM3_CTL_REG &= ~CPR2_TIM3_CG_EN;
}
static inline void wj_pmu_write_tim3_serie_en(wj_pmu_reg_cpr2_t *pmu_base)
{
    pmu_base->CPR2_TIM3_CTL_REG |= CPR2_TIM3_SERIE_SEL_EN;
}
static inline void wj_pmu_write_tim3_serie_dis(wj_pmu_reg_cpr2_t *pmu_base)
{
    pmu_base->CPR2_TIM3_CTL_REG &= ~CPR2_TIM3_SERIE_SEL_EN;
}
static inline uint32_t wj_pmu_read_tim3_cg_sta(wj_pmu_reg_cpr2_t *pmu_base)
{
    return ((pmu_base->CPR2_TIM3_CTL_REG & CPR2_TIM3_BUSY_Msk) >> CPR2_TIM3_BUSY_Pos);
}

/* CPR2 GPIO1_CLK_CTL_REG, Offset: 0x10 */
static inline void wj_pmu_write_gpio1_cg_en(wj_pmu_reg_cpr2_t *pmu_base)
{
    pmu_base->CPR2_GPIO1_CTL_REG |= CPR2_GPIO1_CG_EN;
}
static inline void wj_pmu_write_gpio1_cg_dis(wj_pmu_reg_cpr2_t *pmu_base)
{
    pmu_base->CPR2_GPIO1_CTL_REG &= ~CPR2_GPIO1_CG_EN;
}
static inline uint32_t wj_pmu_read_gpio1_cg_sta(wj_pmu_reg_cpr2_t *pmu_base)
{
    return ((pmu_base->CPR2_GPIO1_CTL_REG & CPR2_GPIO1_BUSY_Msk) >> CPR2_GPIO1_BUSY_Pos);
}

/* CPR2 USI2_CLK_CTL_REG, Offset: 0x14 */
static inline void wj_pmu_write_usi2_cg_en(wj_pmu_reg_cpr2_t *pmu_base)
{
    pmu_base->CPR2_USI2_CTL_REG |= CPR2_USI2_CG_EN;
}
static inline void wj_pmu_write_usi2_cg_dis(wj_pmu_reg_cpr2_t *pmu_base)
{
    pmu_base->CPR2_USI2_CTL_REG &= ~CPR2_USI2_CG_EN;
}
static inline uint32_t wj_pmu_read_usi2_cg_sta(wj_pmu_reg_cpr2_t *pmu_base)
{
    return ((pmu_base->CPR2_USI2_CTL_REG & CPR2_USI2_BUSY_Msk) >> CPR2_USI2_BUSY_Pos);
}

/* CPR2 USI3_CLK_CTL_REG, Offset: 0x18 */
static inline void wj_pmu_write_usi3_cg_en(wj_pmu_reg_cpr2_t *pmu_base)
{
    pmu_base->CPR2_USI3_CTL_REG |= CPR2_USI3_CG_EN;
}
static inline void wj_pmu_write_usi3_cg_dis(wj_pmu_reg_cpr2_t *pmu_base)
{
    pmu_base->CPR2_USI3_CTL_REG &= ~CPR2_USI3_CG_EN;
}
static inline uint32_t wj_pmu_read_usi3_cg_sta(wj_pmu_reg_cpr2_t *pmu_base)
{
    return ((pmu_base->CPR2_USI3_CTL_REG & CPR2_USI3_BUSY_Msk) >> CPR2_USI3_BUSY_Pos);
}

/* CPR2 UART2_CLK_CTL_REG, Offset: 0x1C */
static inline void wj_pmu_write_uart2_cg_en(wj_pmu_reg_cpr2_t *pmu_base)
{
    pmu_base->CPR2_UART2_CLK_CTL_REG |= CPR2_UART2_CG_EN;
}
static inline void wj_pmu_write_uart2_cg_dis(wj_pmu_reg_cpr2_t *pmu_base)
{
    pmu_base->CPR2_UART2_CLK_CTL_REG &= ~CPR2_UART2_CG_EN;
}
static inline uint32_t wj_pmu_read_uart2_cg_sta(wj_pmu_reg_cpr2_t *pmu_base)
{
    return ((pmu_base->CPR2_UART2_CLK_CTL_REG & CPR2_UART2_BUSY_Msk) >> CPR2_UART2_BUSY_Pos);
}

/* CPR2 UART3_CLK_CTL_REG, Offset: 0x20 */
static inline void wj_pmu_write_uart3_cg_en(wj_pmu_reg_cpr2_t *pmu_base)
{
    pmu_base->CPR2_UART3_CLK_CTL_REG |= CPR2_UART3_CG_EN;
}
static inline void wj_pmu_write_uart3_cg_dis(wj_pmu_reg_cpr2_t *pmu_base)
{
    pmu_base->CPR2_UART3_CLK_CTL_REG &= ~CPR2_UART3_CG_EN;
}
static inline uint32_t wj_pmu_read_uart3_cg_sta(wj_pmu_reg_cpr2_t *pmu_base)
{
    return ((pmu_base->CPR2_UART3_CLK_CTL_REG & CPR2_UART3_BUSY_Msk) >> CPR2_UART3_BUSY_Pos);
}

/* CPR2 I2S2_CLK_CTL_REG, Offset: 0x24 */
static inline void wj_pmu_write_i2s2_cg_en(wj_pmu_reg_cpr2_t *pmu_base)
{
    pmu_base->CPR2_I2S2_CTL_REG |= CPR2_I2S2_CG_EN;
}
static inline void wj_pmu_write_i2s2_cg_dis(wj_pmu_reg_cpr2_t *pmu_base)
{
    pmu_base->CPR2_I2S2_CTL_REG &= ~CPR2_I2S2_CG_EN;
}
static inline uint32_t wj_pmu_read_i2s2_cg_sta(wj_pmu_reg_cpr2_t *pmu_base)
{
    return ((pmu_base->CPR2_I2S2_CTL_REG & CPR2_I2S2_BUSY_Msk) >> CPR2_I2S2_BUSY_Pos);
}
static inline void wj_pmu_write_i2s23_en(wj_pmu_reg_cpr2_t *pmu_base)
{
    pmu_base->CPR2_I2S2_CTL_REG |= CPR2_I2S23_CG_EN;
}
static inline void wj_pmu_write_i2s23_dis(wj_pmu_reg_cpr2_t *pmu_base)
{
    pmu_base->CPR2_I2S2_CTL_REG &= ~CPR2_I2S23_CG_EN;
}

/* CPR2 I2S3_CLK_CTL_REG, Offset: 0x28 */
static inline void wj_pmu_write_i2s3_cg_en(wj_pmu_reg_cpr2_t *pmu_base)
{
    pmu_base->CPR2_I2S3_CTL_REG |= CPR2_I2S3_CG_EN;
}
static inline void wj_pmu_write_i2s3_cg_dis(wj_pmu_reg_cpr2_t *pmu_base)
{
    pmu_base->CPR2_I2S3_CTL_REG &= ~CPR2_I2S3_CG_EN;
}
static inline uint32_t wj_pmu_read_i2s3_cg_sta(wj_pmu_reg_cpr2_t *pmu_base)
{
    return ((pmu_base->CPR2_I2S3_CTL_REG & CPR2_I2S3_BUSY_Msk) >> CPR2_I2S3_BUSY_Pos);
}

/* CPR2 SPDIF_CLK_CTL_REG, Offset: 0x2C */
static inline void wj_pmu_write_spdif_cg_en(wj_pmu_reg_cpr2_t *pmu_base)
{
    pmu_base->CPR2_SPDIF_CTL_REG |= CPR2_SPDIF_CG_EN;
}
static inline void wj_pmu_write_spdif_cg_dis(wj_pmu_reg_cpr2_t *pmu_base)
{
    pmu_base->CPR2_SPDIF_CTL_REG &= ~CPR2_SPDIF_CG_EN;
}
static inline uint32_t wj_pmu_read_spdif_cg_sta(wj_pmu_reg_cpr2_t *pmu_base)
{
    return ((pmu_base->CPR2_SPDIF_CTL_REG & CPR2_SPDIF_BUSY_Msk) >> CPR2_SPDIF_BUSY_Pos);
}

/* CPR2 FMC_AHB1_CLK_CTL_REG, Offset: 0x30 */
static inline void wj_pmu_write_ahb1_cg_en(wj_pmu_reg_cpr2_t *pmu_base)
{
    pmu_base->CPR2_FMC_AHB1_CTL_REG |= CPR2_FMC_AHB1_CG_EN;
}
static inline void wj_pmu_write_ahb1_cg_dis(wj_pmu_reg_cpr2_t *pmu_base)
{
    pmu_base->CPR2_FMC_AHB1_CTL_REG &= ~CPR2_FMC_AHB1_CG_EN;
}
static inline uint32_t wj_pmu_read_ahb1_cg_sta(wj_pmu_reg_cpr2_t *pmu_base)
{
    return ((pmu_base->CPR2_FMC_AHB1_CTL_REG & CPR2_FMC_AHB1_BUSY_Msk) >> CPR2_FMC_AHB1_BUSY_Pos);
}

/* CPR2 TDM_CLK_CTL_REG, Offset: 0x34 */
static inline void wj_pmu_write_tdm_cg_en(wj_pmu_reg_cpr2_t *pmu_base)
{
    pmu_base->CPR2_TDM_CTL_REG |= CPR2_TDM_CG_EN;
}
static inline void wj_pmu_write_tdm_cg_dis(wj_pmu_reg_cpr2_t *pmu_base)
{
    pmu_base->CPR2_TDM_CTL_REG &= ~CPR2_TDM_CG_EN;
}
static inline uint32_t wj_pmu_read_tdm_cg_sta(wj_pmu_reg_cpr2_t *pmu_base)
{
    return ((pmu_base->CPR2_TDM_CTL_REG & CPR2_TDM_BUSY_Msk) >> CPR2_TDM_BUSY_Pos);
}

/* CPR2 PDM_CLK_CTL_REG, Offset: 0x38 */
static inline void wj_pmu_write_pdm_cg_en(wj_pmu_reg_cpr2_t *pmu_base)
{
    pmu_base->CPR2_PDM_CTL_REG |= CPR2_PDM_CG_EN;
}
static inline void wj_pmu_write_pdm_cg_dis(wj_pmu_reg_cpr2_t *pmu_base)
{
    pmu_base->CPR2_PDM_CTL_REG &= ~CPR2_PDM_CG_EN;
}
static inline uint32_t wj_pmu_read_pdm_cg_sta(wj_pmu_reg_cpr2_t *pmu_base)
{
    return ((pmu_base->CPR2_PDM_CTL_REG & CPR2_PDM_BUSY_Msk) >> CPR2_PDM_BUSY_Pos);
}

/* CPR2 DMA1_CLK_CTL_REG, Offset: 0x3c */
static inline void wj_pmu_write_dma1_cg_en(wj_pmu_reg_cpr2_t *pmu_base)
{
    pmu_base->CPR2_DMA1_CLK_CTL_REG |= CPR2_DMA1_CG_EN;
}
static inline void wj_pmu_write_dma1_cg_dis(wj_pmu_reg_cpr2_t *pmu_base)
{
    pmu_base->CPR2_DMA1_CLK_CTL_REG &= ~CPR2_DMA1_CG_EN;
}
static inline uint32_t wj_pmu_read_dma1_cg_sta(wj_pmu_reg_cpr2_t *pmu_base)
{
    return ((pmu_base->CPR2_DMA1_CLK_CTL_REG & CPR2_DMA1_BUSY_Msk) >> CPR2_DMA1_BUSY_Pos);
}

/* CPR2 PWM1_CLK_CTL_REG, Offset: 0x40 */
static inline void wj_pmu_write_pwm1_cg_en(wj_pmu_reg_cpr2_t *pmu_base)
{
    pmu_base->CPR2_PWM1_CLK_CTL_REG |= CPR2_PWM1_CG_EN;
}
static inline void wj_pmu_write_pwm1_cg_dis(wj_pmu_reg_cpr2_t *pmu_base)
{
    pmu_base->CPR2_PWM1_CLK_CTL_REG &= ~CPR2_PWM1_CG_EN;
}
static inline uint32_t wj_pmu_read_pwm1_cg_sta(wj_pmu_reg_cpr2_t *pmu_base)
{
    return ((pmu_base->CPR2_PWM1_CLK_CTL_REG & CPR2_PWM1_BUSY_Msk) >> CPR2_PWM1_BUSY_Pos);
}

/* CPR2 DMA_CH8_SEL_REG, Offset: 0x44 */
static inline void wj_pmu_write_dma1_ch8_mux(wj_pmu_reg_cpr2_t *pmu_base, uint32_t mux)
{
    pmu_base->CPR2_DMA_CH8_SEL_REG = CPR2_DMA1_CH8_SEL(mux);
}
static inline uint32_t wj_pmu_read_dma1_ch8_mux(wj_pmu_reg_cpr2_t *pmu_base)
{
    return ((pmu_base->CPR2_DMA_CH8_SEL_REG & CPR2_DMA1_CH8_SEL_Msk) >> CPR2_DMA1_CH8_SEL_Pos);
}

/* CPR2 DMA_CH9_SEL_REG, Offset: 0x48 */
static inline void wj_pmu_write_dma1_ch9_mux(wj_pmu_reg_cpr2_t *pmu_base, uint32_t mux)
{
    pmu_base->CPR2_DMA_CH9_SEL_REG = CPR2_DMA1_CH9_SEL(mux);
}
static inline uint32_t wj_pmu_read_dma1_ch9_mux(wj_pmu_reg_cpr2_t *pmu_base)
{
    return ((pmu_base->CPR2_DMA_CH9_SEL_REG & CPR2_DMA1_CH9_SEL_Msk) >> CPR2_DMA1_CH9_SEL_Pos);
}

/* CPR2 DMA_CH10_SEL_REG, Offset: 0x4c */
static inline void wj_pmu_write_dma1_ch10_mux(wj_pmu_reg_cpr2_t *pmu_base, uint32_t mux)
{
    pmu_base->CPR2_DMA_CH10_SEL_REG = CPR2_DMA1_CH10_SEL(mux);
}
static inline uint32_t wj_pmu_read_dma1_ch10_mux(wj_pmu_reg_cpr2_t *pmu_base)
{
    return ((pmu_base->CPR2_DMA_CH10_SEL_REG & CPR2_DMA1_CH10_SEL_Msk) >> CPR2_DMA1_CH10_SEL_Pos);
}

/* CPR2 DMA_CH11_SEL_REG, Offset: 0x50 */
static inline void wj_pmu_write_dma1_ch11_mux(wj_pmu_reg_cpr2_t *pmu_base, uint32_t mux)
{
    pmu_base->CPR2_DMA_CH11_SEL_REG = CPR2_DMA1_CH11_SEL(mux);
}
static inline uint32_t wj_pmu_read_dma1_ch11_mux(wj_pmu_reg_cpr2_t *pmu_base)
{
    return ((pmu_base->CPR2_DMA_CH11_SEL_REG & CPR2_DMA1_CH11_SEL_Msk) >> CPR2_DMA1_CH11_SEL_Pos);
}

/* CPR2 DMA_CH12_SEL_REG, Offset: 0x54 */
static inline void wj_pmu_write_dma1_ch12_mux(wj_pmu_reg_cpr2_t *pmu_base, uint32_t mux)
{
    pmu_base->CPR2_DMA_CH12_SEL_REG = CPR2_DMA1_CH12_SEL(mux);
}
static inline uint32_t wj_pmu_read_dma1_ch12_mux(wj_pmu_reg_cpr2_t *pmu_base)
{
    return ((pmu_base->CPR2_DMA_CH12_SEL_REG & CPR2_DMA1_CH12_SEL_Msk) >> CPR2_DMA1_CH12_SEL_Pos);
}

/* CPR2 DMA_CH13_SEL_REG, Offset: 0x58 */
static inline void wj_pmu_write_dma1_ch13_mux(wj_pmu_reg_cpr2_t *pmu_base, uint32_t mux)
{
    pmu_base->CPR2_DMA_CH13_SEL_REG = CPR2_DMA1_CH13_SEL(mux);
}
static inline uint32_t wj_pmu_read_dma1_ch13_mux(wj_pmu_reg_cpr2_t *pmu_base)
{
    return ((pmu_base->CPR2_DMA_CH13_SEL_REG & CPR2_DMA1_CH13_SEL_Msk) >> CPR2_DMA1_CH13_SEL_Pos);
}

/* CPR2 DMA_CH14_SEL_REG, Offset: 0x5c */
static inline void wj_pmu_write_dma1_ch14_mux(wj_pmu_reg_cpr2_t *pmu_base, uint32_t mux)
{
    pmu_base->CPR2_DMA_CH14_SEL_REG = CPR2_DMA1_CH14_SEL(mux);
}
static inline uint32_t wj_pmu_read_dma1_ch14_mux(wj_pmu_reg_cpr2_t *pmu_base)
{
    return ((pmu_base->CPR2_DMA_CH14_SEL_REG & CPR2_DMA1_CH14_SEL_Msk) >> CPR2_DMA1_CH14_SEL_Pos);
}

/* CPR2 DMA_CH15_SEL_REG, Offset: 0x60 */
static inline void wj_pmu_write_dma1_ch15_mux(wj_pmu_reg_cpr2_t *pmu_base, uint32_t mux)
{
    pmu_base->CPR2_DMA_CH15_SEL_REG = CPR2_DMA1_CH15_SEL(mux);
}
static inline uint32_t wj_pmu_read_dma1_ch15_mux(wj_pmu_reg_cpr2_t *pmu_base)
{
    return ((pmu_base->CPR2_DMA_CH15_SEL_REG & CPR2_DMA1_CH15_SEL_Msk) >> CPR2_DMA1_CH15_SEL_Pos);
}

/* CPR2 MS1SWRST, Offset: 0x64 */
static inline void wj_pmu_write_fmc1_rst_en(wj_pmu_reg_cpr2_t *pmu_base)
{
    pmu_base->CPR2_MS1SWRST |= CPR2_FMC1_RST_EN;
}
static inline void wj_pmu_write_fmc1_rst_dis(wj_pmu_reg_cpr2_t *pmu_base)
{
    pmu_base->CPR2_MS1SWRST &= ~CPR2_FMC1_RST_EN;
}
static inline uint32_t wj_pmu_read_fmc1_rst_sta(wj_pmu_reg_cpr2_t *pmu_base)
{
    return ((pmu_base->CPR2_MS1SWRST & CPR2_FMC1_RST_Msk) >> CPR2_FMC1_RST_Pos);
}
static inline void wj_pmu_write_fmc_sub_rst_en(wj_pmu_reg_cpr2_t *pmu_base)
{
    pmu_base->CPR2_MS1SWRST |= CPR2_FMC_SUB_RST_EN;
}
static inline void wj_pmu_write_fmc_sub_rst_dis(wj_pmu_reg_cpr2_t *pmu_base)
{
    pmu_base->CPR2_MS1SWRST &= ~CPR2_FMC_SUB_RST_EN;
}
static inline uint32_t wj_pmu_read_fmc_sub_rst_sta(wj_pmu_reg_cpr2_t *pmu_base)
{
    return ((pmu_base->CPR2_MS1SWRST & CPR2_FMC_SUB_RST_Msk) >> CPR2_FMC_SUB_RST_Pos);
}
static inline void wj_pmu_write_mcc1_rst_en(wj_pmu_reg_cpr2_t *pmu_base)
{
    pmu_base->CPR2_MS1SWRST |= CPR2_MCC1_RST_EN;
}
static inline void wj_pmu_write_mcc1_rst_dis(wj_pmu_reg_cpr2_t *pmu_base)
{
    pmu_base->CPR2_MS1SWRST &= ~CPR2_MCC1_RST_EN;
}
static inline uint32_t wj_pmu_read_mcc1_rst_sta(wj_pmu_reg_cpr2_t *pmu_base)
{
    return ((pmu_base->CPR2_MS1SWRST & CPR2_MCC1_RST_Msk) >> CPR2_MCC1_RST_Pos);
}
static inline void wj_pmu_write_dma1_rst_en(wj_pmu_reg_cpr2_t *pmu_base)
{
    pmu_base->CPR2_MS1SWRST |= CPR2_DMA1_RST_EN;
}
static inline void wj_pmu_write_dma1_rst_dis(wj_pmu_reg_cpr2_t *pmu_base)
{
    pmu_base->CPR2_MS1SWRST &= ~CPR2_DMA1_RST_EN;
}
static inline uint32_t wj_pmu_read_dma1_rst_sta(wj_pmu_reg_cpr2_t *pmu_base)
{
    return ((pmu_base->CPR2_MS1SWRST & CPR2_DMA1_RST_Msk) >> CPR2_DMA1_RST_Pos);
}

/* CPR2 APB1SWRST, Offset: 0x68 */
static inline void wj_pmu_write_codec_rst_en(wj_pmu_reg_cpr2_t *pmu_base)
{
    pmu_base->CPR2_APB1SWRST |= CPR2_CODEC_RST_EN;
}
static inline void wj_pmu_write_codec_rst_dis(wj_pmu_reg_cpr2_t *pmu_base)
{
    pmu_base->CPR2_APB1SWRST &= ~CPR2_CODEC_RST_EN;
}
static inline uint32_t wj_pmu_read_codec_rst_sta(wj_pmu_reg_cpr2_t *pmu_base)
{
    return ((pmu_base->CPR2_APB1SWRST & CPR2_CODEC_RST_Msk) >> CPR2_CODEC_RST_Pos);
}
static inline void wj_pmu_write_spdif_rst_en(wj_pmu_reg_cpr2_t *pmu_base)
{
    pmu_base->CPR2_APB1SWRST |= CPR2_SPDIF_RST_EN;
}
static inline void wj_pmu_write_spdif_rst_dis(wj_pmu_reg_cpr2_t *pmu_base)
{
    pmu_base->CPR2_APB1SWRST &= ~CPR2_SPDIF_RST_EN;
}
static inline uint32_t wj_pmu_read_spdif_rst_sta(wj_pmu_reg_cpr2_t *pmu_base)
{
    return ((pmu_base->CPR2_APB1SWRST & CPR2_SPDIF_RST_Msk) >> CPR2_SPDIF_RST_Pos);
}
static inline void wj_pmu_write_i2s2_rst_en(wj_pmu_reg_cpr2_t *pmu_base)
{
    pmu_base->CPR2_APB1SWRST |= CPR2_I2S2_RST_EN;
}
static inline void wj_pmu_write_i2s2_rst_dis(wj_pmu_reg_cpr2_t *pmu_base)
{
    pmu_base->CPR2_APB1SWRST &= ~CPR2_I2S2_RST_EN;
}
static inline uint32_t wj_pmu_read_i2s2_rst_sta(wj_pmu_reg_cpr2_t *pmu_base)
{
    return ((pmu_base->CPR2_APB1SWRST & CPR2_I2S2_RST_Msk) >> CPR2_I2S2_RST_Pos);
}
static inline void wj_pmu_write_i2s3_rst_en(wj_pmu_reg_cpr2_t *pmu_base)
{
    pmu_base->CPR2_APB1SWRST |= CPR2_I2S3_RST_EN;
}
static inline void wj_pmu_write_i2s3_rst_dis(wj_pmu_reg_cpr2_t *pmu_base)
{
    pmu_base->CPR2_APB1SWRST &= ~CPR2_I2S3_RST_EN;
}
static inline uint32_t wj_pmu_read_i2s3_rst_sta(wj_pmu_reg_cpr2_t *pmu_base)
{
    return ((pmu_base->CPR2_APB1SWRST & CPR2_I2S3_RST_Msk) >> CPR2_I2S3_RST_Pos);
}

static inline void wj_pmu_write_tdm_rst_en(wj_pmu_reg_cpr2_t *pmu_base)
{
    pmu_base->CPR2_APB1SWRST |= CPR2_TDM_RST_EN;
}
static inline void wj_pmu_write_tdm_rst_dis(wj_pmu_reg_cpr2_t *pmu_base)
{
    pmu_base->CPR2_APB1SWRST &= ~CPR2_TDM_RST_EN;
}
static inline uint32_t wj_pmu_read_tdm_rst_sta(wj_pmu_reg_cpr2_t *pmu_base)
{
    return ((pmu_base->CPR2_APB1SWRST & CPR2_TDM_RST_Msk) >> CPR2_TDM_RST_Pos);
}
static inline void wj_pmu_write_pdm_rst_en(wj_pmu_reg_cpr2_t *pmu_base)
{
    pmu_base->CPR2_APB1SWRST |= CPR2_PDM_SUB_RST_EN;
}
static inline void wj_pmu_write_pdm_rst_dis(wj_pmu_reg_cpr2_t *pmu_base)
{
    pmu_base->CPR2_APB1SWRST &= ~CPR2_PDM_SUB_RST_EN;
}
static inline uint32_t wj_pmu_read_pdm_rst_sta(wj_pmu_reg_cpr2_t *pmu_base)
{
    return ((pmu_base->CPR2_APB1SWRST & CPR2_PDM_SUB_RST_Msk) >> CPR2_PDM_SUB_RST_Pos);
}
static inline void wj_pmu_write_usi2_rst_en(wj_pmu_reg_cpr2_t *pmu_base)
{
    pmu_base->CPR2_APB1SWRST |= CPR2_USI2_RST_EN;
}
static inline void wj_pmu_write_usi2_rst_dis(wj_pmu_reg_cpr2_t *pmu_base)
{
    pmu_base->CPR2_APB1SWRST &= ~CPR2_USI2_RST_EN;
}
static inline uint32_t wj_pmu_read_usi2_rst_sta(wj_pmu_reg_cpr2_t *pmu_base)
{
    return ((pmu_base->CPR2_APB1SWRST & CPR2_USI2_RST_Msk) >> CPR2_USI2_RST_Pos);
}
static inline void wj_pmu_write_usi3_rst_en(wj_pmu_reg_cpr2_t *pmu_base)
{
    pmu_base->CPR2_APB1SWRST |= CPR2_USI3_RST_EN;
}
static inline void wj_pmu_write_usi3_rst_dis(wj_pmu_reg_cpr2_t *pmu_base)
{
    pmu_base->CPR2_APB1SWRST &= ~CPR2_USI3_RST_EN;
}
static inline uint32_t wj_pmu_read_usi3_rst_sta(wj_pmu_reg_cpr2_t *pmu_base)
{
    return ((pmu_base->CPR2_APB1SWRST & CPR2_USI3_RST_Msk) >> CPR2_USI3_RST_Pos);
}

static inline void wj_pmu_write_gpio1_rst_en(wj_pmu_reg_cpr2_t *pmu_base)
{
    pmu_base->CPR2_APB1SWRST |= CPR2_GPIO1_RST_EN;
}
static inline void wj_pmu_write_gpio1_rst_dis(wj_pmu_reg_cpr2_t *pmu_base)
{
    pmu_base->CPR2_APB1SWRST &= ~CPR2_GPIO1_RST_EN;
}
static inline uint32_t wj_pmu_read_gpio1_rst_sta(wj_pmu_reg_cpr2_t *pmu_base)
{
    return ((pmu_base->CPR2_APB1SWRST & CPR2_GPIO1_RST_Msk) >> CPR2_GPIO1_RST_Pos);
}
static inline void wj_pmu_write_pwm1_rst_en(wj_pmu_reg_cpr2_t *pmu_base)
{
    pmu_base->CPR2_APB1SWRST |= CPR2_PWM1_RST_EN;
}
static inline void wj_pmu_write_pwm1_rst_dis(wj_pmu_reg_cpr2_t *pmu_base)
{
    pmu_base->CPR2_APB1SWRST &= ~CPR2_PWM1_RST_EN;
}
static inline uint32_t wj_pmu_read_pwm1_rst_sta(wj_pmu_reg_cpr2_t *pmu_base)
{
    return ((pmu_base->CPR2_APB1SWRST & CPR2_PWM1_RST_Msk) >> CPR2_PWM1_RST_Pos);
}
static inline void wj_pmu_write_tim2_rst_en(wj_pmu_reg_cpr2_t *pmu_base)
{
    pmu_base->CPR2_APB1SWRST |= CPR2_TIM2_RST_EN;
}
static inline void wj_pmu_write_tim2_rst_dis(wj_pmu_reg_cpr2_t *pmu_base)
{
    pmu_base->CPR2_APB1SWRST &= ~CPR2_TIM2_RST_EN;
}
static inline uint32_t wj_pmu_read_tim2_rst_sta(wj_pmu_reg_cpr2_t *pmu_base)
{
    return ((pmu_base->CPR2_APB1SWRST & CPR2_TIM2_RST_Msk) >> CPR2_TIM2_RST_Pos);
}
static inline void wj_pmu_write_tim3_rst_en(wj_pmu_reg_cpr2_t *pmu_base)
{
    pmu_base->CPR2_APB1SWRST |= CPR2_TIM3_RST_EN;
}
static inline void wj_pmu_write_tim3_rst_dis(wj_pmu_reg_cpr2_t *pmu_base)
{
    pmu_base->CPR2_APB1SWRST &= ~CPR2_TIM3_RST_EN;
}
static inline uint32_t wj_pmu_read_tim3_rst_sta(wj_pmu_reg_cpr2_t *pmu_base)
{
    return ((pmu_base->CPR2_APB1SWRST & CPR2_TIM3_RST_Msk) >> CPR2_TIM3_RST_Pos);
}

static inline void wj_pmu_write_uart2_rst_en(wj_pmu_reg_cpr2_t *pmu_base)
{
    pmu_base->CPR2_APB1SWRST |= CPR2_UART2_RST_EN;
}
static inline void wj_pmu_write_uart2_rst_dis(wj_pmu_reg_cpr2_t *pmu_base)
{
    pmu_base->CPR2_APB1SWRST &= ~CPR2_UART2_RST_EN;
}
static inline uint32_t wj_pmu_read_uart2_rst_sta(wj_pmu_reg_cpr2_t *pmu_base)
{
    return ((pmu_base->CPR2_APB1SWRST & CPR2_UART2_RST_Msk) >> CPR2_UART2_RST_Pos);
}
static inline void wj_pmu_write_uart3_rst_en(wj_pmu_reg_cpr2_t *pmu_base)
{
    pmu_base->CPR2_APB1SWRST |= CPR2_UART3_RST_EN;
}
static inline void wj_pmu_write_uart3_rst_dis(wj_pmu_reg_cpr2_t *pmu_base)
{
    pmu_base->CPR2_APB1SWRST &= ~CPR2_UART3_RST_EN;
}
static inline uint32_t wj_pmu_read_uart3_rst_sta(wj_pmu_reg_cpr2_t *pmu_base)
{
    return ((pmu_base->CPR2_APB1SWRST & CPR2_UART3_RST_Msk) >> CPR2_UART3_RST_Pos);
}

/* CPR2 CODEC _CLK _CTL_REG, Offset: 0x6C */
static inline void wj_pmu_write_codec_cg_en(wj_pmu_reg_cpr2_t *pmu_base)
{
    pmu_base->CPR2_CODEC_CLK_CTL_REG |= CPR2_CODEC_CG_EN;
}
static inline void wj_pmu_write_codec_cg_dis(wj_pmu_reg_cpr2_t *pmu_base)
{
    pmu_base->CPR2_CODEC_CLK_CTL_REG &= ~CPR2_CODEC_CG_EN;
}
static inline uint32_t wj_pmu_read_codec_cg_sta(wj_pmu_reg_cpr2_t *pmu_base)
{
    return ((pmu_base->CPR2_CODEC_CLK_CTL_REG & CPR2_CODEC_CG_Msk) >> CPR2_CODEC_CG_Pos);
}

#ifdef __cplusplus
}
#endif

#endif
