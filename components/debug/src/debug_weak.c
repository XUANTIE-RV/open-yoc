/*
 * Copyright (C) 2016 YunOS Project. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <stdint.h>

__attribute__((weak)) uint8_t g_mmlk_cnt;
__attribute__((weak)) uint32_t dumpsys_mm_leakcheck(uint32_t call_cnt, int32_t query_index)
{
    return 0;
}

__attribute__((weak)) uint32_t dumpsys_mm_info_func(uint32_t mm_status)
{
    return 0;
}