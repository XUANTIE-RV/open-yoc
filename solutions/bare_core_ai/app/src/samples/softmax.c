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
#include "softmax_test_data_f32.h"

#include "test_utils.h"
#include "xnnl.h"
#include "xnnl_infer_shape.h"
#include "xnnl_rvv.h"
#include <csi_core.h>
#include <drv/common.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int softmax_main(int argc, char **argv)
{
    int *buffer0 = (int *)align_fast_malloc(2 * sizeof(int32_t));
	memcpy(buffer0, (int *)&softmax_test_data_f32[4],
		   sizeof(2 * sizeof(int32_t)));

	int32_t dim = buffer0[0];
	int32_t axis = buffer0[1];

	align_free(buffer0);

	int32_t *buffer_shape = (int32_t *)align_fast_malloc(dim * sizeof(int32_t));
	memcpy(buffer_shape, (int *)&softmax_test_data_f32[4 + 3 * 4], dim * sizeof(int32_t));

	int size = 1;
	for (int i = 0; i < dim; i++)
	{
		size *= buffer_shape[i];
	}

	float *input_buffer = (float *)align_fast_malloc(size * sizeof(float));
	memcpy(input_buffer, (int *)&softmax_test_data_f32[4 + 3 * 4 + dim * 4], size * sizeof(float));

	float *ref_buffer = (float *)malloc(size * sizeof(float));
	memcpy(ref_buffer, (int *)&softmax_test_data_f32[4 + 3 * 4 + dim * 4 + size * 4], size * sizeof(float));

    int32_t *shape_o = buffer_shape;
    int32_t dim_o = dim;
    int32_t *infer_shape_o;
    int32_t infer_dim_o;
    int32_t infer_size_o = xnnl_softmax_infer_shape(&infer_shape_o, &infer_dim_o, buffer_shape, dim);
    check_infer_shape(shape_o, dim_o, infer_shape_o, infer_dim_o);
    align_free(infer_shape_o);

#if (TYPE == 32)
    test_softmax_op_fp32(input_buffer, ref_buffer, buffer_shape, dim, axis, xnnl_softmax_fp32);
#elif (TYPE == 16)
    test_softmax_op_fp16(input_buffer, ref_buffer, buffer_shape, dim, axis, xnnl_softmax_fp16);
#endif

    return 1;
}
