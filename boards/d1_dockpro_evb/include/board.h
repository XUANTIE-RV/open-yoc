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

#define CLOCK_GETTIME_USE_TIMER_ID     0
// uart config
#define CONSOLE_UART_IDX                (0)
#define CONFIG_CLI_USART_BAUD           (115200)
#define CONFIG_CONSOLE_UART_BUFSIZE     (512)

#define WLAN_ENABLE_PIN     PG12
#define WLAN_POWER_PIN      0xFFFFFFFF

#define LED_PIN             PC1     // LED RGB


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

#ifdef CONFIG_BOARD_DISPLAY
#include "display/board_display.h"
#endif

#ifdef __cplusplus
}
#endif

#endif /* __BOARD_H__ */

