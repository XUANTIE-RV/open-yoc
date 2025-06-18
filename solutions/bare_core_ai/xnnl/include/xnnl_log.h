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

/// @file

#ifndef INCLUDE_XNNL_LOG_H_
#define INCLUDE_XNNL_LOG_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup LOG Log
 * @{
 */

/**
 * @brief       Getting the log level
 *
 * @return      There are four levels for the log, -2 refers to "Debug", -1 refers to "Info",
 *              0 refers to "Warning", 1 refers to "Error", the default is "Error".
 */
int xnnl_log_get_level();

/**
 * @brief       Setting the log level
 *
 * @param[in]   level    The log level
 *
 * @details
 * The value of log level include:
 * - Debug: -2
 * - Info: -1
 * - Warning: 0
 * - Error: 1
 */
void xnnl_log_set_level(int level);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif  // INCLUDE_XNNL_LOG_H_
