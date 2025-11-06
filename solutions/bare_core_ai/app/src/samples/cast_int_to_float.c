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
#include "cast_int_to_float_test_data_f32.h"
#include "test_utils.h"
#include "xnnl.h"
#include "xnnl_infer_shape.h"
#include "xnnl_rvv.h"
#include <stdint.h>
#include <stdlib.h>

int cast_int_to_float_main(int argc, char **argv)
{


    int32_t *dim_buffer = (int32_t *)fastmalloc(1 * sizeof(int32_t));
    memcpy(dim_buffer, (int32_t *)&cast_int_to_float_test_data_f32[1 * 4],
           1 * sizeof(int32_t));

    int32_t dim = dim_buffer[0];

    int32_t *shape_buffer = (int32_t *)fastmalloc(dim * sizeof(int32_t));
    memcpy(shape_buffer, (int32_t *)&cast_int_to_float_test_data_f32[2 * 4],
           dim * sizeof(int32_t));

    int32_t size = 1;
    int32_t shape[dim];
    for (int i = 0; i < dim; i++)
    {
        shape[i] = shape_buffer[i];
        size *= shape_buffer[i];
    }

    int32_t *input_buffer = (int32_t *)fastmalloc(size * sizeof(int32_t));
    memcpy(input_buffer, (int32_t *)&cast_int_to_float_test_data_f32[2 * 4 + dim * 4],
           size * sizeof(int32_t));

    int32_t *ref_buffer = (int32_t *)fastmalloc(size * sizeof(int32_t));
    memcpy(ref_buffer, (int32_t *)&cast_int_to_float_test_data_f32[2 * 4 + dim * 4 + size * 4],
           size * sizeof(int32_t));

    test_cast_int_to_float_op_fp32(input_buffer, ref_buffer, size, xnnl_cast_int_to_float_fp32);

    return 1;
}
