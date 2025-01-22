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

#if defined(__riscv_matrix) || defined(__riscv_xtheadmatrix)
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

extern int matrix_main(int argc, char **argv);
int example_core_matrix()
{
    int rc;

    rc = matrix_main(0, NULL);
    if (rc == 0)
        printf("matrix runs successfully!\n");
    else
        printf("matrix runs fail!\n");

    return rc;
}
#endif


