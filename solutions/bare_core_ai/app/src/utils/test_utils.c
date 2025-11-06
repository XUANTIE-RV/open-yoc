
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

#include "test_utils.h"
#include <csi_core.h>
#include <drv/common.h>
#include <stdlib.h>
#include <time.h>
#include "xnnl.h"

extern void *fastmalloc(size_t size);
extern void fastfree(void *ptr);

#define xt_malloc align_fast_malloc
#define xt_free align_free
#define read_cycles() __get_MCYCLE()
#define clean_cycles() __clear_MCYCLE()
#define CPU_FREQUENCY_HZ 14860000 // 14.86MHz

int _xt_csrr_vlenb()
{
    int a = 0;
    asm volatile("csrr %0, vlenb" : "=r"(a) : : "memory");
    return a;
}

void rearrange_output_dup_32(float *output, float *input, int M, int N)
{
    for (int k = 0; k < N / 32; k++)
    {
        for (int i = 0; i < M; i++)
        {
            for (int j_prime = 0; j_prime < 32; j_prime++)
            {
                int output_index = k * M * 32 + i * 32 + j_prime;

                int j = k * 32 + j_prime;

                int input_index = i * N + j;

                output[input_index] = input[output_index];
            }
        }
    }
}

void rearrange_input_dup_32(float *output, float *input, int M, int N)
{
    for (int i = 0; i < M; i++)
    {
        for (int j = 0; j < N; j++)
        {
            int k = j / 32;

            int j_prime = j % 32;

            int _index = k * M * 32 + i * 32 + j_prime;

            int input_index = i * N + j;

            output[_index] = input[input_index];
        }
    }
}

void rearrange_output_dup_64(float *output, float *input, int M, int N)
{
    for (int k = 0; k < N / 64; k++)
    {
        for (int i = 0; i < M; i++)
        {
            for (int j_prime = 0; j_prime < 64; j_prime++)
            {
                int output_index = k * M * 64 + i * 64 + j_prime;

                int j = k * 64 + j_prime;

                int input_index = i * N + j;

                output[input_index] = input[output_index];
            }
        }
    }
}

void rearrange_input_dup_64(float *output, float *input, int M, int N)
{
    for (int i = 0; i < M; i++)
    {
        for (int j = 0; j < N; j++)
        {
            int k = j / 64;

            int j_prime = j % 64;

            int _index = k * 128 * 64 + i * 64 + j_prime;

            int input_index = i * N + j;

            output[_index] = input[input_index];
        }
    }
}

int LOOP_CNT = 10;
#define CYCLE_TEST(code_block)                   \
    do                                           \
    {                                            \
        uint64_t total_time = 0;                 \
        do                                       \
        {                                        \
            code_block;                          \
        } while (0);                             \
        for (int i = 0; i < LOOP_CNT; i++)       \
        {                                        \
            total_time -= read_cycles();         \
            do                                   \
            {                                    \
                code_block;                      \
            } while (0);                         \
            total_time += read_cycles();         \
        }                                        \
        uint64_t cycles = total_time / LOOP_CNT; \
        printf("Elapsed cycles: %ld\n", cycles); \
    } while (0)

static int16_t float32_to_float16(float value)
{
    if (value > 65519.0)
    {
        printf("Saturate to f16 max value: 65504 (%f)\n", value);
        return 0x7BFF;
    }
    if (value < -65519.0)
    {
        printf("Saturate to f16 min value: -65504 (%f)\n", value);
        return 0xFBFF;
    }

    union FP32
    {
        uint32_t u;
        float f;
    };

    const union FP32 f32inf = {255UL << 23};
    const union FP32 f16inf = {31UL << 23};
    const union FP32 magic = {15UL << 23};
    const uint32_t sign_mask = 0x80000000U;
    const uint32_t round_mask = ~0xFFFU;

    union FP32 in;
    in.f = value;
    uint32_t sign = in.u & sign_mask;
    in.u ^= sign;

    int16_t out = 0;

    if (in.u >= f32inf.u)
    {
        out = (in.u > f32inf.u) ? (int16_t)0x7FFFU : (int16_t)0x7C00U;
    }
    else
    {
        in.u &= round_mask;
        in.f *= magic.f;
        in.u -= round_mask;
        if (in.u > f16inf.u)
        {
            in.u = f16inf.u;
        }
        out = (int16_t)(in.u >> 13);
    }

    out |= (int16_t)(sign >> 16);

    return out;
}

static float float16_to_float32(int16_t value)
{
    union FP32
    {
        uint32_t u;
        float f;
    };

    const union FP32 magic = {(254UL - 15UL) << 23};
    const union FP32 was_inf_nan = {(127UL + 16UL) << 23};
    union FP32 out;

    out.u = (value & 0x7FFFU) << 13;
    out.f *= magic.f;
    if (out.f >= was_inf_nan.f)
    {
        out.u |= 255UL << 23;
    }
    out.u |= (value & 0x8000UL) << 16;

    return out.f;
}

void xt_rvv_convert_fp32_to_fp16(float *src, __fp16 *dst, int32_t size)
{
    int16_t *dst_i16 = (int16_t *)dst;
    for (int i = 0; i < size; i++)
    {
        dst_i16[i] = float32_to_float16(src[i]);
    }
}

void xt_rvv_convert_fp16_to_fp32(__fp16 *src, float *dst, int32_t size)
{
    int16_t *src_i16 = (int16_t *)src;
    for (int i = 0; i < size; i++)
    {
        dst[i] = float16_to_float32(src_i16[i]);
    }
}

static float compute_cs_fp32(float *a, float *b, int32_t size)
{
    double dot_sum = 0.0;
    double a_norm = 0.0;
    double b_norm = 0.0;
    float res = 0.0;

    for (int i = 0; i < size; i++)
    {
        float fa = (float)a[i];
        float fb = (float)b[i];
        dot_sum += (fa * fb);
        a_norm += (fa * fa);
        b_norm += (fb * fb);
    }
    res = dot_sum / (sqrt(a_norm * b_norm));
    return res;
}

static float compute_cs_int32(int32_t *a, int32_t *b, uint32_t size)
{
    double dot_sum = 0.0;
    double a_norm = 0.0;
    double b_norm = 0.0;
    float res = 0.0;

    for (int i = 0; i < size; i++)
    {
        dot_sum += (a[i] * b[i]);
        a_norm += (a[i] * a[i]);
        b_norm += (b[i] * b[i]);
    }
    res = dot_sum / (sqrt(a_norm * b_norm));
    return res;
}

