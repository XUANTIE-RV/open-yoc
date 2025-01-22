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
 * @return
 */
extern void rvm_uart_drv_register(int idx);

/**
 * @brief  register driver of iic
 * @param  [in] idx : index of the iic controller
 * @return
 */
extern void rvm_iic_drv_register(int idx);

/**
 * @brief  register driver of eflash
 * @param  [in] idx : 0 is the default
 * @return
 */
extern void rvm_eflash_drv_register(int idx);

/**
 * @brief  register driver of qsflash
 * @param  [in] idx : 0 is the default
 * @return
 */
extern void rvm_spiflash_drv_register(int idx);

/**
 * @brief  register driver of adc
 * @param  [in] idx : index of the adc
 * @return
 */
extern void rvm_adc_drv_register(int idx);

#ifdef __cplusplus
}
#endif

#endif
