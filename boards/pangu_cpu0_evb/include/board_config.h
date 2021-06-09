/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     board_config.h
 * @brief    header File for pin definition
 * @version  V1.0
 * @date     02. June 2018
 ******************************************************************************/

#ifndef _BOARD_CONFIG_H_
#define _BOARD_CONFIG_H_

#include <stdint.h>
#ifdef CONFIG_CSI_V1
#include "pin_name.h"
#include "pinmux.h"
#endif
#ifdef CONFIG_CSI_V2
#include "soc.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define CONFIG_PANGU_16M_SDRAM

#if 1
#define CONSOLE_TXD         PA19
#define CONSOLE_RXD         PA20
#define CONSOLE_TXD_FUNC    PA19_UART1_TX
#define CONSOLE_RXD_FUNC    PA20_UART1_RX
#define CONSOLE_UART_IDX    1
#else
// TODO:tmall
#define CONSOLE_TXD         PA2
#define CONSOLE_RXD         PA0
#define CONSOLE_TXD_FUNC    PA2_UART0_TX
#define CONSOLE_RXD_FUNC    PA0_UART0_RX
#define CONSOLE_UART_IDX    0
#endif

#ifdef CONFIG_PANGU_16M_SDRAM
    #define APP_KEY_MUTE    PB23
    #define APP_KEY_VOL_INC PA25
    #define APP_KEY_VOL_DEC PB27
    #define LED_USE_PIN     PA23
    #define PANGU_PA        PB21
#else
    #define APP_KEY_MUTE    PB23
    #define APP_KEY_VOL_INC PB21
    #define APP_KEY_VOL_DEC PB4
    #define LED_USE_PIN     PB0
    #define PANGU_PA        PB22
#endif

#ifndef CONFIG_PANGU_16M_SDRAM
#define BT_UART_IDX          2

#define BT_UART_TXD          PB24
#define BT_UART_RXD          PB25
#define BT_UART_TXD_FUNC     PB24_UART2_TX
#define BT_UART_RXD_FUNC     PB25_UART2_RX

#define BT_DIS_PIN           PB31
#else
#define BT_UART_IDX          0

#define BT_UART_TXD          PA2
#define BT_UART_RXD          PA0
#define BT_UART_TXD_FUNC     PA2_UART0_TX
#define BT_UART_RXD_FUNC     PA0_UART0_RX

#define BT_DIS_PIN           PB4
#endif

#define QSPIFLASH_IDX                   0
#define EXAMPLE_QSPI_IDX                0

#define USI0_SCLK       PA21
#define USI0_SD0        PA22
#define USI0_IDX  0

#define WLAN_ENABLE_PIN PB2
#define WLAN_POWER_PIN 0xFFFFFFFF
/* 
* for aos hal defines
*/
#define AOS_HAL_GPIO_ENABLED        1
#define AOS_HAL_UART_ENABLED        1
#define AOS_HAL_SPI_ENABLED         1
#define AOS_HAL_I2C_ENABLED         1
#define AOS_HAL_FLASH_ENABLED       1
#define AOS_HAL_TIMER_ENABLED       1
#define AOS_HAL_PWM_ENABLED         1
#define AOS_HAL_WDG_ENABLED         1
#define AOS_HAL_RTC_ENABLED         1
#define AOS_HAL_ADC_ENABLED         0
#define AOS_HAL_DAC_ENABLED         0
#define AOS_HAL_RNG_ENABLED         0
#define AOS_HAL_I2S_ENABLED         1
#define AOS_HAL_QSPI_ENABLED        1
#define AOS_HAL_CAN_ENABLED         0
#define AOS_HAL_NAND_ENABLED        0
#define AOS_HAL_NOR_ENABLED         0

#if (AOS_HAL_GPIO_ENABLED > 0)

/* gpio app macro default value
   need to ajust for each board
*/
#ifndef HALAPP_GPIO_OUT
#define HALAPP_GPIO_OUT PA3
#endif

#ifndef HALAPP_GPIO_IN
#define HALAPP_GPIO_IN PA3
#endif

#ifndef HALAPP_GPIO_INT
#define HALAPP_GPIO_INT PA3
#endif
#endif /* AOS_HAL_GPIO_ENABLED > 0 */

typedef enum{
    PORT_UART_STD  = 0,
    PORT_UART_AT   = 1,
    PORT_UART_SIZE,
    PORT_UART_INVALID = 255,
    PORT_UART_DEMO = PORT_UART_AT,
}PORT_UART_TYPE;

typedef enum {
    PORT_ADC_1,
    PORT_ADC_2,
    PORT_ADC_SIZE,
    PORT_ADC_INVALID =255,
    PORT_ADC_DEMO = PORT_ADC_1,
} PORT_ADC_TYPE;

typedef enum {
    PORT_DAC_1,
    PORT_DAC_SIZE,
    PORT_DAC_INVALID =255,
    PORT_DAC_DEMO = PORT_DAC_1,
} PORT_DAC_TYPE;

typedef enum {
    PORT_TIMER_0,
    PORT_TIMER_1,
    PORT_TIMER_SIZE,
    PORT_TIMER_INVALID = 255,
    PORT_TIMER_DEMO = PORT_TIMER_1,
} PORT_TIMER_TYPE;

typedef enum {
    PORT_PWM_1,
    PORT_PWM_2,
    PORT_PWM_3,
    PORT_PWM_4,
    PORT_PWM_SIZE,
    PORT_PWM_INVALID = 255,
    PORT_PWM_DEMO = PORT_PWM_3,
} PORT_PWM_TYPE;

typedef enum {
    PORT_I2C_1 = 0,
    PORT_I2C_SIZE,
    PORT_I2C_INVALID = 255,
} PORT_I2C_TYPE;

typedef enum {
    PORT_SPI_1,
    PORT_SPI_SIZE,
    PORT_SPI_INVALID =255,
    PORT_SPI_DEMO = PORT_SPI_1,
} PORT_SPI_TYPE;

typedef enum {
    PORT_WDG_0 = 0,
    PORT_WDG_SIZE,
    PORT_WDG_INVALID = 255,
} PORT_WDG_TYPE;

typedef enum {
    PORT_RTC_0 = 0,
    PORT_RTC_SIZE,
    PORT_RTC_INVALID =255,
} PORT_RTC_TYPE;

#ifdef __cplusplus
}
#endif

#endif /* _BOARD_CONFIG_H_ */