static void check_diff_fp32(float *a, float *b, int32_t size, float threshold)
{
    bool err = false;
    int err_cnt = 0;
    float max_diff = 0.0f;
    int idx = 0;
    for (int i = 0; i < size; i++)
    {
        float diff = a[i] - b[i];
        if (fabs(diff) > threshold)
        {
            // printf("[%d]: (%f) - (%f) = %f ❌\n", i, a[i], b[i], diff);
            err = true;
            err_cnt += 1;
            if (fabs(diff) > fabs(max_diff))
            {
                max_diff = diff;
                idx = i;
            }
        }
        else
        {
            // printf("[%d]: (%f) - (%f) = %f ✅\n", i, a[i], b[i], diff);
        }
    }
    if (err)
    {
        printf(
            "Failure: [%d / %d], Maximum difference from reference is [%d]: %f\n",
            err_cnt, size, idx, max_diff);
    }
}

void check_result_fp32(float *ref, float *out, int32_t size)
{
    check_diff_fp32(ref, out, size, 0.1f);
    float cos = compute_cs_fp32(ref, out, size);
    if (cos < 0.999f)
    {
        printf("[Failed] The cos sim is: %f\n", cos);
    }
    else
    {
        printf("[Successful] The cos sim is: %f\n", cos);
    }
}

void check_result_int32(int32_t *ref, int32_t *out, int32_t size)
{
    float cos = compute_cs_int32(ref, out, size);
    if (cos < 0.999f)
    {
        printf("[Failed] The cos sim is: %f\n", cos);
    }
    else
    {
        printf("[Successful] The cos sim is: %f\n", cos);
    }
}

static void shape2string(int32_t *shape, int32_t dim, char *shape_str,
                         int32_t size)
{
    if (shape == NULL || dim == 0 || shape_str == NULL || size == 0)
        return;
    int count = 0;
    count += snprintf(&shape_str[count], size - count, "[");
    for (int i = 0; i < dim; i++)
    {
        if (count >= size)
            break;
        if (i == dim - 1)
        {
            count += snprintf(&shape_str[count], size - count, "%d", shape[i]);
        }
        else
        {
            count += snprintf(&shape_str[count], size - count, "%d, ", shape[i]);
        }
    }
    count += snprintf(&shape_str[count], size - count, "]");
    shape_str[count] = 0;
}

void check_infer_shape(int32_t *shape, int32_t dim, int32_t *infer_shape,
                       int32_t infer_dim)
{
    if (dim != infer_dim)
    {
        printf("Dimension \'%d\' and \'%d\' do not match!\n", dim, infer_dim);
        return;
    }

    bool match = true;
    for (int i = 0; i < dim; i++)
    {
        if (shape[i] != infer_shape[i])
        {
            match = false;
        }
    }

    if (!match)
    {
        int size = 64;
        char shape_str_1[size];
        char shape_str_2[size];
        shape2string(shape, dim, shape_str_1, size);
        shape2string(infer_shape, infer_dim, shape_str_2, size);
        printf("Shape %s and %s do not match!\n", shape_str_1, shape_str_2);
        return;
    }

    printf("[Successful] Check infer shape\n");
}

void test_slice_op_fp32(float *input, float *ref, int32_t *shape_i, int32_t *shape_o, int32_t dim_i, int32_t *starts,
                        int32_t *ends, int32_t *steps,
                        int (*slice)(float *, float *, int32_t *, int32_t *, int32_t, int32_t *, int32_t *,
                                     int32_t *))
{
    int size = 1;
    int32_t _shape_o[dim_i];
    for (int32_t i = 0; i < dim_i; i++)
    {
        if (starts[i] < 0)
        {
            starts[i] += shape_i[i];
        }
        if (ends[i] < 0)
        {
            ends[i] += shape_i[i];
        }
        if (starts[i] < 0)
            starts[i] = 0;
        if (ends[i] > shape_i[i])
            ends[i] = shape_i[i];
        if (ends[i] < starts[i])
            ends[i] = starts[i];

        _shape_o[i] = (ends[i] - starts[i] + steps[i] - 1) / steps[i];
    }
    for (int i = 0; i < dim_i; i++)
    {
        size *= _shape_o[i];
    }
    float *output = (float *)xt_malloc(size * sizeof(float));

    CYCLE_TEST(slice(input, output, shape_i, shape_o, dim_i, starts, ends, steps););

    check_result_fp32(ref, output, size);
    xt_free(input);
    xt_free(output);
    xt_free(shape_o);
    xt_free(shape_i);
    free(ref);
    xt_free(starts);
    xt_free(ends);
    xt_free(steps);
}

void test_slice_op_fp16(float *input, float *ref, int32_t *shape_i, int32_t *shape_o, int32_t dim_i, int32_t *starts,
                        int32_t *ends, int32_t *steps,
                        int (*slice)(__fp16 *, __fp16 *, int32_t *, int32_t *, int32_t, int32_t *, int32_t *,
                                     int32_t *))
{
    int in_size = 1, out_size = 1;
    for (int i = 0; i < dim_i; i++)
    {
        in_size *= shape_i[i];
    }
    int32_t _shape_o[dim_i];
    for (int32_t i = 0; i < dim_i; i++)
    {
        if (starts[i] < 0)
        {
            starts[i] += shape_i[i];
        }
        if (ends[i] < 0)
        {
            ends[i] += shape_i[i];
        }
        if (starts[i] < 0)
            starts[i] = 0;
        if (ends[i] > shape_i[i])
            ends[i] = shape_i[i];
        if (ends[i] < starts[i])
            ends[i] = starts[i];

        _shape_o[i] = (ends[i] - starts[i] + steps[i] - 1) / steps[i];
    }
    for (int i = 0; i < dim_i; i++)
    {
        out_size *= _shape_o[i];
    }
    __fp16 *input_f16 = (__fp16 *)xt_malloc(in_size * sizeof(__fp16));

    xt_rvv_convert_fp32_to_fp16(input, input_f16, in_size);
    xt_free(input);
    __fp16 *output_f16 = (__fp16 *)xt_malloc(out_size * sizeof(__fp16));

    CYCLE_TEST(slice(input_f16, output_f16, shape_i, shape_o, dim_i, starts, ends, steps););

    float *output = (float *)xt_malloc(out_size * sizeof(float));
    xt_rvv_convert_fp16_to_fp32(output_f16, output, out_size);
    check_result_fp32(ref, output, out_size);
    xt_free(input_f16);
    xt_free(output_f16);
    xt_free(output);
    xt_free(shape_o);
    xt_free(shape_i);
    free(ref);
    xt_free(starts);
    xt_free(ends);
    xt_free(steps);
}

