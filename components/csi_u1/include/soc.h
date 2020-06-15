/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

/**************************************************************************//**
 * @file     soc.h
 * @brief    CSI Core Peripheral Access Layer Header File for
 *           CSKYSOC Device Series
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/


#ifndef SOC_H
#define SOC_H

#ifdef __cplusplus
extern "C" {
#endif

#define CPU_INDEX_AP                0
#define CPU_INDEX_CP                1
#define CPU_INDEX_SP                2

#define CPU_NAME_CP                 "cp"
#define CPU_NAME_AP                 "ap"
#define CPU_NAME_SP                 "sp"

/* -------------------------  Interrupt Number Definition  ------------------------ */

typedef enum IRQn {
    /* ----------------------  U1 Specific Interrupt Numbers  --------------------- */
    RTC_IRQn                =   0,      /* rtc Interrupt */
    UART0_IRQn              =   1,      /* uart0 Interrupt */
    PWR_IRQn                =   2,      /* pwr(coretim) Interrupt */
    GPIO_IRQn               =   3,      /* gpio Interrupt */
    APWDT_IRQn              =   4,      /* ap_wdt Interrupt */
    MAILBOX_IRQn            =   5,      /* mailbox Interrupt */
    UART1_IRQn              =   6,      /* uart1 Interrupt */
    UART2_IRQn              =   7,      /* uart2 Interrupt */
    UART3_IRQn              =   8,      /* uart3 Interrupt */
    I2C0_IRQn               =   9,      /* i2c0 Interrupt */
    I2C1_IRQn               =   10,     /* i2c1 Interrupt */
    SPI0_IRQn               =   11,     /* spi0 Interrupt */
    DMAS_IRQn               =   12,     /* dmas Interrupt */
    AUXADC_IRQn             =   13,     /* auxadc Interrupt */
    PWM_IRQn                =   14,     /* pwm Interrupt */
    AT_SPEC_TIM0_IRQn       =   32,     /* In pwr */
    AT_SPEC_TIM1_IRQn       =   33,     /* In pwr */
    AT_SPEC_TIM2_IRQn       =   34,     /* In pwr */
    AT_SNAP_IRQn            =   35,     /* In pwr */
    PLL1_ADJ_FINISH_IRQn    =   36,     /* In pwr */
    AT_CALIB_FINISH_IRQn    =   37,     /* In pwr */
    CP_UNIT_PD_IRQn         =   38,     /* In pwr */
    CP_UNIT_PU_IRQn         =   39,     /* In pwr */
    AP_UNIT_PD_IRQn         =   42,     /* In pwr */
    AP_UNIT_PU_IRQn         =   43,     /* In pwr */
    RF_TP_TM2_S2F_IRQn      =   48,     /* In pwr */
    CK802_CTIM_INT_VLD_IRQn =   49,     /* In pwr */
    SEC_M4_PC_MONITOR_IRQn  =   50,     /* In pwr */
    AP_M4_PC_MONITOR_IRQn   =   51,     /* In pwr */
    CP_M4_PC_MONITOR_IRQn   =   52,     /* In pwr */
    SLP_U1RXD_ACT_IRQn      =   53      /* In pwr */
} IRQn_Type;

/* -------------------------  Modules Clock Manager Definition  ------------------------ */
typedef enum {
    CLOCK_MANAGER_GPIO0           = 0,
    CLOCK_MANAGER_SPI0            = 1,
    CLOCK_MANAGER_WDT             = 2,
    CLOCK_MANAGER_RTC             = 3,
    CLOCK_MANAGER_PWM             = 4,
    CLOCK_MANAGER_UART0           = 5,
    CLOCK_MANAGER_I2C0            = 6,
    CLOCK_MANAGER_I2C1            = 7,
    CLOCK_MANAGER_UART1           = 8,
    CLOCK_MANAGER_UART2           = 9,
    CLOCK_MANAGER_TIM             = 10,
    CLOCK_MANAGER_I2S             = 11,
    CLOCK_MANAGER_ADC             = 12,
    CLOCK_MANAGER_SPI1            = 13,
    CLOCK_MANAGER_CMPCTRL         = 14,
    CLOCK_MANAGER_ETB             = 15,
    CLOCK_MANAGER_GPIO1           = 16,
    CLOCK_MANAGER_DMAC0           = 17,
    CLOCK_MANAGER_CRC             = 18,
    CLOCK_MANAGER_CORETIM         = 19,
    CLOCK_MANAGER_DMAC1           = 20,
    CLOCK_MANAGER_TIM1            = 21,
    CLOCK_MANAGER_RTC1            = 22,
    CLOCK_MANAGER_SASC            = 25,
    CLOCK_MANAGER_APB1            = 26,
    CLOCK_MANAGER_APB0            = 27,
    CLOCK_MANAGER_SMS             = 30,
    CLOCK_MANAGER_SHA             = 32,
    CLOCK_MANAGER_TRNG            = 33,
    CLOCK_MANAGER_AES             = 34,
    CLOCK_MANAGER_RSA             = 35,
    CLOCK_MANAGER_SIPC            = 36
} clock_manager_e;

