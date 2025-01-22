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

#ifndef __CONSOLE_UART_IDX_H__
#define __CONSOLE_UART_IDX_H__

#include <aos/aos.h>

#ifdef __cplusplus
extern "C" {
#endif

void console_init(int id, uint32_t baud, uint16_t buf_size);
void console_deinit();
const char *console_get_devname(void);
void *console_get_uart(void);
uint16_t console_get_buffer_size(void);

#ifdef __cplusplus
}
#endif

#endif /* __CONSOLE_UART_IDX_H__ */

