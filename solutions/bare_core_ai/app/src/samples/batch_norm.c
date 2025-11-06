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

#include "app_main.h"
#include "batch_norm_test_data_f32.h"
#include "test_utils.h"
#include "xnnl.h"
#include "xnnl_infer_shape.h"
#include "xnnl_rvv.h"
#include <stdint.h>
#include <stdlib.h>

int batch_norm_main(int argc, char **argv)
{

    int32_t *buffer = (int32_t *)fastmalloc(batch_norm_test_data_f32_len * sizeof(int32_t));
    memcpy(buffer, (int32_t *)&batch_norm_test_data_f32[1 * 4], batch_norm_test_data_f32_len * sizeof(int32_t));

    int32_t dim = buffer[0];
    int32_t reflen = buffer[1];
    int32_t *shape = (int32_t *)(buffer + 2);
    int32_t size = 1;
    for (int i = 0; i < dim; i++)
    {
        size *= shape[i];
    }
    bool gamma_flag = buffer[dim + 2];
    bool beta_flag = buffer[dim + 3];

    float *input = (float *)(buffer + dim + 4);
    float *gamma_data = (float *)(buffer + dim + 4 + size);
    float *beta_data = (float *)(buffer + dim + 4 + size + shape[1]);
    float *mean = (float *)(buffer + dim + 4 + size + shape[1] * 2);
    float *variance = (float *)(buffer + dim + 4 + size + shape[1] * 3);
    float *ref = (float *)(buffer + dim + 4 + size + shape[1] * 4);
    float *epsilon_data = (float *)(buffer + dim + 4 + size + shape[1] * 4 + reflen);
    float *gamma = gamma_flag ? gamma_data : NULL;
    float *beta = beta_flag ? beta_data : NULL;
    float *epsilon = (abs(epsilon_data[0] - 1e-5f) == 0.f) ? NULL : epsilon_data;

#if (TYPE == 32)
    test_norm_op_fp32(input, ref, shape, dim, mean, variance, gamma, beta, epsilon,
                      xnnl_batch_norm_fp32);
#elif (TYPE == 16)
    test_norm_op_fp16(input, ref, shape, dim, mean, variance, gamma, beta, epsilon,
                      xnnl_batch_norm_fp16);
#endif

    return 1;
}
