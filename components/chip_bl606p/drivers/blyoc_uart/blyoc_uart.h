
#ifndef __BLYOC_UART_H__
#define __BLYOC_UART_H__

typedef void (*cb_uart_notify_t)(void *arg);
typedef struct bl_uart_notify {
    cb_uart_notify_t rx_cb;
    void            *rx_cb_arg;

    cb_uart_notify_t tx_cb;
    void            *tx_cb_arg;
} bl_uart_notify_t;

void uart_generic_notify_handler(void *utx);
int bl_uart_int_rx_notify_register(uint8_t id, cb_uart_notify_t cb, void *arg);
int bl_uart_int_tx_notify_register(uint8_t id, cb_uart_notify_t cb, void *arg);
int bl_uart_int_rx_notify_unregister(uint8_t id, cb_uart_notify_t cb, void *arg);
int bl_uart_int_tx_notify_unregister(uint8_t id, cb_uart_notify_t cb, void *arg);

void blcsi_uart_init(uint8_t id);
int32_t blcsi_uart_send(uint8_t id, uint8_t *buf, uint32_t len);
void blcsi_uart_uninit(uint8_t id);

#endif

