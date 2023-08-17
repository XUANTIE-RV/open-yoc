/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef HAL_SPI_IMPL_H
#define HAL_SPI_IMPL_H

#include <stdint.h>

#include <devices/driver.h>
#include <devices/spi.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct spi_driver {
    driver_t drv;
    int (*config)(rvm_dev_t *dev, rvm_hal_spi_config_t *config);
    int (*config_get)(rvm_dev_t *dev, rvm_hal_spi_config_t *config);
    int (*send)(rvm_dev_t *dev, const uint8_t *data, size_t size, uint32_t timeout);
    int (*recv)(rvm_dev_t *dev, uint8_t *data, size_t size, uint32_t timeout);
    int (*send_recv)(rvm_dev_t *dev, uint8_t *tx_data, uint8_t *rx_data, size_t size, uint32_t timeout);
    int (*send_then_recv)(rvm_dev_t *dev, uint8_t *tx_data, size_t tx_size, uint8_t *rx_data, size_t rx_size, uint32_t timeout);
    int (*send_then_send)(rvm_dev_t *dev, uint8_t *tx1_data, size_t tx1_size, uint8_t *tx2_data, size_t tx2_size, uint32_t timeout);
} spi_driver_t;

#ifdef __cplusplus
}
#endif

#endif