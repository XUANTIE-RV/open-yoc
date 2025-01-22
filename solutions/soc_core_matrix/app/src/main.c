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

#include <aos/aos.h>
#include "aos/cli.h"
#include "app_main.h"
#include "app_init.h"

#define TAG "app"

extern int example_core_matrix();
int main(void)
{
    board_yoc_init();

    printf("soc_core_matrix demo start!\r\n");
#if defined(__riscv_matrix) || defined(__riscv_xtheadmatrix)
    example_core_matrix();
#else
    printf("matrix is not support for this cpu!\n");
#endif
    return 0;
}

