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

#ifndef HAL_IIC_IMPL_H
#define HAL_IIC_IMPL_H

#include <stdint.h>

#include <devices/driver.h>
#include <devices/iic.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct iic_driver {
    driver_t drv;
    uint32_t timeout;
    int (*config)(rvm_dev_t *dev, rvm_hal_iic_config_t *config);
    int (*config_get)(rvm_dev_t *dev, rvm_hal_iic_config_t *config);
    int (*master_send)(rvm_dev_t *dev, uint16_t dev_addr, const void *data, uint32_t size, uint32_t timeout);
    int (*master_recv)(rvm_dev_t *dev, uint16_t dev_addr, void *data, uint32_t size, uint32_t timeout);
    int (*slave_send)(rvm_dev_t *dev, const void *data, uint32_t size, uint32_t timeout);
    int (*slave_recv)(rvm_dev_t *dev, void *data, uint32_t size, uint32_t timeout);
    int (*mem_write)(rvm_dev_t *dev, uint16_t dev_addr, uint16_t mem_addr, uint16_t mem_addr_size,\
                     const void *data, uint32_t size, uint32_t timeout);
    int (*mem_read)(rvm_dev_t *dev, uint16_t dev_addr, uint16_t mem_addr, uint16_t mem_addr_size,\
                    void *data, uint32_t size, uint32_t timeout);
    int (*trans_dma_enable)(rvm_dev_t *dev, bool enable);
} iic_driver_t;

#ifdef __cplusplus
}
#endif

#endif