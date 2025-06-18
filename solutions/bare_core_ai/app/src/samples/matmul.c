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
#include "matmul_test_data_f32.h"
#include "test_utils.h"
#include "xnnl.h"
#include "xnnl_infer_shape.h"
#include "xnnl_rvv.h"
#include <stdint.h>
#include <stdlib.h>

int matmul_main(int argc, char **argv)
{
    int32_t *metadata_buffer = (int32_t *)fastmalloc(5 * sizeof(int32_t));

    memcpy(metadata_buffer, (int *)&matmul_test_data_f32[4], 5 * sizeof(int32_t));

    bool trans_a = metadata_buffer[0];
    bool trans_b = metadata_buffer[1];
    int32_t dim_a = metadata_buffer[2];
    int32_t dim_b = metadata_buffer[3];
    int32_t dim_c = metadata_buffer[4];

    fastfree(metadata_buffer);

    int32_t *shape_a = (int32_t *)malloc(dim_a * sizeof(int32_t));
    int32_t *shape_b = (int32_t *)malloc(dim_b * sizeof(int32_t));
    int32_t *shape_c = (int32_t *)malloc(dim_c * sizeof(int32_t));

    int32_t size_a = 1;
    int32_t size_b = 1;
    int32_t size_c = 1;

    int32_t *shape_buffer =
        (int32_t *)fastmalloc((dim_a + dim_b + dim_c) * sizeof(int32_t));

    memcpy(shape_buffer, (int *)&matmul_test_data_f32[4 + 5 * 4],
           (dim_a + dim_b + dim_c) * sizeof(int32_t));

    for (int i = 0; i < dim_a; i++)
    {
        shape_a[i] = shape_buffer[i];
        size_a *= shape_a[i];
    }
    for (int i = 0; i < dim_b; i++)
    {
        shape_b[i] = shape_buffer[dim_a + i];
        size_b *= shape_b[i];
    }
    for (int i = 0; i < dim_c; i++)
    {
        shape_c[i] = shape_buffer[dim_a + dim_b + i];
        size_c *= shape_c[i];
    }

    fastfree(shape_buffer);

    int32_t offset = (5 + dim_a + dim_b + dim_c + 1) * 4;

    float *A = (float *)align_fast_malloc(size_a * sizeof(float));
    memcpy(A, (int *)&matmul_test_data_f32[offset], size_a * sizeof(float));

    float *B = (float *)align_fast_malloc(size_b * sizeof(float));
    memcpy(B, (int *)&matmul_test_data_f32[offset + size_a * 4],
           size_b * sizeof(float));

    float *ref = (float *)malloc(size_c * sizeof(float));
    memcpy(ref, (int *)&matmul_test_data_f32[offset + size_a * 4 + size_b * 4],
           size_c * sizeof(float));

    int32_t *infer_shape_c;
    int32_t infer_dim_c;
    int32_t infer_size_c =
        xnnl_matmul_infer_shape(&infer_shape_c, &infer_dim_c, shape_a, dim_a,
                                shape_b, dim_b, trans_a, trans_b);
    check_infer_shape(shape_c, dim_c, infer_shape_c, infer_dim_c);
    align_free(infer_shape_c);

#if (TYPE == 32)
    test_matmul_fp32(A, B, ref, shape_a, dim_a, shape_b, dim_b, shape_c, dim_c,
                     trans_a, trans_b, xnnl_matmul_fp32);
#elif (TYPE == 16)
    test_matmul_fp16(A, B, ref, shape_a, dim_a, shape_b, dim_b, shape_c, dim_c,
                     trans_a, trans_b, xnnl_matmul_fp16);
#endif

    return 1;
}
