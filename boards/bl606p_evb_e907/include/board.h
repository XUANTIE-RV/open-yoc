/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __BOARD_H__
#define __BOARD_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <soc.h>

// uart config
#define CONSOLE_UART_IDX                (0)
#define CONFIG_CLI_USART_BAUD           (2000000)
#define CONFIG_CONSOLE_UART_BUFSIZE     (1024)

/**
 * @brief  init the board for default: pin mux, etc.
 * re-implement if need.
 * @return
 */
void board_init(void);

#ifdef CONFIG_BOARD_AUDIO
#include "audio/board_audio.h"
#endif

#ifdef CONFIG_BOARD_BUTTON
#include "button/board_button.h"
#endif

#ifdef __cplusplus
}
#endif

#endif /* __BOARD_H__ */

