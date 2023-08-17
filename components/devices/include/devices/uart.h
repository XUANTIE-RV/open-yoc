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

/// For ADB mode
#define RVM_HAL_SHADOW_PORT_MASK 0x80

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

/**
 \brief      Set the config for a uart dev
 \param[in]  dev      Pointer to device object.
 \param[in]  config   the uart config
 \return     0 : on success,  otherwise is error
 */
int rvm_hal_uart_config(rvm_dev_t *dev, rvm_hal_uart_config_t *config);

/**
 \brief      Get the config for a uart dev
 \param[in]  dev      Pointer to device object.
 \param[out] config   the uart config
 \return     0 : on success,  otherwise is error
 */
int rvm_hal_uart_config_get(rvm_dev_t *dev, rvm_hal_uart_config_t *config);

/**
 \brief      Set send type with a sync or async mode
 \param[in]  dev      Pointer to device object.
 \param[in]  type     the send data type
 \return     0 : on success,  otherwise is error
 */
int rvm_hal_uart_set_type(rvm_dev_t *dev, enum rvm_hal_uart_type_t type);

/**
 \brief      Set the receive buffer size
 \param[in]  dev      Pointer to device object.
 \param[in]  size     the buffer size
 \return     0 : on success,  otherwise is error
 */
int rvm_hal_uart_set_buffer_size(rvm_dev_t *dev, uint32_t size);

/**
 \brief      Transmit data on a uart dev with a poll mode
 \param[in]  dev      Pointer to device object.
 \param[in]  data     pointer to the start of data
 \param[in]  size     number of bytes to transmit
 \return     actual send data num
 */
int rvm_hal_uart_send_poll(rvm_dev_t *dev, const void *data, uint32_t size);

/**
 \brief       Receive data on a uart dev with a poll mode
 \param[in]   dev      Pointer to device object.
 \param[out]  data     pointer to the buffer which will store incoming data
 \param[in]   size     number of bytes to receive
 \return      the actual recived data number
 */
int rvm_hal_uart_recv_poll(rvm_dev_t *dev, void *data, uint32_t size);

/**
 \brief      Transmit data on a uart dev
 \param[in]  dev      Pointer to device object.
 \param[in]  data     pointer to the start of data
 \param[in]  size     number of bytes to transmit
 \param[in]  timeout  timeout in milisecond, set this value to AOS_WAIT_FOREVER if you want to wait forever
 \return     0 : on success,  otherwise is error;
 \return     actual send data num : on success,  otherwise is error, when uart type is UART_TYPE_SYNC
 */
int rvm_hal_uart_send(rvm_dev_t *dev, const void *data, uint32_t size, uint32_t timeout_ms);

/**
 \brief       Receive data on a uart dev
 \param[in]   dev      Pointer to device object.
 \param[out]  data     pointer to the buffer which will store incoming data
 \param[in]   size     number of bytes to receive
 \param[in]   timeout  timeout in milisecond, set this value to AOS_WAIT_FOREVER if you want to wait forever
 \return      the actual recived data number, < 0 when error occur
 */
int rvm_hal_uart_recv(rvm_dev_t *dev, void *data, uint32_t size, uint32_t timeout_ms);

/**
 \brief       Set a event callback
 \param[in]   dev      Pointer to device object.
 \param[out]  event    the event callback function
 \param[in]   priv     the argument for the callback function
 \return      None
 */
void rvm_hal_uart_set_event(rvm_dev_t *dev, void (*event)(rvm_dev_t *dev, int event_id, void *priv), void *priv);

/**
 \brief       Get a default config
 \param[out]  config   Pointer to the default config data
 \return      None
 */
void rvm_hal_uart_config_default(rvm_hal_uart_config_t *config);

/**
 \brief      Enable or disable transmition DMA mode
 \param[in]  dev      Pointer to device object.
 \param[in]  enable   true:enable, false:disable
 \return     0 : on success,  otherwise is error
 */
int rvm_hal_uart_trans_dma_enable(rvm_dev_t *dev, bool enable);


#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <devices/vfs_uart.h>
#endif

#ifdef __cplusplus
}
#endif

#endif
