/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef HAL_QSPI_IMPL_H
#define HAL_QSPI_IMPL_H

#include <stdint.h>

#include <devices/driver.h>
#include <devices/qspi.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct qspi_driver {
    driver_t drv;
    int (*config)(rvm_dev_t *dev, rvm_hal_qspi_config_t *config);
    int (*config_get)(rvm_dev_t *dev, rvm_hal_qspi_config_t *config);
    int (*send)(rvm_dev_t *dev, rvm_hal_qspi_cmd_t *cmd, const uint8_t *data, size_t size, uint32_t timeout);
    int (*recv)(rvm_dev_t *dev, rvm_hal_qspi_cmd_t *cmd, uint8_t *data, size_t size, uint32_t timeout);
    int (*send_recv)(rvm_dev_t *dev, rvm_hal_qspi_cmd_t *cmd, uint8_t *tx_data, uint8_t *rx_data, size_t size, uint32_t timeout);
} qspi_driver_t;

#ifdef __cplusplus
}
#endif

#endif