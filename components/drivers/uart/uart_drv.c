/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <aos/log.h>
#include <aos/kernel.h>
#include "hal/uart_impl.h"

#include <aos/ringbuffer.h>

#include "drv/usart.h"

#include <yoc/lpm.h>

#define UART_RB_SIZE    1024

#define TAG "uart_drv"

#define EVENT_WRITE 0x0F0F0000
#define EVENT_READ  0x00000F0F

#define UART_MAX_NUM (4)
typedef struct {
    aos_dev_t      device;
    int            flowctrl;
    usart_handle_t handle;
    char          *recv_buf;
    dev_ringbuf_t  read_buffer;
    aos_mutex_t    mutex;
    aos_event_t    event_write_read;
    int            event_state;
    void (*write_event)(aos_dev_t *dev, int event_id, void *priv);
    void *priv;
    void *init_cfg;
    int   type;
} uart_dev_t;
static uart_dev_t *g_uart_idx[UART_MAX_NUM];

#define uart(dev) ((uart_dev_t *)dev)

static int32_t get_uart_dev_idx(int32_t uart_idx)
{
    int32_t ret = -1;

    for (int i = 0; i < UART_MAX_NUM; i++) {
        if (g_uart_idx[i] != NULL) {
            if (g_uart_idx[i]->device.id == uart_idx) {
                ret = i;                                // get the same idx device
                break;
            }
        }
    }

    return ret;
}

static int32_t get_free_uart_idx(int32_t uart_idx)
{
    int32_t ret = get_uart_dev_idx(uart_idx);

    if (ret == -1){
        for (int i = 0; i < UART_MAX_NUM; i++) {
            if (g_uart_idx[i] == NULL) {
                ret = i;                                // get null idx devcie
                break;
            }
        }
    }

    return ret;
}



static aos_dev_t *uart_csky_init(driver_t *drv, void *config, int id)
{
    uart_dev_t *uart = (uart_dev_t *)device_new(drv, sizeof(uart_dev_t), id);

    if (uart) {
        uart->init_cfg   = config;
    }
    return (aos_dev_t *)uart;
}

#define uart_csky_uninit device_free

static void usart_csky_event_cb_fun(int32_t uart_idx, usart_event_e event)
{
    uart_dev_t* uart = NULL;
    int32_t idx = get_uart_dev_idx(uart_idx);

    if (idx >= 0 && idx < UART_MAX_NUM) {
        uart = g_uart_idx[idx];
    }

    if (!uart)
        return;

    switch (event) {
    case USART_EVENT_SEND_COMPLETE:
        if ((uart->event_state & USART_EVENT_WRITE) == 0) {
            uart->event_state |= USART_EVENT_WRITE;

            if (uart->write_event)
                uart->write_event((aos_dev_t *)uart, USART_EVENT_WRITE, uart->priv);
        }

        if (aos_event_set(&uart->event_write_read, EVENT_WRITE, AOS_EVENT_OR) != 0) {
        }
        break;

    case USART_EVENT_RECEIVED: {
        int32_t ret;
#if 0
        /* flow ctrl */
        if (usart_dev->flowctrl) {
            ret = ringbuffer_available_write_space(usart_dev->read_buffer);

            if (ret <= 64) {
                csi_usart_interrupt_on_off(handle, USART_INTR_READ, 0);
                break;
            }
        }

#endif
        if (uart->recv_buf != NULL) {
            char buffer[16];

            do {
                ret = csi_usart_receive_query(uart->handle, buffer, sizeof(buffer));

                if (ret > 0) {
                    if (ringbuffer_write(&uart->read_buffer, (uint8_t*)buffer, ret) != ret) {
                        if (uart->write_event) {
                            uart->write_event((aos_dev_t *)uart, USART_OVERFLOW, uart->priv);
                        }
                        break;
                    }
                }
            } while (ret);
        }

        if ((uart->event_state & USART_EVENT_READ) == 0) {

            if (uart->write_event) {
                uart->event_state |= USART_EVENT_READ;
                uart->write_event((aos_dev_t *)uart, USART_EVENT_READ, uart->priv);
            }
        }
        aos_event_set(&uart->event_write_read, EVENT_READ, AOS_EVENT_OR);
        break;
    };

    case USART_EVENT_RX_OVERFLOW: {
        char buffer[16];
        uint32_t ret;

        do {
            /* lost some data, clean hw buffer and ringbuffer */
            ret = csi_usart_receive_query(uart->handle, buffer, sizeof(buffer));
        } while (ret);

        if (uart->recv_buf != NULL)
            ringbuffer_clear(&uart->read_buffer);
        uart->event_state &= ~USART_EVENT_READ;

        break;
    }

    case USART_EVENT_RX_FRAMING_ERROR:
    default:
        // LOGW(TAG, "uart%d event %d", idx, event);
        break;
    }
}

