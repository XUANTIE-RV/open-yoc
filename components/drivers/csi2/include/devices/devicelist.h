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

#ifndef YOC_DEVICELIST_H
#define YOC_DEVICELIST_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  register driver of uart
 * @param  [in] idx : index of the uart
 * @return None
 */
extern void rvm_uart_drv_register(int idx);

/**
 * @brief  register driver of iic
 * @param  [in] idx : index of the iic
 * @return None
 */
extern void rvm_iic_drv_register(int idx);

/**
 * @brief  register driver of eflash
 * @param  [in] idx : index of the eflash
 * @return None
 */
extern void rvm_eflash_drv_register(int idx);

/**
 * @brief  register driver of spiflash
 * @param  [in] idx : index of the spiflash
 * @return None
 */
extern void rvm_spiflash_drv_register(int idx);

/**
 * @brief  register driver of spinandflash
 * @param  [in] idx : index of the spinandflash
 * @return None
 */
extern void rvm_spinandflash_drv_register(int idx);

/**
 * @brief  register driver of adc
 * @param  [in] idx : index of the adc
 * @return None
 */
extern void rvm_adc_drv_register(int idx);

/**
 * @brief  register driver of rtc
 * @param  [in] idx : index of the rtc
 * @return None
 */
extern void rvm_rtc_drv_register(int idx);

/**
 * @brief  register driver of wdt
 * @param  [in] idx : index of the wdt
 * @return None
 */
extern void rvm_wdt_drv_register(int idx);

/**
 * @brief  register driver of gpio pin
 * @param  [in] pin_name : gpio pin name
 * @return None
 */
extern void rvm_gpio_pin_drv_register(int pin_name);

/**
 * @brief  register driver of clk
 * @return None
 */
extern void rvm_clk_drv_register(void);

/**
 * @brief  register driver of timer
 * @param  [in] idx : index of the timer
 * @return None
 */
extern void rvm_timer_drv_register(int idx);

/**
 * @brief  register driver of i2s
 * @param  [in] idx : index of the i2s
 * @return None
 */
extern void rvm_i2s_drv_register(int idx);

/**
 * @brief  register driver of pwm
 * @param  [in] idx : index of the pwm
 * @return None
 */
extern void rvm_pwm_drv_register(int idx);

/**
 * @brief  register driver of spi
 * @param  [in] idx : index of the spi
 * @return None
 */
extern void rvm_spi_drv_register(int idx);

/**
 * @brief  register driver of qspi
 * @param  [in] idx : index of the qspi
 * @return None
 */
extern void rvm_qspi_drv_register(int idx);

#if defined(CONFIG_COMP_SDMMC)
#include <devices/blockdev.h>
/**
 * @brief  register driver of mmc card
 * @return None
 */
extern void rvm_mmc_drv_register(int idx, rvm_hal_mmc_config_t *config);

/**
 * @brief  register driver of sd card
 * @return None
 */
extern void rvm_sd_drv_register(int idx, rvm_hal_sd_config_t *config);
#endif

#ifdef __cplusplus
}
#endif

#endif
