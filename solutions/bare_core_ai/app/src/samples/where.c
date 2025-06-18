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

#include <stdint.h>

#include "app_main.h"
#include "test_utils.h"
#include "where_test_data_f32.h"
#include "xnnl.h"
#include "xnnl_infer_shape.h"
#include "xnnl_rvv.h"
#include <math.h>
#include <stdlib.h>

static void calculate_broadcast_shape(int32_t *shape_x, int32_t *shape_y,
									  int32_t *shape_c, int32_t *shape_o,
									  int32_t dim_x, int32_t dim_y,
									  int32_t dim_c, int32_t max_dim);
int where_main(int argc, char **argv)
{

	int *buffer0 = (int *)align_fast_malloc(sizeof(int32_t));
	memcpy(buffer0, (int *)&where_test_data_f32[4], sizeof(int32_t));

	int32_t dim_x = buffer0[0];

	align_free(buffer0);

	int *buffer1 = (int *)align_fast_malloc((dim_x + 1) * sizeof(int32_t));
	memcpy(buffer1, (int *)&where_test_data_f32[4 + 4],
		   (dim_x + 1) * sizeof(int32_t));

	int32_t *shape_x = (int32_t *)align_fast_malloc(dim_x * sizeof(int32_t));

	int32_t size_x = 1;
	for (int i = 0; i < dim_x; i++)
	{
		shape_x[i] = buffer1[i];
		size_x *= shape_x[i];
	}

	int32_t dim_y = buffer1[dim_x];

	int32_t *shape_y = (int32_t *)align_fast_malloc(dim_y * sizeof(int32_t));

	int32_t size_y = 1;

	align_free(buffer1);

	int *buffer2 = (int *)align_fast_malloc((dim_y + 1) * sizeof(int32_t));
	memcpy(buffer2, (int *)&where_test_data_f32[4 + 4 + (dim_x + 1) * 4],
		   (dim_y + 1) * sizeof(int32_t));

	for (int i = 0; i < dim_y; i++)
	{
		shape_y[i] = buffer2[i];
		size_y *= shape_y[i];
	}

	int32_t dim_c = buffer2[dim_y];
	align_free(buffer2);

	int *buffer3 = (int *)align_fast_malloc((dim_c) * sizeof(int32_t));
	memcpy(buffer3,
		   (int *)&where_test_data_f32[4 + 4 + (dim_x + 1) * 4 + (dim_y + 1) * 4],
		   dim_c * sizeof(int32_t));

	int32_t *shape_c = (int32_t *)align_fast_malloc(dim_c * sizeof(int32_t));

	int32_t size_c = 1;
	for (int i = 0; i < dim_c; i++)
	{
		shape_c[i] = buffer3[i];
		size_c *= shape_c[i];
	}
	align_free(buffer3);


	int32_t max_dim = fmax(dim_c, fmax(dim_x, dim_y));

	int32_t *shape_o = (int32_t *)align_fast_malloc(max_dim * sizeof(int32_t));

	int32_t size_out = 1;
	int32_t dim_o = max_dim;

	calculate_broadcast_shape(shape_x, shape_y, shape_c, shape_o, dim_x, dim_y,
							  dim_c, max_dim);
	for (int i = 0; i < max_dim; i++)
	{
		size_out *= shape_o[i];
	}

	float *buffer_x = (float *)align_fast_malloc(size_x * sizeof(float));
	memcpy(buffer_x,
		   (int *)&where_test_data_f32[4 + 4 + (dim_x + 1) * 4 + (dim_y + 1) * 4 +
									   dim_c * 4],
		   size_x * sizeof(float));

	float *buffer_y = (float *)align_fast_malloc(size_y * sizeof(float));
	memcpy(buffer_y,
		   (int *)&where_test_data_f32[4 + 4 + (dim_x + 1) * 4 + (dim_y + 1) * 4 +
									   dim_c * 4 + size_x * 4],
		   size_y * sizeof(float));

	int32_t *buffer_cond = (int32_t *)align_fast_malloc(size_c * sizeof(int32_t));
	memcpy(buffer_cond,
		   (int *)&where_test_data_f32[4 + 4 + (dim_x + 1) * 4 + (dim_y + 1) * 4 +
									   dim_c * 4 + size_x * 4 + size_y * 4],
		   size_c * sizeof(int32_t));

	float *buffer_c = (float *)align_fast_malloc(size_c * sizeof(float));
	for (int i = 0; i < size_c; i++)
	{
		buffer_c[i] = (float)buffer_cond[i];
	}
	align_free(buffer_cond);

	float *buffer_ref = (float *)malloc(size_out * sizeof(float));
	memcpy(buffer_ref,
		   (int *)&where_test_data_f32[4 + 4 + (dim_x + 1) * 4 + (dim_y + 1) * 4 +
									   dim_c * 4 + size_x * 4 + size_y * 4 +
									   size_c * 4],
		   size_out * sizeof(float));

	int32_t *infer_shape_o;
	int32_t infer_dim_o;
	int32_t infer_size_c =
		xnnl_where_infer_shape(&infer_shape_o, &infer_dim_o, shape_x, dim_x,
							   shape_y, dim_y, shape_c, dim_c);

	check_infer_shape(shape_o, dim_o, infer_shape_o, infer_dim_o);
	align_free(infer_shape_o);

#if (TYPE == 32)
	test_where_op_fp32(buffer_c, buffer_x, buffer_y, buffer_ref, shape_c, dim_c,
					   shape_x, dim_x, shape_y, dim_y, shape_o, dim_o,
					   xnnl_where_fp32);
#elif (TYPE == 16)
	test_where_op_fp16(buffer_c, buffer_x, buffer_y, buffer_ref, shape_c, dim_c,
					   shape_x, dim_x, shape_y, dim_y, shape_o, dim_o,
					   xnnl_where_fp16);
#endif

	return 1;
}

static void calculate_broadcast_shape(int32_t *shape_x, int32_t *shape_y,
									  int32_t *shape_c, int32_t *shape_o,
									  int32_t dim_x, int32_t dim_y,
									  int32_t dim_c, int32_t max_dim)
{
	for (int i = 0; i < max_dim; ++i)
	{
		int32_t x_dim = (i < dim_x) ? shape_x[dim_x - 1 - i] : 1;
		int32_t y_dim = (i < dim_y) ? shape_y[dim_y - 1 - i] : 1;
		int32_t c_dim = (i < dim_c) ? shape_c[dim_c - 1 - i] : 1;

		shape_o[max_dim - 1 - i] = fmax(x_dim, fmax(y_dim, c_dim));
	}
}