void test_where_op_fp32(float *condition, float *x, float *y, float *ref,
                        int32_t *shape_c, int32_t dim_c, int32_t *shape_x,
                        int32_t dim_x, int32_t *shape_y, int32_t dim_y,
                        int32_t *shape_o, int32_t dim_o,
                        int (*where)(float *, float *, float *, float *,
                                     int32_t *, int32_t, int32_t *, int32_t,
                                     int32_t *, int32_t, int32_t *, int32_t))
{
    int size = 1;
    for (int i = 0; i < dim_o; i++)
    {
        size *= shape_o[i];
    }
    float *output = (float *)xt_malloc(size * sizeof(float));

    CYCLE_TEST(where(condition, x, y, output, shape_c, dim_c, shape_x, dim_x, shape_y, dim_y, shape_o, dim_o));

    check_result_fp32(ref, output, size);
    xt_free(output);
    free(ref);
    xt_free(condition);
    xt_free(x);
    xt_free(y);
    xt_free(shape_c);
    xt_free(shape_x);
    xt_free(shape_y);
    xt_free(shape_o);
}

void test_where_op_fp16(float *condition, float *x, float *y, float *ref,
                        int32_t *shape_c, int32_t dim_c, int32_t *shape_x,
                        int32_t dim_x, int32_t *shape_y, int32_t dim_y,
                        int32_t *shape_o, int32_t dim_o,
                        int (*where)(__fp16 *, __fp16 *, __fp16 *, __fp16 *,
                                     int32_t *, int32_t, int32_t *, int32_t,
                                     int32_t *, int32_t, int32_t *, int32_t))
{
    int x_size = 1, y_size = 1, out_size = 1;
    for (int i = 0; i < dim_x; i++)
    {
        x_size *= shape_x[i];
    }
    for (int i = 0; i < dim_y; i++)
    {
        y_size *= shape_y[i];
    }
    for (int i = 0; i < dim_o; i++)
    {
        out_size *= shape_o[i];
    }
    __fp16 *x_f16 = (__fp16 *)xt_malloc(x_size * sizeof(__fp16));
    xt_rvv_convert_fp32_to_fp16(x, x_f16, x_size);
    xt_free(x);
    __fp16 *y_f16 = (__fp16 *)xt_malloc(y_size * sizeof(__fp16));
    xt_rvv_convert_fp32_to_fp16(y, y_f16, y_size);
    xt_free(y);
    __fp16 *cond_f16 = (__fp16 *)xt_malloc(out_size * sizeof(__fp16));
    xt_rvv_convert_fp32_to_fp16(condition, cond_f16, out_size);
    xt_free(condition);
    __fp16 *output_f16 = (__fp16 *)xt_malloc(out_size * sizeof(__fp16));

    CYCLE_TEST(where(cond_f16, x_f16, y_f16, output_f16, shape_c, dim_c, shape_x, dim_x, shape_y, dim_y, shape_o, dim_o));
    xt_free(x_f16);
    xt_free(y_f16);
    xt_free(cond_f16);

    float *output = (float *)xt_malloc(out_size * sizeof(float));
    xt_rvv_convert_fp16_to_fp32(output_f16, output, out_size);
    check_result_fp32(ref, output, out_size);
    xt_free(output_f16);
    xt_free(output);
    free(ref);
    xt_free(shape_c);
    xt_free(shape_x);
    xt_free(shape_y);
    xt_free(shape_o);
}

void test_gather_op_fp32(float *input, float *ref, int32_t *index, int32_t *shape_in, int32_t dim_in,
                         int32_t *shape_idx, int32_t dim_idx, int32_t axis,
                         int (*gather)(float *, float *, int32_t *, int32_t *, int32_t, int32_t *,
                                       int32_t, int32_t))
{
    int size_c = 1;
    for (int i = 0; i < dim_in; i++)
    {
        if (i != axis)
        {
            size_c *= shape_in[i];
        }
    }
    for (int i = 0; i < dim_idx; i++)
    {
        size_c *= shape_idx[i];
    }

    float *output = (float *)xt_malloc(size_c * sizeof(float));

    CYCLE_TEST(gather(input, output, index, shape_in, dim_in, shape_idx, dim_idx, axis));
    check_result_fp32(ref, output, size_c);
    xt_free(input);
    free(ref);
    xt_free(index);
    xt_free(output);
    xt_free(shape_in);
    xt_free(shape_idx);
}

void test_gather_op_fp16(float *input, float *ref, int32_t *index, int32_t *shape_in, int32_t dim_in,
                         int32_t *shape_idx, int32_t dim_idx, int32_t axis,
                         int (*gather)(__fp16 *, __fp16 *, int32_t *, int32_t *, int32_t, int32_t *,
                                       int32_t, int32_t))
{
    int size_c = 1, size_in = 1;

    for (int i = 0; i < dim_in; i++)
    {
        if (i != axis)
        {
            size_c *= shape_in[i];
        }
        size_in *= shape_in[i];
    }
    for (int i = 0; i < dim_idx; i++)
    {
        size_c *= shape_idx[i];
    }

    __fp16 *input_f16 = (__fp16 *)xt_malloc(size_in * sizeof(__fp16));
    xt_rvv_convert_fp32_to_fp16(input, input_f16, size_in);
    xt_free(input);
    __fp16 *output_f16 = (__fp16 *)xt_malloc(size_c * sizeof(__fp16));

    CYCLE_TEST(gather(input_f16, output_f16, index, shape_in, dim_in, shape_idx, dim_idx, axis));
    xt_free(input_f16);
    float *output = (float *)xt_malloc(size_c * sizeof(float));
    xt_rvv_convert_fp16_to_fp32(output_f16, output, size_c);
    xt_free(output_f16);
    check_result_fp32(ref, output, size_c);
    xt_free(output);
    free(ref);
    xt_free(index);
    xt_free(shape_in);
    xt_free(shape_idx);
}

