/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

/**************************************************************************//**
 * @file     soc.h
 * @brief    CSI Core Peripheral Access Layer Header File for
 *           CSKYSOC Device Series
 * @version  V1.0
 * @date     06. Mar 2019
 * @vendor   csky
 * @chip     pangu
 ******************************************************************************/

#ifndef _SOC_H_
#define _SOC_H_

#ifndef SYSTEM_CLOCK
#define SYSTEM_CLOCK        (48000000U) // 48Mhz fpga
#define  IHS_VALUE          (48000000U)
#endif

#ifndef LSP_DEFAULT_FREQ
#define LSP_DEFAULT_FREQ    (48000000U) // 48Mhz fpga
#define EHS_VALUE           (12000000U)
#define ILS_VALUE           (12000000U)
#endif

#ifndef DSP_CLOCK
#define DSP_CLOCK           (250000000U) // 250Mhz
#endif

#ifndef RTC_CLOCK
#define RTC_CLOCK           (32768U)  // 32.768Mhz
#endif

#ifndef AUDIO_CLOCK
#define AUDIO_CLOCK         (49152000U)  // 49.152Mhz
#endif


/* ----------------------  YunVoice_V2 804ef Specific Interrupt Numbers  --------------------- */
typedef enum IRQn {
    ERROR_INTC_IRQn                    =   0,      /* error Interrupt */
    DW_WDT_IRQn                        =   1,      /* wdt Interrupt */
    WJ_PLL_IRQn                        =   2,      /* pll lose lock Interrupt */
    WJ_PLL_FRAC_IRQn                   =   3,      /* pll lose lock Interrupt */
    WJ_WCOM_IRQn                       =   4,      /* cpu0 wcom Interrupt */
    WJ_RCOM_IRQn                       =   5,      /* cpu0 rcom Interrupt */
    WJ_PMU_IRQn                        =   6,      /* PMU Interrupt */
    WJ_MCA_IRQn                        =   7,      /* MCA Interrupt */
    DW_DMAC0_IRQn                      =   8,      /* DMA0 Interrupt */
    DW_DMAC1_IRQn                      =   9,      /* DMA1 Interrupt */
    CORET_IRQn                         =   10,     /* CTIM0 Interrupt */
    WJ_RTC_IRQn                        =   11,     /* RTC Interrupt */
    WJ_SDIO1_IRQn                      =   12,     /* SDMMC Interrupt */
    WJ_FFT_IRQn                        =   13,     /* FFT Interrupt */
    CD_QSPI_IRQn                       =   14,     /* QSPI Interrupt */
    WJ_SDIO0_IRQn                      =   15,     /* SDIO Interrupt */
    WJ_CODEC_WHISPER_IRQn              =   16,     /* codec whisper Interrupt */
    WJ_CODEC_INTC_IRQn                 =   17,     /* code Interrupt */
    WJ_GSK_INTC_IRQn                   =   18,     /* gsk Interrupt */
    WJ_LPTIMER_INT0_IRQn               =   19,     /* timer0 Interrupt */
    WJ_LPTIMER_INT1_IRQn               =   20,     /* timer0  Interrupt */
    DW_TIMER0_INT0_IRQn                =   21,     /* timer1 Interrupt */
    DW_TIMER0_INT1_IRQn                =   22,     /* timer1 Interrupt */
    DW_TIMER1_INT0_IRQn                =   23,     /* timer2 Interrupt */
    DW_TIMER1_INT1_IRQn                =   24,     /* timer2  Interrupt */
    DW_TIMER2_INT0_IRQn                =   25,     /* timer3 Interrupt */
    DW_TIMER2_INT1_IRQn                =   26,     /* timer3  Interrupt */
    WJ_USI0_IRQn                       =   27,     /* USI0 Interrupt */
    WJ_USI1_IRQn                       =   28,     /* USI1 Interrupt */
    WJ_USI2_IRQn                       =   29,     /* USI2 Interrupt */
    WJ_USI3_IRQn                       =   30,     /* USI3 Interrupt */
    WJ_I2S0_IRQn                       =   31,     /* I2S0 Interrupt */
    WJ_I2S1_IRQn                       =   32,     /* I2S1 Interrupt */
    WJ_I2S2_IRQn                       =   33,     /* I2S2 Interrupt */
    WJ_I2S3_IRQn                       =   34,     /* I2S3 Interrupt */
    WJ_SPDIF0_IRQn                     =   35,     /* spdif0 Interrupt */
    DW_UART0_HS_IRQn                   =   36,     /* I2S0 Interrupt */
    DW_UART1_HS_IRQn                   =   37,     /* I2S1 Interrupt */
    DW_UART2_IRQn                      =   38,     /* I2S2 Interrupt */
    DW_UART3_IRQn                      =   39,     /* I2S3 Interrupt */
    WJ_TDM_IRQn                        =   40,     /* tdm Interrupt */
    WJ_PDM_IRQn                        =   41,     /* pdm Interrupt */
    DW_GPIO0_IRQn                      =   42,     /* gpio0 Interrupt */
    WJ_PLL_LL_IRQn                     =   43,     /* pll lose lock Interrupt */
    WJ_PLL_LL_FRAC_IRQn                =   44,     /* pll lose lock FRAC Interrupt */
    WJ_PWM0_IRQn                       =   45,     /* pwm0 Interrupt */
    WJ_RSA_IRQn                        =   46,     /* rsa Interrupt */
    WJ_SHA_IRQn                        =   47,     /* sha Interrupt */
    WJ_AES_IRQn                        =   48,     /* aes Interrupt */
    OSR_RNG_IRQn                       =   49,     /* trng Interrupt */
    DW_GPIO1_IRQn                      =   50,     /* gpio1 Interrupt */
    WJ_PWM1_IRQn                       =   51,     /* pwm1 Interrupt */
} irqn_type_t;

