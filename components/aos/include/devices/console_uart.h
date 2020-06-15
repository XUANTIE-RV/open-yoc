/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __CONSOLE_UART_H__
#define __CONSOLE_UART_H__

#include <aos/aos.h>

#ifdef __cplusplus
extern "C" {
#endif

void console_init(int id, uint32_t baud, uint16_t buf_size);
void console_deinit();
const char *console_get_devname(void);
uint16_t console_get_buffer_size(void);

#ifdef __cplusplus
}
#endif

#endif /* __CONSOLE_UART_H__ */