/* ================================================================================ */
/* ================      Processor and Core Peripheral Section     ================ */
/* ================================================================================ */

/* --------  Configuration of the CK801 Processor and Core Peripherals  ------- */
#define __CK802_REV                 0x0000U   /* Core revision r0p0 */
#define __MPU_PRESENT               0         /* MGU present or not */
#define __VIC_PRIO_BITS             2         /* Number of Bits used for Priority Levels */
#define __Vendor_SysTickConfig      0         /* Set to 1 if different SysTick Config is used */

#include "csi_core.h"                     /* Processor and core peripherals */
#include "stdint.h"
#include "power_manager.h"
#include "sys_freq.h"

typedef enum {
    CKENUM_DMA_UART0_RX,
    CKENUM_DMA_UART0_TX,
    CKENUM_DMA_UART1_RX,
    CKENUM_DMA_UART1_TX,
    CKENUM_DMA_ADC_RX,
    CKENUM_DMA_ADC_TX,
    CKENUM_DMA_SPI1_RX,
    CKENUM_DMA_SPI1_TX,
    CKENUM_DMA_SPI0_RX,
    CKENUM_DMA_SPI0_TX,
    CKENUM_DMA_IIC0_RX,
    CKENUM_DMA_IIC0_TX,
    CKENUM_DMA_IIC1_RX,
    CKENUM_DMA_IIC1_TX,
    CKENUM_DMA_IIS_RX,
    CKENUM_DMA_IIS_TX,
    CKENUM_DMA_MEMORY
} ckenum_dma_device_e;

/* ================================================================================ */
/* ================       Device Specific Peripheral Section       ================ */
/* ================================================================================ */

#define CONFIG_USART_NUM            2
#define CONFIG_WDT_NUM              1
#define CONFIG_MBOX_NUM             3
#define CONFIG_RPTUN_NUM            2
#define CONFIG_GPIO_NUM             1
#define CONFIG_GPIO_PIN_NUM         64
#define CONFIG_SPI_NUM              1
#define CONFIG_IIC_NUM              2
#define CONFIG_USRSOCK_NUM          1
#define CONFIG_NET_USRSOCK_CONNS    6
#define CONFIG_NET_NACTIVESOCKETS   16
#define CONFIG_RTC_NUM              1
#define CONFIG_IPCFLASH_SIZE        0x64000
#define CONFIG_NETDB_DNSCLIENT_LIFESEC 3600

/* ================================================================================ */
/* ================              Peripheral memory map             ================ */
/* ================================================================================ */
/* --------------------------  CHIP memory map  ------------------------------- */

enum {
    ADDR_SPACE_ERROR      = -1,
    ADDR_SPACE_EFLASH     =  0,
    ADDR_SPACE_SRAM       =  1,
    ADDR_SPACE_PERIPHERAL =  2,
    ADDR_SPACE_TCIP       =  3
} addr_space_e;

#define CSKY_SRAM_BASE             (0x21000000UL)
#define CSKY_SRAM_SIZE             (0x30000UL)

#define CSKY_UART0_BASE            (0xB2000000UL)
#define CSKY_UART1_BASE            (0xB00B0000UL)
#define CSKY_UART2_BASE            (0xB00C0000UL)
#define CSKY_UART3_BASE            (0xB00D0000UL)

#define CSKY_WDT_BASE              (0xB0090000UL)

#define CSKY_MAILBOX_AP_BASE       (0xB0030010UL)
#define CSKY_MAILBOX_CP_BASE       (0xB0030000UL)
#define CSKY_MAILBOX_SP_BASE       (0xB0030020UL)

#define CSKY_PWR_BASE              (0xB0040000UL)
#define CSKY_PWR_SFRST_CTL         (0xB004011cUL)
#define CSKY_PWR_RES_REG2          (0xB0040260UL)

#define CSKY_GPIO0_BASE            (0xB0060000UL)
#define CSKY_SPI0_BASE             (0xB0110000UL)
#define CSKY_I2C0_BASE             (0xB00E0000UL)
#define CSKY_I2C1_BASE             (0xB00F0000UL)

#define CSKY_PWR_RESET_NORMAL      (0x00000000UL)
#define CSKY_PWR_RESET_ROMBOOT     (0xaaaa1234UL)
#define CSKY_PWR_RESET_RECOVERY    (0xbbbb1234UL)

#define CSKY_PWR_SFRST_RESET       (1 << 0)

#define CSKY_RTC_BASE              (0xB2020000UL)

#ifdef __cplusplus
}
#endif

#endif  /* SOC_H */