#define CONFIG_MAILBOX_NUM      3
#define CONFIG_MAILBOX_CHANNEL_NUM      2

/* ================================================================================ */
/* ================      Processor and Core Peripheral Section     ================ */
/* ================================================================================ */

/* --------  Configuration of the CK805 Processor and Core Peripherals  ------- */
#define __CK805_REV               0x0000U   /* Core revision r0p0 */
#define __MGU_PRESENT             0         /* MGU present or not */
#define __NVIC_PRIO_BITS          2         /* Number of Bits used for Priority Levels */

#include "csi_core.h"                     /* Processor and core peripherals */
#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    //DMA0
    DMA_HF_I2S0_TX,     //0
    DMA_HF_I2S0_RX,
    DMA_HF_I2S1_TX,
    DMA_HF_I2S1_RX,
    DMA_HF_USI0_TX,
    DMA_HF_USI0_RX,     //5
    DMA_HF_USI1_TX,
    DMA_HF_USI1_RX,
    DMA_HF_UART0_TX,
    DMA_HF_UART0_RX,
    DMA_HF_UART1_TX,    //10
    DMA_HF_UART1_RX,    //11
    //DMA1
    //fixed
    DMA_HF_CODEC_ADC_0, //12
    DMA_HF_CODEC_ADC_1,
    DMA_HF_CODEC_ADC_4,
    DMA_HF_CODEC_ADC_5, //15
    DMA_HF_UART2_TX,
    DMA_HF_UART2_RX,
    DMA_HF_USI2_TX,
    DMA_HF_USI2_RX,     //19
    //variable
    DMA_HF_I2S2_TX,     //20
    DMA_HF_I2S2_RX,
    DMA_HF_CODEC_ADC_6,
    DMA_HF_CODEC_ADC_7,
    DMA_HF_CODEC_DAC,
    DMA_HF_RESERVED_1,  //25
    DMA_HF_I2S3_TX,
    DMA_HF_I2S3_RX,
    DMA1_HF_USI3_TX,
    DMA1_HF_USI3_RX,
    DMA_HF_UART3_TX,    //30
    DMA_HF_UART3_RX,
    DMA_HF_RESERVED_2,  //32
    DMA_HF_RESERVED_3,
    DMA_HF_SPDIF_TX,
    DMA_HF_SPDIF_RX,    //35
    DMA_HF_RESERVED_4,
    DMA_HF_RESERVED_5,  //37
    DMA_HF_TDM_0,
    DMA_HF_TDM_1,
    DMA_HF_TDM_2,       //40
    DMA_HF_TDM_3,
    DMA_HF_TDM_4,       //42
    DMA_HF_TDM_5,
    DMA_HF_TDM_6,
    DMA_HF_TDM_7,       //45
    DMA_HF_PDM_0,
    DMA_HF_PDM_1,       //47
    DMA_HF_PDM_2,
    DMA_HF_PDM_3,
    DMA_HF_CODEC_ADC_2, //50
    DMA_HF_CODEC_ADC_3, //51
    DMA_HF_MEMORY,
} dw_dma_hf_t;

