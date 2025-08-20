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

#include <stdio.h>
#include "serf/minilibc_stdio.h"

fmt_out_fn g_current_outputs = NULL;
fmt_in_fn g_current_inputs = NULL;

extern int uart_putc(int ch);
extern int uart_getc(void);

int fputc(int ch, FILE *stream)
{
    if (g_current_outputs) {
        g_current_outputs((char *)&ch, 1);
        return 0;
    }
    return uart_putc(ch);
}

int fgetc(FILE *stream)
{
    if (g_current_inputs)
        return g_current_inputs();
    return uart_getc();
}