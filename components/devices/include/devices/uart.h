/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef DEVICE_UART_PAI_H
#define DEVICE_UART_PAI_H

#ifdef __cplusplus
extern "C" {
#endif

#include <devices/device.h>

#define USART_EVENT_READ  (1UL << 0)
#define USART_EVENT_WRITE (1UL << 1)
#define USART_OVERFLOW    (1UL << 2)

/*
 * UART data width
 */
typedef enum {
    DATA_WIDTH_5BIT,
    DATA_WIDTH_6BIT,
    DATA_WIDTH_7BIT,
    DATA_WIDTH_8BIT,
    DATA_WIDTH_9BIT
} rvm_hal_uart_data_width_t;

/*
 * UART stop bits
 */
typedef enum {
    STOP_BITS_1 = 0,
    STOP_BITS_2 = 1
} rvm_hal_uart_stop_bits_t;

/*
 * UART flow control
 */
typedef enum {
    FLOW_CONTROL_DISABLED,
    FLOW_CONTROL_CTS,
    FLOW_CONTROL_RTS,
    FLOW_CONTROL_CTS_RTS
} rvm_hal_uart_flow_control_t;

/*
 * UART parity
 */
typedef enum {
    PARITY_NONE = 0, ///< No Parity (default)
    PARITY_EVEN = 1, ///< Even Parity
    PARITY_ODD  = 2, ///< Odd Parity
} rvm_hal_uart_parity_t;
/*
 * UART mode
 */
typedef enum {
    MODE_TX,
    MODE_RX,
    MODE_TX_RX
} rvm_hal_uart_mode_t;

typedef struct {
    uint32_t                baud_rate;
    rvm_hal_uart_data_width_t   data_width;
    rvm_hal_uart_parity_t       parity;
    rvm_hal_uart_stop_bits_t    stop_bits;
    rvm_hal_uart_flow_control_t flow_control;
    rvm_hal_uart_mode_t         mode;
} rvm_hal_uart_config_t;

typedef struct uart_init_config {
    int idx;
} rvm_hal_uart_init_config_t;

enum rvm_hal_uart_type_t {
    UART_TYPE_ASYNC,
    UART_TYPE_SYNC
};

#define rvm_hal_uart_open(name) rvm_hal_device_open(name)
#define rvm_hal_uart_close(dev) rvm_hal_device_close(dev)

int rvm_hal_uart_config(rvm_dev_t *dev, rvm_hal_uart_config_t *config);
int rvm_hal_uart_set_type(rvm_dev_t *dev, enum rvm_hal_uart_type_t type);
int rvm_hal_uart_set_buffer_size(rvm_dev_t *dev, uint32_t size);
int rvm_hal_uart_send_poll(rvm_dev_t *dev, const void *data, uint32_t size);
int rvm_hal_uart_recv_poll(rvm_dev_t *dev, void *data, uint32_t size);
int rvm_hal_uart_send(rvm_dev_t *dev, const void *data, uint32_t size, uint32_t timeout_ms);
int rvm_hal_uart_recv(rvm_dev_t *dev, void *data, uint32_t size, uint32_t timeout_ms);
void rvm_hal_uart_set_event(rvm_dev_t *dev, void (*event)(rvm_dev_t *dev, int event_id, void *priv), void *priv);
void rvm_hal_uart_config_default(rvm_hal_uart_config_t *config);


#ifdef __cplusplus
}
#endif

#endif