typedef enum {
    ADDR_SPACE_ERROR      = -1,
    ADDR_SPACE_EFLASH     =  0,
    ADDR_SPACE_SRAM       =  1,
    ADDR_SPACE_PERIPHERAL =  2,
    ADDR_SPACE_TCIP       =  3
} addr_space_e;

/* -------------------------  Wakeup Number Definition  ------------------------ */
typedef enum Wakeupn {
    DW_GPIO0_Wakeupn         =   0,     /* GPIO0 wakeup */
    DW_AONTIM_Wakeupn        =   1,     /* AONTIM wakeup */
    WJ_RTC_Wakeupn           =   2,     /* RTC wakeup */
    WJ_CODEC_WT_Wakeun       =   3      /* codec whisper wakeup */
} Wakeupn_Type_t;

typedef enum {
    PA0 = 0,
    PA1,
    PA2,
    PA3,
    PA4,
    PA5,
    PA6,
    PA7,
    PA8,
    PA9,
    PA10,
    PA11,
    PA12,
    PA13,
    PA14,
    PA15,
    PA16,
    PA17,
    PA18,
    PA19,
    PA20,
    PA21,
    PA22,
    PA23,
    PA24,
    PA25,
    PA26,
    PA27,
    PA28,
    PA29,
    PA30,
    PA31,

    PB0,
    PB1,
    PB2,
    PB3,
    PB4,
    PB5,
    PB6,
    PB7,
    PB8,
    PB9,
    PB10,
    PB11,
    PB12,
    PB13,
    PB14,
    PB15,
    PB16,
    PB17,
    PB18,
    PB19,
    PB20,
    PB21,
    PB22,
    PB23,
    PB24,
    PB25,
    PB26,
    PB27,
    PB28,
    PB29,
    PB30,
    PB31
} pin_name_t;

