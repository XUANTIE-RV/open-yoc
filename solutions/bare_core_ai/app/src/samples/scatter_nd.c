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
#include "scatter_nd_test_data_f32.h"
#include "test_utils.h"
#include "xnnl.h"
#include "xnnl_infer_shape.h"
#include "xnnl_rvv.h"
#include <stdint.h>
#include <stdlib.h>

int scatter_nd_main(int argc, char **argv)
{
    int32_t *metadata_buffer = (int32_t *)fastmalloc(1 * sizeof(int32_t));
    memcpy(metadata_buffer, (int32_t *)&scatter_nd_test_data_f32[1 * 4],
           1 * sizeof(int32_t));

    int32_t dim_in = metadata_buffer[0];
    int32_t size_in = 1;
    int32_t *shape_in = (int32_t *)malloc(dim_in * sizeof(int32_t));
    int32_t *shape_buffer = (int32_t *)fastmalloc(dim_in * sizeof(int32_t));
    memcpy(shape_buffer, (int32_t *)&scatter_nd_test_data_f32[2 * 4],
           dim_in * sizeof(int32_t));
    for (int i = 0; i < dim_in; i++)
    {
        shape_in[i] = shape_buffer[i];
        size_in *= shape_in[i];
    }

    int32_t *_buffer = (int32_t *)fastmalloc(1 * sizeof(int32_t));
    memcpy(_buffer, (int32_t *)&scatter_nd_test_data_f32[2 * 4 + dim_in * 4],
           1 * sizeof(int32_t));

    int32_t dim_indices = _buffer[0];
    int32_t size_indice = 1;
    int32_t *shape_indices = (int32_t *)malloc(dim_indices * sizeof(int32_t));

    int32_t *shape_indices_buffer = (int32_t *)fastmalloc(dim_indices * sizeof(int32_t));
    memcpy(shape_indices_buffer, (int32_t *)&scatter_nd_test_data_f32[2 * 4 + dim_in * 4 + 4],
           dim_indices * sizeof(int32_t));
    for (int i = 0; i < dim_indices; i++)
    {
        shape_indices[i] = shape_indices_buffer[i];
        size_indice *= shape_indices[i];
    }

    int32_t dim_updates = scatter_nd_test_data_f32[2 * 4 + dim_in * 4 + 4 + dim_indices * 4];
    int32_t *shape_updates = (int32_t *)malloc(dim_updates * sizeof(int32_t));

    int32_t *shape_updates_buffer = (int32_t *)fastmalloc(dim_updates * sizeof(int32_t));
    memcpy(shape_updates_buffer, (int32_t *)&scatter_nd_test_data_f32[2 * 4 + dim_in * 4 + 4 + dim_indices * 4 + 4],
           dim_updates * sizeof(int32_t));
    int32_t size_update = 1;
    for (int i = 0; i < dim_updates; i++)
    {
        shape_updates[i] = shape_updates_buffer[i];
        size_update *= shape_updates[i];
    }

    int32_t dim_out = scatter_nd_test_data_f32[2 * 4 + dim_in * 4 + 4 + dim_indices * 4 + 4 + dim_updates * 4];
    int32_t *shape_out = (int32_t *)malloc(dim_out * sizeof(int32_t));
    int32_t size_out = 1;
    int32_t *shape_out_buffer = (int32_t *)fastmalloc(dim_out * sizeof(int32_t));
    memcpy(shape_out_buffer, (int32_t *)&scatter_nd_test_data_f32[2 * 4 + dim_in * 4 + 4 + dim_indices * 4 + 4 + dim_updates * 4 + 4],
           dim_out * sizeof(int32_t));
    for (int i = 0; i < dim_out; i++)
    {
        shape_out[i] = shape_out_buffer[i];
        size_out *= shape_out[i];
    }

    float *input_buffer = (float *)fastmalloc(size_in * sizeof(float));
    memcpy(input_buffer, (float *)&scatter_nd_test_data_f32[2 * 4 + dim_in * 4 + 4 + dim_indices * 4 + 4 + dim_updates * 4 + 4 + dim_out * 4],
           size_in * sizeof(float));

    int32_t *indices_buffer = (int32_t *)fastmalloc(size_indice * sizeof(int32_t));
    memcpy(indices_buffer, (int32_t *)&scatter_nd_test_data_f32[2 * 4 + dim_in * 4 + 4 + dim_indices * 4 + 4 + dim_updates * 4 + 4 + dim_out * 4 + size_in * 4],
           size_indice * sizeof(int32_t));

    float *updates_buffer = (float *)fastmalloc(size_update * sizeof(float));
    memcpy(updates_buffer, (float *)&scatter_nd_test_data_f32[2 * 4 + dim_in * 4 + 4 + dim_indices * 4 + 4 + dim_updates * 4 + 4 + dim_out * 4 + size_in * 4 + size_indice * 4],
           size_update * sizeof(float));

    float *ref_buffer = (float *)fastmalloc(size_out * sizeof(float));
    memcpy(ref_buffer, (float *)&scatter_nd_test_data_f32[2 * 4 + dim_in * 4 + 4 + dim_indices * 4 + 4 + dim_updates * 4 + 4 + dim_out * 4 + size_in * 4 + size_indice * 4 + size_update * 4],
           size_out * sizeof(float));

#if (TYPE == 32)
    test_scatter_nd_op_fp32(input_buffer, indices_buffer, updates_buffer, ref_buffer, shape_in, dim_in, shape_indices,
                            dim_indices, shape_updates, dim_updates, xnnl_scatter_nd_fp32);
#elif (TYPE == 16)
    test_scatter_nd_op_fp16(input_buffer, indices_buffer, updates_buffer, ref_buffer, shape_in, dim_in, shape_indices,
                            dim_indices, shape_updates, dim_updates, xnnl_scatter_nd_fp16);
#endif

    return 1;
}
