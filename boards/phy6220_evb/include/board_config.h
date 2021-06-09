/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef __BOARD_CONFIG_H__
#define __BOARD_CONFIG_H__

#include <pin_name.h>


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

#define CONSOLE_UART_IDX 0
#define CONSOLE_TXD                 P9
#define CONSOLE_RXD                 P10
#define CONSOLE_TXD_FUNC            FMUX_UART0_TX
#define CONSOLE_RXD_FUNC            FMUX_UART0_RX

#endif /* __BOARD_CONFIG_H__ */
