/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dev_internal.h>
#include <devices/impl/uart_impl.h>
#include <aos/kernel.h>

#define UART_DRIVER(dev)  ((uart_driver_t*)(dev->drv))
#define UART_VAILD(dev) do { \
    if (device_valid(dev, "uart") != 0 && device_valid(dev, "usb_serial") != 0) \
        return -1; \
} while(0)

#define UART_TX_LOCK(dev)                                                \
    do                                                                   \
    {                                                                    \
        if (!aos_mutex_is_valid(&UART_DRIVER(dev)->tx_mtx))              \
        {                                                                \
            aos_mutex_new(&UART_DRIVER(dev)->tx_mtx);                    \
            aos_mutex_lock(&UART_DRIVER(dev)->tx_mtx, AOS_WAIT_FOREVER); \
        }                                                                \
    } while (0)
#define UART_TX_UNLOCK(dev)                                \
    do                                                     \
    {                                                      \
        if (aos_mutex_is_valid(&UART_DRIVER(dev)->tx_mtx)) \
            aos_mutex_unlock(&UART_DRIVER(dev)->tx_mtx);   \
    } while (0)
#define UART_RX_LOCK(dev)                                                \
    do                                                                   \
    {                                                                    \
        if (!aos_mutex_is_valid(&UART_DRIVER(dev)->rx_mtx))              \
        {                                                                \
            aos_mutex_new(&UART_DRIVER(dev)->rx_mtx);                    \
            aos_mutex_lock(&UART_DRIVER(dev)->rx_mtx, AOS_WAIT_FOREVER); \
        }                                                                \
    } while (0)
#define UART_RX_UNLOCK(dev)                                \
    do                                                     \
    {                                                      \
        if (aos_mutex_is_valid(&UART_DRIVER(dev)->rx_mtx)) \
            aos_mutex_unlock(&UART_DRIVER(dev)->rx_mtx);   \
    } while (0)

void rvm_hal_uart_config_default(rvm_hal_uart_config_t *config)
{
    config->baud_rate = 115200;
    config->data_width = DATA_WIDTH_8BIT;
    config->parity = PARITY_NONE;
    config->stop_bits = STOP_BITS_1;
    config->flow_control = FLOW_CONTROL_DISABLED;
    config->mode = MODE_TX_RX;
}

int rvm_hal_uart_config(rvm_dev_t *dev, rvm_hal_uart_config_t *config)
{
    int ret;

    UART_VAILD(dev);
    if (!config) {
        return -EINVAL;
    }

    device_lock(dev);
    ret = UART_DRIVER(dev)->config(dev, config);
    device_unlock(dev);

    return ret;
}

int rvm_hal_uart_config_get(rvm_dev_t *dev, rvm_hal_uart_config_t *config)
{
    int ret;

    UART_VAILD(dev);
    if (!config) {
        return -EINVAL;
    }

    device_lock(dev);
    ret = UART_DRIVER(dev)->config_get(dev, config);
    device_unlock(dev);

    return ret;
}

int rvm_hal_uart_set_type(rvm_dev_t *dev, enum rvm_hal_uart_type_t type)
{
    int ret;

    UART_VAILD(dev);

    device_lock(dev);
    ret = UART_DRIVER(dev)->set_type(dev, type);
    device_unlock(dev);

    return ret;
}

int rvm_hal_uart_set_buffer_size(rvm_dev_t *dev, uint32_t size)
{
    int ret;

    UART_VAILD(dev);

    device_lock(dev);
    ret = UART_DRIVER(dev)->set_buffer_size(dev, size);
    device_unlock(dev);

    return ret;
}

int rvm_hal_uart_send_poll(rvm_dev_t *dev, const void *data, uint32_t size)
{
    if (size == 0 || NULL == data) {
        return -EINVAL;
    }

    int ret;

    UART_VAILD(dev);

    ret = UART_DRIVER(dev)->send_poll(dev, data, size);

    return ret;
}

int rvm_hal_uart_recv_poll(rvm_dev_t *dev, void *data, uint32_t size)
{
    int ret;

    UART_VAILD(dev);

    ret = UART_DRIVER(dev)->recv_poll(dev, data, size);

    return ret;
}

int rvm_hal_uart_send(rvm_dev_t *dev, const void *data, uint32_t size, uint32_t timeout_ms)
{
    if (size == 0 || NULL == data) {
        return -EINVAL;
    }

    int ret;

    UART_VAILD(dev);

    UART_TX_LOCK(dev);
    ret = UART_DRIVER(dev)->send(dev, data, size, timeout_ms);
    UART_TX_UNLOCK(dev);

    return ret;
}

int rvm_hal_uart_recv(rvm_dev_t *dev, void *data, uint32_t size, uint32_t timeout_ms)
{
    int ret;

    UART_VAILD(dev);

    UART_RX_LOCK(dev);
    ret = UART_DRIVER(dev)->recv(dev, data, size, timeout_ms);
    UART_RX_UNLOCK(dev);

    return ret;
}

void rvm_hal_uart_set_event(rvm_dev_t *dev, void (*event)(rvm_dev_t *dev, int event_id, void *priv), void *priv)
{
    aos_check_param(dev);
    if (device_valid(dev, "uart") != 0 && device_valid(dev, "usb_serial") != 0)
        return;

    device_lock(dev);
    UART_DRIVER(dev)->set_event(dev, event, priv);
    device_unlock(dev);
}

int rvm_hal_uart_trans_dma_enable(rvm_dev_t *dev, bool enable)
{
    int ret;

    UART_VAILD(dev);

    device_lock(dev);
    ret = UART_DRIVER(dev)->trans_dma_enable(dev, enable);
    device_unlock(dev);

    return ret;
}