static void matmul_get_MKN(int32_t *shape_a, int32_t dim_a, int32_t *shape_b,
                           int32_t dim_b, bool trans_a, bool trans_b, int *M, int *K,
                           int *N)
{
    *M = dim_a == 1 ? 1 : (!trans_a ? shape_a[dim_a - 2] : shape_a[dim_a - 1]);
    *N = dim_b == 1 ? 1 : (!trans_b ? shape_b[dim_b - 1] : shape_b[dim_b - 2]);
    *K = dim_a == 1 ? shape_a[0]
                    : (!trans_a ? shape_a[dim_a - 1] : shape_a[dim_a - 2]);
}

void test_matmul_fp32(float *A, float *B, float *ref, int32_t *shape_a,
                      int32_t dim_a, int32_t *shape_b, int32_t dim_b,
                      int32_t *shape_c, int32_t dim_c, bool trans_a,
                      bool trans_b,
                      int (*matmul)(float *, float *, float *, int32_t *,
                                    int32_t, int32_t *, int32_t, int32_t *,
                                    int32_t, bool, bool, void *))
{
    int32_t size_a = 1;
    int32_t size_b = 1;
    int32_t size_c = 1;
    for (int i = 0; i < dim_a; i++)
    {
        size_a *= shape_a[i];
    }
    for (int i = 0; i < dim_b; i++)
    {
        size_b *= shape_b[i];
    }
    for (int i = 0; i < dim_c; i++)
    {
        size_c *= shape_c[i];
    }

    int M, K, N;
    matmul_get_MKN(shape_a, dim_a, shape_b, dim_b, trans_a, trans_b, &M, &K, &N);
    float *C = (float *)xt_malloc(size_c * sizeof(float));
    void *rB_buffer = xt_malloc(K * N * sizeof(float));

    CYCLE_TEST(matmul(A, B, C, shape_a, dim_a, shape_b, dim_b, shape_c, dim_c, trans_a, trans_b, rB_buffer));

    check_result_fp32(ref, C, size_c);
    xt_free(C);
    xt_free(A);
    xt_free(B);
    xt_free(rB_buffer);
    free(ref);
    free(shape_a);
    free(shape_b);
    free(shape_c);
}

void test_matmul_fp16(float *A, float *B, float *ref, int32_t *shape_a,
                      int32_t dim_a, int32_t *shape_b, int32_t dim_b,
                      int32_t *shape_c, int32_t dim_c, bool trans_a,
                      bool trans_b,
                      int (*matmul)(__fp16 *, __fp16 *, __fp16 *, int32_t *,
                                    int32_t, int32_t *, int32_t, int32_t *,
                                    int32_t, bool, bool, void *))
{
    int32_t size_a = 1;
    int32_t size_b = 1;
    int32_t size_c = 1;
    for (int i = 0; i < dim_a; i++)
    {
        size_a *= shape_a[i];
    }
    for (int i = 0; i < dim_b; i++)
    {
        size_b *= shape_b[i];
    }
    for (int i = 0; i < dim_c; i++)
    {
        size_c *= shape_c[i];
    }

    __fp16 *A_fp16 = (__fp16 *)xt_malloc(size_a * sizeof(__fp16));

    xt_rvv_convert_fp32_to_fp16(A, A_fp16, size_a);
    xt_free(A);
    __fp16 *B_fp16 = (__fp16 *)xt_malloc(size_b * sizeof(__fp16));

    xt_rvv_convert_fp32_to_fp16(B, B_fp16, size_b);
    xt_free(B);
    int M, K, N;
    matmul_get_MKN(shape_a, dim_a, shape_b, dim_b, trans_a, trans_b, &M, &K, &N);
    __fp16 *C_fp16 = (__fp16 *)xt_malloc(size_c * sizeof(__fp16));
    void *rB_buffer = xt_malloc(K * N * sizeof(__fp16));

    CYCLE_TEST(matmul(A_fp16, B_fp16, C_fp16, shape_a, dim_a, shape_b, dim_b, shape_c, dim_c, trans_a, trans_b, rB_buffer));
    xt_free(rB_buffer);
    xt_free(A_fp16);
    xt_free(B_fp16);

    float *C = (float *)xt_malloc(size_c * sizeof(float));
    xt_rvv_convert_fp16_to_fp32(C_fp16, C, size_c);
    check_result_fp32(ref, C, size_c);
    xt_free(C);
    xt_free(C_fp16);
    free(ref);
    free(shape_a);
    free(shape_b);
    free(shape_c);
}

void test_layer_norm_op_fp32(float *input, float *ref, int32_t *shape,
                             int32_t dim, float *gamma, float *beta,
                             int32_t axis, float *epsilon,
                             int (*layer_norm)(float *, float *, int32_t *,
                                               int32_t, float *, float *,
                                               int32_t, float *))
{
    int axis_r = axis >= 0 ? axis : (axis + dim);
    size_t vlenb = _xt_csrr_vlenb();
    int size = 1;
    for (int i = 0; i < dim; i++)
    {
        size *= shape[i];
    }

    float *output = (float *)xt_malloc(size * sizeof(float));

    if (dim == 2 && shape[0] % 2 == 0 && shape[1] % 32 == 0 && axis_r == 1 &&
        (vlenb == 64 || vlenb == 128 || (vlenb == 512 && gamma == NULL && beta == NULL)))
    {
        float *input_buffer = (float *)xt_malloc(shape[0] * shape[1] * sizeof(float));
        rearrange_input_dup_32(input_buffer, input, shape[0], shape[1]);
        xt_free(input);
        CYCLE_TEST(layer_norm(input_buffer, output, shape, dim, gamma, beta, axis, epsilon));
        float *output_buffer = (float *)xt_malloc(shape[0] * shape[1] * sizeof(float));
        rearrange_output_dup_32(output_buffer, output, shape[0], shape[1]);
        xt_free(output);
        check_result_fp32(ref, output_buffer, size);
        xt_free(output_buffer);
        free(ref);
        xt_free(shape);
        xt_free(input_buffer);

        if (gamma != NULL)
        {
            xt_free(gamma);
        }
        if (beta != NULL)
        {
            xt_free(beta);
        }
        if (epsilon != NULL)
        {
            xt_free(epsilon);
        }
    }
    else
    {
        CYCLE_TEST(layer_norm(input, output, shape, dim, gamma, beta, axis, epsilon));
        xt_free(shape);
        xt_free(input);
        check_result_fp32(ref, output, size);
        xt_free(output);
        if (gamma != NULL)
        {
            xt_free(gamma);
        }
        if (beta != NULL)
        {
            xt_free(beta);
        }
        if (epsilon != NULL)
        {
            xt_free(epsilon);
        }
        free(ref);
    }
}

