/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#if defined(CONFIG_LOG_IPC_CP) && CONFIG_LOG_IPC_CP
#include <stdio.h>
#include <aos/aos.h>
#include "devices/impl/uart_impl.h"

#define TAG "uart_drv_ipc"

#define EVENT_WRITE 0x000000F0
#define EVENT_READ  0x0000000F

typedef struct {
    rvm_dev_t      device;
    aos_event_t    event_write_read;
    void (*write_event)(rvm_dev_t *dev, int event_id, void *priv);
    void *priv;
    int type;
    int (*rx_read)(const uint8_t *buf, uint32_t size);
    int (*tx_write)(const uint8_t *data, uint32_t size);
} ipc_uart_dev_t;
static ipc_uart_dev_t *g_ipc_uart_idx[1];

#define ipc_uart(dev) ((ipc_uart_dev_t *)dev)


static rvm_dev_t *ipc_uart_csky_init(driver_t *drv, void *config, int id)
{
    ipc_uart_dev_t *ipc_uart = (ipc_uart_dev_t *)rvm_hal_device_new(drv, sizeof(ipc_uart_dev_t), id);

    g_ipc_uart_idx[0] = ipc_uart(ipc_uart);

    return (rvm_dev_t *)ipc_uart;
}

#define ipc_uart_csky_uninit rvm_hal_device_free

static int ipc_uart_csky_open(rvm_dev_t *dev)
{
    if (aos_event_new(&ipc_uart(dev)->event_write_read, 0) != 0) {
        return -1;
    }

    if (g_ipc_uart_idx[0] == NULL) {
        g_ipc_uart_idx[0] = ipc_uart(dev);
    }

    return 0;
}

static int ipc_uart_csky_close(rvm_dev_t *dev)
{
    g_ipc_uart_idx[0] = NULL;

    aos_event_free(&ipc_uart(dev)->event_write_read);
    return 0;
}

static int ipc_uart_csky_config(rvm_dev_t *dev, rvm_hal_uart_config_t *config)
{
    return 0;
}

static int ipc_uart_csky_set_type(rvm_dev_t *dev, int type)
{
    ipc_uart(dev)->type = type;

    return 0;
}

static int ipc_uart_csky_set_buffer_size(rvm_dev_t *dev, uint32_t size)
{
    return 0;
}

static int ipc_uart_csky_send(rvm_dev_t *dev, const void *data, uint32_t size, uint32_t timeout_ms)
{
    ipc_uart(dev)->tx_write((const uint8_t *)data, size);
    return 0;
}

static int ipc_uart_csky_recv(rvm_dev_t *dev, void *data, uint32_t size, unsigned int timeout_ms)
{
    unsigned int actl_flags;
    int          ret = 0;
    long long    time_enter, used_time;
    void *       temp_buf   = data;
    uint32_t     temp_count = size;
    time_enter              = aos_now_ms();

    while (1) {
        ret = ipc_uart(dev)->rx_read((const uint8_t *)temp_buf, temp_count);

        temp_count = temp_count - ret;
        temp_buf   = (uint8_t *)temp_buf + ret;

        if (temp_count == 0 || timeout_ms == 0 ||
            timeout_ms <= (used_time = aos_now_ms() - time_enter)) {
            break;
        }

        if (aos_event_get(&ipc_uart(dev)->event_write_read, EVENT_READ, AOS_EVENT_OR_CLEAR, &actl_flags,
                          timeout_ms - used_time) == -1) {
            break;
        }
    }

    return size - temp_count;
}

static void ipc_uart_csky_event(rvm_dev_t *dev, void (*event)(rvm_dev_t *dev, int event_id, void *priv),
                            void * priv)
{
    ipc_uart(dev)->priv        = priv;
    ipc_uart(dev)->write_event = event;
}

static uart_driver_t ipc_uart_driver = {
    .drv = {
        .name   = "uart",
        .init   = ipc_uart_csky_init,
        .uninit = ipc_uart_csky_uninit,
        .open   = ipc_uart_csky_open,
        .close  = ipc_uart_csky_close,
    },
    .config          = ipc_uart_csky_config,
    .set_type        = ipc_uart_csky_set_type,
    .set_buffer_size = ipc_uart_csky_set_buffer_size,
    .send            = ipc_uart_csky_send,
    .recv            = ipc_uart_csky_recv,
    .set_event       = ipc_uart_csky_event,
};

void log_ipc_uart_register(int idx, int (*read)(const uint8_t *buf, uint32_t size),
                            int (*write)(const uint8_t *data, uint32_t size))
{
    rvm_driver_register(&ipc_uart_driver.drv, NULL, idx);

    ipc_uart_dev_t* ipc_uart = g_ipc_uart_idx[0];

    ipc_uart->tx_write = write;
    ipc_uart->rx_read = read;  
}

void log_ipc_read_event(void)
{
    ipc_uart_dev_t* ipc_uart = g_ipc_uart_idx[0];

    if (ipc_uart->write_event) {
        ipc_uart->write_event((rvm_dev_t *)ipc_uart, USART_EVENT_READ, ipc_uart->priv);
    }
    aos_event_set(&ipc_uart->event_write_read, EVENT_READ, AOS_EVENT_OR);
}

#endif
