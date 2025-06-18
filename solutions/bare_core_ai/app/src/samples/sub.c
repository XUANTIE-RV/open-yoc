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
#include "sub_test_data_f32.h"
#include "test_utils.h"
#include "xnnl.h"
#include "xnnl_infer_shape.h"
#include "xnnl_rvv.h"
#include <stdint.h>
#include <stdlib.h>

int sub_main(int argc, char **argv)
{
	int32_t *metadata_buffer = (int32_t *)fastmalloc(3 * sizeof(int32_t));
	memcpy(metadata_buffer, (int32_t *)&sub_test_data_f32[1 * 4],
		   3 * sizeof(int32_t));

	int32_t dim_in0 = metadata_buffer[0];
	int32_t dim_in1 = metadata_buffer[1];
	int32_t dim_out = metadata_buffer[2];

	fastfree(metadata_buffer);

	int32_t *shape_in0 = (int32_t *)malloc(dim_in0 * sizeof(int32_t));
	int32_t *shape_in1 = (int32_t *)malloc(dim_in1 * sizeof(int32_t));
	int32_t *shape_out = (int32_t *)malloc(dim_out * sizeof(int32_t));

	memcpy(shape_in0, (int32_t *)&sub_test_data_f32[(1 + 3) * 4],
		   dim_in0 * sizeof(int32_t));
	memcpy(shape_in1, (int32_t *)&sub_test_data_f32[(1 + 3 + dim_in0) * 4],
		   dim_in1 * sizeof(int32_t));
	memcpy(shape_out,
		   (int32_t *)&sub_test_data_f32[(1 + 3 + dim_in0 + dim_in1) * 4],
		   dim_out * sizeof(int32_t));

	int in_size0 = 1;
	for (int i = 0; i < dim_in0; i++)
	{
		in_size0 *= shape_in0[i];
	}

	int in_size1 = 1;
	for (int i = 0; i < dim_in1; i++)
	{
		in_size1 *= shape_in1[i];
	}

	int out_size = 1;
	for (int i = 0; i < dim_out; i++)
	{
		out_size *= shape_out[i];
	}

	int32_t data_start_index = (1 + 3 + dim_in0 + dim_in1 + dim_out) * 4;
	float *data_ptr = &sub_test_data_f32[data_start_index];

	float *input0_buffer = (float *)align_fast_malloc(in_size0 * sizeof(float));
	float *input1_buffer = (float *)align_fast_malloc(in_size1 * sizeof(float));
	float *ref_buffer = (float *)malloc(out_size * sizeof(float));

	memcpy(input0_buffer, data_ptr, in_size0 * sizeof(float));
	memcpy(input1_buffer, data_ptr + in_size0, in_size1 * sizeof(float));
	memcpy(ref_buffer, data_ptr + in_size0 + in_size1, out_size * sizeof(float));

	int32_t *shape_o = shape_out;
	int32_t dim_o = dim_out;
	int32_t *infer_shape_o;
	int32_t infer_dim_o;
	int32_t infer_size_o = xnnl_sub_infer_shape(
		&infer_shape_o, &infer_dim_o, shape_in0, dim_in0, shape_in1, dim_in1);
	check_infer_shape(shape_o, dim_o, infer_shape_o, infer_dim_o);
	align_free(infer_shape_o);

#if (TYPE == 32)
	test_binary_op_fp32(input0_buffer, input1_buffer, ref_buffer, shape_in0,
						dim_in0, shape_in1, dim_in1, shape_out, dim_out,
						xnnl_sub_fp32);
#elif (TYPE == 16)
	test_binary_op_fp16(input0_buffer, input1_buffer, ref_buffer, shape_in0,
						dim_in0, shape_in1, dim_in1, shape_out, dim_out,
						xnnl_sub_fp16);
#endif

	return 1;
}