void test_layer_norm_op_fp16(float *input, float *ref, int32_t *shape,
                             int32_t dim, float *gamma, float *beta,
                             int32_t axis, float *epsilon,
                             int (*layer_norm)(__fp16 *, __fp16 *, int32_t *,
                                               int32_t, __fp16 *, __fp16 *,
                                               int32_t, float *))
{
    int axis_r = axis >= 0 ? axis : (axis + dim);
    int size = 1;
    size_t vlenb = _xt_csrr_vlenb();
    for (int i = 0; i < dim; i++)
    {
        size *= shape[i];
    }
    int norm_size = 1;
    int32_t axis_temp = axis >= 0 ? axis : axis + dim;
    for (int i = axis_temp; i < dim; i++)
    {
        norm_size *= shape[i];
    }
    __fp16 *input_f16 = (__fp16 *)xt_malloc(size * sizeof(__fp16));

    __fp16 *gamma_fp16;
    __fp16 *beta_fp16;
    xt_rvv_convert_fp32_to_fp16(input, input_f16, size);

    if (gamma)
    {
        gamma_fp16 = (__fp16 *)xt_malloc(norm_size * sizeof(__fp16));
        xt_rvv_convert_fp32_to_fp16(gamma, gamma_fp16, norm_size);
    }
    __fp16 *gamma_ptr = gamma ? gamma_fp16 : NULL;

    if (beta)
    {
        beta_fp16 = (__fp16 *)xt_malloc(norm_size * sizeof(__fp16));
        xt_rvv_convert_fp32_to_fp16(beta, beta_fp16, norm_size);
    }
    __fp16 *beta_ptr = beta ? beta_fp16 : NULL;

    float total_time = 0;
    float cycles = 0;
    __fp16 *output_f16 = (__fp16 *)xt_malloc(size * sizeof(__fp16));

    float *output = (float *)xt_malloc(size * sizeof(float));

    if (dim == 2 && shape[0] % 2 == 0 && shape[1] % 64 == 0 && axis_r == 1 &&
        (vlenb == 64 || vlenb == 128 || (vlenb == 512 && gamma == NULL && beta == NULL)))
    {
        float *input_buffer = (float *)xt_malloc(shape[0] * shape[1] * sizeof(float));
        rearrange_input_dup_64(input_buffer, input, shape[0], shape[1]);
        xt_rvv_convert_fp32_to_fp16(input_buffer, input_f16, size);
        CYCLE_TEST(layer_norm(input_f16, output_f16, shape, dim, gamma_ptr, beta_ptr, axis, epsilon));
        xt_rvv_convert_fp16_to_fp32(output_f16, output, size);
        float *output_buffer = (float *)xt_malloc(shape[0] * shape[1] * sizeof(float));
        rearrange_output_dup_64(output_buffer, output, shape[0], shape[1]);
        check_result_fp32(ref, output_buffer, size);
        xt_free(output_buffer);
    }
    else
    {
        CYCLE_TEST(layer_norm(input_f16, output_f16, shape, dim, gamma_ptr, beta_ptr, axis, epsilon));
        if (gamma)
        {
            xt_free(gamma);
            xt_free(gamma_fp16);
        }
        if (beta)
        {
            xt_free(beta);
            xt_free(beta_fp16);
        }
        if (epsilon)
        {
            xt_free(epsilon);
        }
        xt_free(input_f16);
        xt_free(shape);
        float *output = (float *)xt_malloc(size * sizeof(float));
        xt_rvv_convert_fp16_to_fp32(output_f16, output, size);

        check_result_fp32(ref, output, size);

        xt_free(output_f16);
        xt_free(output);
        free(ref);
    }
}

void test_binary_op_fp32(float *input0, float *input1, float *ref,
                         int32_t *shape_in0, int32_t dim_in0,
                         int32_t *shape_in1, int32_t dim_in1,
                         int32_t *shape_out, int32_t dim_out,
                         int (*binary_op)(float *, float *, float *, int32_t *,
                                          int32_t, int32_t *, int32_t,
                                          int32_t *, int32_t))
{
    int size_out = 1;
    for (int i = 0; i < dim_out; i++)
    {
        size_out *= shape_out[i];
    }
    float *output = (float *)xt_malloc(size_out * sizeof(float));

    CYCLE_TEST(binary_op(input0, input1, output, shape_in0, dim_in0, shape_in1, dim_in1, shape_out, dim_out));
    check_result_fp32(ref, output, size_out);
    xt_free(output);
    xt_free(input0);
    xt_free(input1);

    free(shape_in0);
    free(shape_in1);
    free(shape_out);
    free(ref);
}

void test_binary_op_fp16(float *input0, float *input1, float *ref,
                         int32_t *shape_in0, int32_t dim_in0,
                         int32_t *shape_in1, int32_t dim_in1,
                         int32_t *shape_out, int32_t dim_out,
                         int (*binary_op)(__fp16 *, __fp16 *, __fp16 *,
                                          int32_t *, int32_t, int32_t *,
                                          int32_t, int32_t *, int32_t))
{
    int32_t size_in0 = 1;
    int32_t size_in1 = 1;
    int32_t size_out = 1;
    for (int i = 0; i < dim_in0; i++)
    {
        size_in0 *= shape_in0[i];
    }
    for (int i = 0; i < dim_in1; i++)
    {
        size_in1 *= shape_in1[i];
    }
    for (int i = 0; i < dim_out; i++)
    {
        size_out *= shape_out[i];
    }

    __fp16 *input0_f16 = (__fp16 *)xt_malloc(size_in0 * sizeof(__fp16));
    __fp16 *input1_f16 = (__fp16 *)xt_malloc(size_in1 * sizeof(__fp16));
    __fp16 *output_f16 = (__fp16 *)xt_malloc(size_out * sizeof(__fp16));
    xt_rvv_convert_fp32_to_fp16(input0, input0_f16, size_in0);
    xt_rvv_convert_fp32_to_fp16(input1, input1_f16, size_in1);

    CYCLE_TEST(binary_op(input0_f16, input1_f16, output_f16, shape_in0, dim_in0, shape_in1, dim_in1, shape_out, dim_out));

    xt_free(input0_f16);
    xt_free(input1_f16);

    float *output = (float *)xt_malloc(size_out * sizeof(float));
    xt_rvv_convert_fp16_to_fp32(output_f16, output, size_out);
    check_result_fp32(ref, output, size_out);

    xt_free(output_f16);
    xt_free(output);
    xt_free(input0);
    xt_free(input1);
    free(ref);

    free(shape_in0);
    free(shape_in1);
    free(shape_out);
}

