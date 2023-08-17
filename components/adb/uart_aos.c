#include <stdio.h>
#include <errno.h>

#include <aos/aos.h>
#include <devices/uart.h>
#include <devices/driver.h>
#include <devices/devicelist.h>

#include "yoc/adb_port.h"

typedef struct {
    rvm_dev_t      *dev;
    channel_event_t cb;
    void           *priv;
} aos_uart_t;

static aos_uart_t adb_uart;
static int adb_flag = 0;
static int uart_reuse = 0;
static int adb_uart_inited = 0;

#define ADB_UART_RINGBUF_SIZE 2048

static void adb_uart_event(rvm_dev_t *uart_hdl, int event_id, void *priv)
{
    if (event_id == USART_EVENT_READ) {
        aos_uart_t *uart = (aos_uart_t *)&adb_uart;
        if (uart->cb) {
            uart->cb(ADB_CHANNEL_EVENT_READ, uart->priv);
        }
    }
}

static void *adb_uart_init(void *config, int idx)
{
    char name[12];
    rvm_hal_uart_config_t cfg;

    if (adb_uart_inited == 1)
        return &adb_uart;

    memset(&adb_uart, 0, sizeof(aos_uart_t));

    uint8_t port = idx & (~RVM_HAL_SHADOW_PORT_MASK);
    rvm_dev_t *uart_dev = rvm_hal_device_find("uart", port);
    if (uart_dev && uart_reuse == 0) {
        uart_reuse = 1;
        rvm_hal_uart_config_get(uart_dev, &cfg);
        // force close
        uart_dev->ref = 1;
        rvm_hal_uart_close(uart_dev);
    }

    rvm_uart_drv_register(idx);
    snprintf(name, sizeof(name), "uart%d", idx);
    adb_uart.dev = rvm_hal_uart_open(name);
    if (adb_uart.dev == NULL) {
        return NULL;
    }
    if (config)
        rvm_hal_uart_config(adb_uart.dev, config);
    else if (uart_reuse)
        rvm_hal_uart_config(adb_uart.dev, &cfg);

    rvm_hal_uart_set_buffer_size(adb_uart.dev, ADB_UART_RINGBUF_SIZE);
    adb_uart_inited = 1;
    return &adb_uart;
}

static void adb_uart_deinit(void *hdl)
{
    aos_uart_t *uart = (aos_uart_t *)hdl;
    // force close
    uart->dev->ref = 1;
    rvm_hal_uart_close(uart->dev);
    if (uart_reuse) {
        char name[12];
        snprintf(name, sizeof(name), "uart%d", uart->dev->id & (~RVM_HAL_SHADOW_PORT_MASK));
        rvm_hal_uart_open(name);
        uart_reuse = 0;
    }
    adb_uart_inited = 0;
}

static int adb_uart_set_event(void *hdl, channel_event_t evt_cb, void *priv)
{
    aos_uart_t *uart = (aos_uart_t *)hdl;
    uart->cb   = evt_cb;
    uart->priv = priv;

    if (evt_cb) {
        adb_flag = 1;
    } else {
        adb_flag = 0;
    }

    rvm_hal_uart_set_event(uart->dev, adb_uart_event, priv);

    return 0;
}

static int adb_uart_send(void *hdl, const char *data, int size)
{
    aos_uart_t *uart = (aos_uart_t *)hdl;
    return (rvm_hal_uart_send(uart->dev, data, size, AOS_WAIT_FOREVER));
}

static int adb_uart_recv(void *hdl, const char *data, int size, int timeout)
{
    aos_uart_t *uart = (aos_uart_t *)hdl;

    if (adb_flag == 0) {
        return 0;
    }

    uint32_t recv_len = 0;

    recv_len = rvm_hal_uart_recv(uart->dev, (void *)data, size, timeout);

    return recv_len;
}

adb_channel_t adb_uart_channel = {
    .init       = adb_uart_init,
    .deinit     = adb_uart_deinit,
    .set_event  = adb_uart_set_event,
    .send       = adb_uart_send,
    .recv       = adb_uart_recv,
};
