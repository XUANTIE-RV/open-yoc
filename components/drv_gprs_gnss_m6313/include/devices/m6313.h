/**
 * @file eftl_si.h
 * @brief Public APIs of EFC drivers
 *
 * Copyright (C) 2017 Sanechips Technology Co., Ltd.
 * @author Hui Wu <wu.hui1@sanechips.com.cn>
 * @ingroup si_id
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef DEVICE_M6313_H
#define DEVICE_M6313_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* xxx_pin= 0 indicate the pin is not use */
typedef struct {
    const char  *device_name;
    int         reset_pin;
    int         state_pin;
    uint32_t    baud;
    uint32_t    buf_size;
    uint8_t     enable_flowctl;
} m6313_param_t;

/**
 * This function will init atparser for esp8266
 * @param[in]   task         userver_tast
 * @param[in]   idx          uart_port
 * @param[in]   baud         uart_baud
 * @param[in]   buf_size     uart_rb_size
 * @param[in]   flow_control uart_flowcontrol
 * @return      Zero on success, -1 on failed
 */
int m6313_module_init(utask_t *task, m6313_param_t *param);

void gprs_m6313_register(utask_t *task, m6313_param_t *param);

void gnss_m6313_register(utask_t *task, m6313_param_t *param);

#ifdef __cplusplus
}
#endif

#endif
