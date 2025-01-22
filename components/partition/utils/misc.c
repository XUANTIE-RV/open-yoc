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
#include "misc.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void dump_data(uint8_t *data, int32_t len)
{
    int32_t i;

    for (i = 0; i < len; i++) {
        if (i % 16 == 0 && i != 0) {
            printf("\n");
        }
        printf("%02x ", data[i]);
    }
    printf("\n");
}