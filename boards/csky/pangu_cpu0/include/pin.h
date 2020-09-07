/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     pin.h
 * @brief    header File for pin definition
 * @version  V1.0
 * @date     02. June 2018
 ******************************************************************************/

#ifndef _PIN_H_
#define _PIN_H_

#include <stdint.h>
#include "pin_name.h"
#include "pinmux.h"
#include "soc.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CONFIG_PANGU_16M_SDRAM

#if 1
#define CONSOLE_TXD         PA19
#define CONSOLE_RXD         PA20
#define CONSOLE_TXD_FUNC    PA19_UART1_TX
#define CONSOLE_RXD_FUNC    PA20_UART1_RX
#define CONSOLE_IDX    1
#else
// TODO:tmall
#define CONSOLE_TXD         PA2
#define CONSOLE_RXD         PA0
#define CONSOLE_TXD_FUNC    PA2_UART0_TX
#define CONSOLE_RXD_FUNC    PA0_UART0_RX
#define CONSOLE_IDX    0
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

#ifdef __cplusplus
}
#endif

#endif /* _PIN_H_ */