static int uart_csky_open(aos_dev_t *dev)
{
    int32_t idx = get_free_uart_idx(dev->id);

    if (!(idx >= 0 && idx < UART_MAX_NUM)) {
        return -1;
    }

    if (aos_mutex_new(&uart(dev)->mutex) != 0) {
        goto error0;
    }

    if (aos_event_new(&uart(dev)->event_write_read, 0) != 0) {
        goto error1;
    }

    uart(dev)->recv_buf = (char *)aos_malloc(UART_RB_SIZE);

    if (uart(dev)->recv_buf == NULL) {
        goto error2;
    }

    ringbuffer_create(&uart(dev)->read_buffer, uart(dev)->recv_buf, UART_RB_SIZE);

    uart(dev)->handle = csi_usart_initialize(dev->id, usart_csky_event_cb_fun);

    if (uart(dev)->handle != NULL) {
        g_uart_idx[idx] = uart(dev);
        return 0;
    }

error2:
    aos_event_free(&uart(dev)->event_write_read);
error1:
    aos_mutex_free(&uart(dev)->mutex);
error0:

    return -1;
}

static int uart_csky_close(aos_dev_t *dev)
{
    csi_usart_uninitialize(uart(dev)->handle);
    int32_t idx = get_uart_dev_idx(dev->id);
    if (idx != -1) {
        g_uart_idx[idx] = NULL;
    }
    aos_mutex_free(&uart(dev)->mutex);
    aos_event_free(&uart(dev)->event_write_read);
    aos_free(uart(dev)->recv_buf);

    return 0;
}

static int uart_csky_config(aos_dev_t *dev, uart_config_t *config)
{
    int32_t ret = csi_usart_config(uart(dev)->handle, config->baud_rate, USART_MODE_ASYNCHRONOUS,
                                   config->parity, config->stop_bits, config->data_width);

    if (ret < 0) {
        return -EIO;
    }

    ret = csi_usart_config_flowctrl(uart(dev)->handle, config->flow_control);

    if (ret == 0) {
        uart(dev)->flowctrl = 1;
    }

    return 0;
}

static int uart_csky_set_type(aos_dev_t *dev, int type)
{
    uart(dev)->type = type;

    return 0;
}

static int uart_csky_set_buffer_size(aos_dev_t *dev, uint32_t size)
{
    char *tmp = NULL;

    if (size > 0) {
        tmp = aos_malloc(size);

        if (tmp == NULL) {
            return -ENOMEM;
        }
    }

    aos_free(uart(dev)->recv_buf);

    uart(dev)->recv_buf = tmp;
    if (tmp)
        ringbuffer_create(&uart(dev)->read_buffer, tmp, size);

    return 0;
}

