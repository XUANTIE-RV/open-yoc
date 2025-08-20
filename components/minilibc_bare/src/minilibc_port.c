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

#if !CONFIG_SEMIHOST_NATIVE
#include <stdio.h>
#include <csi_config.h>
#include <drv/uart.h>

extern csi_uart_t g_console_handle;

__attribute__((weak)) int write(int __fd, __const void *__buf, int __n)
{
    return 0;
}

int fputc(int ch, FILE *stream)
{
    if (ch == '\n') {
        csi_uart_putc(&g_console_handle, '\r');
    }

    csi_uart_putc(&g_console_handle, ch);
    return 0;
}

int uart_putc(int ch)
{
    if (ch == '\n') {
        csi_uart_putc(&g_console_handle, '\r');
    }

    csi_uart_putc(&g_console_handle, ch);
    return 0;
}

int fgetc(FILE *stream)
{
    (void)stream;

    return csi_uart_getc(&g_console_handle);
}

#endif /* end CONFIG_SEMIHOST_NATIVE */

