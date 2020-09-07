/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

/**************************************************************************//**
 * @file     soc.h
 * @brief    CSI Core Peripheral Access Layer Header File for
 *           SILANSOC Device Series
 * @version  V1.0
 * @date     04. April 2019
 ******************************************************************************/

#ifndef _SOC_H_
#define _SOC_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef SYSTEM_CLOCK
#define  SYSTEM_CLOCK        (160000000)
#endif

#ifndef LSP_DEFAULT_FREQ
#define  LSP_DEFAULT_FREQ    (160000000)
#endif

#include "sys_freq.h"
/* -------------------------  Interrupt Number Definition  ------------------------ */

typedef enum IRQn {
    /* ----------------------  SILAN Specific Interrupt Numbers  --------------------- */
    UNKNOWN0_IRQn                    =   0,
    UNKNOWN1_IRQn                    =   1,
    SARADC_IRQn                      =   2,       /* ADC Interrupt */
    SPIFLASH_IRQn                    =   3,       /* SPIFLASH Interrupt */
    UART_IRQn                        =   4,       /* UART Interrupt */
    PDM_IRQn                         =   5,       /* PDM Interrupt */
    GPIO1_IRQn                       =   6,       /* GPIO1 Interrupt */
    GPIO2_IRQn                       =   7,       /* GPIO2 Interrupt */
    TIMER_IRQn                       =   8,       /* TIMER Interrupt */
    SDMAC_IRQn                       =   9,       /* SDMAC Interrupt */
    SDMMC_IRQn                       =   10,      /* SDMMC Interrupt */
    IIR_IRQn                         =   11,      /* IIR Interrupt */
    SDIO_IRQn                        =   12,      /* SDIO Interrupt */
    USBFS_IRQn                       =   13,      /* USBFS Interrupt */
    USBHS_IRQn                       =   14,      /* USBHS Interrupt */
    CXC_IRQn                         =   15,      /* CXC Interrupt */
    ADMAC_IRQn                       =   16,      /* ADMAC Interrupt */
    SPDIF_IRQn                       =   17,      /* SPDIF Interrupt */
    PCM_IRQn                         =   18,      /* PCM Interrupt */
    IIS_IRQn                         =   19,      /* IIS Interrupt */
    RTC_IRQn                         =   20,      /* RTC Interrupt */
    PMU_IRQn                         =   21,      /* PMU Interrupt */
    DMAC_IRQn                        =   22,      /* DMAC Interrupt */
    UNKNOWN2_IRQn                    =   23,
    UNKNOWN3_IRQn                    =   24,
    UNKNOWN4_IRQn                    =   25,
    UNKNOWN5_IRQn                    =   26,
    UNKNOWN6_IRQn                    =   27,
    UNKNOWN7_IRQn                    =   28,
    UNKNOWN8_IRQn                    =   29,
    UNKNOWN9_IRQn                    =   30,
    CORET_IRQn                       =   31,      /* core Timer Interrupt */
}
IRQn_Type;


/* ================================================================================ */
/* ================      Processor and Core Peripheral Section     ================ */
/* ================================================================================ */

/* --------  Configuration of the CK801 Processor and Core Peripherals  ------- */
#define __CK803_REV               0x0000U   /* Core revision r0p0 */
#define __MPU_PRESENT             1         /* MGU present or not */
#define __VIC_PRIO_BITS           2         /* Number of Bits used for Priority Levels */
#define __Vendor_SysTickConfig    1         /* Set to 1 if different SysTick Config is used */

#include "csi_core.h"                     /* Processor and core peripherals */
#include "stdint.h"

/* ================================================================================ */
/* ================       Device Specific Peripheral Section       ================ */
/* ================================================================================ */
#define CONFIG_TIMER_NUM           5
#define CONFIG_RTC_NUM             1
#define CONFIG_USART_NUM           4
#define CONFIG_GPIO_NUM            2
#define CONFIG_GPIO_PIN_NUM        42
#define CONFIG_SPIFLASH_NUM        1
#define CONFIG_IIC_NUM             2
#define CONFIG_ADC_NUM             1
#define CONFIG_PWM_NUM             8
#define CONFIG_I2S_NUM             7
#define CONFIG_WDT_NUM             1
#define CONFIG_SPI_NUM             2

/* ADC CHANNEL NUMBER */
#define ADC_IN0    0
#define ADC_IN1    1
#define ADC_IN2    2
#define ADC_IN3    3
#define ADC_IN4    4
#define ADC_IN5    5
#define ADC_IN6    6
#define ADC_IN7    7

