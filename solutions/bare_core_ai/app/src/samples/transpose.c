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
#include "transpose_test_data_f32.h"
#include "test_utils.h"
#include "xnnl.h"
#include "xnnl_infer_shape.h"
#include "xnnl_rvv.h"
#include <stdint.h>
#include <stdlib.h>

int transpose_main(int argc, char **argv)
{

    int32_t *dim_buffer = (int32_t *)fastmalloc(1 * sizeof(int32_t));
    memcpy(dim_buffer, (int32_t *)&transpose_test_data_f32[1 * 4],
           1 * sizeof(int32_t));

    int32_t dim = dim_buffer[0];

    int32_t *shape_buffer = (int32_t *)fastmalloc(dim * 3 * sizeof(int32_t));
    memcpy(shape_buffer, (int32_t *)&transpose_test_data_f32[2 * 4],
           dim * 3 * sizeof(int32_t));

    int32_t shape_i[dim];
    int32_t shape_o[dim];
    int32_t permute[dim];
    int32_t size_i = 1;
    int32_t size_o = 1;
    for (int i = 0; i < dim; i++)
    {
        shape_i[i] = shape_buffer[0 * dim + i];
        permute[i] = shape_buffer[1 * dim + i];
        shape_o[i] = shape_buffer[2 * dim + i];
        size_i *= shape_i[i];
        size_o *= shape_o[i];
    }
    int data_start_index = 2 * 4 + dim * 3 * 4;
    float *data_ptr = &transpose_test_data_f32[data_start_index];

    float *input_buffer = (float *)align_fast_malloc(size_i * sizeof(float));
    memcpy(input_buffer, data_ptr, size_i * sizeof(float));

    float *data_ptr2 = &transpose_test_data_f32[data_start_index + size_i * 4];

    float *ref_buffer = (float *)align_fast_malloc(size_i * sizeof(float));
    memcpy(ref_buffer, data_ptr2, size_i * sizeof(float));

    int32_t *infer_shape_o;
    int32_t infer_dim_o;
    int32_t infer_size_c =
        xnnl_transpose_infer_shape(&infer_shape_o, &infer_dim_o, shape_i, dim, permute);
    check_infer_shape(shape_o, dim, infer_shape_o, infer_dim_o);
    align_free(infer_shape_o);

#if (TYPE == 32)
    test_transpose_fp32(input_buffer, ref_buffer, shape_i, shape_o, dim, permute, xnnl_transpose_fp32);
#elif (TYPE == 16)
    test_transpose_fp16(input_buffer, ref_buffer, shape_i, shape_o, dim, permute, xnnl_transpose_fp16);
#endif

    return 1;
}
