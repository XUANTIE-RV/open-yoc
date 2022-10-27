/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef __BOARD_CONFIG_H__
#define __BOARD_CONFIG_H__

#include <pin_name.h>

/* 开发板定义 */
#ifndef BOARD_W800
#define BOARD_W800
#endif

/* AT业务串口 */
#define CONFIG_AT_UART_IDX 4
#define CONFIG_AT_UART_NAME "uart4"

/* HCI通信串口 */
#define CONFIG_HCI_UART_IDX 1

/* 调试UART口 */
#define CONSOLE_UART_IDX  0

/* YMODEM */
#define YMODEM_UART_IDX 4

#define CONSOLE_TXD                 PB4
#define CONSOLE_RXD                 PB5
#define CONSOLE_TXD_FUNC            PB4_UART4_TX
#define CONSOLE_RXD_FUNC            PB5_UART4_RX

#define AT_UART_TXD                 PB19
#define AT_UART_RXD                 PB20
#define AT_UART_TXD_FUNC            PB19_UART0_TX
#define AT_UART_RXD_FUNC            PB20_UART0_RX

#define HCI_UART_TXD                PB6
#define HCI_UART_RXD                PB7
#define HCI_UART_TXD_FUNC           PB6_UART1_TX
#define HCI_UART_RXD_FUNC           PB7_UART1_RX

/* 按键Key */
#define APP_USER_KEY                PB10

/* LED */
#define APP_PWM_LED                 PA7
#define APP_PWM_LED_FUNC            PA7_PWM
#define APP_PWM_LED_CH              4

/* BT RESET */
#define BT_RESET_PIN                PB11

/* PSRAM */
#define PSRAM_CK_IO                     PB0
#define PSRAM_CS_IO                     PB1
#define PSRAM_DAT0_IO                     PB2
#define PSRAM_DAT1_IO                     PB3
#define PSRAM_DAT2_IO                     PB4
#define PSRAM_DAT3_IO                     PB5
#define PSRAM_CK_FUNC                   PB0_PSRAM_CK
#define PSRAM_CS_FUNC                   PB1_PSRAM_CS
#define PSRAM_DAT0_FUNC                   PB2_PSRAM_DAT0
#define PSRAM_DAT1_FUNC                   PB3_PSRAM_DAT1
#define PSRAM_DAT2_FUNC                   PB4_PSRAM_DAT2
#define PSRAM_DAT3_FUNC                   PB5_PSRAM_DAT3

#define NSP_CLK_PIN                       PB8
#define NSP_SDA_PIN                       PB9


/* UART示例 默认串口波特率 */
#define BAUD_RATE_UART 1000000

/* UART示例引脚配置 */
#define CONFIG_UART_DEMO_IDX    CONFIG_AT_UART_IDX
#define UART_DEMO_TX            CONSOLE_TXD
#define UART_DEMO_RX            CONSOLE_RXD
#define UART_DEMO_TX_FUNC       CONSOLE_TXD_FUNC
#define UART_DEMO_RX_FUNC       CONSOLE_RXD_FUNC

#endif /* __BOARD_CONFIG_H__ */