/*I2S NUMBER*/
#define SILAN_I2S_ID_I1        0
#define SILAN_I2S_ID_I2        1
#define SILAN_I2S_ID_I3        2
#define SILAN_I2S_ID_O1        3
#define SILAN_I2S_ID_O2        4
#define SILAN_I2S_ID_O3        5
#define SILAN_I2S_ID_NONE      6
#define SILAN_I2S_ID_PCM       7


/* ================================================================================ */
/* ================              Peripheral memory map             ================ */
/* ================================================================================ */
/* --------------------------  CPU FPGA memory map  ------------------------------- */
#define CSKY_QSPIMEM_BASE           (0x03000000)
#define CSKY_UART_BASE              (0x41010000UL)

#define SILAN_SYSTEM_CTL_REG12      (0x1a000030)
#define SILAN_SYSTEM_CTL_REG14      (0x1a000038)

#define SILAN_SRAM1_BASE                    (0x20000000)
#define SILAN_SDRAM_BASE                    (0x22000000)
#define SILAN_DSP_DRAM_BASE                 (0x23000000)

#define SILAN_SPDIF_I_BASE                  (0x40000000)
#define SILAN_SPDIF_O_BASE                  (0x40010000)
#define SILAN_I1_I2S_BASE                   (0x40020000)
#define SILAN_I2_I2S_BASE                   (0x40030000)
#define SILAN_I3_I2S_BASE                   (0x40040000)
#define SILAN_O1_I2S_BASE                   (0x40050000)
#define SILAN_O2_I2S_BASE                   (0x40060000)
#define SILAN_O3_I2S_BASE                   (0x40070000)
#define SILAN_PCM_I2S_BASE                  (0x40090000)
#define SILAN_PDM_BASE                      (0x400B0000)
#define SILAN_IIR_DMA_BASE                  (0x40100000)

#define SILAN_UART1_BASE                    (0x41000000)
#define SILAN_UART2_BASE                    (0x41010000)
#define SILAN_UART3_BASE                    (0x41020000)
#define SILAN_UART4_BASE                    (0x41030000)
#define SILAN_SPI1_BASE                     (0x41040000)
#define SILAN_I2C1_BASE                     (0x41050000)
#define SILAN_I2C2_BASE                     (0x41060000)
#define SILAN_GPIO1_BASE                    (0x41070000)
#define SILAN_GPIO2_BASE                    (0x41080000)
#define SILAN_SPI2_BASE                     (0x410A0000)
#define SILAN_PDB_BASE                      (0x410B0000)
#define SILAN_ADC_BASE                      (0x410C0000)
#define SILAN_PWM_BASE                      (0x410D0000)
#define SILAN_SSP_BASE                      (0x410E0000)
#define SILAN_TIMER_BASE                    (0x41860000)
#define SILAN_MCU_WDOG_BASE                 (0x41870000)
#define SILAN_RISC_WDOG_BASE                (0x41880000)
#define SILAN_DSP_WDOG_BASE                 (0x41890000)

#define SILAN_SYSCFG_BASE                   (0x42010000)
#define SILAN_CXC_BASE                      (0x42020000)
#define SILAN_INTR_STS_BASE                 (0x42030000)
#define SILAN_SYSCTRL_BASE                  (0x42040000)
#define SILAN_DMA_REQ_BASE                  (0x42050000)
#define SILAN_RTC_BASE                      (0x42060000)
#define SILAN_CODEC1_BASE                   (0x42061000)
#define SILAN_CODEC2_BASE                   (0x42062000)
#define SILAN_CODEC_HC_BASE                 (0x42063000)

#define SILAN_SDMMC_BASE                    (0x43000000)
#define SILAN_SDIO_BASE                     (0x43010000)
#define SILAN_IIR_CFG_BASE                  (0x43020000)
#define SILAN_OTP_CFG_BASE                  (0x43030000)
#define SILAN_USBHS_BASE                    (0x43100000)
#define SILAN_USBFS_BASE                    (0x43200000)
#define SILAN_ADMAC_BASE                    (0x43300000)
#define SILAN_SF_CFG_BASE                   (0x43400000)
#define SILAN_SDRAM_CFG_BASE                (0x43500000)
#define SILAN_SDMAC_BASE                    (0x43700000)

#define SDMMC_BASE_ADDR                     SILAN_SDMMC_BASE
/* ================================================================================ */
/* ================             Peripheral declaration             ================ */
/* ================================================================================ */
#define CSKY_UART                  ((   CSKY_UART_TypeDef *)    CSKY_UART_BASE)

#ifdef __cplusplus
}
#endif

#endif  /* _SOC_H_ */
