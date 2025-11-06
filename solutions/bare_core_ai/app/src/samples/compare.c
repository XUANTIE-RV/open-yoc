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
#include "compare_test_data_f32.h"
#include "test_utils.h"
#include "xnnl.h"
#include "xnnl_infer_shape.h"
#include "xnnl_rvv.h"
#include <stdint.h>
#include <stdlib.h>

int compare_main(int argc, char **argv)
{

    int32_t *_buffer = (int32_t *)fastmalloc(2 * sizeof(int32_t));
    memcpy(_buffer, (int32_t *)&compare_test_data_f32[1 * 4],
           2 * sizeof(int32_t));

    int32_t type = _buffer[0];
    int32_t dim_in0 = _buffer[1];
    int32_t shape_in0[dim_in0];

    int32_t *dim_buffer = (int32_t *)fastmalloc((dim_in0 + 1) * sizeof(int32_t));
    memcpy(dim_buffer, (int32_t *)&compare_test_data_f32[3 * 4],
           (dim_in0 + 1) * sizeof(int32_t));

    int in_size0 = 1;
    for (int i = 0; i < dim_in0; i++)
    {
        shape_in0[i] = dim_buffer[i];
        in_size0 *= shape_in0[i];
    }

    int32_t dim_in1 = dim_buffer[dim_in0];
    int in_size1 = 1;
    int32_t shape_in1[dim_in1];
    int32_t *dim_buffer_2 = (int32_t *)fastmalloc((dim_in1 + 1) * sizeof(int32_t));
    memcpy(dim_buffer_2, (int32_t *)&compare_test_data_f32[2 * 4 + 4 + (dim_in0 + 1) * 4],
           (dim_in1 + 1) * sizeof(int32_t));
    for (int i = 0; i < dim_in1; i++)
    {
        shape_in1[i] = dim_buffer_2[i];
        in_size1 *= shape_in1[i];
    }

    int32_t dim_out = dim_buffer_2[dim_in1];
    int32_t shape_out[dim_out];

    int32_t *dim_buffer_3 = (int32_t *)fastmalloc(dim_out * sizeof(int32_t));
    memcpy(dim_buffer_3, (int32_t *)&compare_test_data_f32[2 * 4 + 4 + (dim_in0 + 1) * 4 + (dim_in0 + 1) * 4],
           dim_out * sizeof(int32_t));
    int out_size = 1;
    for (int i = 0; i < dim_out; i++)
    {
        shape_out[i] = dim_buffer_3[i];
        out_size *= shape_out[i];
    }

    float *input_buffer0 = (float *)align_fast_malloc(in_size0 * sizeof(float));
    memcpy(input_buffer0, (float *)&compare_test_data_f32[2 * 4 + 4 + (dim_in0 + 1) * 4 + (dim_in0 + 1) * 4 + dim_out * 4], in_size0 * sizeof(float));

    float *input_buffer1 = (float *)align_fast_malloc(in_size1 * sizeof(float));
    memcpy(input_buffer1, (float *)&compare_test_data_f32[2 * 4 + 4 + (dim_in0 + 1) * 4 + (dim_in0 + 1) * 4 + dim_out * 4 + in_size0 * 4], in_size1 * sizeof(float));

    float *ref_buffer = (float *)align_fast_malloc(out_size * sizeof(float));
    memcpy(ref_buffer, (float *)&compare_test_data_f32[2 * 4 + 4 + (dim_in0 + 1) * 4 + (dim_in0 + 1) * 4 + dim_out * 4 + in_size0 * 4 + in_size1 * 4], out_size * sizeof(float));

#if (TYPE == 32)
    test_compare_op_fp32(input_buffer0, input_buffer1, ref_buffer, shape_in0, dim_in0, shape_in1, dim_in1, shape_out,
                         dim_out, type, xnnl_compare_fp32);
#elif (TYPE == 16)
    test_compare_op_fp16(input_buffer0, input_buffer1, ref_buffer, shape_in0, dim_in0, shape_in1, dim_in1, shape_out,
                         dim_out, type, xnnl_compare_fp16);
#endif

    return 1;
}
