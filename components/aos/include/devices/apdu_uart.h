/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __APDU_UART_H__
#define __APDU_UART_H__

#include <aos/aos.h>

#ifdef __cplusplus
extern "C" {
#endif

void apdu_init(int id, uint32_t baud, uint16_t buf_size);
void apdu_deinit();
const char *apdu_get_devname(void);
uint16_t apdu_get_buffer_size(void);

int apdu_uart_putc(int ch);
int apdu_uart_getc(void);

#ifdef __cplusplus
}
#endif

#endif /* __APDU_UART_H__ */