void test_softmax_op_fp32(float *input, float *ref, int32_t *shape, int32_t dim,
                          int32_t axis,
                          int (*softmax)(float *, float *, int32_t *, int32_t,
                                         int32_t))
{
    uint32_t axis_r = axis > 0 ? axis : axis + dim;
    int size = 1;
    for (int i = 0; i < dim; i++)
    {
        size *= shape[i];
    }

    if (dim == 2 && axis_r == 1)
    {

        if (shape[1] % 32 == 0 && shape[0] % 2 == 0)
        {
            float *input_buffer =
                (float *)xt_malloc(shape[0] * shape[1] * sizeof(float));
            rearrange_input_dup_32(input_buffer, input, shape[0], shape[1]);
            xt_free(input);
            float *output = (float *)xt_malloc(size * sizeof(float));
            CYCLE_TEST(softmax(input_buffer, output, shape, dim, axis));
            xt_free(input_buffer);
            float *output_buffer =
                (float *)xt_malloc(shape[0] * shape[1] * sizeof(float));
            rearrange_output_dup_32(output_buffer, output, shape[0], shape[1]);
            check_result_fp32(ref, output_buffer, size);
            xt_free(output);
            xt_free(output_buffer);
            free(ref);
            xt_free(shape);
            return;
        }
    }
    else
    {
        float *output = (float *)xt_malloc(size * sizeof(float));
        CYCLE_TEST(softmax(input, output, shape, dim, axis));
        check_result_fp32(ref, output, size);
        xt_free(output);
        free(ref);
        xt_free(input);
        xt_free(shape);
        return;
    }
}

void test_softmax_op_fp16(float *input, float *ref, int32_t *shape, int32_t dim,
                          int32_t axis,
                          int (*softmax)(__fp16 *, __fp16 *, int32_t *, int32_t,
                                         int32_t))
{
    int size = 1;
    for (int i = 0; i < dim; i++)
    {
        size *= shape[i];
    }
    __fp16 *input_f16 = (__fp16 *)xt_malloc(size * sizeof(__fp16));

    xt_rvv_convert_fp32_to_fp16(input, input_f16, size);

    float total_time = 0;
    float cycles = 0;

    xt_free(input);
    __fp16 *output_f16 = (__fp16 *)xt_malloc(size * sizeof(__fp16));

    CYCLE_TEST(softmax(input_f16, output_f16, shape, dim, axis));
    xt_free(input_f16);
    float *output = (float *)xt_malloc(size * sizeof(float));
    xt_rvv_convert_fp16_to_fp32(output_f16, output, size);
    check_result_fp32(ref, output, size);
    xt_free(shape);
    xt_free(output_f16);
    xt_free(output);
    free(ref);
}

void test_unary_op_fp32(float *input, float *ref, int32_t size,
                        int (*unary_op)(float *, float *, int32_t))
{
    float *output = (float *)xt_malloc(size * sizeof(float));

    CYCLE_TEST(unary_op(input, output, size));

    check_result_fp32(ref, output, size);
    xt_free(output);
    xt_free(input);
    free(ref);
}

void test_unary_op_fp16(float *input, float *ref, int32_t size,
                        int (*unary_op)(__fp16 *, __fp16 *, int32_t))
{
    __fp16 *input_f16 = (__fp16 *)xt_malloc(size * sizeof(__fp16));
    xt_rvv_convert_fp32_to_fp16(input, input_f16, size);
    xt_free(input);
    __fp16 *output_f16 = (__fp16 *)xt_malloc(size * sizeof(__fp16));

    CYCLE_TEST(unary_op(input_f16, output_f16, size));
    xt_free(input_f16);
    float *output = (float *)xt_malloc(size * sizeof(float));
    xt_rvv_convert_fp16_to_fp32(output_f16, output, size);
    check_result_fp32(ref, output, size);
    xt_free(output_f16);
    xt_free(output);
    free(ref);
}

void test_transpose_fp32(float *input, float *ref, int32_t *shape_i, int32_t *shape_o, int32_t dim,
                         int32_t *permute,
                         int (*transpose)(float *, float *, int32_t *, int32_t *, int32_t,
                                          int32_t *))
{
    int32_t size = 1;
    for (int i = 0; i < dim; i++)
    {
        size *= shape_i[i];
    }

    float *output = (float *)xt_malloc(size * sizeof(float));

    CYCLE_TEST(transpose(input, output, shape_i, shape_o, dim, permute));

    check_result_fp32(ref, output, size);
    xt_free(output);
}

void test_transpose_fp16(float *input, float *ref, int32_t *shape_i, int32_t *shape_o, int32_t dim,
                         int32_t *permute,
                         int (*transpose)(__fp16 *, __fp16 *, int32_t *, int32_t *, int32_t,
                                          int32_t *))
{
    int32_t size = 1;
    for (int i = 0; i < dim; i++)
    {
        size *= shape_i[i];
    }
    __fp16 *input_f16 = (__fp16 *)xt_malloc(size * sizeof(__fp16));
    __fp16 *output_f16 = (__fp16 *)xt_malloc(size * sizeof(__fp16));
    xt_rvv_convert_fp32_to_fp16(input, input_f16, size);
    CYCLE_TEST(transpose(input_f16, output_f16, shape_i, shape_o, dim, permute));

    float *output = (float *)xt_malloc(size * sizeof(float));
    xt_rvv_convert_fp16_to_fp32(output_f16, output, size);
    check_result_fp32(ref, output, size);
    xt_free(input_f16);
    xt_free(output_f16);
    xt_free(output);
}