typedef enum {
    PA0_UART0_RX        = 0,
    PA1_UART0_RTS       = 0,                                               PA1_CODEC_I2C_DOUT           = 3,
    PA2_UART0_TX        = 0,                                               PA2_CODEC_I2C_DIN            = 3,
    PA3_I2S0_SCLK       = 0,                                               PA3_APB0_CLK_DIV_2M          = 3,
    PA4_I2S0_WSCLK      = 0,                                               PA4_RTC_32K                  = 3,
    PA5_I2S0_SDI        = 0,                                               PA5_OSC_32K                  = 3,
    PA6_I2S0_SDO        = 0,                                               PA6_CODEC_I2S_ADC12_BCLK     = 3,
    PA7_QSPI_SCLK       = 0,                                               PA7_CODEC_I2S_ADC12_LRO      = 3,
    PA8_QSPI_SS0        = 0,                                               PA8_CODEC_I2S_ADC12_SDATA    = 3,
    PA9_QSPI_DATA0      = 0,                                               PA9_CODEC_I2S_ADC34_BCLK     = 3,
    PA10_QSPI_DATA1     = 0,                                               PA10_CODEC_I2S_ADC34_LRO     = 3,
    PA11_QSPI_DATA2     = 0,                                               PA11_CODEC_I2S_ADC34_SDATA   = 3,
    PA12_QSPI_DATA3     = 0,                                               PA12_CODEC_I2S_ADC56_BCLK    = 3,
    PA13_SDIO_DAT2      = 0,                                               PA13_CODEC_I2S_ADC56_LRO     = 3,
    PA14_SDIO_DAT3      = 0,                                               PA14_CODEC_I2S_ADC56_SDATA   = 3,
    PA15_SDIO_CMD       = 0,                                               PA15_CODEC_I2S_ADC78_BCLK    = 3,
    PA16_SDIO_CLK       = 0,                                               PA16_CODEC_I2S_ADC78_LRO     = 3,
    PA17_SDIO_DAT0      = 0,                                               PA17_CODEC_I2S_ADC78_SDATA   = 3,
    PA18_SDIO_DAT1      = 0,                                               PA18_CODDEC_DAC_MBCLK        = 3,
    PA19_UART1_TX       = 0,                                               PA19_CODDEC_DAC_MLRO         = 3,
    PA20_UART1_RX       = 0,                        PA20_PWM0_O1     = 2,  PA20_CODEC_DAC_MSDATA        = 3,
    PA21_USI0_SCLK      = 0,                        PA21_PWM0_O3     = 2,
    PA22_USI0_SD0       = 0,                        PA22_PWM0_O5     = 2,
    PA23_EMMC_CLK       = 0,  PA23_I2S1_SCLK  = 1,  PA23_PWM0_O7     = 2,
    PA24_EMMC_DAT0      = 0,  PA24_I2S1_WSCLK = 1,  PA24_PWM0_O9     = 2,
    PA25_EMMC_DAT1      = 0,  PA25_I2S1_SDA   = 1,  PA25_PWM0_O11    = 2,
    PA26_EMMC_DAT2      = 0,  PA26_I2S1_MCLK  = 1,  PA26_PWM0_IO0    = 2,  PA26_USI1_SCLK            = 3,
    PA27_EMMC_DAT3      = 0,  PA27_JTG_TDI    = 1,  PA27_PWM0_IO2    = 2,  PA27_USI1_SD0             = 3,
    PA28_EMMC_CMD       = 0,  PA28_JTG_TDO    = 1,  PA28_PWM0_IO4    = 2,  PA28_USI1_SD1             = 3,
    PA29_UART0_CTS      = 0,  PA29_JTG_TRST   = 1,  PA29_PWM0_IO6    = 2,  PA29_USI1_NSS             = 3,
    PA30_CPU_TCLK       = 0,  PA30_JTG_TCK    = 1,  PA30_PWM0_IO8    = 2,  PA30_USI0_SD1             = 3,
    PA31_CPU_TMS        = 0,  PA31_JTG_TMS    = 1,  PA31_PWM0_IO10   = 2,  PA31_USI0_NSS             = 3,
    PB0_I2S2_SCLK       = 0,  PB0_DMIC_CLK11  = 1,  PB0_UART3_TX     = 2,
    PB1_I2S2_WSCLK      = 0,  PB1_DMIC_CLK12  = 1,  PB1_UART3_RX     = 2,
    PB2_I2S2_SDA        = 0,  PB2_DMIC_IN1    = 1,
    PB3_I2S2_MCLK       = 0,  PB3_DMIC_CLK21  = 1,
    PB4_I2S3_SCLK       = 0,  PB4_DMIC_CLK22  = 1,  PB4_USI3_SCLK    = 2,
    PB5_I2S3_WSCLK      = 0,  PB5_DMIC_IN2    = 1,  PB5_USI3_SD0     = 2,
    PB6_I2S3_SDA        = 0,  PB6_DMIC_CLK31  = 1,
    PB7_I2S3_MCLK       = 0,  PB7_DMIC_CLK32  = 1,
    PB8_DMIC_IN3        = 1,
    PB9_DMIC_CLK41      = 1,
    PB10_DMIC_CLK42     = 1,
    PB11_DMIC_IN4       = 1,
    PB12_SPDIF_DAT_IN   = 0,
    PB13_SPDIF_DAT_OUT  = 0,
    PB14_TDM_BCLK       = 1,
    PB15_TDM_WCLK       = 1,
    PB16_TDM_DAT        = 1,
    PB17_PDM_CLK        = 1,
    PB18_PDM_DAT0       = 1,
    PB19_PDM_DAT1       = 1,
    PB20_PWM1_O1        = 0,  PB20_PDM_DAT2  = 1,
    PB21_PWM1_O3        = 0,  PB21_PDM_DAT3  = 1,
    PB22_PWM1_O5        = 0,
    PB23_PWM1_O7        = 0,
    PB24_PWM1_O9        = 0,  PB24_UART2_TX  = 1,
    PB25_PWM1_O11       = 0,  PB25_UART2_RX  = 1,
    PB26_PWM1_IO0       = 0,  PB26_UART2_RTS = 1,
    PB27_PWM1_IO2       = 0,  PB27_UART2_CTS = 1,
    PB28_PWM1_IO4       = 0,  PB28_USI2_SCLK = 1,
    PB29_PWM1_IO6       = 0,  PB29_USI2_SD0  = 1,
    PB30_PWM1_IO8       = 0,  PB30_USI2_SD1  = 1,
    PB31_PWM1_IO10      = 0,  PB31_USI2_NSS  = 1,
    PIN_FUNC_GPIO       = 4,
} pin_func_t;

