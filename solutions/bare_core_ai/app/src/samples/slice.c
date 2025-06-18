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
#include "slice_test_data_f32.h"
#include "test_utils.h"
#include "xnnl.h"
#include "xnnl_infer_shape.h"
#include "xnnl_rvv.h"
#include <math.h>
#include <stdint.h>
#include <stdlib.h>

int slice_main(int argc, char **argv)
{

    int32_t *dim_buffer = (int32_t *)fastmalloc(sizeof(int32_t));
    memcpy(dim_buffer, (int *)&slice_test_data_f32[1 * 4], sizeof(int32_t));
    int dim_i = dim_buffer[0];

    fastfree(dim_buffer);

    int32_t *shape_i = (int32_t *)align_fast_malloc(dim_i * sizeof(int32_t));
    memcpy(shape_i, (int *)&slice_test_data_f32[2 * 4], dim_i * sizeof(int32_t));

    int in_size = 1, out_size = 1;
    for (int i = 0; i < dim_i; i++)
    {
        in_size *= shape_i[i];
    }

    int32_t *starts = (int *)align_fast_malloc(dim_i * sizeof(int));
    memcpy(starts, (int *)&slice_test_data_f32[2 * 4 + dim_i * 4],
           dim_i * sizeof(int));

    int32_t *ends = (int *)align_fast_malloc(dim_i * sizeof(int));
    memcpy(ends, (int *)&slice_test_data_f32[2 * 4 + dim_i * 2 * 4],
           dim_i * sizeof(int));

    int32_t *steps = (int *)align_fast_malloc(dim_i * sizeof(int));
    memcpy(steps, (int *)&slice_test_data_f32[2 * 4 + dim_i * 3 * 4],
           dim_i * sizeof(int));

    for (int i = 0; i < dim_i; i++)
    {
        starts[i] = fmin(starts[i], shape_i[i]);
        ends[i] = fmin(ends[i], shape_i[i]);
        if (ends[i] < 0)
        {
            ends[i] += shape_i[i];
        }
    }

    float *input = (float *)align_fast_malloc(in_size * sizeof(float));
    memcpy(input, (int *)&slice_test_data_f32[2 * 4 + dim_i * 4 * 4],
           in_size * sizeof(float));

    int32_t *shape_o = (int32_t *)malloc(dim_i * sizeof(int32_t));
    for (int i = 0; i < dim_i; i++)
    {
        shape_o[i] = (ends[i] - starts[i] + steps[i] - 1) / steps[i];

        out_size *= shape_o[i];
    }
    free(shape_o);

    float *ref = (float *)malloc(out_size * sizeof(float));
    memcpy(ref, (int *)&slice_test_data_f32[2 * 4 + dim_i * 4 * 4 + in_size * 4],
           out_size * sizeof(float));

    int32_t *shape_o_buffer = (int *)align_fast_malloc(dim_i * sizeof(int32_t));

#if (TYPE == 32)
    test_slice_op_fp32(input, ref, shape_i, shape_o_buffer, dim_i, starts, ends, steps,
                       xnnl_slice_fp32);
#elif (TYPE == 16)
    test_slice_op_fp16(input, ref, shape_i, shape_o_buffer, dim_i, starts, ends, steps,
                       xnnl_slice_fp16);
#endif

    return 0;
}
