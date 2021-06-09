/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     soc.h
 * @brief    CSI Core Peripheral Access Layer Header File for
 *           CSKYSOC Device Series
 * @version  V1.0
 * @date     7. April 2020
 ******************************************************************************/

#ifndef _SOC_H_
#define _SOC_H_

#include <stdint.h>
#include <csi_core.h>
#include <sys_clk.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef EHS_VALUE
#define EHS_VALUE               26450000U
#endif

#ifndef ELS_VALUE
#define ELS_VALUE               32768U
#endif

#ifndef IHS_VALUE
#define IHS_VALUE               26450000U
#endif

#ifndef ILS_VALUE
#define ILS_VALUE               32768U
#endif

typedef enum {
    User_Software_IRQn             =  0U,      /* User software interrupt */
    Supervisor_Software_IRQn       =  1U,      /* Supervisor software interrupt */
    Machine_Software_IRQn          =  3U,      /* Machine software interrupt */
    User_Timer_IRQn                =  4U,      /* User timer interrupt */
    Supervisor_Timer_IRQn          =  5U,      /* Supervisor timer interrupt */
    CORET_IRQn                     =  7U,      /* core Timer Interrupt */
    Machine_External_IRQn          =  11U,     /* Machine external interrupt */
    DW_GPIO_IRQn                   =  16U,
    DW_TIMER0_IRQn                 =  18U,
    DW_TIMER1_IRQn                 =  19U,
    DW_WDT_IRQn                    =  21U,
    DW_UART0_IRQn                  =  22U,
    DW_UART1_IRQn                  =  23U,
    DW_IIC_IRQn                    =  25U,
    WJ_I2S456_IRQn                 =  26U,
    DW_SPI1_IRQn                   =  27U,
    DW_SPI0_IRQn                   =  28U,
    WJ_RTC_IRQn                    =  29U,
    WJ_I2S0_IRQn                   =  30U,
    WJ_ADC_IRQn                    =  31U,
    WJ_I2S1_IRQn                   =  32U,
    WJ_DMA_IRQn                    =  33U,
    WJ_PMU_IRQn                    =  34U,
    WJ_PWM_IRQn                    =  35U,
    WJ_I2S2_IRQn                   =  37U,
    WJ_I2S3_IRQn                   =  38U,
    DW_TIMER2_IRQn                 =  39U,
    DW_TIMER3_IRQn                 =  40U,
    CD_QSPI_IRQn                   =  41U,
    DW_TIMER4_IRQn                 =  42U,
    DW_TIMER5_IRQn                 =  43U,
    DW_TIMER6_IRQn                 =  44U,
    DW_TIMER7_IRQn                 =  45U,
} irqn_type_t;

typedef enum {
    DW_GPIO_Wakeupn                =    0U,     /* GPIO0 wakeup */
    DW_TIM0_Wakeupn                =    1U,     /* TIM0 wakeup */
    DW_TIM1_Wakeupn                =    2U,     /* TIM1 wakeup */
    WJ_WDT_Wakeupn                 =    3U,     /* WDT wakeup */
    DW_UART0_Wakeupn               =    4U,     /* UART0 wakeup */
    DW_UART1_Wakeupn               =    5U,     /* UART1 wakeup */
    DW_IIC_Wakeupn                 =    7U,     /* I2C0 wakeup */
    DW_SPI1_Wakeupn                =    9U,     /* SPI1 wakeup */
    DW_SPI0_Wakeupn                =   10U,     /* SPI0 wakeup */
    WJ_RTC_Wakeupn                 =   11U,     /* RTC0 wakeup */
    WJ_I2S0_Wakeupn                =   12U,     /* I2S0 wakeup*/
    WJ_ADC_Wakeupn                 =   13U,     /* ADC wakeup */
    WJ_I2S1_Wakeupn                =   14U,     /* I2S1 wakeup*/
    WJ_DMA_Wakeupn                 =   15U,     /* DMA0 wakeup */
    WJ_PWM_Wakeupn                 =   16U,     /* PWM wakeup */
    WJ_I2S2_Wakeupn                =   18U,     /* I2S2 wakeup*/
    WJ_I2S3_Wakeupn                =   19U,     /* I2S3 wakeup*/
    DW_TIM2_Wakeupn                =   20U,     /* TIM2 wakeup */
    DW_TIM3_Wakeupn                =   21U,     /* TIM3 wakeup */
    DW_TIM4_Wakeupn                =   23U,     /* TIM4 wakeup */
    DW_TIM5_Wakeupn                =   24U,     /* TIM5 wakeup */
    DW_TIM6_Wakeupn                =   25U,     /* TIM6 wakeup */
    DW_TIM7_Wakeupn                =   26U,     /* TIM7 wakeup */
    WJ_HAD_Wakeupn                 =   28U,     /* HAD wakeup */
    WJ_IOCTL_Wakeupn               =   29U,     /* IOCTOL wakeup */
} wakeupn_type_t;

