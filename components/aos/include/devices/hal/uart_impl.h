/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef HAL_UART_IMPL_H
#define HAL_UART_IMPL_H

#include <stdint.h>

#include <devices/driver.h>
#include <devices/uart.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct uart_driver {
    driver_t drv;
    int (*config)(aos_dev_t *dev, uart_config_t *config);
    int (*set_buffer_size)(aos_dev_t *dev, uint32_t size);
    int (*send)(aos_dev_t *dev, const void *data, uint32_t size);
    int (*recv)(aos_dev_t *dev, void *data, uint32_t size, unsigned int timeout_ms);
    int (*set_type)(aos_dev_t *dev, int type);
    void (*set_event)(aos_dev_t *dev, void (*event)(aos_dev_t *dev, int event_id, void *priv), void *priv);
} uart_driver_t;

#ifdef __cplusplus
}
#endif

#endif