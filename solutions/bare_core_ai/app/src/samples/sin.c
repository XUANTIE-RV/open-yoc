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
#include "sin_test_data_f32.h"
#include "test_utils.h"
#include "xnnl.h"
#include "xnnl_infer_shape.h"
#include "xnnl_rvv.h"
#include <stdint.h>
#include <stdlib.h>

int sin_main(int argc, char **argv)
{

    int32_t *metadata_buffer = (int32_t *)fastmalloc(5 * sizeof(int32_t));
    memcpy(metadata_buffer, (int32_t *)&sin_test_data_f32[1 * 4],
           5 * sizeof(int32_t));

    int32_t dim = metadata_buffer[0];
    int32_t size = 1;

    int32_t *shape = (int32_t *)malloc(dim * sizeof(int32_t));
    for (int i = 0; i < dim; i++)
    {
        shape[i] = metadata_buffer[1 + i];
        size *= metadata_buffer[1 + i];
    }
    fastfree(metadata_buffer);

    int32_t data_start_index = 6 * 4;
    float *data_ptr = &sin_test_data_f32[data_start_index];

    float *input_buffer = (float *)align_fast_malloc(size * sizeof(float));
    memcpy(input_buffer, data_ptr, size * sizeof(float));

    float *ref_buffer = (float *)malloc(size * sizeof(float));
    memcpy(ref_buffer, data_ptr + size, size * sizeof(float));

#if (TYPE == 32)
    test_unary_op_fp32(input_buffer, ref_buffer, size, xnnl_sin_fp32);
#elif (TYPE == 16)
    test_unary_op_fp16(input_buffer, ref_buffer, size, xnnl_sin_fp16);
#endif

    return 1;
}
