#ifndef AT_INTERNAL_H
#define AT_INTERNAL_H

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#define AT_UART_EVENT_READ  (1UL << 0)
#define AT_UART_EVENT_WRITE (1UL << 1)
#define AT_UART_OVERFLOW    (1UL << 2)

typedef void (*uart_event_t)(int event_id, void *priv);

void *at_uart_init(const char *name, void *config);
int at_uart_set_event(void *uart_hdl, uart_event_t evt_cb, void *priv);
int at_uart_set_baud(void *hdl, int uartbaud);
int at_uart_send(void *hdl, const char *data, int size);
int at_uart_recv(void *hdl, const char *data, int size, int timeout);

#ifdef __cplusplus
}
#endif

#endif