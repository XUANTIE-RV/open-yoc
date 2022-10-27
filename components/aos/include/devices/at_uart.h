/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef DEVICE_AT_UART_H
#define DEVICE_AT_UART_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int32_t at_uart_init(void);
int32_t at_uart_deinit(void);

void at_uart_send_str(const char *fmt, ...);
void at_uart_send_data(uint8_t *data, uint32_t len);

void at_uart_send_evt_str(const char *name, const char *para);
void at_uart_send_evt_str_errid(const char *name, const char *para, int errid);
void at_uart_send_evt_int(const char *name, uint32_t val);
void at_uart_send_evt_recv(const char *name, uint16_t id, void *payload, uint16_t tot_len);

#ifdef __cplusplus
}
#endif

#endif