void test_mean_op_fp32(float *input, float *ref, int32_t *shape, int32_t dim, int32_t *axes,
                       int32_t dim_a,
                       int (*mean)(float *, float *, int32_t *, int32_t, int32_t *, int32_t))
{
    int size = 1;
    for (int i = 0; i < dim; i++)
    {
        size *= shape[i];
    }
    int32_t *axes_r = (int32_t *)fastmalloc(dim_a * sizeof(int32_t));
    for (int i = 0; i < dim_a; i++)
    {
        axes_r[i] = axes[i] >= 0 ? axes[i] : axes[i] + dim;
    }
    int out_size = size;
    for (int i = 0; i < dim_a; i++)
    {
        out_size /= shape[axes_r[i]];
    }
    float *output = (float *)fastmalloc(out_size * sizeof(float));

    CYCLE_TEST(mean(input, output, shape, dim, axes, dim_a));

    check_result_fp32(ref, output, out_size);
    fastfree(output);
    fastfree(axes_r);
}

void test_mean_op_fp16(float *input, float *ref, int32_t *shape, int32_t dim, int32_t *axes,
                       int32_t dim_a,
                       int (*mean)(__fp16 *, __fp16 *, int32_t *, int32_t, int32_t *, int32_t))
{
    int size = 1;
    for (int i = 0; i < dim; i++)
    {
        size *= shape[i];
    }
    int32_t *axes_r = (int32_t *)fastmalloc(dim_a * sizeof(int32_t));
    for (int i = 0; i < dim_a; i++)
    {
        axes_r[i] = axes[i] >= 0 ? axes[i] : axes[i] + dim;
    }
    int out_size = size;
    for (int i = 0; i < dim_a; i++)
    {
        out_size /= shape[axes_r[i]];
    }

    __fp16 *input_f16 = (__fp16 *)fastmalloc(size * sizeof(__fp16));
    __fp16 *output_f16 = (__fp16 *)fastmalloc(out_size * sizeof(__fp16));

    xt_rvv_convert_fp32_to_fp16(input, input_f16, size);

    CYCLE_TEST(mean(input_f16, output_f16, shape, dim, axes, dim_a));

    float *output = (float *)fastmalloc(out_size * sizeof(float));

    xt_rvv_convert_fp16_to_fp32(output_f16, output, out_size);

    check_result_fp32(ref, output, out_size);
    fastfree(input_f16);
    fastfree(output_f16);
    fastfree(output);
    fastfree(axes_r);
}

void test_expand_op_fp32(float *input, float *ref, int32_t *shape_in, int32_t dim_in,
                         int32_t *shape_out, int32_t dim_out,
                         int (*expand)(float *, float *, int32_t *, int32_t, int32_t *, int32_t))
{
    int size_out = 1;
    for (int i = 0; i < dim_out; i++)
    {
        size_out *= shape_out[i];
    }
    float *output = (float *)fastmalloc(size_out * sizeof(float));
    CYCLE_TEST(expand(input, output, shape_in, dim_in, shape_out, dim_out));
    check_result_fp32(ref, output, size_out);
    fastfree(output);
}

void test_expand_op_fp16(float *input, float *ref, int32_t *shape_in, int32_t dim_in,
                         int32_t *shape_out, int32_t dim_out,
                         int (*expand_fp16)(__fp16 *, __fp16 *, int32_t *, int32_t, int32_t *,
                                            int32_t))
{
    int size_out = 1;
    for (int i = 0; i < dim_out; i++)
    {
        size_out *= shape_out[i];
    }
    float *output = (float *)fastmalloc(size_out * sizeof(float));

    int size_in = 1;
    for (int i = 0; i < dim_in; i++)
    {
        size_in *= shape_in[i];
    }
    __fp16 *in_fp16 = (__fp16 *)fastmalloc(size_in * sizeof(__fp16));
    __fp16 *out_fp16 = (__fp16 *)fastmalloc(size_out * sizeof(__fp16));
    xt_rvv_convert_fp32_to_fp16(input, in_fp16, size_in);
    CYCLE_TEST(expand_fp16(in_fp16, out_fp16, shape_in, dim_in, shape_out, dim_out));
    xt_rvv_convert_fp16_to_fp32(out_fp16, output, size_out);
    check_result_fp32(ref, output, size_out);
    fastfree(output);
    fastfree(in_fp16);
    fastfree(out_fp16);
}

void test_cast_int_to_float_op_fp32(int32_t *input, float *ref, int32_t size,
                                    int (*cast_op)(int32_t *, float *, int32_t))
{
    float *output = (float *)fastmalloc(size * sizeof(float));
    CYCLE_TEST(cast_op(input, output, size));
    check_result_fp32(ref, output, size);
    fastfree(output);
}

void test_compare_op_fp32(float *input0, float *input1, float *ref, int32_t *shape_in0,
                          int32_t dim_in0, int32_t *shape_in1, int32_t dim_in1, int32_t *shape_out,
                          int32_t dim_out, int32_t type,
                          int (*compare)(float *, float *, float *, int32_t *, int32_t, int32_t *,
                                         int32_t, int32_t *, int32_t, int32_t))
{
    int size_out = 1;
    for (int i = 0; i < dim_out; i++)
    {
        size_out *= shape_out[i];
    }
    float *output = (float *)fastmalloc(size_out * sizeof(float));
    CYCLE_TEST(compare(input0, input1, output, shape_in0, dim_in0, shape_in1, dim_in1, shape_out, dim_out,
            type));
    check_result_fp32(ref, output, size_out);
    fastfree(output);
}

void test_compare_op_fp16(float *input0, float *input1, float *ref, int32_t *shape_in0,
                          int32_t dim_in0, int32_t *shape_in1, int32_t dim_in1, int32_t *shape_out,
                          int32_t dim_out, int32_t type,
                          int (*compare)(__fp16 *, __fp16 *, __fp16 *, int32_t *, int32_t,
                                         int32_t *, int32_t, int32_t *, int32_t, int32_t))
{
    int32_t size_in0 = 1;
    int32_t size_in1 = 1;
    int32_t size_out = 1;
    for (int i = 0; i < dim_in0; i++)
    {
        size_in0 *= shape_in0[i];
    }
    for (int i = 0; i < dim_in1; i++)
    {
        size_in1 *= shape_in1[i];
    }
    for (int i = 0; i < dim_out; i++)
    {
        size_out *= shape_out[i];
    }

    __fp16 *input0_f16 = (__fp16 *)fastmalloc(size_in0 * sizeof(__fp16));
    __fp16 *input1_f16 = (__fp16 *)fastmalloc(size_in1 * sizeof(__fp16));
    __fp16 *output_f16 = (__fp16 *)fastmalloc(size_out * sizeof(__fp16));
    xt_rvv_convert_fp32_to_fp16(input0, input0_f16, size_in0);
    xt_rvv_convert_fp32_to_fp16(input1, input1_f16, size_in1);

    CYCLE_TEST(compare(input0_f16, input1_f16, output_f16, shape_in0, dim_in0, shape_in1, dim_in1, shape_out,
            dim_out, type));

    float *output = (float *)fastmalloc(size_out * sizeof(float));
    xt_rvv_convert_fp16_to_fp32(output_f16, output, size_out);
    check_result_fp32(ref, output, size_out);

    fastfree(input0_f16);
    fastfree(input1_f16);
    fastfree(output_f16);
    fastfree(output);
}

