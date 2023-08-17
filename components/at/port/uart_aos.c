#include <stdio.h>
#include <errno.h>

#include <aos/aos.h>
#include <devices/uart.h>
#include <devices/driver.h>

#include "yoc/at_port.h"

typedef struct {
    rvm_dev_t      *dev;
    channel_event_t cb;
    void           *priv;
} aos_uart_t;

static aos_uart_t at_uart;
static int at_flag = 0;

#define AT_UART_RINGBUF_SIZE 2048

static void at_uart_event(rvm_dev_t *uart_hdl, int event_id, void *priv)
{
    if (event_id == USART_EVENT_READ) {
        aos_uart_t *uart = (aos_uart_t *)&at_uart;
        if (uart->cb) {
            uart->cb(AT_CHANNEL_EVENT_READ, uart->priv);
        }
    }
}

static void *at_uart_init(const char *name, void *config)
{
    // char *dev_name  = strdup(name);
    // int len = strlen(name);

    // int idx = name[len-1] - 0x30;

    // dev_name[len-1] = 0;

    // memset(&at_uart, 0, sizeof(aos_uart_t));

    at_uart.dev = rvm_hal_device_open(name);

    // aos_free(dev_name);
    if (at_uart.dev == NULL) {
        return NULL;
    }
    rvm_hal_uart_config(at_uart.dev, config);
    rvm_hal_uart_set_buffer_size(at_uart.dev, AT_UART_RINGBUF_SIZE);

    return &at_uart;
}

static int at_uart_set_event(void *hdl, channel_event_t evt_cb, void *priv)
{
    aos_uart_t *uart = (aos_uart_t *)hdl;
    uart->cb   = evt_cb;
    uart->priv = priv;

    if (evt_cb) {
        at_flag = 1;
    } else {
        at_flag = 0;
    }

    rvm_hal_uart_set_event(uart->dev, at_uart_event, priv);

    return 0;
}

static int at_uart_set_baud(void *hdl, int uartbaud)
{
    aos_uart_t *uart = (aos_uart_t *)hdl;
    rvm_hal_uart_config_t config;
    memset(&config, 0, sizeof(config));
    rvm_hal_uart_config_default(&config);
    config.baud_rate = uartbaud;
    return (rvm_hal_uart_config(uart->dev, &config));
  
}

static int at_uart_send(void *hdl, const char *data, int size)
{
    aos_uart_t *uart = (aos_uart_t *)hdl;
    return (rvm_hal_uart_send(uart->dev, data, size, AOS_WAIT_FOREVER));
}

static int at_uart_recv(void *hdl, const char *data, int size, int timeout)
{
    aos_uart_t *uart = (aos_uart_t *)hdl;

    if (at_flag == 0) {
        return 0;
    }

    uint32_t recv_len = 1;

    recv_len = rvm_hal_uart_recv(uart->dev, (void *)data, size, timeout);

    return recv_len;
}

at_channel_t uart_channel = {
    .init       = at_uart_init,
    .set_event  = at_uart_set_event,
    .set_baud   = at_uart_set_baud,
    .send       = at_uart_send,
    .recv       = at_uart_recv,
};
