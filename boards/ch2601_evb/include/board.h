/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __BOARD_H__
#define __BOARD_H__

#include <stdint.h>
#include <soc.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CLOCK_GETTIME_USE_TIMER_ID      (0)
// uart config
#define CONSOLE_UART_IDX                (0)
#define CONFIG_CLI_USART_BAUD           (115200)
#define CONFIG_CONSOLE_UART_BUFSIZE     (128)
#define CONSOLE_TXD                    PA23
#define CONSOLE_RXD                    PA24
#define CONSOLE_TXD_FUNC               PA23_UART0_TX
#define CONSOLE_RXD_FUNC               PA24_UART0_RX


/**
 * @brief  init the board for default: pin mux, etc.
 * re-implement if need.
 * @return
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* __BOARD_H__ */

