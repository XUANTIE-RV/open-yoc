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

extern int __dtor_end__;
extern int __ctor_end__;
extern int __ctor_start__;

typedef void (*func_ptr) (void);

__attribute__((weak)) void *__dso_handle = 0;

__attribute__((weak)) void cxx_system_init(void)
{
    func_ptr *p;
    for (p = (func_ptr *)&__ctor_end__ - 1; p >= (func_ptr *)&__ctor_start__; p--) {
        (*p) ();
    }
#ifdef HAVE_INIT_ARRAY_LD
    extern func_ptr __ctors_start__[];
    extern func_ptr __ctors_end__[];
    for (p = __ctors_start__; p < __ctors_end__; p++) {
        (*p)();
    }
#endif
}