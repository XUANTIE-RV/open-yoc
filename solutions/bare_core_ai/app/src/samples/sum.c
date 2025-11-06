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

#include "sum_test_data_f32.h"
#include "app_main.h"
#include "test_utils.h"
#include "xnnl.h"
#include "xnnl_infer_shape.h"
#include "xnnl_rvv.h"
#include <stdint.h>
#include <stdlib.h>

int sum_main(int argc, char **argv)
{

    int32_t *metadata_buffer = (int32_t *)fastmalloc(1 * sizeof(int32_t));
    memcpy(metadata_buffer, (int32_t *)&sum_test_data_f32[1 * 4], 1 * sizeof(int32_t));
    int32_t dim = metadata_buffer[0];

    int32_t *shape_buffer = (int32_t *)fastmalloc((dim + 1) * sizeof(int32_t));
    memcpy(shape_buffer, (int32_t *)&sum_test_data_f32[1 * 4 + 4], (dim + 1) * sizeof(int32_t));

    int size = 1;
    for (int i = 0; i < dim; i++)
    {
        size *= shape_buffer[i];
    }
    int len_axes = shape_buffer[dim];

    int32_t *axes_buffer = (int32_t *)fastmalloc(len_axes * sizeof(int32_t));
    memcpy(axes_buffer, (int32_t *)&sum_test_data_f32[1 * 4 + 4 + (dim + 1) * 4], len_axes * sizeof(int32_t));

    int32_t keepdims = sum_test_data_f32[1 * 4 + 4 + (dim + 1) * 4 + len_axes * 4];

    float *input_buffer = (float *)fastmalloc(size * sizeof(float));
    memcpy(input_buffer, (float *)&sum_test_data_f32[1 * 4 + 4 + (dim + 1) * 4 + len_axes * 4 + 4], size * sizeof(float));

    int32_t *axes_r = (int32_t *)fastmalloc(len_axes * sizeof(int32_t));
    for (int i = 0; i < len_axes; i++)
    {
        axes_r[i] = axes_buffer[i] >= 0 ? axes_buffer[i] : axes_buffer[i] + dim;
    }
    int out_size = size;
    for (int i = 0; i < len_axes; i++)
    {
        out_size /= shape_buffer[axes_r[i]];
    }

    float *out_buffer = (float *)fastmalloc(out_size * sizeof(float));
    memcpy(out_buffer, (float *)&sum_test_data_f32[1 * 4 + 4 + (dim + 1) * 4 + len_axes * 4 + 4 + size * 4], out_size * sizeof(float));

#if (TYPE == 32)
    test_mean_op_fp32(input_buffer, out_buffer, shape_buffer, dim, axes_buffer, len_axes, xnnl_sum_fp32);
#elif (TYPE == 16)
    test_mean_op_fp16(input_buffer, out_buffer, shape_buffer, dim, axes_buffer, len_axes, xnnl_sum_fp16);

#endif

    return 1;
}