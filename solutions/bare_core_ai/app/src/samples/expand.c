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
#include "expand_test_data_f32.h"
#include "test_utils.h"
#include "xnnl.h"
#include "xnnl_infer_shape.h"
#include "xnnl_rvv.h"
#include <stdint.h>
#include <stdlib.h>

int expand_main(int argc, char **argv)
{

    int32_t *dim_in_b = (int32_t *)fastmalloc(1 * sizeof(int32_t));
    memcpy(dim_in_b, (int32_t *)&expand_test_data_f32[1 * 4],
           1 * sizeof(int32_t));
    int32_t dim_in = dim_in_b[0];

    int32_t size_in = 1;
    int32_t shape_in[dim_in];

    int32_t *shape_buffer = (int32_t *)fastmalloc((dim_in + 1) * sizeof(int32_t));
    memcpy(shape_buffer, (int32_t *)&expand_test_data_f32[1 * 4 + 4],
           (dim_in + 1) * sizeof(int32_t));

    for (int i = 0; i < dim_in; i++)
    {
        shape_in[i] = shape_buffer[i];
        size_in *= shape_buffer[i];
    }

    int32_t dim_out = shape_buffer[dim_in];
    int32_t shape_out[dim_out];
    int32_t size_out = 1;

    int32_t *shape_out_buffer = (int32_t *)fastmalloc(dim_out * sizeof(int32_t));
    memcpy(shape_out_buffer, (int32_t *)&expand_test_data_f32[1 * 4 + 4 + (dim_in + 1) * 4],
           dim_out * sizeof(int32_t));
    for (int i = 0; i < dim_out; i++)
    {
        shape_out[i] = shape_out_buffer[i];
        size_out *= shape_out[i];
    }

    float *input_buffer = (float *)align_fast_malloc(size_in * sizeof(float));
    memcpy(input_buffer, (float *)&expand_test_data_f32[1 * 4 + 4 + (dim_in + 1) * 4 + dim_out * 4], size_in * sizeof(float));

    float *ref_buffer = (float *)malloc(size_out * sizeof(float));
    memcpy(ref_buffer, (float *)&expand_test_data_f32[1 * 4 + 4 + (dim_in + 1) * 4 + dim_out * 4 + size_in * 4], size_out * sizeof(float));

#if (TYPE == 32)
    test_expand_op_fp32(input_buffer, ref_buffer, shape_in, dim_in, shape_out, dim_out, xnnl_expand_fp32);
#elif (TYPE == 16)
    test_expand_op_fp16(input_buffer, ref_buffer, shape_in, dim_in, shape_out, dim_out, xnnl_expand_fp16);
#endif

    return 1;
}
