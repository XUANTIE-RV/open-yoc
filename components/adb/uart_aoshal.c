#include <stdio.h>
#include <errno.h>

#include <aos/aos.h>
#include <aos/hal/uart.h>

#include "yoc/adb_port.h"

typedef struct {
    uart_dev_t     *dev;
    channel_event_t cb;
    void           *priv;
} aos_uart_t;

static aos_uart_t g_adb_uart;

static int32_t adb_uart_event(uart_dev_t *uart_hdl)
{
    // if (event_id == UART_EVENT_RECEIVE_COMPLETE) {
    aos_uart_t *uart = &g_adb_uart;
    if (uart->cb) {
        uart->cb(ADB_CHANNEL_EVENT_READ, NULL);
    }
    //}
    return 0;
}

static int adb_uart_init(void *hdl, void *config)
{
    uart_dev_t *uart = (uart_dev_t *)hdl;

    return hal_uart_init(uart);
}

static int adb_uart_set_event(void *hdl, channel_event_t evt_cb, void *priv)
{
    uart_dev_t *uart = (uart_dev_t *)hdl;

    g_adb_uart.cb   = evt_cb;
    g_adb_uart.priv = priv;

    return hal_uart_recv_cb_reg(uart, adb_uart_event);
}

static int adb_uart_send(void *hdl, const char *data, int size)
{
    uart_dev_t *uart = (uart_dev_t *)hdl;

    return hal_uart_send(uart, data, size, AOS_WAIT_FOREVER);
}

static int adb_uart_recv(void *hdl, const char *data, int size, int timeout)
{
    uart_dev_t *uart = (uart_dev_t *)hdl;

    if (g_adb_uart.cb == NULL) {
        return 0;
    }

    uint32_t recv_size = 0;

    hal_uart_recv_II(uart, (void *)data, size, &recv_size, timeout);

    return recv_size;
}

adb_channel_t adb_uart_channel = {
    .init      = adb_uart_init,
    .set_event = adb_uart_set_event,
    .send      = adb_uart_send,
    .recv      = adb_uart_recv,
};
