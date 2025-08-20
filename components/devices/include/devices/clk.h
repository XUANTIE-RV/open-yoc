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

#ifndef _DEVICE_CLK_H_
#define _DEVICE_CLK_H_

#include <stdio.h>
#include <stdlib.h>

#include <devices/device.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    RVM_HAL_APB_CLK = 0,
    RVM_HAL_AHB_CLK,
    RVM_HAL_CPU_CLK,
    RVM_HAL_SYS_CLK,
    RVM_HAL_CORETIME_CLK,
    RVM_HAL_UART_CLK,
    RVM_HAL_SPI_CLK,
    RVM_HAL_IIC_CLK,
    RVM_HAL_I2S_CLK,
    RVM_HAL_PWM_CLK,
    RVM_HAL_ADC_CLK,
    RVM_HAL_QSPI_CLK,
    RVM_HAL_USI_CLK,
    RVM_HAL_TIMER_CLK,
    RVM_HAL_RTC_CLK,
    RVM_HAL_WDT_CLK,
    RVM_HAL_SDIO_CLK,
    RVM_HAL_EMMC_CLK,
    RVM_HAL_USB_CLK,
    RVM_HAL_REF_CLK,

    RVM_HAL_END_CLK_ID
} rvm_hal_clk_id_t;

#define rvm_hal_clk_open(name)        rvm_hal_device_open(name)
#define rvm_hal_clk_close(dev)        rvm_hal_device_close(dev)

/**
  \brief       Enable device clock
  \param[in]   dev      Pointer to device object.
  \param[in]   dev_name Pointer to the device name, such as "uart0"
  \return      0 on success, else on fail.
*/
int rvm_hal_clk_enable(rvm_dev_t *dev, const char *dev_name);

/**
  \brief       Disable device clock
  \param[in]   dev      Pointer to device object.
  \param[in]   dev_name Pointer to the device name, such as "uart0"
  \return      0 on success, else on fail.
*/
int rvm_hal_clk_disable(rvm_dev_t *dev, const char *dev_name);

/**
  \brief       Get device frequency by id
  \param[in]   dev      Pointer to device object.
  \param[in]   clk_id   The clock id, \ref rvm_hal_clk_id_t
  \param[in]   idx      The clock device index
  \param[out]  freq     The frequency with the device
  \return      0 on success, else on fail.
*/
int rvm_hal_clk_get_freq(rvm_dev_t *dev, rvm_hal_clk_id_t clk_id, uint32_t idx, uint32_t *freq);

/**
  \brief       Set device frequency by id
  \param[in]   dev      Pointer to device object.
  \param[in]   clk_id   The clock id, \ref rvm_hal_clk_id_t
  \param[in]   idx      The clock device index
  \param[in]   freq     The frequency with the device
  \return      0 on success, else on fail.
*/
int rvm_hal_clk_set_freq(rvm_dev_t *dev, rvm_hal_clk_id_t clk_id, uint32_t idx, uint32_t freq);


#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <devices/vfs_clk.h>
#endif

#ifdef __cplusplus
}
#endif

#endif
