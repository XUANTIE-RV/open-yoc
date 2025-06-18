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
#include "gather_test_data_f32.h"
#include "test_utils.h"
#include "xnnl.h"
#include "xnnl_infer_shape.h"
#include "xnnl_rvv.h"
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

#include "test_utils.h"
#include <csi_core.h>
#include <drv/common.h>
#include <stdlib.h>
#include <time.h>

int gather_main(int argc, char **argv)
{
	int *buffer_dim = (int *)align_fast_malloc(2 * sizeof(int32_t));
	memcpy(buffer_dim, (int *)&gather_test_data_f32[4], 2 * sizeof(int32_t));

	int in_size = 1, indices_size = 1;
	int axis = buffer_dim[0];
	int32_t dim = buffer_dim[1];

	align_free(buffer_dim);

	int *buffer_shape_in = (int *)align_fast_malloc((dim + 1)* sizeof(int32_t));
	memcpy(buffer_shape_in, (int *)&gather_test_data_f32[4 + 2 * 4], (dim + 1) * sizeof(int32_t));

	for (int i = 0; i < dim; i++)
	{
		in_size *= buffer_shape_in[i];
	}
	int32_t dim_idx = buffer_shape_in[dim];

	int *buffer_shape_idx = (int *)align_fast_malloc(dim_idx * sizeof(int32_t));
	memcpy(buffer_shape_idx, (int *)&gather_test_data_f32[4 + 2 * 4 + (dim + 1) * 4], dim_idx * sizeof(int32_t));

	for (int i = 0; i < dim_idx; i++)
	{
		indices_size *= buffer_shape_idx[i];
	}

	float *buffer_input = (float *)align_fast_malloc(in_size * sizeof(float));
	memcpy(buffer_input, (float *)&gather_test_data_f32[4 + 2 * 4 + (dim + 1 + dim_idx) * 4], in_size * sizeof(float));

	float *buffer_idx = (float *)align_fast_malloc(indices_size * sizeof(float));
	memcpy(buffer_idx, (float *)&gather_test_data_f32[4 + 2 * 4 + (dim + 1 + dim_idx) * 4 + in_size * 4], indices_size * sizeof(float));

	int size_c = 1;
    for (int i = 0; i < dim; i++) {
        if(i != axis) {
            size_c *= buffer_shape_in[i];
        }
    }
    for (int i = 0; i < dim_idx; i++) {
        size_c *= buffer_shape_idx[i];
    }

	float *buffer_ref = (float *)malloc(size_c * sizeof(float));
	memcpy(buffer_ref, (float *)&gather_test_data_f32[4 + 2 * 4 + (dim + 1 + dim_idx) * 4 + in_size * 4 + indices_size * 4], size_c * sizeof(float));


#if (TYPE == 32)
	test_gather_op_fp32(buffer_input, buffer_ref, buffer_idx, buffer_shape_in, dim, buffer_shape_idx, dim_idx, axis,
						xnnl_gather_fp32);
#elif (TYPE == 16)
	test_gather_op_fp16(buffer_input, buffer_ref, buffer_idx, buffer_shape_in, dim, buffer_shape_idx, dim_idx, axis,
						xnnl_gather_fp16);
#endif

	return 1;
}
