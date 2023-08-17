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
    aos_mutex_t tx_mtx;
    aos_mutex_t rx_mtx;
    int (*config)(rvm_dev_t *dev, rvm_hal_uart_config_t *config);
    int (*config_get)(rvm_dev_t *dev, rvm_hal_uart_config_t *config);
    int (*set_buffer_size)(rvm_dev_t *dev, uint32_t size);
    int (*send_poll)(rvm_dev_t *dev, const void *data, uint32_t size);
    int (*recv_poll)(rvm_dev_t *dev, void *data, uint32_t size);
    int (*send)(rvm_dev_t *dev, const void *data, uint32_t size, uint32_t timeout_ms);
    int (*recv)(rvm_dev_t *dev, void *data, uint32_t size, uint32_t timeout_ms);
    int (*set_type)(rvm_dev_t *dev, int type);
    void (*set_event)(rvm_dev_t *dev, void (*event)(rvm_dev_t *dev, int event_id, void *priv), void *priv);
    int (*trans_dma_enable)(rvm_dev_t *dev, bool enable);
} uart_driver_t;

#ifdef __cplusplus
}
#endif

#endif