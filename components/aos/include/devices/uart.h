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
} hal_uart_data_width_t;

/*
 * UART stop bits
 */
typedef enum {
    STOP_BITS_1 = 0,
    STOP_BITS_2 = 1
} hal_uart_stop_bits_t;

/*
 * UART flow control
 */
typedef enum {
    FLOW_CONTROL_DISABLED,
    FLOW_CONTROL_CTS,
    FLOW_CONTROL_RTS,
    FLOW_CONTROL_CTS_RTS
} hal_uart_flow_control_t;

/*
 * UART parity
 */
typedef enum {
    PARITY_NONE = 0, ///< No Parity (default)
    PARITY_EVEN = 1, ///< Even Parity
    PARITY_ODD  = 2, ///< Odd Parity
} hal_uart_parity_t;
/*
 * UART mode
 */
typedef enum {
    MODE_TX,
    MODE_RX,
    MODE_TX_RX
} hal_uart_mode_t;

typedef struct {
    uint32_t                baud_rate;
    hal_uart_data_width_t   data_width;
    hal_uart_parity_t       parity;
    hal_uart_stop_bits_t    stop_bits;
    hal_uart_flow_control_t flow_control;
    hal_uart_mode_t         mode;
} uart_config_t;

typedef struct uart_init_config {
    int idx;
} uart_init_config_t;

enum uart_type_t {
    UART_TYPE_GENERAL,
    UART_TYPE_CONSOLE,
};

#define uart_open(name) device_open(name)
#define uart_open_id(name, id) device_open_id(name, id)
#define uart_close(dev) device_close(dev)

int uart_config(aos_dev_t *dev, uart_config_t *config);
int uart_set_type(aos_dev_t *dev, enum uart_type_t type);
int uart_set_buffer_size(aos_dev_t *dev, uint32_t size);
int uart_send(aos_dev_t *dev, const void *data, uint32_t size);
int uart_recv(aos_dev_t *dev, void *data, uint32_t size, unsigned int timeout_ms);
void uart_set_event(aos_dev_t *dev, void (*event)(aos_dev_t *dev, int event_id, void *priv), void *priv);
void uart_config_default(uart_config_t *config);


#ifdef __cplusplus
}
#endif

#endif
