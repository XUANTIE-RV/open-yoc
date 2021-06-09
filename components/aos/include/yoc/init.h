/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef YOC_INCLUDE_INIT_H
#define YOC_INCLUDE_INIT_H

#include <stdint.h>
#include <devices/console_uart.h>
#include <devices/apdu_uart.h>
#include <yoc/lpm.h>

#ifdef __cplusplus
extern "C" {
#endif

/* board */
void board_init(void);
void board_base_init(void);
void board_audio_init(void);
void board_enter_lpm(pm_policy_t policy);
void board_leave_lpm(pm_policy_t policy);

#ifdef __cplusplus
}
#endif

#endif
