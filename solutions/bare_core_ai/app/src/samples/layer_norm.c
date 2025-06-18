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
#include "layer_norm_test_data_f32.h"
#include "test_utils.h"
#include "xnnl.h"
#include "xnnl_infer_shape.h"
#include "xnnl_rvv.h"
#include <stdint.h>
#include <stdlib.h>

int layer_norm_main(int argc, char **argv)
{
    int *buffer = (int *)align_fast_malloc(1 * sizeof(int32_t));
    memcpy(buffer, (int *)&layer_norm_test_data_f32[4], 1 * sizeof(int32_t));

    int32_t dim = buffer[0];

    align_free(buffer);

    int32_t *buffer_shape = (int32_t *)align_fast_malloc((dim + 3) * sizeof(int32_t));
    memcpy(buffer_shape, (int *)&layer_norm_test_data_f32[4 + 4], (dim + 3) * sizeof(int32_t));

    int32_t size = 1;
    for (int i = 0; i < dim; i++)
    {
        size *= buffer_shape[i];
    }
    int32_t axis = buffer_shape[dim];
    bool gamma_flag = buffer_shape[dim + 1];
    bool beta_flag = buffer_shape[dim + 2];

    int32_t norm_size = 1;
    int axis_r = axis >= 0 ? axis : (axis + dim);
    for (int i = axis_r; i < dim; i++)
    {
        norm_size *= buffer_shape[i];
    }

    float *input_buffer = (float *)align_fast_malloc(size * sizeof(float));
    memcpy(input_buffer, (int *)&layer_norm_test_data_f32[4 + 4 + (dim + 3) * 4], size * sizeof(float));

    float *gamma_buffer = (float *)align_fast_malloc(norm_size * sizeof(float));
    memcpy(gamma_buffer, (int *)&layer_norm_test_data_f32[4 + 4 + (dim + 3) * 4 + size * 4],norm_size * sizeof(float));

    float *beta_buffer = (float *)align_fast_malloc(norm_size * sizeof(float));
    memcpy(beta_buffer, (float *)&layer_norm_test_data_f32[4 + 4 + (dim + 3) * 4 + size * 4 + norm_size * 4],norm_size * sizeof(float));

    float *eps_buffer = (float *)align_fast_malloc(1 * sizeof(float));
    memcpy(eps_buffer, (int *)&layer_norm_test_data_f32[4 + 4 + (dim + 3) * 4 + size * 4 + norm_size * 4 * 2],1 * sizeof(float));

    float *ref_buffer = (float *)malloc(size * sizeof(float));
    memcpy(ref_buffer, (int *)&layer_norm_test_data_f32[4 + 4 + (dim + 3) * 4 + size * 4 + norm_size * 4 * 2 + 4], size * sizeof(float));

    float *gamma = gamma_flag ? gamma_buffer : NULL;
    if(gamma == NULL) {
        align_free(gamma_buffer);
    }

    float *beta = beta_flag ? beta_buffer : NULL;
    if(beta == NULL) {
        align_free(beta_buffer);
    }

    float *epsilon = (abs(eps_buffer[0] - 1e-5f) == 0.f) ? NULL : eps_buffer;
    if(epsilon == NULL) {
        align_free(eps_buffer);
    }


    int32_t *shape_o = input_buffer;
    int32_t dim_o = dim;
    int32_t *infer_shape_o;
    int32_t infer_dim_o;
    int32_t infer_size_o =
        xnnl_layer_norm_infer_shape(&infer_shape_o, &infer_dim_o, input_buffer, dim);
    check_infer_shape(shape_o, dim_o, infer_shape_o, infer_dim_o);
    align_free(infer_shape_o);

#if (TYPE == 32)
    test_layer_norm_op_fp32(input_buffer, ref_buffer, buffer_shape, dim, gamma, beta, axis, epsilon,
                            xnnl_layer_norm_fp32);
#elif (TYPE == 16)
    test_layer_norm_op_fp16(input_buffer, ref_buffer, buffer_shape, dim, gamma, beta, axis, epsilon,
                            xnnl_layer_norm_fp16);
#endif

    return 1;
}
