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

#ifndef DEVICE_IIC_PAI_H
#define DEVICE_IIC_PAI_H

#ifdef __cplusplus
extern "C" {
#endif

#include <devices/device.h>

/*----- IIC Control Codes: Mode -----*/
typedef enum {
    MODE_MASTER,             ///< IIC Master
    MODE_SLAVE               ///< IIC Slave
} rvm_hal_iic_mode_t;

/*----- IIC Control Codes: IIC Bus Speed -----*/
typedef enum {
    BUS_SPEED_STANDARD  = 0, ///< Standard Speed (100kHz)
    BUS_SPEED_FAST      = 1, ///< Fast Speed     (400kHz)
    BUS_SPEED_FAST_PLUS = 2, ///< Fast+ Speed    (  1MHz)
    BUS_SPEED_HIGH      = 3  ///< High Speed     (3.4MHz)
} rvm_hal_iic_speed_t;

/*----- IIC Control Codes: IIC Address Mode -----*/
typedef enum {
    ADDR_7BIT        = 0,  ///< 7-bit address mode
    ADDR_10BIT       = 1   ///< 10-bit address mode
} rvm_hal_iic_address_mode_t;

typedef struct {
    rvm_hal_iic_mode_t          mode;
    rvm_hal_iic_speed_t         speed;
    rvm_hal_iic_address_mode_t  addr_mode;
    int32_t                     slave_addr;
} rvm_hal_iic_config_t;

#define I2C_MEM_ADDR_SIZE_8BIT  0 /* i2c menory address size 8bit */
#define I2C_MEM_ADDR_SIZE_16BIT 1 /* i2c menory address size 16bit */

#define rvm_hal_iic_open(name) rvm_hal_device_open(name)
#define rvm_hal_iic_close(dev) rvm_hal_device_close(dev)

int rvm_hal_iic_config(rvm_dev_t *dev, rvm_hal_iic_config_t *config);
int rvm_hal_iic_config_get(rvm_dev_t *dev, rvm_hal_iic_config_t *config);
int rvm_hal_iic_master_send(rvm_dev_t *dev, uint16_t dev_addr, const void *data, uint32_t size, uint32_t timeout);
int rvm_hal_iic_master_recv(rvm_dev_t *dev, uint16_t dev_addr, void *data, uint32_t size, uint32_t timeout);
int rvm_hal_iic_slave_send(rvm_dev_t *dev, const void *data, uint32_t size, uint32_t timeout);
int rvm_hal_iic_slave_recv(rvm_dev_t *dev, void *data, uint32_t size, uint32_t timeout);
int rvm_hal_iic_mem_write(rvm_dev_t *dev, uint16_t dev_addr, uint16_t mem_addr, uint16_t mem_addr_size, const void *data, uint32_t size, uint32_t timeout);
int rvm_hal_iic_mem_read(rvm_dev_t *dev, uint16_t dev_addr, uint16_t mem_addr, uint16_t mem_addr_size, void *data, uint32_t size, uint32_t timeout);
void rvm_hal_iic_config_default(rvm_hal_iic_config_t *config);
int rvm_hal_iic_trans_dma_enable(rvm_dev_t *dev, bool enable);


#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <devices/vfs_iic.h>
#endif

#ifdef __cplusplus
}
#endif

#endif
