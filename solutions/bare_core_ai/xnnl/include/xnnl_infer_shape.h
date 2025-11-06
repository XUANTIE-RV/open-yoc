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

/// @file

#ifndef INCLUDE_XNNL_INFER_SHAPE_H_
#define INCLUDE_XNNL_INFER_SHAPE_H_

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup INFER_SHAPE Op Output Shape Infer
 * @{
 */

/**
 * @defgroup INFER_SHAPE_ACTIVATION Activation Functions
 * @{
 */

/*********************************** activation *********************************/

/**
 * @brief       Softmax output shape infer function
 *
 * @param[out]  shape_o     Shape of the output
 * @param[out]  dim_o       Number of dimensions of output
 * @param[in]   shape_i     Shape of the input
 * @param[in]   dim_i       Number of dimensions of input
 * @return      Number of elements in the output
 */
int32_t xnnl_softmax_infer_shape(int32_t **shape_o, int32_t *dim_o, int32_t *shape_i,
                                 int32_t dim_i);

/**
 * @brief       Silu output shape infer function
 *
 * @param[out]  shape_o     Shape of the output
 * @param[out]  dim_o       Number of dimensions of output
 * @param[in]   shape_i     Shape of the input
 * @param[in]   dim_i       Number of dimensions of input
 * @return      Number of elements in the output
 */
int32_t xnnl_silu_infer_shape(int32_t **shape_o, int32_t *dim_o, int32_t *shape_i, int32_t dim_i);

/**
 * @}
 */

/**
 * @defgroup INFER_SHAPE_BINARY Binary Functions
 * @{
 */

/*********************************** binary *********************************/

/**
 * @brief       Add output shape infer function
 *
 * @param[out]  shape_o     Shape of the output
 * @param[out]  dim_o       Number of dimensions of output
 * @param[in]   shape_in0   Shape of the input0
 * @param[in]   dim_in0     Number of dimensions of input0
 * @param[in]   shape_in1   Shape of the input1
 * @param[in]   dim_in1     Number of dimensions of input1
 * @return      Number of elements in the output
 */
int32_t xnnl_add_infer_shape(int32_t **shape_o, int32_t *dim_o, int32_t *shape_in0, int32_t dim_in0,
                             int32_t *shape_in1, int32_t dim_in1);

/**
 * @brief       Sub output shape infer function
 *
 * @param[out]  shape_o     Shape of the output
 * @param[out]  dim_o       Number of dimensions of output
 * @param[in]   shape_in0   Shape of the input0
 * @param[in]   dim_in0     Number of dimensions of input0
 * @param[in]   shape_in1   Shape of the input1
 * @param[in]   dim_in1     Number of dimensions of input1
 * @return      Number of elements in the output
 */
int32_t xnnl_sub_infer_shape(int32_t **shape_o, int32_t *dim_o, int32_t *shape_in0, int32_t dim_in0,
                             int32_t *shape_in1, int32_t dim_in1);

/**
 * @brief       Mul output shape infer function
 *
 * @param[out]  shape_o     Shape of the output
 * @param[out]  dim_o       Number of dimensions of output
 * @param[in]   shape_in0   Shape of the input0
 * @param[in]   dim_in0     Number of dimensions of input0
 * @param[in]   shape_in1   Shape of the input1
 * @param[in]   dim_in1     Number of dimensions of input1
 * @return      Number of elements in the output
 */
int32_t xnnl_mul_infer_shape(int32_t **shape_o, int32_t *dim_o, int32_t *shape_in0, int32_t dim_in0,
                             int32_t *shape_in1, int32_t dim_in1);

/**
 * @brief       Div output shape infer function
 *
 * @param[out]  shape_o     Shape of the output
 * @param[out]  dim_o       Number of dimensions of output
 * @param[in]   shape_in0   Shape of the input0
 * @param[in]   dim_in0     Number of dimensions of input0
 * @param[in]   shape_in1   Shape of the input1
 * @param[in]   dim_in1     Number of dimensions of input1
 * @return      Number of elements in the output
 */
int32_t xnnl_div_infer_shape(int32_t **shape_o, int32_t *dim_o, int32_t *shape_in0, int32_t dim_in0,
                             int32_t *shape_in1, int32_t dim_in1);
/**
 * @}
 */

/**
 * @defgroup INFER_SHAPE_GEMM Gemm Functions
 * @{
 */

/*********************************** gemm *********************************/

/**
 * @brief       Gemm output shape infer function
 *
 * @param[out]  shape_o     Shape of the output
 * @param[out]  dim_o       Number of dimensions of output
 * @param[in]   M           The dimension M in gemm
 * @param[in]   N           The dimension N in gemm
 * @return      Number of elements in the output
 */
int32_t xnnl_gemm_infer_shape(int32_t **shape_o, int32_t *dim_o, int32_t M, int32_t N);

/**
 * @brief       Batch gemm output shape infer function
 *
 * @param[out]  shape_o     Shape of the output
 * @param[out]  dim_o       Number of dimensions of output
 * @param[in]   M           The dimension M in gemm
 * @param[in]   N           The dimension N in gemm
 * @param[in]   batch_a     Batches of the matrix-A
 * @param[in]   batch_b     Batches of the matrix-B
 * @return      Number of elements in the output
 */
int32_t xnnl_batch_gemm_infer_shape(int32_t **shape_o, int32_t *dim_o, int32_t M, int32_t N,
                                    int32_t batch_a, int32_t batch_b);
/**
 * @}
 */

/**
 * @defgroup OTHERS Others Functions
 * @{
 */

/*********************************** others *********************************/

/**
 * @brief       Cast output shape infer function
 *
 * @param[out]  shape_o     Shape of the output
 * @param[out]  dim_o       Number of dimensions of output
 * @param[in]   shape_i     Shape of the input
 * @param[in]   dim_i       Number of dimensions of input
 * @return      Number of elements in the output
 */
int32_t xnnl_cast_infer_shape(int32_t **shape_o, int32_t *dim_o, int32_t *shape_i, int32_t dim_i);

/**
 * @brief       Compare output shape infer function
 *
 * @param[out]  shape_o     Shape of the output
 * @param[out]  dim_o       Number of dimensions of output
 * @param[in]   shape_in0   Shape of the input0
 * @param[in]   dim_in0     Number of dimensions of input0
 * @param[in]   shape_in1   Shape of the input1
 * @param[in]   dim_in1     Number of dimensions of input1
 * @return      Number of elements in the output
 */
int32_t xnnl_compare_infer_shape(int32_t **shape_o, int32_t *dim_o, int32_t *shape_in0,
                                 int32_t dim_in0, int32_t *shape_in1, int32_t dim_in1);

/**
 * @brief       Expand output shape infer function
 *
 * @param[out]  shape_o     Shape of the output
 * @param[out]  dim_o       Number of dimensions of output
 * @param[in]   shape_tar   Shape of the target
 * @param[in]   dim_tar     Number of dimensions of target
 * @return      Number of elements in the output
 */
int32_t xnnl_expand_infer_shape(int32_t **shape_o, int32_t *dim_o, int32_t *shape_tar,
                                int32_t dim_tar);

/**
 * @brief       Scatter_nd output shape infer function
 *
 * @param[out]  shape_o     Shape of the output
 * @param[out]  dim_o       Number of dimensions of output
 * @param[in]   shape_in    Shape of the input
 * @param[in]   dim_in      Number of dimensions of input
 * @return      Number of elements in the output
 */
int32_t xnnl_scatter_nd_infer_shape(int32_t **shape_o, int32_t *dim_o, int32_t *shape_in,
                                    int32_t dim_in);
/**
 * @}
 */

/**
 * @defgroup INFER_SHAPE_INDEXING Indexing Functions
 * @{
 */

/*********************************** indexing *********************************/

/**
 * @brief       Transpose output shape infer function
 *
 * @param[out]  shape_o     Shape of the output
 * @param[out]  dim_o       Number of dimensions of output
 * @param[in]   shape_i     Shape of the input
 * @param[in]   dim_i       Number of dimensions of input
 * @param[in]   permute     Describe how to transpose
 * @return      Number of elements in the output
 */
int32_t xnnl_transpose_infer_shape(int32_t **shape_o, int32_t *dim_o, int32_t *shape_i,
                                   int32_t dim_i, int32_t *permute);

/**
 * @brief       Split output shape infer function
 *
 * @param[out]  shape_o     Shape of the output
 * @param[out]  dim_o       Number of dimensions of output
 * @param[in]   shape_i     Shape of the input
 * @param[in]   dim_i       Number of dimensions of input
 * @param[in]   split_index Pointer to the split data
 * @param[in]   split_num   Number of split
 * @param[in]   axis        Used to specify which dimension the operation is performed along
 * @return      Number of elements in the output
 */
int32_t xnnl_split_infer_shape(int32_t ***shape_o, int32_t *dim_o, int32_t *shape_i, int32_t dim_i,
                               int32_t *split_index, int32_t split_num, int32_t axis);

/**
 * @brief       Slice output shape infer function
 *
 * @param[out]  shape_o     Shape of the output
 * @param[out]  dim_o       Number of dimensions of output
 * @param[in]   dim_i       Number of dimensions of input
 * @param[in]   starts      Pointer to the start data
 * @param[in]   ends        Pointer to the end data
 * @param[in]   steps       Pointer to the step data
 * @return      Number of elements in the output
 */
int32_t xnnl_slice_infer_shape(int32_t **shape_o, int32_t *dim_o, int32_t dim_i, int32_t *starts,
                               int32_t *ends, int32_t *steps);

/**
 * @brief       Gather output shape infer function
 *
 * @param[out]  shape_o     Shape of the output
 * @param[out]  dim_o       Number of dimensions of output
 * @param[in]   shape_idx   Shape of the index
 * @param[in]   dim_idx     Number of dimensions of index
 * @return      Number of elements in the output
 */
int32_t xnnl_gather_infer_shape(int32_t **shape_o, int32_t *dim_o, int32_t *shape_idx,
                                int32_t dim_idx);

/**
 * @brief       Where output shape infer function
 *
 * @param[out]  shape_o      Shape of the output
 * @param[out]  dim_o        Number of dimensions of output
 * @param[in]   shape_x      Shape of the x
 * @param[in]   dim_x        Number of dimensions of x
 * @param[in]   shape_y      Shape of the y
 * @param[in]   dim_y        Number of dimensions of y
 * @param[in]   shape_c      Shape of the condition
 * @param[in]   dim_c        Number of dimensions of condition
 * @return      Number of elements in the output
 */
int32_t xnnl_where_infer_shape(int32_t **shape_o, int32_t *dim_o, int32_t *shape_x, int32_t dim_x,
                               int32_t *shape_y, int32_t dim_y, int32_t *shape_c, int32_t dim_c);
/**
 * @}
 */

/**
 * @defgroup INFER_SHAPE_MATMUL Matmul Functions
 * @{
 */

/*********************************** matmul *********************************/

/**
 * @brief       Matmul output shape infer function
 *
 * @param[out]  shape_c     Shape of the matrix-C
 * @param[out]  dim_c       Number of dimensions of matrix-C
 * @param[in]   shape_a     Shape of the matrix-A
 * @param[in]   dim_a       Number of dimensions of matrix-A
 * @param[in]   shape_b     Shape of the matrix-B
 * @param[in]   dim_b       Number of dimensions of matrix-B
 * @param[in]   trans_a     Whether the matrix-A is transposed
 * @param[in]   trans_b     Whether the matrix-B is transposed
 * @return      Number of elements in the output
 */
int32_t xnnl_matmul_infer_shape(int32_t **shape_c, int32_t *dim_c, int32_t *shape_a, int32_t dim_a,
                                int32_t *shape_b, int32_t dim_b, bool trans_a, bool trans_b);
/**
 * @}
 */

/**
 * @defgroup INFER_SHAPE_MATH Math Functions
 * @{
 */

/*************************************** math *************************************/

/**
 * @brief       Exp output shape infer function
 *
 * @param[out]  shape_o     Shape of the output
 * @param[out]  dim_o       Number of dimensions of output
 * @param[in]   shape_i     Shape of the input
 * @param[in]   dim_i       Number of dimensions of input
 * @return      Number of elements in the output
 */
int32_t xnnl_exp_infer_shape(int32_t **shape_o, int32_t *dim_o, int32_t *shape_i, int32_t dim_i);

/**
 * @brief       Square output shape infer function
 *
 * @param[out]  shape_o     Shape of the output
 * @param[out]  dim_o       Number of dimensions of output
 * @param[in]   shape_i     Shape of the input
 * @param[in]   dim_i       Number of dimensions of input
 * @return      Number of elements in the output
 */
int32_t xnnl_square_infer_shape(int32_t **shape_o, int32_t *dim_o, int32_t *shape_i, int32_t dim_i);

/**
 * @brief       Sqrt output shape infer function
 *
 * @param[out]  shape_o     Shape of the output
 * @param[out]  dim_o       Number of dimensions of output
 * @param[in]   shape_i     Shape of the input
 * @param[in]   dim_i       Number of dimensions of input
 * @return      Number of elements in the output
 */
int32_t xnnl_sqrt_infer_shape(int32_t **shape_o, int32_t *dim_o, int32_t *shape_i, int32_t dim_i);

/**
 * @brief       Sin output shape infer function
 *
 * @param[out]  shape_o     Shape of the output
 * @param[out]  dim_o       Number of dimensions of output
 * @param[in]   shape_i     Shape of the input
 * @param[in]   dim_i       Number of dimensions of input
 * @return      Number of elements in the output
 */
int32_t xnnl_sin_infer_shape(int32_t **shape_o, int32_t *dim_o, int32_t *shape_i, int32_t dim_i);

/**
 * @brief       Cos output shape infer function
 *
 * @param[out]  shape_o     Shape of the output
 * @param[out]  dim_o       Number of dimensions of output
 * @param[in]   shape_i     Shape of the input
 * @param[in]   dim_i       Number of dimensions of input
 * @return      Number of elements in the output
 */
int32_t xnnl_cos_infer_shape(int32_t **shape_o, int32_t *dim_o, int32_t *shape_i, int32_t dim_i);

/**
 * @brief       Neg output shape infer function
 *
 * @param[out]  shape_o     Shape of the output
 * @param[out]  dim_o       Number of dimensions of output
 * @param[in]   shape_i     Shape of the input
 * @param[in]   dim_i       Number of dimensions of input
 * @return      Number of elements in the output
 */
int32_t xnnl_neg_infer_shape(int32_t **shape_o, int32_t *dim_o, int32_t *shape_i, int32_t dim_i);

/**
 * @brief       Rsqrt output shape infer function
 *
 * @param[out]  shape_o     Shape of the output
 * @param[out]  dim_o       Number of dimensions of output
 * @param[in]   shape_i     Shape of the input
 * @param[in]   dim_i       Number of dimensions of input
 * @return      Number of elements in the output
 */
int32_t xnnl_rsqrt_infer_shape(int32_t **shape_o, int32_t *dim_o, int32_t *shape_i, int32_t dim_i);
/**
 * @}
 */

/**
 * @defgroup INFER_SHAPE_NORM Normalization Functions
 * @{
 */

/*********************************** normalization *********************************/

/**
 * @brief       Batch_norm output shape infer function
 *
 * @param[out]  shape_o     Shape of the output
 * @param[out]  dim_o       Number of dimensions of output
 * @param[in]   shape_i     Shape of the input
 * @param[in]   dim_i       Number of dimensions of input
 * @return      Number of elements in the output
 */
int32_t xnnl_batch_norm_infer_shape(int32_t **shape_o, int32_t *dim_o, int32_t *shape_i,
                                    int32_t dim_i);

/**
 * @brief       Layer_norm output shape infer function
 *
 * @param[out]  shape_o     Shape of the output
 * @param[out]  dim_o       Number of dimensions of output
 * @param[in]   shape_i     Shape of the input
 * @param[in]   dim_i       Number of dimensions of input
 * @return      Number of elements in the output
 */
int32_t xnnl_layer_norm_infer_shape(int32_t **shape_o, int32_t *dim_o, int32_t *shape_i,
                                    int32_t dim_i);

/**
 * @brief       Group_norm output shape infer function
 *
 * @param[out]  shape_o     Shape of the output
 * @param[out]  dim_o       Number of dimensions of output
 * @param[in]   shape_i     Shape of the input
 * @param[in]   dim_i       Number of dimensions of input
 * @return      Number of elements in the output
 */
int32_t xnnl_group_norm_infer_shape(int32_t **shape_o, int32_t *dim_o, int32_t *shape_i,
                                    int32_t dim_i);

/**
 * @}
 */

/**
 * @defgroup INFER_SHAPE_REDUCTION Reduction Functions
 * @{
 */

/*************************************** reduction *************************************/

/**
 * @brief       Mean output shape infer function
 *
 * @param[out]  shape_o     Shape of the output
 * @param[out]  dim_o       Number of dimensions of output
 * @param[in]   shape_i     Shape of the input
 * @param[in]   dim_i       Number of dimensions of input
 * @param[in]   keepdims    The keepdims parameter indicates whether to retain the original
 *                          dimensions
 * @param[in]   axes        Shape of the axes
 * @param[in]   dim_a       Number of dimensions of axes
 * @return      Number of elements in the output
 */
int32_t xnnl_mean_infer_shape(int32_t **shape_o, int32_t *dim_o, int32_t *shape_i, int32_t dim_i,
                              int32_t keepdims, int32_t *axes, int32_t dim_a);

/**
 * @brief       Sum output shape infer function
 *
 * @param[out]  shape_o     Shape of the output
 * @param[out]  dim_o       Number of dimensions of output
 * @param[in]   shape_i     Shape of the input
 * @param[in]   dim_i       Number of dimensions of input
 * @param[in]   keepdims    The keepdims parameter indicates whether to retain the original
 *                          dimensions
 * @param[in]   axes        Shape of the axes
 * @param[in]   dim_a       Number of dimensions of axes
 * @return      Number of elements in the output
 */
int32_t xnnl_sum_infer_shape(int32_t **shape_o, int32_t *dim_o, int32_t *shape_i, int32_t dim_i,
                             int32_t keepdims, int32_t *axes, int32_t dim_a);
/**
 * @}
 */

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif  // INCLUDE_XNNL_INFER_SHAPE_H_
