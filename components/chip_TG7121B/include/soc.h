/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef SOC_H
#define SOC_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef IHS_VALUE
#define  IHS_VALUE    (24000000)
#endif

#ifndef EHS_VALUE
#define  EHS_VALUE    (16000000)
#endif

#ifndef ILS_VALUE
#define  ILS_VALUE    (32768)
#endif

#ifndef ELS_VALUE
#define  ELS_VALUE    (32768)
#endif

/* -------------------------  Interrupt Number Definition  ------------------------ */

typedef enum {
  EXTI_IRQn                      =   0,
  WWDT_IRQn                      =   1,
  LPWKUP_IRQn                    =   2,
  BLE_IRQn                       =   3,
  RTC_IRQn                       =   4,
  DMA_IRQn                       =   5,
  QSPI_IRQn                      =   6,
  ECC_IRQn                       =   7,
  CACHE_IRQn                     =   8,
  TRNG_IRQn                      =   9,
  IWDT_IRQn                      =  10,
  CRYPT_IRQn                     =  11,
  PDM_IRQn                       =  12,
  BLE_WKUP_IRQn                  =  13,
  ADC_IRQn                       =  14,
  ADTIM1_IRQn                    =  15,
  BSTIM1_IRQn                    =  16,
  GPTIMA1_IRQn                   =  17,
  GPTIMB1_IRQn                   =  18,
  BLE_ERR_IRQn                   =  19,
  LVD33_IRQn                     =  20,
  GPTIMC1_IRQn                   =  21,
  LPTIM_IRQn                     =  22,
  I2C1_IRQn                      =  23,
  I2C2_IRQn                      =  24,
  SPI1_IRQn                      =  25,
  SPI2_IRQn                      =  26,
  UART1_IRQn                     =  27,
  UART2_IRQn                     =  28,
  UART3_IRQn                     =  29,
  BLE_FIFO_IRQn                  =  30,
  BLE_CRYPT_IRQn                 =  31,
}
IRQn_Type_e;

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

#include "csi_core.h"                     /* Processor and core peripherals */

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

#define CONFIG_PMU_NUM  1
#define CONFIG_CRC_NUM  1
#define CONFIG_EFLASH_NUM  1
#define CONFIG_IIC_NUM  2
#define CONFIG_TRNG_NUM 1
#define CONFIG_AES_NUM  1
#define CONFIG_RSA_NUM  1
#define CONFIG_SHA_NUM  1
#define CONFIG_SPI_NUM  2
#define CONFIG_PWM_NUM  1
#define CONFIG_PER_PWM_CHANNEL_NUM    12
#define CONFIG_TIMER_NUM 4
#define CONFIG_RTC_NUM 2
#define CONFIG_WDT_NUM 1
#define CONFIG_DMA_CHANNEL_NUM 4
#define CONFIG_GPIO_NUM            2
#define CONFIG_GPIO_PIN_NUM        34
#define CONFIG_USART_NUM 3
#define CONFIG_ETH_NUM 2
#define CONFIG_ADC_NUM 1
#define CONFIG_I2S_NUM 1
#define CONFIG_ACMP_NUM 1

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

#define CSKY_EFLASH_BASE           (0x18000000UL)
#define CSKY_EFLASH_SIZE           (0x80000UL)
#define CSKY_SRAM_BASE             (0x00000000UL)
#define CSKY_SRAM_SIZE             (0xC000UL)

/* AHB */
#define CSKY_AHB_ARB_BASE          (0x40000000UL)
#define CSKY_DMAC0_BASE            (0x40001000UL)
#define CSKY_PMU_BASE              (0x40002000UL)
#define CSKY_CLKGEN_BASE           (0x40002000UL)
#define CSKY_CRC_BASE              (0x40003000UL)
#define CSKY_DMAC1_BASE            (0x40004000UL)
#define CSKY_OTP_BASE              (0x4003F000UL)
#define CSKY_AES_BASE              (0x40006000UL)
#define CSKY_SRAM_SASC_BASE        (0x40007000UL)
#define CSKY_SHA_BASE              (0x40008000UL)
#define CSKY_TRNG_BASE             (0x40009000UL)
#define CSKY_RSA_BASE              (0x4000a000UL)
#define CSKY_EFLASH_CONTROL_BASE   (0x4003f000UL)
#define CSKY_APB0_BRIDGE_BASE      (0x50000000UL)
#define CSKY_APB1_BRIDGE_BASE      (0x50010000UL)

/* APB0 */
#define CSKY_WDT_BASE              (0x50001000UL)
#define CSKY_SPI0_BASE             (0x50002000UL)
#define CSKY_RTC0_BASE             (0x50003000UL)
#define CSKY_UART0_BASE            (0x50004000UL)
#define CSKY_UART1_BASE            (0x50005000UL)
#define CSKY_GPIO0_BASE            (0x50006000UL)
#define CSKY_I2C0_BASE             (0x50007000UL)
#define CSKY_I2S_BASE              (0x50008000UL)
#define CSKY_GPIO1_BASE            (0x50009000UL)
#define CSKY_SIPC_BASE             (0x5000a000UL)

/* APB1 */
#define CSKY_TIM0_BASE             (0x50011000UL)
#define CSKY_SPI1_BASE             (0x50012000UL)
#define CSKY_I2C1_BASE             (0x50013000UL)
#define CSKY_PWM_BASE              (0x50014000UL)
#define CSKY_UART2_BASE            (0x50015000UL)
#define CSKY_ADC_CTL_BASE          (0x50016000UL)
#define CSKY_CMP_CTL_BASE          (0x50017000UL)
#define CSKY_ETB_BASE              (0x50018000UL)
#define CSKY_TIM1_BASE             (0x50019000UL)
#define CSKY_RTC1_BASE             (0x5001a000UL)

#define SHA_CONTEXT_SIZE        320
#define SOC_LPM_RESUME_ADDR     0x1003f7f0

// #define CONFIG_DW_DMAC

/* ================================================================================ */
/* ================             Peripheral declaration             ================ */
/* ================================================================================ */
#define CSKY_UART1                 ((   CSKY_UART_TypeDef *)    CSKY_UART1_BASE)
#define CSKY_SHA                   ((   CSKY_SHA_TypeDef *)     CSKY_SHA_BASE)

#ifdef __cplusplus
}
#endif

#endif  /* SOC_H */
