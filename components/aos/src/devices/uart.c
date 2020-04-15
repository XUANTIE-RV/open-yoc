/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <devices/hal/uart_impl.h>

#define UART_DRIVER(dev)  ((uart_driver_t*)(dev->drv))
#define UART_VAILD(dev) do { \
    if (device_valid(dev, "uart") != 0) \
        return -1; \
} while(0)

void uart_config_default(uart_config_t *config)
{
    config->baud_rate = 115200;
    config->data_width = DATA_WIDTH_8BIT;
    config->parity = PARITY_NONE;
    config->stop_bits = STOP_BITS_1;
    config->flow_control = FLOW_CONTROL_DISABLED;
    config->mode = MODE_TX_RX;
}

int uart_config(aos_dev_t *dev, uart_config_t *config)
{
    int ret;

    UART_VAILD(dev);

    device_lock(dev);
    ret = UART_DRIVER(dev)->config(dev, config);
    device_unlock(dev);

    return ret;
}

int uart_set_type(aos_dev_t *dev, enum uart_type_t type)
{
    int ret;

    UART_VAILD(dev);

    device_lock(dev);
    ret = UART_DRIVER(dev)->set_type(dev, type);
    device_unlock(dev);

    return ret;
}

int uart_set_buffer_size(aos_dev_t *dev, uint32_t size)
{
    int ret;

    UART_VAILD(dev);

    device_lock(dev);
    ret = UART_DRIVER(dev)->set_buffer_size(dev, size);
    device_unlock(dev);

    return ret;
}

int uart_send(aos_dev_t *dev, const void *data, uint32_t size)
{
    if (size == 0 || NULL == data) {
        return -EINVAL;
    }

    int ret;

    UART_VAILD(dev);

    device_lock(dev);
    ret = UART_DRIVER(dev)->send(dev, data, size);
    device_unlock(dev);

    return ret;
}

int uart_recv(aos_dev_t *dev, void *data, uint32_t size, unsigned int timeout_ms)
{
    int ret;

    UART_VAILD(dev);

    device_lock(dev);
    ret = UART_DRIVER(dev)->recv(dev, data, size, timeout_ms);
    device_unlock(dev);

    return ret;
}

void uart_set_event(aos_dev_t *dev, void (*event)(aos_dev_t *dev, int event_id, void *priv), void *priv)
{
    aos_check_param(dev);
    if (device_valid(dev, "uart") != 0)
        return;

    device_lock(dev);
    UART_DRIVER(dev)->set_event(dev, event, priv);
    device_unlock(dev);
}