#define CONFIG_GPIO_NUM         2U

#define CONFIG_IRQ_NUM          54
/* ================================================================================ */
/* ================              Peripheral memory map             ================ */
/* ================================================================================ */
#define CD_QSPIMEM_BASE               0x08000000UL
#define WJ_CK804_TCM0_BASE            0xFFE0000UL
#define WJ_CK804_TCM1_BASE            0xFFE8000UL
#define WJ_CK804_TCM2_BASE            0xFFF0000UL
#define WJ_CK804_TCM3_BASE            0xFFF8000UL
#define WJ_CK804_TCM4_BASE            0x10000000UL
#define WJ_CK804_TCM5_BASE            0x10008000UL
#define WJ_CK804_TCM6_BASE            0x10010000UL
#define WJ_CK804_TCM7_BASE            0x10018000UL
#define WJ_CK804_TCM_SIZE             0x8000UL

#define WJ_FMC_BASE                   0x18000000UL
#define WJ_FMC_SIZE                   0x800000UL
#define WJ_SRAM_BASE                  WJ_CK804_TCM2_BASE

/* AHB SUB0 */
#define DW_SDIO0_BASE                 0x21000000UL
#define DW_SDIO1_BASE                 0x22000000UL
#define DW_DMAC0_BASE                 0x23000000UL
#define WJ_SASC_BASE                  0x24000000UL
/* AHB SUB1 */
#define WJ_AES_BASE                   0x28000000UL
#define WJ_SHA_BASE                   0x29000000UL
#define WJ_FFT_BASE                   (0x2A000000UL)
/* AHB SUB2 */
#define WJ_AONCPR_BASE                0x30000000UL
#define WJ_CPR1_BASE                  0x31000000UL
#define WJ_MCC0_BASE                  0x32000000UL
#define WJ_RSA_BASE                   0x33000000UL//(0x1A000000UL)//
#define OSR_RNG_BASE                  0x34000000UL

