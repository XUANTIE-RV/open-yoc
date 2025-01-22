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
#include <aos/aos.h>

#define MATRIX_THREAD_NUM (4)
static volatile int g_matrix_cnt;
static aos_task_t g_matrix_handles[MATRIX_THREAD_NUM];
static volatile int g_matrix_ret[MATRIX_THREAD_NUM];

extern int matrix_main(int argc, char **argv);

static void matrix_thread(void *arg)
{
    g_matrix_ret[g_matrix_cnt] = matrix_main(0, NULL);
    g_matrix_cnt++;
}

int example_core_matrix()
{
    int rc;

    for (int i = 0; i < MATRIX_THREAD_NUM; i++) {
        rc = aos_task_new_ext(&g_matrix_handles[i], "app_task", matrix_thread,
                              NULL, 16*1024, AOS_DEFAULT_APP_PRI);
        if (rc) {
            printf("may be oom! rc = %d\n", rc);
            goto error;
        }
    }

    while (g_matrix_cnt < MATRIX_THREAD_NUM) {
        aos_msleep(50);
    }
    for (int i = 0; i < MATRIX_THREAD_NUM; i++) {
        if (g_matrix_ret[i])
            goto error;
    }
    printf("matrix runs successfully!\n");
    return 0;

error:
    printf("matrix runs fail!\n");
    return -1;
}

#endif