typedef enum {
    DW_TIM0_CLK_MANAGERN           =  0U,
    DW_TIM1_CLK_MANAGERN           =  1U,
    DW_RTC_CLK_MANAGERN            =  2U,
    DW_RTC1_CLK_MANAGERN           =  2U,
    DW_WDT_CLK_MANAGERN            =  3U,
    DW_SPI0_CLK_MANAGERN           =  4U,
    DW_UART0_CLK_MANAGERN          =  5U,
    DW_UART2_CLK_MANAGERN          =  5U,
    DW_IIC0_CLK_MANAGERN           =  6U,
    WJ_PWM_CLK_MANAGERN            =  7U,
    WJ_QSPIC0_CLK_MANAGERN         =  8U,
    WJ_PWMR_CLK_MANAGERN           =  9U,
    WJ_EFUSE_CLK_MANAGERN          = 10U,
    WJ_I2S0_CLK_MANAGERN           = 11U,
    WJ_I2S1_CLK_MANAGERN           = 12U,
    DW_GPIO0_CLK_MANAGERN          = 13U,
    DW_GPIO1_CLK_MANAGERN          = 13U,
    DW_TIM2_CLK_MANAGERN           = 14U,
    DW_TIM3_CLK_MANAGERN           = 15U,
    DW_SPI1_CLK_MANAGERN           = 16U,
    DW_UART1_CLK_MANAGERN          = 17U,
    DW_I2C1_CLK_MANAGERN           = 18U,
    WJ_ADC_CLK_MANAGERN            = 19U,
    WJ_ETB_CLK_MANAGERN            = 20U,
    WJ_I2S2_CLK_MANAGERN           = 21U,
    WJ_I2S3_CLK_MANAGERN           = 22U,
    WJ_IOCTRL_CLK_MANAGERN         = 23U,
    WJ_CODEC_CLK_MANAGERN          = 24U,
    WJ_DMA0_CLK_MANAGERN           = 27U,
    WJ_DMA1_CLK_MANAGERN           = 27U,
    WJ_USB_CLK_MANAGERN            = 28U,
} clk_manager_type_t;

typedef enum {
    DW_UART0_RX_DMAN               = 0U,
    DW_UART0_TX_DMAN               = 1U,
    DW_UART1_RX_DMAN               = 2U,
    DW_UART1_TX_DMAN               = 3U,
    MEMORY_DMAN                    = 4U,
} dman_type_t;


typedef enum {
    PA0         = 0U,
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
} pin_name_t;


