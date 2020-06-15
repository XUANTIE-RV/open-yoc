/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include "soc.h"
#include "pin_name.h"
#include <drv/usart.h>
#include "board_config.h"
#include <pinmux.h>
#include "ringbuffer.h"
#include "app_printf.h"

#define UART_RINGBUF_LEN 64

char recv_buf[UART_RINGBUF_LEN];
dev_ringbuf_t   ring_buffer;

static volatile uint8_t tx_async_flag = 0;
static usart_handle_t g_csi_usart;

typedef struct {
    int32_t    tx;
    int32_t    rx;
    uint16_t   uart_id;
    uint32_t   tx_func;
    uint32_t   rx_func;
} usart_pinmap_t;
const static usart_pinmap_t g_usart_pinmap[] = {
    {
        PB0,
        PB1,
        1,
        PB0_UART1_TX,
        PB1_UART1_RX
    },
    {
        PA4,
        PA5,
        2,
        PA4_UART2_TX,
        PA5_UART2_RX
    }
};


static int32_t usart_pin_init(int32_t uart_id)
{
    for(int i = 0; i < sizeof(g_usart_pinmap) / sizeof(usart_pinmap_t); i++) {
        if(g_usart_pinmap[i].uart_id == uart_id) {
            drv_pinmux_config(g_usart_pinmap[i].tx, g_usart_pinmap[i].tx_func);
            drv_pinmux_config(g_usart_pinmap[i].rx, g_usart_pinmap[i].rx_func);
            return i;
        }
    }
    return -1;
}

int32_t usart_receive(void *data, uint32_t num)
{
    uint8_t *temp_buf = (uint8_t *)data;
    return ringbuffer_read(&ring_buffer, temp_buf, num);
}

int32_t usart_send(const void *data, uint32_t num)
{
    csi_usart_send(g_csi_usart, data, num);
    while (tx_async_flag == 0);

    tx_async_flag = 0;
    return 0;
}


static void usart_event_cb(int32_t idx, uint32_t event)
{
    int32_t ret;
    uint8_t tmp_buf[16];

    switch (event) {
        case USART_EVENT_SEND_COMPLETE:
            tx_async_flag = 1;
            break;

        case USART_EVENT_RECEIVE_COMPLETE:
        case USART_EVENT_RECEIVED:
            do {
                ret = csi_usart_receive_query(g_csi_usart, tmp_buf, 1);
                if (ret > 0) {
                    LOGD("ringbuffer_write data = 0x%x\n", tmp_buf[0]);
                    if (ringbuffer_write(&ring_buffer, tmp_buf, ret) != ret) {
                        break;
                    }
                }
            } while(ret);
            break;
        default:
            break;
    }
}

int usart_init(int32_t uart_id)
{
    int32_t idx;
    idx = usart_pin_init(uart_id);
    if(idx < 0)
        return -1;
    g_csi_usart = csi_usart_initialize(idx, usart_event_cb);

    ringbuffer_create(&ring_buffer, recv_buf, UART_RINGBUF_LEN);

    /* config the USART */
    return csi_usart_config(g_csi_usart, 115200, USART_MODE_ASYNCHRONOUS, USART_PARITY_NONE, USART_STOP_BITS_1, USART_DATA_BITS_8);
}
