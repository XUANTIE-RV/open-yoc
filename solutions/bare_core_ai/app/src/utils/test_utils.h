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

#ifndef INCLUDE_TEST_UTILS_H_
#define INCLUDE_TEST_UTILS_H_

#include <app_main.h>
#include <float.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C"
{
#endif

void check_result_fp32(float *ref, float *out, int32_t size);
void check_result_int32(int32_t *ref, int32_t *out, int32_t size);
void check_infer_shape(int32_t *shape, int32_t dim, int32_t *infer_shape,
						int32_t infer_dim);
void xt_rvv_convert_fp32_to_fp16(float *src, __fp16 *dst, int32_t size);
void xt_rvv_convert_fp16_to_fp32(__fp16 *src, float *dst, int32_t size);

void test_unary_op_fp32(float *input, float *ref, int32_t size,
						int (*unary_op)(float *, float *, int32_t));
void test_unary_op_fp16(float *input, float *ref, int32_t size,
						int (*unary_op)(__fp16 *, __fp16 *, int32_t));

void test_softmax_op_fp32(float *input, float *ref, int32_t *shape, int32_t dim,
							int32_t axis,
							int (*softmax)(float *, float *, int32_t *, int32_t,
											int32_t));
void test_softmax_op_fp16(float *input, float *ref, int32_t *shape, int32_t dim,
							int32_t axis,
							int (*softmax)(__fp16 *, __fp16 *, int32_t *, int32_t,
											int32_t));

void test_binary_op_fp32(float *input0, float *input1, float *ref,
							int32_t *shape_in0, int32_t dim_in0,
							int32_t *shape_in1, int32_t dim_in1,
							int32_t *shape_out, int32_t dim_out,
							int (*binary_op)(float *, float *, float *, int32_t *,
											int32_t, int32_t *, int32_t,
											int32_t *, int32_t));
void test_binary_op_fp16(float *input0, float *input1, float *ref,
							int32_t *shape_in0, int32_t dim_in0,
							int32_t *shape_in1, int32_t dim_in1,
							int32_t *shape_out, int32_t dim_out,
							int (*binary_op)(__fp16 *, __fp16 *, __fp16 *,
											int32_t *, int32_t, int32_t *,
											int32_t, int32_t *, int32_t));

void test_layer_norm_op_fp32(float *input, float *ref, int32_t *shape,
								int32_t dim, float *gamma, float *beta,
								int32_t axis, float *epsilon,
								int (*layer_norm)(float *, float *, int32_t *,
												int32_t, float *, float *,
												int32_t, float *));
void test_layer_norm_op_fp16(float *input, float *ref, int32_t *shape,
								int32_t dim, float *gamma, float *beta,
								int32_t axis, float *epsilon,
								int (*layer_norm)(__fp16 *, __fp16 *, int32_t *,
												int32_t, __fp16 *, __fp16 *,
												int32_t, float *));

void test_matmul_fp32(float *A, float *B, float *ref, int32_t *shape_a,
					  int32_t dim_a, int32_t *shape_b, int32_t dim_b,
					  int32_t *shape_c, int32_t dim_c, bool trans_a,
					  bool trans_b,
					  int (*matmul)(float *, float *, float *, int32_t *,
									int32_t, int32_t *, int32_t, int32_t *,
									int32_t, bool, bool, void *));
void test_matmul_fp16(float *A, float *B, float *ref, int32_t *shape_a,
					  int32_t dim_a, int32_t *shape_b, int32_t dim_b,
					  int32_t *shape_c, int32_t dim_c, bool trans_a,
					  bool trans_b,
					  int (*matmul)(__fp16 *, __fp16 *, __fp16 *, int32_t *,
									int32_t, int32_t *, int32_t, int32_t *,
									int32_t, bool, bool, void *));

void test_gather_op_fp32(float *input, float *ref, int32_t *index, int32_t *shape_in, int32_t dim_in,
							int32_t *shape_idx, int32_t dim_idx, int32_t axis,
							int (*gather)(float *, float *, int32_t *, int32_t *, int32_t, int32_t *,
										int32_t, int32_t));
void test_gather_op_fp16(float *input, float *ref, int32_t *index, int32_t *shape_in, int32_t dim_in,
							int32_t *shape_idx, int32_t dim_idx, int32_t axis,
							int (*gather)(__fp16 *, __fp16 *, int32_t *, int32_t *, int32_t, int32_t *,
										int32_t, int32_t));

void test_slice_op_fp32(float *input, float *ref, int32_t *shape_i, int32_t *shape_o, int32_t dim_i, int32_t *starts,
						int32_t *ends, int32_t *steps,
						int (*slice)(float *, float *, int32_t *, int32_t *, int32_t, int32_t *, int32_t *,
										int32_t *));
void test_slice_op_fp16(float *input, float *ref, int32_t *shape_i, int32_t *shape_o, int32_t dim_i, int32_t *starts,
						int32_t *ends, int32_t *steps,
						int (*slice)(__fp16 *, __fp16 *, int32_t *, int32_t *, int32_t, int32_t *, int32_t *,
										int32_t *));

void test_where_op_fp32(float *condition, float *x, float *y, float *ref,
						int32_t *shape_c, int32_t dim_c, int32_t *shape_x,
						int32_t dim_x, int32_t *shape_y, int32_t dim_y,
						int32_t *shape_o, int32_t dim_o,
						int (*where)(float *, float *, float *, float *,
										int32_t *, int32_t, int32_t *, int32_t,
										int32_t *, int32_t, int32_t *, int32_t));
void test_where_op_fp16(float *condition, float *x, float *y, float *ref,
						int32_t *shape_c, int32_t dim_c, int32_t *shape_x,
						int32_t dim_x, int32_t *shape_y, int32_t dim_y,
						int32_t *shape_o, int32_t dim_o,
						int (*where)(__fp16 *, __fp16 *, __fp16 *, __fp16 *,
										int32_t *, int32_t, int32_t *, int32_t,
										int32_t *, int32_t, int32_t *, int32_t));

#ifdef __cplusplus
}
#endif

#endif // INCLUDE_TEST_UTILS_H_