static int uart_csky_send(aos_dev_t *dev, const void *data, uint32_t size)
{
    if (uart(dev)->type == UART_TYPE_CONSOLE) {
        int i;
        for (i = 0; i < size; i++) {
            //dont depend interrupt
            csi_usart_putchar(uart(dev)->handle, *((uint8_t *)data + i));
        }
    } else {
        unsigned int actl_flags = 0;
        csi_usart_send(uart(dev)->handle, data, size);
        aos_event_get(&uart(dev)->event_write_read, EVENT_WRITE, AOS_EVENT_OR_CLEAR, &actl_flags,
                      AOS_WAIT_FOREVER);
    }

    return 0;
}

static int uart_csky_recv(aos_dev_t *dev, void *data, uint32_t size, unsigned int timeout_ms)
{
    unsigned int actl_flags;
    int          ret = 0;
    long long    time_enter, used_time;
    void *       temp_buf   = data;
    uint32_t     temp_count = size;
    time_enter              = aos_now_ms();
    uart(dev)->event_state &= ~USART_EVENT_READ;

    while (1) {
        if (uart(dev)->recv_buf != NULL)
            ret = ringbuffer_read(&uart(dev)->read_buffer, (uint8_t *)temp_buf, temp_count);
        else
            ret = csi_usart_receive_query(&uart(dev)->handle, (uint8_t *)temp_buf, temp_count);

        temp_count = temp_count - ret;
        temp_buf   = (uint8_t *)temp_buf + ret;

        if (temp_count == 0 || timeout_ms == 0 ||
            timeout_ms <= (used_time = aos_now_ms() - time_enter)) {
            break;
        }

        if (aos_event_get(&uart(dev)->event_write_read, EVENT_READ, AOS_EVENT_OR_CLEAR, &actl_flags,
                          timeout_ms - used_time) == -1) {
            break;
        }
    }

    return size - temp_count;
}

static void uart_csky_event(aos_dev_t *dev, void (*event)(aos_dev_t *dev, int event_id, void *priv),
                            void * priv)
{
    uart(dev)->priv        = priv;
    uart(dev)->write_event = event;
}

enum {
    EVENT_SEND_COMPLETE     = 0,  ///< Send completed; however USART may still transmit data
    EVENT_RECEIVE_COMPLETE  = 1,  ///< Receive completed
    EVENT_TRANSFER_COMPLETE = 2,  ///< Transfer completed
    EVENT_TX_COMPLETE       = 3,  ///< Transmit completed (optional)
    EVENT_TX_UNDERFLOW      = 4,  ///< Transmit data not available (Synchronous Slave)
    EVENT_RX_OVERFLOW       = 5,  ///< Receive data overflow
    EVENT_RX_TIMEOUT        = 6,  ///< Receive character timeout (optional)
    EVENT_RX_BREAK          = 7,  ///< Break detected on receive
    EVENT_RX_FRAMING_ERROR  = 8,  ///< Framing error detected on receive
    EVENT_RX_PARITY_ERROR   = 9,  ///< Parity error detected on receive
    EVENT_CTS               = 10, ///< CTS state changed (optional)
    EVENT_DSR               = 11, ///< DSR state changed (optional)
    EVENT_DCD               = 12, ///< DCD state changed (optional)
    EVENT_RI                = 13, ///< RI  state changed (optional)
    EVENT_RECEIVED          = 14, ///< Data Received, only in usart fifo, call receive()/transfer() get the data
};

static uart_driver_t uart_driver = {
    .drv = {
        .name   = "uart",
        .init   = uart_csky_init,
        .uninit = uart_csky_uninit,
        .open   = uart_csky_open,
        .close  = uart_csky_close,
    },
    .config          = uart_csky_config,
    .set_type        = uart_csky_set_type,
    .set_buffer_size = uart_csky_set_buffer_size,
    .send            = uart_csky_send,
    .recv            = uart_csky_recv,
    .set_event       = uart_csky_event,
};

void uart_csky_register(int uart_idx)
{
    driver_register(&uart_driver.drv, NULL, uart_idx);
}

usart_handle_t dev_get_handler(aos_dev_t *dev)
{
    uart_dev_t *uart = (uart_dev_t *)dev;

    return uart->handle;
}
