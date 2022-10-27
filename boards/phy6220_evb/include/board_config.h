/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef __BOARD_CONFIG_H__
#define __BOARD_CONFIG_H__

#include <pin_name.h>
#include "pin.h"

#define BAUD_RATE_UART  115200
#define BUTTON_GPIO_1 	P15
#define BUTTON_GPIO_2 	P14

/* 开发板定义 */
#ifndef BOARD_PHY6220_EVB
#define BOARD_PHY6220_EVB
#endif

#define PIN_LED_R  P23
#define PIN_LED_G  P31
#define PIN_LED_B  P32

#define PIN_LED_R_CHANNEL  FMUX_PWM0
#define PIN_LED_G_CHANNEL  FMUX_PWM1
#define PIN_LED_B_CHANNEL  FMUX_PWM2

#define PWM_LED_R_PORT     0
#define PWM_LED_G_PORT     1
#define PWM_LED_B_PORT     2

/* 默认串口波特率 */
#define BAUD_RATE_UART     115200

#define CONSOLE_UART_IDX 0
#define CONSOLE_TXD                 P9
#define CONSOLE_RXD                 P10
#define CONSOLE_TXD_FUNC            FMUX_UART0_TX
#define CONSOLE_RXD_FUNC            FMUX_UART0_RX

/* UART示例引脚配置 */
#define CONFIG_UART_DEMO_IDX    1
#define UART_DEMO_TX            P25
#define UART_DEMO_RX            P26
#define UART_DEMO_TX_FUNC       FMUX_UART1_TX
#define UART_DEMO_RX_FUNC       FMUX_UART1_RX

/* SPI示例引脚配置 */
#define CONSOLE_SPI_DEMO_IDX    0
#define SPI_DEMO_MOSI 	        P32
#define SPI_DEMO_MISO 	        P31
#define SPI_DEMO_CLOCK 	        P33
#define SPI_DEMO_CS 	        P34
#define SPI_DEMO_MOSI_FUNC		FMUX_SPI_1_TX
#define SPI_DEMO_MISO_FUNC  	FMUX_SPI_1_RX
#define SPI_DEMO_CLOCK_FUNC		FMUX_SPI_1_SCK
#define SPI_DEMO_CS_FUNC		FMUX_SPI_1_SSN

#endif /* __BOARD_CONFIG_H__ */
