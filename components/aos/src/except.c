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
#include <string.h>
#include <aos/kernel.h>


static void except_process_function(int err, const char *file, int line, const char *func_name, void *caller)
{
#if defined(CONFIG_DEBUG) && (CONFIG_DEBUG > 0)
    printf("Except! errno is %s, function: %s at %s:%d, caller: %p\n", strerror(err), func_name, file, line, caller);
    while(1);
#else
    printf("Except! errno is %s, caller: %p\n", strerror(err), caller);
    aos_reboot();
#endif
}

static except_process_t process = except_process_function;

void aos_set_except_callback(except_process_t except)
{
    process = except;
}

void aos_set_except_default()
{
    process = except_process_function;
}

void aos_except_process(int err, const char *file, int line, const char *func_name, void *caller)
{
    if (process)
        process(err, file, line, func_name, caller);
}
