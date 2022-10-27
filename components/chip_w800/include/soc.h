/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef SOC_H
#define SOC_H

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------  Interrupt Number Definition  ------------------------ */

#include "wm_regs.h"

#define CORET_IRQn SYS_TICK_IRQn

/* ================================================================================ */
/* ================      Processor and Core Peripheral Section     ================ */
/* ================================================================================ */

/* --------  Configuration of the CK801 Processor and Core Peripherals  ------- */
//#define __CK802_REV               0x0000U   /* Core revision r0p0 */
//#define __MPU_PRESENT             0         /* MGU present or not */
//#define __VIC_PRIO_BITS          2         /* Number of Bits used for Priority Levels */
//#define __Vendor_SysTickConfig    0         /* Set to 1 if different SysTick Config is used */

#include "csi_core.h"                     /* Processor and core peripherals */

/* ================================================================================ */
/* ================       Device Specific Peripheral Section       ================ */
/* ================================================================================ */

#define CONFIG_USART_NUM                5
#define CONFIG_TIMER_NUM                6
#define CONFIG_WDT_NUM                  1
#define CONFIG_RTC_NUM                  1
#define CONFIG_GPIO_NUM                 48
#define CONFIG_IIC_NUM                  1
#define CONFIG_LSPI_NUM                 1
#define CONFIG_EFLASH_NUM               1
#define CONFIG_SPIFLASH_NUM             1
#define CONFIG_PWM_NUM                  5
#define CONFIG_PER_PWM_CHANNEL_NUM      5
#define CONFIG_TRNG_NUM                 1
#define CONFIG_CRC_NUM                  1
#define CONFIG_SHA_NUM                  1
#define CONFIG_AES_NUM                  1
#define CONFIG_PMU_NUM                  1
#define CONFIG_I2S_NUM                  1
#define CONFIG_ADC_NUM                  1

/* ================================================================================ */
/* ================              Peripheral memory map             ================ */
/* ================================================================================ */
/* --------------------------  CHIP memory map  ------------------------------- */

#define CSKY_UART0_BASE            (HR_UART0_BASE_ADDR)
#define CSKY_UART1_BASE            (HR_UART1_BASE_ADDR)
#define CSKY_UART2_BASE            (HR_UART2_BASE_ADDR)
#define CSKY_UART3_BASE            (HR_UART3_BASE_ADDR)
#define CSKY_UART4_BASE            (HR_UART4_BASE_ADDR)

#define CSKY_WDT_BASE              (HR_WDG_BASE_ADDR)

#define CSKY_RTC_BASE              (HR_PMU_RTC_CTRL1)

#define CSKY_GPIOA_BASE            (HR_GPIOA_BASE_ADDR)
#define CSKY_GPIOB_BASE            (HR_GPIOB_BASE_ADDR)

#define CSKY_TIMER_BASE            (HR_TIMER_BASE_ADDR)

#define CSKY_I2C_BASE              (HR_I2C_BASE_ADDR)

#define CSKY_LSPI_BASE             (HR_SPI_BASE_ADDR)

#define CSKY_PWM_BASE              (HR_PWM_REG_BASE)

#define CSKY_PMU_BASE              (HR_PMU_BASE_ADDR)

#define CSKY_EFLASH_CONTROL_BASE   (HR_FLASH_BASE_ADDR)

#define CSKY_I2S_BASE              (HR_I2S_REG_BASE)

#define CSKY_ADC_CTL_BASE			(HR_SD_ADC_BASE_ADDR)

#define CONFIG_OTP_BASE_ADDR 0
#define CONFIG_OTP_BANK_SIZE 1024
#ifdef __cplusplus
}
#endif

#endif  /* SOC_H */
