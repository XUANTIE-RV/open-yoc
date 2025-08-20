 /*
 * Copyright (C) 2017-2024 Alibaba Group Holding Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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