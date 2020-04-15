/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef YOC_INCLUDE_INIT_H
#define YOC_INCLUDE_INIT_H

#include <stdint.h>
#include <yoc/uservice.h>
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

/* base driver */
extern void uart_csky_register(int idx);
extern void flash_csky_register(int idx);

/* service */
extern void console_init(int id, uint32_t baud, uint16_t buf_size);
extern void console_deinit();
extern const char *console_get_devname(void);
extern uint16_t console_get_buffer_size(void);

extern int  lwip_tcpip_init(void);
extern void at_server_init(utask_t *task);
extern void netmgr_service_init(utask_t *task);
extern void netmgr_service_deinit();
extern void netmgr_service_stop();

#ifdef __cplusplus
}
#endif

#endif