void test_scatter_nd_op_fp32(float *input, int32_t *indices, float *updates, float *ref,
                             int32_t *shape_in, int32_t dim_in, int32_t *shape_indices,
                             int32_t dim_indices, int32_t *shape_updates, int32_t dim_updates,
                             int (*scatter_nd)(float *, int32_t *, float *, float *, int32_t *,
                                               int32_t, int32_t *, int32_t, int32_t *, int32_t))
{
    int size_out = 1;
    for (int i = 0; i < dim_in; i++)
    {
        size_out *= shape_in[i];
    }
    float *output = (float *)fastmalloc(size_out * sizeof(float));
    CYCLE_TEST(scatter_nd(input, indices, updates, output, shape_in, dim_in, shape_indices, dim_indices,
               shape_updates, dim_updates));
    check_result_fp32(ref, output, size_out);
    fastfree(output);
}

void test_scatter_nd_op_fp16(float *input, int32_t *indices, float *updates, float *ref,
                             int32_t *shape_in, int32_t dim_in, int32_t *shape_indices,
                             int32_t dim_indices, int32_t *shape_updates, int32_t dim_updates,
                             int (*scatter_nd)(__fp16 *, int32_t *, __fp16 *, __fp16 *, int32_t *,
                                               int32_t, int32_t *, int32_t, int32_t *, int32_t))
{
    int size_in = 1;
    for (int i = 0; i < dim_in; i++)
    {
        size_in *= shape_in[i];
    }
    int size_update = 1;
    for (int i = 0; i < dim_updates; i++)
    {
        size_update *= shape_updates[i];
    }
    __fp16 *in_fp16 = (__fp16 *)fastmalloc(size_in * sizeof(__fp16));
    __fp16 *out_fp16 = (__fp16 *)fastmalloc(size_in * sizeof(__fp16));
    __fp16 *update_fp16 = (__fp16 *)fastmalloc(size_update * sizeof(__fp16));
    xt_rvv_convert_fp32_to_fp16(input, in_fp16, size_in);
    xt_rvv_convert_fp32_to_fp16(updates, update_fp16, size_update);

    float *output = (float *)fastmalloc(size_in * sizeof(float));
    CYCLE_TEST(scatter_nd(in_fp16, indices, update_fp16, out_fp16, shape_in, dim_in, shape_indices,
               dim_indices, shape_updates, dim_updates));
    xt_rvv_convert_fp16_to_fp32(out_fp16, output, size_in);
    check_result_fp32(ref, output, size_in);
    fastfree(in_fp16);
    fastfree(out_fp16);
    fastfree(update_fp16);
    fastfree(output);
}

void test_norm_op_fp32(float *input, float *ref, int32_t *shape, int32_t dim, float *mean,
                       float *variance, float *gamma, float *beta, float *epsilon,
                       int (*batch_norm)(float *, float *, int32_t *, int32_t, float *, float *,
                                         float *, float *, float *))
{
    int size = 1;
    for (int i = 0; i < dim; i++)
    {
        size *= shape[i];
    }
    float *output = (float *)fastmalloc(size * sizeof(float));

    CYCLE_TEST(batch_norm(input, output, shape, dim, mean, variance, gamma, beta, epsilon));

    check_result_fp32(ref, output, size);
    fastfree(output);
}

void test_norm_op_fp16(float *input, float *ref, int32_t *shape, int32_t dim, float *mean,
                       float *variance, float *gamma, float *beta, float *epsilon,
                       int (*batch_norm)(__fp16 *, __fp16 *, int32_t *, int32_t, __fp16 *, __fp16 *,
                                         __fp16 *, __fp16 *, float *))
{
    int size = 1;
    for (int i = 0; i < dim; i++)
    {
        size *= shape[i];
    }
    int c_size = shape[1];
    __fp16 *input_f16 = (__fp16 *)fastmalloc(size * sizeof(__fp16));
    __fp16 *output_f16 = (__fp16 *)fastmalloc(size * sizeof(__fp16));
    __fp16 *mean_fp16 = (__fp16 *)fastmalloc(c_size * sizeof(__fp16));
    __fp16 *var_fp16 = (__fp16 *)fastmalloc(c_size * sizeof(__fp16));
    __fp16 *gamma_fp16 = (__fp16 *)fastmalloc(c_size * sizeof(__fp16));
    __fp16 *beta_fp16 = (__fp16 *)fastmalloc(c_size * sizeof(__fp16));
    xt_rvv_convert_fp32_to_fp16(input, input_f16, size);
    xt_rvv_convert_fp32_to_fp16(mean, mean_fp16, c_size);
    xt_rvv_convert_fp32_to_fp16(variance, var_fp16, c_size);

    if (gamma)
    {
        xt_rvv_convert_fp32_to_fp16(gamma, gamma_fp16, c_size);
    }
    __fp16 *gamma_ptr = gamma ? gamma_fp16 : NULL;

    if (beta)
    {
        xt_rvv_convert_fp32_to_fp16(beta, beta_fp16, c_size);
    }
    __fp16 *beta_ptr = beta ? beta_fp16 : NULL;

    CYCLE_TEST(batch_norm(input_f16, output_f16, shape, dim, mean_fp16, var_fp16, gamma_ptr, beta_ptr,
               epsilon));

    float *output = (float *)fastmalloc(size * sizeof(float));
    xt_rvv_convert_fp16_to_fp32(output_f16, output, size);

    check_result_fp32(ref, output, size);
    fastfree(input_f16);
    fastfree(output_f16);
    fastfree(output);
    fastfree(mean_fp16);
    fastfree(var_fp16);
    fastfree(gamma_fp16);
    fastfree(beta_fp16);
}
