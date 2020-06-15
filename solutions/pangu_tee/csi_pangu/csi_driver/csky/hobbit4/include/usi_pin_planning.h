/*
 * Copyright (C) 2017 C-SKY Microsystems Co., Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/******************************************************************************
 * @file     usi_pin_planning.h
 * @brief    header file for usi pin.
 * @version  V1.0
 * @date     18. July 2018
 ******************************************************************************/
#include <stdint.h>
#include <drv_common.h>
#include <csi_config.h>

//USI device number
#define CONFIG_USI_USART_NUM CONFIG_USI_NUM
#define CONFIG_USI_SPI_NUM CONFIG_USI_NUM
#define CONFIG_USI_I2C_NUM CONFIG_USI_NUM

//USI_PGPIO information
typedef enum {
    ENUM_USI_DEV_USART,
    ENUM_USI_DEV_SPI,
    ENUM_USI_DEV_I2C,
} usi_dev_type_e;

//USI_SPI information
typedef struct {
    int32_t pin_sclk;
    int32_t pin_mosi;
    int32_t pin_miso;
    int32_t pin_ssel;
    int32_t usi_idx;
    uint16_t function;
} usi_spi_info_t;

//USI_I2C information
typedef struct {
    int32_t pin_sda;
    int32_t pin_scl;
    int32_t usi_idx;
    uint16_t function;
} usi_i2c_info_t;
