/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <aos/hal/uart.h>
#include <aos/kernel.h>
#include <errno.h>
#include <drv/usart.h>
#include <aos/ringbuffer.h>

#define HAL_UART_RINGBUF_LEN 256

typedef struct {
    usart_handle_t  handle;
    char            *recv_buf;
    aos_sem_t       uart_sem;
    dev_ringbuf_t   read_buffer;
} hal_uart_priv_t;

static hal_uart_priv_t uart_list[6];

static void uart_event_cb(int32_t idx, uint32_t event)
{
    int32_t ret;
    uint8_t tmp_buf[16];
    switch (event) {
        case USART_EVENT_SEND_COMPLETE:
            aos_sem_signal(&uart_list[idx].uart_sem);
            break;
        case USART_EVENT_RECEIVE_COMPLETE:
        case USART_EVENT_RECEIVED:
            do {
                ret = csi_usart_receive_query(uart_list[idx].handle, tmp_buf, 1);
                if (ret > 0) {
                    if (ringbuffer_write(&uart_list[idx].read_buffer, tmp_buf, ret) != ret) {
                        break;
                    }
                }
            } while(ret);
            break;
        default:
            break;
    }
}
/*****************************************/
typedef struct {
    uint32_t                idx;
    uint32_t                baud_rate;
    usart_data_bits_e       data_width;
    usart_parity_e          parity;
    usart_stop_bits_e       stop_bits;
    usart_flowctrl_type_e   flow_control;
    void                    *priv;
} csi_uart_config_t;
/******************************************/
static void analysis_uart_data_width_config(uart_dev_t *uart, csi_uart_config_t *uart_config)
{
    switch (uart->config.data_width) {
    case DATA_WIDTH_5BIT:
        uart_config->data_width = USART_DATA_BITS_5;
        break;
    case DATA_WIDTH_6BIT:
        uart_config->data_width = USART_DATA_BITS_6;
        break;
    case DATA_WIDTH_7BIT:
        uart_config->data_width = USART_DATA_BITS_7;
        break;
    case DATA_WIDTH_8BIT:
        uart_config->data_width = USART_DATA_BITS_8;
        break;
    case DATA_WIDTH_9BIT:
        uart_config->data_width = USART_DATA_BITS_9;
        break;
    default:
        /* default 8 bit */
        uart_config->data_width = USART_DATA_BITS_8;
        break;
    }
}

static void analysis_uart_parity_config(uart_dev_t *uart, csi_uart_config_t *uart_config)
{
    switch (uart->config.parity) {
    case NO_PARITY:
        uart_config->parity = USART_PARITY_NONE;
        break;
    case ODD_PARITY:
        uart_config->parity = USART_PARITY_ODD;
        break;
    case EVEN_PARITY:
        uart_config->parity = USART_PARITY_EVEN;
        break;
    default:
        /* default */
        uart_config->parity = USART_PARITY_NONE;
        break;
    }
}

static void analysis_uart_config(uart_dev_t *uart, csi_uart_config_t *uart_config)
{
    uart_config->idx = uart->port;
    uart_config->baud_rate = uart->config.baud_rate;
    /* find data width */
    analysis_uart_data_width_config(uart, uart_config);
    /* find parity */
    analysis_uart_parity_config(uart, uart_config);
    uart_config->stop_bits = uart->config.stop_bits;
    uart_config->flow_control = uart->config.flow_control;
}

int32_t hal_uart_init(uart_dev_t *uart)
{
    int32_t ret;
    csi_uart_config_t csi_uart_config;
    usart_handle_t usart_handle;

    if (uart == NULL)
        return -1;
    analysis_uart_config(uart, &csi_uart_config);

    usart_handle = csi_usart_initialize(uart->port, uart_event_cb);
    if (usart_handle == NULL) {
        return -1;
    }
    //store the csi uart handle
    uart->priv = usart_handle;
    uart_list[uart->port].handle = usart_handle;
    //malloc the space for ring buf
    uart_list[uart->port].recv_buf = (char *)aos_malloc(HAL_UART_RINGBUF_LEN);
    //init ring buf
    ringbuffer_create(&uart_list[uart->port].read_buffer, (char *)(uart_list[uart->port].recv_buf), HAL_UART_RINGBUF_LEN);
    //init sem
    ret = aos_sem_new(&uart_list[uart->port].uart_sem, 0);
    if (ret != 0) {
        return -1;
    }
    ret = csi_usart_config(usart_handle, csi_uart_config.baud_rate,
                           USART_MODE_ASYNCHRONOUS, csi_uart_config.parity,
                           csi_uart_config.stop_bits, csi_uart_config.data_width);
    if (ret != 0) {
        return -1;
    }
    return 0;
}

int32_t hal_uart_send(uart_dev_t *uart, const void *data, uint32_t size, uint32_t timeout)
{
    int32_t ret;
    if (uart == NULL)
        return -1;
    csi_usart_send(uart_list[uart->port].handle, data, size);
    ret = aos_sem_wait(&uart_list[uart->port].uart_sem, timeout);
    return ret;
}

int32_t hal_uart_recv(uart_dev_t *uart, void *data, uint32_t expect_size, uint32_t timeout)
{
    int32_t      ret = 0;
    long long    time_enter, used_time;
    void *       temp_buf   = data;
    uint32_t     temp_count = expect_size;
    time_enter              = aos_now_ms();

    while (1) {
        if (uart_list[uart->port].recv_buf != NULL) {
            ret = ringbuffer_read(&uart_list[uart->port].read_buffer, (uint8_t *)temp_buf, temp_count);
        } else {
            ret = csi_usart_receive_query(uart_list[uart->port].handle, (uint8_t *)temp_buf, temp_count);
        }
        temp_count = temp_count - ret;
        temp_buf   = (uint8_t *)temp_buf + ret;
        used_time  = aos_now_ms() - time_enter;

        if (timeout <= used_time || temp_count == 0 || timeout == 0) {
            break;
        }
    }
    if (timeout == 0 || temp_count != 0 || timeout <= used_time) {
        return -1;
    } else {
        return 0;
    }
}

int32_t hal_uart_recv_II(uart_dev_t *uart, void *data, uint32_t expect_size,
                         uint32_t *recv_size, uint32_t timeout)
{
    int32_t      ret = 0;
    long long    time_enter, used_time;
    void *       temp_buf   = data;
    uint32_t     temp_count = expect_size;
    time_enter              = aos_now_ms();

    while (1) {
        if (uart_list[uart->port].recv_buf != NULL) {
            ret = ringbuffer_read(&uart_list[uart->port].read_buffer, (uint8_t *)temp_buf, temp_count);
        } else {
            ret = csi_usart_receive_query(uart_list[uart->port].handle, (uint8_t *)temp_buf, temp_count);
        }
        temp_count = temp_count - ret;
        temp_buf   = (uint8_t *)temp_buf + ret;
        used_time  = aos_now_ms() - time_enter;

        if (timeout <= used_time || temp_count == 0 || timeout == 0) {
            break;
        }
    }
    *recv_size = expect_size - temp_count;
    return 0;
}

int32_t hal_uart_finalize(uart_dev_t *uart)
{
    aos_sem_free(&uart_list[uart->port].uart_sem);
    ringbuffer_destroy(&uart_list[uart->port].read_buffer);
    aos_free(uart_list[uart->port].recv_buf);
    csi_usart_uninitialize(uart_list[uart->port].handle);
    return 0;
}