/* CPU0 CK804 APB0 */
#define DW_WDT_BASE                   0x38000000UL
#define WJ_RTC_BASE                   0x38800000UL
#define WJ_LPTIM6_BASE                0x39000000UL
#define WJ_LPTIM7_BASE                0x39000014UL

#define DW_TIM0_BASE                  0x39800000UL
#define DW_TIM1_BASE                  0x39800014UL
#define DW_TIM2_BASE                  0x90800000UL
#define DW_TIM3_BASE                  0x90800014UL
#define DW_TIM4_BASE                  0x91000000UL
#define DW_TIM5_BASE                  0x91000014UL

#define WJ_EFUSE_BASE                 0x3A000000UL
#define CD_QSPIC0_BASE                0x3A800000UL
#define WJ_PADREG_BASE                0x3B000000UL
#define WJ_TIPC_BASE                  0x3B800000UL
#define DW_GPIO0_BASE                 0x3C000000UL
#define WJ_PWM0_BASE                  0x3C800000UL
#define WJ_I2S0_BASE                  0x3D000000UL
#define WJ_I2S1_BASE                  0x3D800000UL
#define WJ_USI0_BASE                  0x3E000000UL
#define WJ_USI1_BASE                  0x3E800000UL
#define DW_UART0_BASE                 0x3F000000UL
#define DW_UART1_BASE                 0x3F800000UL

#define WJ_CK805_TCM0_BASE            0x80000000UL
#define WJ_CK805_TCM1_BASE            0x80020000UL
#define WJ_CK805_TCM0_SIZE            0x20000UL
#define WJ_CK805_TCM1_SIZE            0x10000UL
/* CK805 SUB0 */
#define DW_DMAC1_BASE                 0x88000000UL
#define WJ_MCA_BASE                   0x89000000UL
#define WJ_MCC1_BASE                  0x8A000000UL
#define WJ_CPR2_BASE                  0x8B000000UL

/* CK805 APB1 */
#define WJ_CODEC_BASE                 0x90000000UL
#define WJ_SPDIF_BASE                 0x91800000UL
#define WJ_PDM_BASE                   0x92000000UL
#define WJ_TDM_BASE                   0x92800000UL
#define DW_GPIO1_BASE                 0x93000000UL
#define WJ_PWM1_BASE                  0x93800000UL
#define WJ_I2S2_BASE                  0x94000000UL
#define WJ_I2S3_BASE                  0x94800000UL
#define WJ_USI2_BASE                  0x95000000UL
#define WJ_USI3_BASE                  0x95800000UL
#define DW_UART2_BASE                 0x96000000UL
#define DW_UART3_BASE                 0x96800000UL

#define SPIFLASH_BASE                 CD_QSPIMEM_BASE

#define DW_SDMMC_BASE_ADDR            DW_SDIO0_BASE
#define SHA_CONTEXT_SIZE              224
#define CONFIG_USI_V25
#define USI_USART_FUN_DIS             1
#define CONFIG_SDIO_NUM               2

#define CONFIG_RTC_FAMILY_A

/* ================================================================================ */
/* ================                  otp declaration               ================ */
/* ================================================================================ */
#define CONFIG_OTP_BASE_ADDR 0
#define CONFIG_OTP_BANK_SIZE 640

/* ================================================================================ */
/* ================             Peripheral declaration             ================ */
/* ================================================================================ */

#define YUN_PMU                   ((pmu_reg_t *)WJ_AONCPR_BASE)
#define YUN_CPR0                  ((cpr0_reg_t *)WJ_CPR0_BASE)
#define YUN_CPR1                  ((cpr1_reg_t *)WJ_CPR1_BASE)

#include <sys_clk.h>
#include <cpr_regs.h>
#include <pmu_regs.h>

#ifdef __cplusplus
}
#endif

#endif  /* SOC_H_ */