typedef enum {
    PA0_I2S0_SCLK                  =  0U,
    PA0_SPI0_CS                    =  1U,
    PA0_UART0_RX                   =  2U,
    PA0_PWM_CH0                    =  3U,
    PA1_I2S0_WSCLK                 =  0U,
    PA1_SPI0_SCK                   =  1U,
    PA1_UART0_TX                   =  2U,
    PA1_PWM_CH1                    =  3U,
    PA2_I2S1_SCLK                  =  0U,
    PA2_IIC0_SCL                   =  1U,
    PA2_SPI1_CS                    =  2U,
    PA2_PWM_CH2                    =  3U,
    PA2_ADC_A0                     =  7U,
    PA3_I2S1_WSCLK                 =  0U,
    PA3_IIC0_SDA                   =  1U,
    PA3_SPI1_SCK                   =  2U,
    PA3_PWM_CH3                    =  3U,
    PA3_ADC_A1                     =  8U,
    PA4_I2S0_SDA                   =  0U,
    PA4_SPI0_MOSI                  =  1U,
    PA4_UART1_CTS                  =  2U,
    PA4_PWM_CH4                    =  3U,
    PA4_ADC_A2                     =  9U,
    PA5_I2S1_SDA                   =  0U,
    PA5_SPI0_MISO                  =  1U,
    PA5_UART1_RTS                  =  2U,
    PA5_PWM_CH5                    =  3U,
    PA5_ADC_A3                     = 10U,
    PA6_I2S0_SCLK                  =  0U,
    PA6_UART0_TX                   =  1U,
    PA6_SPI1_MOSI                  =  2U,
    PA6_PWM_CH6                    =  3U,
    PA6_ADC_A4                     = 11U,
    PA7_I2S0_WSCLK                 =  0U,
    PA7_PWMR_OUT                   =  1U,
    PA7_SPI1_MISO                  =  2U,
    PA7_PWM_CH7                    =  3U,
    PA7_ADC_A5                     = 12U,
    PA8_I2S0_SDA                   =  0U,
    PA8_IIC0_SCL                   =  1U,
    PA8_UART0_RX                   =  2U,
    PA8_PWM_CH8                    =  3U,
    PA8_ADC_A6                     = 13U,
    PA9_I2S1_SDA                   =  0U,
    PA9_IIC0_SDA                   =  1U,
    PA9_PWMR_OUT                   =  2U,
    PA9_PWM_CH9                    =  3U,
    PA9_ADC_A7                     = 14U,
    PA10_I2S0_MCLK                 =  0U,
    PA10_UART0_TX                  =  1U,
    PA10_SPI1_MOSI                 =  2U,
    PA10_SPI1_MISO                 =  3U,
    PA10_ADC_A8                    = 15U,
    PA15_IIC0_SCL                  =  0U,
    PA15_SPI0_CS                   =  1U,
    PA15_PWMR_OUT                  =  2U,
    PA15_PWM_CH4                   =  3U,
    PA15_ADC_A9                    = 20U,
    PA16_IIC0_SDA                  =  0U,
    PA16_SPI0_SCK                  =  1U,
    PA16_UART1_TX                  =  2U,
    PA16_PWM_CH5                   =  3U,
    PA16_ADC_A10                   = 21U,
    PA17_UART0_RX                  =  0U,
    PA17_SPI0_MOSI                 =  1U,
    PA17_I2S0_SCLK                 =  2U,
    PA17_PWM_CH10                  =  3U,
    PA17_ADC_A11                   = 22U,
    PA18_UART0_TX                  =  0U,
    PA18_SPI0_MISO                 =  1U,
    PA18_I2S0_WSCLK                =  2U,
    PA18_PWM_CH11                  =  3U,
    PA18_ADC_A12                   = 23U,
    PA19_JTAG_TMS                  =  0U,
    PA19_UART1_RX                  =  1U,
    PA19_I2S1_SCLK                 =  2U,
    PA19_IIC0_SCL                  =  3U,
    PA19_ADC_A13                   = 24U,
    PA20_JTAG_TCK                  =  0U,
    PA20_UART1_TX                  =  1U,
    PA20_I2S1_WSCLK                =  2U,
    PA20_IIC0_SDA                  =  3U,
    PA20_ADC_A14                   = 25U,
    PA21_UART0_CTS                 =  0U,
    PA21_UART1_CTS                 =  1U,
    PA21_I2S0_SDA                  =  2U,
    PA21_IIC0_SCL                  =  3U,
    PA21_ADC_A15                   = 26U,
    PA22_UART0_RTS                 =  0U,
    PA22_UART1_RTS                 =  1U,
    PA22_I2S1_SDA                  =  2U,
    PA22_IIC0_SDA                  =  3U,
    PA23_IIC0_SCL                  =  0U,
    PA23_UART0_TX                  =  1U,
    PA23_PWM_CH0                   =  2U,
    PA23_SPI0_CS                   =  3U,
    PA24_IIC0_SDA                  =  0U,
    PA24_UART0_RX                  =  1U,
    PA24_PWM_CH1                   =  2U,
    PA24_SPI0_SCK                  =  3U,
    PA25_PWMR_OUT                  =  0U,
    PA25_UART0_CTS                 =  1U,
    PA25_PWM_CH2                   =  2U,
    PA25_SPI0_MOSI                 =  3U,
    PA26_I2S1_MCLK                 =  0U,
    PA26_UART0_RTS                 =  1U,
    PA26_PWM_CH3                   =  2U,
    PA26_SPI0_MISO                 =  3U,
    PA27_I2S0_SCLK                 =  0U,
    PA27_UART1_RX                  =  1U,
    PA27_PWM_CH4                   =  2U,
    PA27_SPI1_CS                   =  3U,
    PA28_I2S0_WSCLK                =  0U,
    PA28_UART1_TX                  =  1U,
    PA28_PWM_CH5                   =  2U,
    PA28_SPI1_SCK                  =  3U,
    PA29_I2S1_SCLK                 =  0U,
    PA29_UART1_CTS                 =  1U,
    PA29_PWM_CH6                   =  2U,
    PA29_SPI1_MOSI                 =  3U,
    PA30_I2S1_WSCLK                =  0U,
    PA30_UART1_RTS                 =  1U,
    PA30_PWM_CH7                   =  2U,
    PA30_SPI1_MISO                 =  3U,
    PA31_I2S0_SDA                  =  0U,
    PA31_PWMR_OUT                  =  1U,
    PA31_PWM_CH8                   =  2U,
    PA31_UART0_TX                  =  3U,
    PIN_FUNC_GPIO                  =  4U,
} pin_func_t;

