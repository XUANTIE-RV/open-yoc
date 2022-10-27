/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __BOARD_H__
#define __BOARD_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <soc.h>

#define CONSOLE_UART_IDX                (2)
#define CONFIG_CLI_USART_BAUD           (2000000)
#define CONFIG_CONSOLE_UART_BUFSIZE     (1024)

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