#define CONFIG_IRQ_NUM              48U

///< AHB
#define SPIFLASH_BASE               0x18000000UL
#define SPIFLASH_SIZE               0x800000U

#define SRAM_BASE                   0x20000000UL
#define SRAM_SIZE                   0x10000U

#define WJ_PMU_BASE                 0x40000000UL
#define WJ_PMU_SIZE                 0x1000U

#define WJ_DMA_BASE                 0x40001000UL
#define WJ_DMA_SIZE                 0x1000U

///< APB0
#define DW_TIMER0_BASE              0x50000000UL
#define DW_TIMER0_SIZE              0x14U
#define DW_TIMER1_BASE              (DW_TIMER0_BASE+DW_TIMER0_SIZE)
#define DW_TIMER1_SIZE              DW_TIMER0_SIZE

#define DW_TIMER2_BASE              0x50000400UL
#define DW_TIMER2_SIZE              0x14U
#define DW_TIMER3_BASE              (DW_TIMER2_BASE+DW_TIMER2_SIZE)
#define DW_TIMER3_SIZE              DW_TIMER2_SIZE

#define WJ_RTC_BASE                 0x50004000UL
#define WJ_RTC_SIZE                 0x400U

#define DW_WDT_BASE                 0x50008000UL
#define DW_WDT_SIZE                 0x400U

#define DW_SPI0_BASE                0x5000C000UL
#define DW_SPI0_SIZE                0x400U

#define DW_UART0_BASE               0x50010000UL
#define DW_UART0_SIZE               0x400U

#define DW_IIC_BASE                 0x50014000UL
#define DW_IIC_SIZE                 0x400U

#define DW_GPIO_BASE                0x50018000UL
#define DW_GPIO_SIZE                0x400U

#define WJ_PWM_BASE                 0x5001C000UL
#define WJ_PWM_SIZE                 0x400U

#define CD_QSPI_BASE                0x5002C000UL
#define CD_QSPI_SIZE                0x400U

#define WJ_I2S0_BASE                0x50040000UL
#define WJ_I2S0_SIZE                0x400U

#define WJ_I2S1_BASE                0x50040400UL
#define WJ_I2S1_SIZE                0x400U

///< APB1
#define DW_TIMER4_BASE              0x60000000UL
#define DW_TIMER4_SIZE              0x14U
#define DW_TIMER5_BASE              (DW_TIMER4_BASE + DW_TIMER4_SIZE)
#define DW_TIMER5_SIZE              DW_TIMER4_SIZE

#define DW_TIMER6_BASE              0x60000400UL
#define DW_TIMER6_SIZE              0x14U
#define DW_TIMER7_BASE              (DW_TIMER6_BASE + DW_TIMER6_SIZE)
#define DW_TIMER7_SIZE              0x14U

#define DW_SPI1_BASE                0x6000C000UL
#define DW_SPI1_SIZE                0x400U

#define DW_UART1_BASE               0x60010000UL
#define DW_UART1_SIZE               0x400U

#define WJ_I2S4_BASE                0x60014000UL
#define WJ_I2S4_SIZE                0x400U
#define WJ_I2S5_BASE                0x60014100UL
#define WJ_I2S5_SIZE                0x400U
#define WJ_I2S6_BASE                0x60014200UL
#define WJ_I2S6_SIZE                0x400U

#define WJ_ADC_BASE                 0x60020000UL
#define WJ_ADC_SIZE                 0x400U

#define WJ_ETB_BASE                 0x60024000UL
#define WJ_ETB_SIZE                 0x1000U

#define WJ_EFUSE_BASE               0x5003C000UL
#define WJ_EFUSE_SIZE               0x400U

#define WJ_IOC_BASE                 0x60030000UL
#define WJ_IOC_SIZE                 0x400U

#define WJ_I2S2_BASE                0x60040400UL
#define WJ_I2S2_SIZE                0x400U
#define WJ_I2S3_BASE                0x60040800UL
#define WJ_I2S3_SIZE                0x400U

#define RCHBAND_CODEC_BASE          0x60044000UL
#define RCHBAND_CODEC_SIZE          0x1000U

#define CONFIG_RTC_FAMILY_D
#define CONFIG_GPIO_NUM             2U


#ifdef __cplusplus
}
#endif

#endif  /* _SOC_H_ */
