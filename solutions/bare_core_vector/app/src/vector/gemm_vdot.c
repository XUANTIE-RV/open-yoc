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

#ifdef __riscv_vector
#include <riscv_vector.h>
#include <string.h>

extern int csrr_vlenb();

/*************************************************************
 * src: [M, K]
 * dst: [M/12, K, 12]
 ************************************************************/
void reordered_A(float *src, float *dst, int M, int K)
{
    int i = 0;
    for (; i + 11 < M; i += 12)
    {
        float *s_ptr = src + i * K;
        float *d_ptr = dst + i * K;
        int stride = 12 * sizeof(float);
        int c = 0;
        while (c < K)
        {
            size_t vl = vsetvl_e32m2(K - c);
            vfloat32m2_t _s0 = vle32_v_f32m2(s_ptr, vl);
            vfloat32m2_t _s1 = vle32_v_f32m2(s_ptr + K, vl);
            vfloat32m2_t _s2 = vle32_v_f32m2(s_ptr + K * 2, vl);
            vfloat32m2_t _s3 = vle32_v_f32m2(s_ptr + K * 3, vl);
            vfloat32m2_t _s4 = vle32_v_f32m2(s_ptr + K * 4, vl);
            vfloat32m2_t _s5 = vle32_v_f32m2(s_ptr + K * 5, vl);
            vfloat32m2_t _s6 = vle32_v_f32m2(s_ptr + K * 6, vl);
            vfloat32m2_t _s7 = vle32_v_f32m2(s_ptr + K * 7, vl);
            vfloat32m2_t _s8 = vle32_v_f32m2(s_ptr + K * 8, vl);
            vfloat32m2_t _s9 = vle32_v_f32m2(s_ptr + K * 9, vl);
            vfloat32m2_t _s10 = vle32_v_f32m2(s_ptr + K * 10, vl);
            vfloat32m2_t _s11 = vle32_v_f32m2(s_ptr + K * 11, vl);
            vsse32_v_f32m2(d_ptr, stride, _s0, vl);
            vsse32_v_f32m2(d_ptr + 1, stride, _s1, vl);
            vsse32_v_f32m2(d_ptr + 2, stride, _s2, vl);
            vsse32_v_f32m2(d_ptr + 3, stride, _s3, vl);
            vsse32_v_f32m2(d_ptr + 4, stride, _s4, vl);
            vsse32_v_f32m2(d_ptr + 5, stride, _s5, vl);
            vsse32_v_f32m2(d_ptr + 6, stride, _s6, vl);
            vsse32_v_f32m2(d_ptr + 7, stride, _s7, vl);
            vsse32_v_f32m2(d_ptr + 8, stride, _s8, vl);
            vsse32_v_f32m2(d_ptr + 9, stride, _s9, vl);
            vsse32_v_f32m2(d_ptr + 10, stride, _s10, vl);
            vsse32_v_f32m2(d_ptr + 11, stride, _s11, vl);
            s_ptr += vl;
            d_ptr += vl * 12;
            c += vl;
        }
    }
    for (; i + 7 < M; i += 8)
    {
        float *s_ptr = src + i * K;
        float *d_ptr = dst + i * K;
        int stride = 8 * sizeof(float);
        int c = 0;
        while (c < K)
        {
            size_t vl = vsetvl_e32m4(K - c);
            vfloat32m4_t _s0 = vle32_v_f32m4(s_ptr, vl);
            vfloat32m4_t _s1 = vle32_v_f32m4(s_ptr + K, vl);
            vfloat32m4_t _s2 = vle32_v_f32m4(s_ptr + K * 2, vl);
            vfloat32m4_t _s3 = vle32_v_f32m4(s_ptr + K * 3, vl);
            vfloat32m4_t _s4 = vle32_v_f32m4(s_ptr + K * 4, vl);
            vfloat32m4_t _s5 = vle32_v_f32m4(s_ptr + K * 5, vl);
            vfloat32m4_t _s6 = vle32_v_f32m4(s_ptr + K * 6, vl);
            vfloat32m4_t _s7 = vle32_v_f32m4(s_ptr + K * 7, vl);
            vsse32_v_f32m4(d_ptr, stride, _s0, vl);
            vsse32_v_f32m4(d_ptr + 1, stride, _s1, vl);
            vsse32_v_f32m4(d_ptr + 2, stride, _s2, vl);
            vsse32_v_f32m4(d_ptr + 3, stride, _s3, vl);
            vsse32_v_f32m4(d_ptr + 4, stride, _s4, vl);
            vsse32_v_f32m4(d_ptr + 5, stride, _s5, vl);
            vsse32_v_f32m4(d_ptr + 6, stride, _s6, vl);
            vsse32_v_f32m4(d_ptr + 7, stride, _s7, vl);
            s_ptr += vl;
            d_ptr += vl * 8;
            c += vl;
        }
    }
    for (; i + 3 < M; i += 4)
    {
        float *s_ptr = src + i * K;
        float *d_ptr = dst + i * K;
        int stride = 4 * sizeof(float);
        int c = 0;
        while (c < K)
        {
            size_t vl = vsetvl_e32m4(K - c);
            vfloat32m4_t _s0 = vle32_v_f32m4(s_ptr, vl);
            vfloat32m4_t _s1 = vle32_v_f32m4(s_ptr + K, vl);
            vfloat32m4_t _s2 = vle32_v_f32m4(s_ptr + K * 2, vl);
            vfloat32m4_t _s3 = vle32_v_f32m4(s_ptr + K * 3, vl);
            vsse32_v_f32m4(d_ptr, stride, _s0, vl);
            vsse32_v_f32m4(d_ptr + 1, stride, _s1, vl);
            vsse32_v_f32m4(d_ptr + 2, stride, _s2, vl);
            vsse32_v_f32m4(d_ptr + 3, stride, _s3, vl);
            s_ptr += vl;
            d_ptr += vl * 4;
            c += vl;
        }
    }
    for (; i + 1 < M; i += 2)
    {
        float *s_ptr = src + i * K;
        float *d_ptr = dst + i * K;
        int stride = 2 * sizeof(float);
        int c = 0;
        while (c < K)
        {
            size_t vl = vsetvl_e32m4(K - c);
            vfloat32m4_t _s0 = vle32_v_f32m4(s_ptr, vl);
            vfloat32m4_t _s1 = vle32_v_f32m4(s_ptr + K, vl);
            vsse32_v_f32m4(d_ptr, stride, _s0, vl);
            vsse32_v_f32m4(d_ptr + 1, stride, _s1, vl);
            s_ptr += vl;
            d_ptr += vl * 2;
            c += vl;
        }
    }
    for (; i < M; i++)
    {
        float *s_ptr = src + i * K;
        float *d_ptr = dst + i * K;
        int c = 0;
        while (c < K)
        {
            size_t vl = vsetvl_e32m4(K - c);
            vfloat32m4_t _src = vle32_v_f32m4(s_ptr, vl);
            vse32_v_f32m4(d_ptr, _src, vl);
            s_ptr += vl;
            d_ptr += vl;
            c += vl;
        }
    }
}

/*************************************************************
 * pack2n = vlenb / sizeof(float) * 2
 * src: [K, N]
 * dst: [N/pack2n, K, pack2n]
 ************************************************************/

void reordered_B(float *src, float *dst, int K, int N)
{
    const int packn = csrr_vlenb() / sizeof(float);
    const int pack2n = packn * 2;
    size_t vl = vsetvl_e32m2(pack2n);

    int j = 0;
    for (; j + pack2n - 1 < N; j += pack2n)
    {
        float *s_ptr = src + j;
        for (int c = 0; c < K; c++)
        {
            vfloat32m2_t _src = vle32_v_f32m2(s_ptr, vl);
            vse32_v_f32m2(dst, _src, vl);
            s_ptr += N;
            dst += vl;
        }
    }
    while (j < N)
    {
        vl = vsetvl_e32m1(N - j);
        float *s_ptr = src + j;
        for (int c = 0; c < K; c++)
        {
            vfloat32m1_t _src = vle32_v_f32m1(s_ptr, vl);
            vse32_v_f32m1(dst, _src, vl);
            s_ptr += N;
            dst += vl;
        }
        j += vl;
    }
}

void vdot_compute(float *dst, float *sa, float *sb, int M, int K, int N)
{
    memset(dst, 0, M * N * sizeof(float));
    const int packn = csrr_vlenb() / sizeof(float);
    const int pack2n = packn * 2;

    int start = 0;
    for (int i = start; i < M - 11; i += 12)
    {
        const float *sa_ptr = sa + i * K;
        int j = 0;
        size_t vl = vsetvl_e32m1(packn);

        for (; j + pack2n - 1 < N; j += pack2n)
        {
            const float *a_ptr = sa_ptr;
            const float *b0_ptr = sb + j * K;
            const float *b1_ptr = b0_ptr + packn;
            float *c0_ptr = dst + i * N + j;
            float *c1_ptr = c0_ptr + packn;

            // [n, 0]
            vfloat32m1_t _acc00 = vfmv_v_f_f32m1(0.0f, vl);
            vfloat32m1_t _acc10 = vfmv_v_f_f32m1(0.0f, vl);
            vfloat32m1_t _acc20 = vfmv_v_f_f32m1(0.0f, vl);
            vfloat32m1_t _acc30 = vfmv_v_f_f32m1(0.0f, vl);
            vfloat32m1_t _acc40 = vfmv_v_f_f32m1(0.0f, vl);
            vfloat32m1_t _acc50 = vfmv_v_f_f32m1(0.0f, vl);
            vfloat32m1_t _acc60 = vfmv_v_f_f32m1(0.0f, vl);
            vfloat32m1_t _acc70 = vfmv_v_f_f32m1(0.0f, vl);
            vfloat32m1_t _acc80 = vfmv_v_f_f32m1(0.0f, vl);
            vfloat32m1_t _acc90 = vfmv_v_f_f32m1(0.0f, vl);
            vfloat32m1_t _acca0 = vfmv_v_f_f32m1(0.0f, vl);
            vfloat32m1_t _accb0 = vfmv_v_f_f32m1(0.0f, vl);
            // [n, 1]
            vfloat32m1_t _acc01 = vmv_v_v_f32m1(_acc00, vl);
            vfloat32m1_t _acc11 = vmv_v_v_f32m1(_acc10, vl);
            vfloat32m1_t _acc21 = vmv_v_v_f32m1(_acc20, vl);
            vfloat32m1_t _acc31 = vmv_v_v_f32m1(_acc30, vl);
            vfloat32m1_t _acc41 = vmv_v_v_f32m1(_acc40, vl);
            vfloat32m1_t _acc51 = vmv_v_v_f32m1(_acc50, vl);
            vfloat32m1_t _acc61 = vmv_v_v_f32m1(_acc60, vl);
            vfloat32m1_t _acc71 = vmv_v_v_f32m1(_acc70, vl);
            vfloat32m1_t _acc81 = vmv_v_v_f32m1(_acc80, vl);
            vfloat32m1_t _acc91 = vmv_v_v_f32m1(_acc90, vl);
            vfloat32m1_t _acca1 = vmv_v_v_f32m1(_acca0, vl);
            vfloat32m1_t _accb1 = vmv_v_v_f32m1(_accb0, vl);

            for (int c = 0; c < K; c++)
            {
                vfloat32m1_t _b0 = vle32_v_f32m1(b0_ptr, vl);
                vfloat32m1_t _b1 = vle32_v_f32m1(b1_ptr, vl);
                b0_ptr += pack2n;
                b1_ptr += pack2n;

                _acc00 = vfmacc_vf_f32m1(_acc00, a_ptr[0], _b0, vl);
                _acc10 = vfmacc_vf_f32m1(_acc10, a_ptr[1], _b0, vl);
                _acc20 = vfmacc_vf_f32m1(_acc20, a_ptr[2], _b0, vl);
                _acc30 = vfmacc_vf_f32m1(_acc30, a_ptr[3], _b0, vl);
                _acc40 = vfmacc_vf_f32m1(_acc40, a_ptr[4], _b0, vl);
                _acc50 = vfmacc_vf_f32m1(_acc50, a_ptr[5], _b0, vl);
                _acc60 = vfmacc_vf_f32m1(_acc60, a_ptr[6], _b0, vl);
                _acc70 = vfmacc_vf_f32m1(_acc70, a_ptr[7], _b0, vl);
                _acc80 = vfmacc_vf_f32m1(_acc80, a_ptr[8], _b0, vl);
                _acc90 = vfmacc_vf_f32m1(_acc90, a_ptr[9], _b0, vl);
                _acca0 = vfmacc_vf_f32m1(_acca0, a_ptr[10], _b0, vl);
                _accb0 = vfmacc_vf_f32m1(_accb0, a_ptr[11], _b0, vl);

                _acc01 = vfmacc_vf_f32m1(_acc01, a_ptr[0], _b1, vl);
                _acc11 = vfmacc_vf_f32m1(_acc11, a_ptr[1], _b1, vl);
                _acc21 = vfmacc_vf_f32m1(_acc21, a_ptr[2], _b1, vl);
                _acc31 = vfmacc_vf_f32m1(_acc31, a_ptr[3], _b1, vl);
                _acc41 = vfmacc_vf_f32m1(_acc41, a_ptr[4], _b1, vl);
                _acc51 = vfmacc_vf_f32m1(_acc51, a_ptr[5], _b1, vl);
                _acc61 = vfmacc_vf_f32m1(_acc61, a_ptr[6], _b1, vl);
                _acc71 = vfmacc_vf_f32m1(_acc71, a_ptr[7], _b1, vl);
                _acc81 = vfmacc_vf_f32m1(_acc81, a_ptr[8], _b1, vl);
                _acc91 = vfmacc_vf_f32m1(_acc91, a_ptr[9], _b1, vl);
                _acca1 = vfmacc_vf_f32m1(_acca1, a_ptr[10], _b1, vl);
                _accb1 = vfmacc_vf_f32m1(_accb1, a_ptr[11], _b1, vl);
                a_ptr += 12;
            }

            vse32_v_f32m1(c0_ptr, _acc00, vl);
            vse32_v_f32m1(c0_ptr + N, _acc10, vl);
            vse32_v_f32m1(c0_ptr + N * 2, _acc20, vl);
            vse32_v_f32m1(c0_ptr + N * 3, _acc30, vl);
            vse32_v_f32m1(c0_ptr + N * 4, _acc40, vl);
            vse32_v_f32m1(c0_ptr + N * 5, _acc50, vl);
            vse32_v_f32m1(c0_ptr + N * 6, _acc60, vl);
            vse32_v_f32m1(c0_ptr + N * 7, _acc70, vl);
            vse32_v_f32m1(c0_ptr + N * 8, _acc80, vl);
            vse32_v_f32m1(c0_ptr + N * 9, _acc90, vl);
            vse32_v_f32m1(c0_ptr + N * 10, _acca0, vl);
            vse32_v_f32m1(c0_ptr + N * 11, _accb0, vl);
            vse32_v_f32m1(c1_ptr, _acc01, vl);
            vse32_v_f32m1(c1_ptr + N, _acc11, vl);
            vse32_v_f32m1(c1_ptr + N * 2, _acc21, vl);
            vse32_v_f32m1(c1_ptr + N * 3, _acc31, vl);
            vse32_v_f32m1(c1_ptr + N * 4, _acc41, vl);
            vse32_v_f32m1(c1_ptr + N * 5, _acc51, vl);
            vse32_v_f32m1(c1_ptr + N * 6, _acc61, vl);
            vse32_v_f32m1(c1_ptr + N * 7, _acc71, vl);
            vse32_v_f32m1(c1_ptr + N * 8, _acc81, vl);
            vse32_v_f32m1(c1_ptr + N * 9, _acc91, vl);
            vse32_v_f32m1(c1_ptr + N * 10, _acca1, vl);
            vse32_v_f32m1(c1_ptr + N * 11, _accb1, vl);
        }
        while (j < N)
        {
            size_t vl = vsetvl_e32m1(N - j);
            const float *a_ptr = sa_ptr;
            const float *b0_ptr = sb + j * K;
            float *c0_ptr = dst + i * N + j;

            vfloat32m1_t _acc00 = vfmv_v_f_f32m1(0.0f, vl);
            vfloat32m1_t _acc10 = vfmv_v_f_f32m1(0.0f, vl);
            vfloat32m1_t _acc20 = vfmv_v_f_f32m1(0.0f, vl);
            vfloat32m1_t _acc30 = vfmv_v_f_f32m1(0.0f, vl);
            vfloat32m1_t _acc40 = vfmv_v_f_f32m1(0.0f, vl);
            vfloat32m1_t _acc50 = vfmv_v_f_f32m1(0.0f, vl);
            vfloat32m1_t _acc60 = vfmv_v_f_f32m1(0.0f, vl);
            vfloat32m1_t _acc70 = vfmv_v_f_f32m1(0.0f, vl);
            vfloat32m1_t _acc80 = vfmv_v_f_f32m1(0.0f, vl);
            vfloat32m1_t _acc90 = vfmv_v_f_f32m1(0.0f, vl);
            vfloat32m1_t _acca0 = vfmv_v_f_f32m1(0.0f, vl);
            vfloat32m1_t _accb0 = vfmv_v_f_f32m1(0.0f, vl);

            for (int c = 0; c < K; c++)
            {
                vfloat32m1_t _b0 = vle32_v_f32m1(b0_ptr, vl);
                b0_ptr += vl;

                _acc00 = vfmacc_vf_f32m1(_acc00, a_ptr[0], _b0, vl);
                _acc10 = vfmacc_vf_f32m1(_acc10, a_ptr[1], _b0, vl);
                _acc20 = vfmacc_vf_f32m1(_acc20, a_ptr[2], _b0, vl);
                _acc30 = vfmacc_vf_f32m1(_acc30, a_ptr[3], _b0, vl);
                _acc40 = vfmacc_vf_f32m1(_acc40, a_ptr[4], _b0, vl);
                _acc50 = vfmacc_vf_f32m1(_acc50, a_ptr[5], _b0, vl);
                _acc60 = vfmacc_vf_f32m1(_acc60, a_ptr[6], _b0, vl);
                _acc70 = vfmacc_vf_f32m1(_acc70, a_ptr[7], _b0, vl);
                _acc80 = vfmacc_vf_f32m1(_acc80, a_ptr[8], _b0, vl);
                _acc90 = vfmacc_vf_f32m1(_acc90, a_ptr[9], _b0, vl);
                _acca0 = vfmacc_vf_f32m1(_acca0, a_ptr[10], _b0, vl);
                _accb0 = vfmacc_vf_f32m1(_accb0, a_ptr[11], _b0, vl);
                a_ptr += 12;
            }

            vse32_v_f32m1(c0_ptr, _acc00, vl);
            vse32_v_f32m1(c0_ptr + N, _acc10, vl);
            vse32_v_f32m1(c0_ptr + N * 2, _acc20, vl);
            vse32_v_f32m1(c0_ptr + N * 3, _acc30, vl);
            vse32_v_f32m1(c0_ptr + N * 4, _acc40, vl);
            vse32_v_f32m1(c0_ptr + N * 5, _acc50, vl);
            vse32_v_f32m1(c0_ptr + N * 6, _acc60, vl);
            vse32_v_f32m1(c0_ptr + N * 7, _acc70, vl);
            vse32_v_f32m1(c0_ptr + N * 8, _acc80, vl);
            vse32_v_f32m1(c0_ptr + N * 9, _acc90, vl);
            vse32_v_f32m1(c0_ptr + N * 10, _acca0, vl);
            vse32_v_f32m1(c0_ptr + N * 11, _accb0, vl);
            j += vl;
        }
    }

    start += ((M - start) / 12) * 12;
    for (int i = start; i < M - 7; i += 8)
    {
        const float *sa_ptr = sa + i * K;
        int j = 0;
        size_t vl = vsetvl_e32m1(packn);
        for (; j + pack2n - 1 < N; j += pack2n)
        {
            const float *a_ptr = sa_ptr;
            const float *b0_ptr = sb + j * K;
            const float *b1_ptr = b0_ptr + packn;
            float *c0_ptr = dst + i * N + j;
            float *c1_ptr = c0_ptr + packn;

            // [n, 0]
            vfloat32m1_t _acc00 = vfmv_v_f_f32m1(0.0f, vl);
            vfloat32m1_t _acc10 = vfmv_v_f_f32m1(0.0f, vl);
            vfloat32m1_t _acc20 = vfmv_v_f_f32m1(0.0f, vl);
            vfloat32m1_t _acc30 = vfmv_v_f_f32m1(0.0f, vl);
            vfloat32m1_t _acc40 = vfmv_v_f_f32m1(0.0f, vl);
            vfloat32m1_t _acc50 = vfmv_v_f_f32m1(0.0f, vl);
            vfloat32m1_t _acc60 = vfmv_v_f_f32m1(0.0f, vl);
            vfloat32m1_t _acc70 = vfmv_v_f_f32m1(0.0f, vl);
            // [n, 1]
            vfloat32m1_t _acc01 = vmv_v_v_f32m1(_acc00, vl);
            vfloat32m1_t _acc11 = vmv_v_v_f32m1(_acc10, vl);
            vfloat32m1_t _acc21 = vmv_v_v_f32m1(_acc20, vl);
            vfloat32m1_t _acc31 = vmv_v_v_f32m1(_acc30, vl);
            vfloat32m1_t _acc41 = vmv_v_v_f32m1(_acc40, vl);
            vfloat32m1_t _acc51 = vmv_v_v_f32m1(_acc50, vl);
            vfloat32m1_t _acc61 = vmv_v_v_f32m1(_acc60, vl);
            vfloat32m1_t _acc71 = vmv_v_v_f32m1(_acc70, vl);

            for (int c = 0; c < K; c++)
            {
                vfloat32m1_t _b0 = vle32_v_f32m1(b0_ptr, vl);
                vfloat32m1_t _b1 = vle32_v_f32m1(b1_ptr, vl);
                b0_ptr += pack2n;
                b1_ptr += pack2n;

                _acc00 = vfmacc_vf_f32m1(_acc00, a_ptr[0], _b0, vl);
                _acc10 = vfmacc_vf_f32m1(_acc10, a_ptr[1], _b0, vl);
                _acc20 = vfmacc_vf_f32m1(_acc20, a_ptr[2], _b0, vl);
                _acc30 = vfmacc_vf_f32m1(_acc30, a_ptr[3], _b0, vl);
                _acc40 = vfmacc_vf_f32m1(_acc40, a_ptr[4], _b0, vl);
                _acc50 = vfmacc_vf_f32m1(_acc50, a_ptr[5], _b0, vl);
                _acc60 = vfmacc_vf_f32m1(_acc60, a_ptr[6], _b0, vl);
                _acc70 = vfmacc_vf_f32m1(_acc70, a_ptr[7], _b0, vl);
                _acc01 = vfmacc_vf_f32m1(_acc01, a_ptr[0], _b1, vl);
                _acc11 = vfmacc_vf_f32m1(_acc11, a_ptr[1], _b1, vl);
                _acc21 = vfmacc_vf_f32m1(_acc21, a_ptr[2], _b1, vl);
                _acc31 = vfmacc_vf_f32m1(_acc31, a_ptr[3], _b1, vl);
                _acc41 = vfmacc_vf_f32m1(_acc41, a_ptr[4], _b1, vl);
                _acc51 = vfmacc_vf_f32m1(_acc51, a_ptr[5], _b1, vl);
                _acc61 = vfmacc_vf_f32m1(_acc61, a_ptr[6], _b1, vl);
                _acc71 = vfmacc_vf_f32m1(_acc71, a_ptr[7], _b1, vl);
                a_ptr += 8;
            }

            vse32_v_f32m1(c0_ptr, _acc00, vl);
            vse32_v_f32m1(c0_ptr + N, _acc10, vl);
            vse32_v_f32m1(c0_ptr + N * 2, _acc20, vl);
            vse32_v_f32m1(c0_ptr + N * 3, _acc30, vl);
            vse32_v_f32m1(c0_ptr + N * 4, _acc40, vl);
            vse32_v_f32m1(c0_ptr + N * 5, _acc50, vl);
            vse32_v_f32m1(c0_ptr + N * 6, _acc60, vl);
            vse32_v_f32m1(c0_ptr + N * 7, _acc70, vl);
            vse32_v_f32m1(c1_ptr, _acc01, vl);
            vse32_v_f32m1(c1_ptr + N, _acc11, vl);
            vse32_v_f32m1(c1_ptr + N * 2, _acc21, vl);
            vse32_v_f32m1(c1_ptr + N * 3, _acc31, vl);
            vse32_v_f32m1(c1_ptr + N * 4, _acc41, vl);
            vse32_v_f32m1(c1_ptr + N * 5, _acc51, vl);
            vse32_v_f32m1(c1_ptr + N * 6, _acc61, vl);
            vse32_v_f32m1(c1_ptr + N * 7, _acc71, vl);
        }
        while (j < N)
        {
            size_t vl = vsetvl_e32m1(N - j);
            const float *a_ptr = sa_ptr;
            const float *b0_ptr = sb + j * K;
            float *c0_ptr = dst + i * N + j;

            vfloat32m1_t _acc00 = vfmv_v_f_f32m1(0.0f, vl);
            vfloat32m1_t _acc10 = vfmv_v_f_f32m1(0.0f, vl);
            vfloat32m1_t _acc20 = vfmv_v_f_f32m1(0.0f, vl);
            vfloat32m1_t _acc30 = vfmv_v_f_f32m1(0.0f, vl);
            vfloat32m1_t _acc40 = vfmv_v_f_f32m1(0.0f, vl);
            vfloat32m1_t _acc50 = vfmv_v_f_f32m1(0.0f, vl);
            vfloat32m1_t _acc60 = vfmv_v_f_f32m1(0.0f, vl);
            vfloat32m1_t _acc70 = vfmv_v_f_f32m1(0.0f, vl);

            for (int c = 0; c < K; c++)
            {
                vfloat32m1_t _b0 = vle32_v_f32m1(b0_ptr, vl);
                b0_ptr += vl;

                _acc00 = vfmacc_vf_f32m1(_acc00, a_ptr[0], _b0, vl);
                _acc10 = vfmacc_vf_f32m1(_acc10, a_ptr[1], _b0, vl);
                _acc20 = vfmacc_vf_f32m1(_acc20, a_ptr[2], _b0, vl);
                _acc30 = vfmacc_vf_f32m1(_acc30, a_ptr[3], _b0, vl);
                _acc40 = vfmacc_vf_f32m1(_acc40, a_ptr[4], _b0, vl);
                _acc50 = vfmacc_vf_f32m1(_acc50, a_ptr[5], _b0, vl);
                _acc60 = vfmacc_vf_f32m1(_acc60, a_ptr[6], _b0, vl);
                _acc70 = vfmacc_vf_f32m1(_acc70, a_ptr[7], _b0, vl);
                a_ptr += 8;
            }

            vse32_v_f32m1(c0_ptr, _acc00, vl);
            vse32_v_f32m1(c0_ptr + N, _acc10, vl);
            vse32_v_f32m1(c0_ptr + N * 2, _acc20, vl);
            vse32_v_f32m1(c0_ptr + N * 3, _acc30, vl);
            vse32_v_f32m1(c0_ptr + N * 4, _acc40, vl);
            vse32_v_f32m1(c0_ptr + N * 5, _acc50, vl);
            vse32_v_f32m1(c0_ptr + N * 6, _acc60, vl);
            vse32_v_f32m1(c0_ptr + N * 7, _acc70, vl);
            j += vl;
        }
    }

    start += ((M - start) >> 3) << 3;
    for (int i = start; i < M - 3; i += 4)
    {
        const float *sa_ptr = sa + i * K;
        int j = 0;
        size_t vl = vsetvl_e32m1(packn);
        for (; j + pack2n - 1 < N; j += pack2n)
        {
            const float *a_ptr = sa_ptr;
            const float *b0_ptr = sb + j * K;
            const float *b1_ptr = b0_ptr + packn;
            float *c0_ptr = dst + i * N + j;
            float *c1_ptr = c0_ptr + packn;

            // [n, 0]
            vfloat32m1_t _acc00 = vfmv_v_f_f32m1(0.0f, vl);
            vfloat32m1_t _acc10 = vfmv_v_f_f32m1(0.0f, vl);
            vfloat32m1_t _acc20 = vfmv_v_f_f32m1(0.0f, vl);
            vfloat32m1_t _acc30 = vfmv_v_f_f32m1(0.0f, vl);
            // [n, 1]
            vfloat32m1_t _acc01 = vmv_v_v_f32m1(_acc00, vl);
            vfloat32m1_t _acc11 = vmv_v_v_f32m1(_acc10, vl);
            vfloat32m1_t _acc21 = vmv_v_v_f32m1(_acc20, vl);
            vfloat32m1_t _acc31 = vmv_v_v_f32m1(_acc30, vl);

            for (int c = 0; c < K; c++)
            {
                vfloat32m1_t _b0 = vle32_v_f32m1(b0_ptr, vl);
                vfloat32m1_t _b1 = vle32_v_f32m1(b1_ptr, vl);
                b0_ptr += pack2n;
                b1_ptr += pack2n;

                _acc00 = vfmacc_vf_f32m1(_acc00, a_ptr[0], _b0, vl);
                _acc10 = vfmacc_vf_f32m1(_acc10, a_ptr[1], _b0, vl);
                _acc20 = vfmacc_vf_f32m1(_acc20, a_ptr[2], _b0, vl);
                _acc30 = vfmacc_vf_f32m1(_acc30, a_ptr[3], _b0, vl);
                _acc01 = vfmacc_vf_f32m1(_acc01, a_ptr[0], _b1, vl);
                _acc11 = vfmacc_vf_f32m1(_acc11, a_ptr[1], _b1, vl);
                _acc21 = vfmacc_vf_f32m1(_acc21, a_ptr[2], _b1, vl);
                _acc31 = vfmacc_vf_f32m1(_acc31, a_ptr[3], _b1, vl);
                a_ptr += 4;
            }

            vse32_v_f32m1(c0_ptr, _acc00, vl);
            vse32_v_f32m1(c0_ptr + N, _acc10, vl);
            vse32_v_f32m1(c0_ptr + N * 2, _acc20, vl);
            vse32_v_f32m1(c0_ptr + N * 3, _acc30, vl);
            vse32_v_f32m1(c1_ptr, _acc01, vl);
            vse32_v_f32m1(c1_ptr + N, _acc11, vl);
            vse32_v_f32m1(c1_ptr + N * 2, _acc21, vl);
            vse32_v_f32m1(c1_ptr + N * 3, _acc31, vl);
        }
        while (j < N)
        {
            size_t vl = vsetvl_e32m1(N - j);
            const float *a_ptr = sa_ptr;
            const float *b0_ptr = sb + j * K;
            float *c0_ptr = dst + i * N + j;

            vfloat32m1_t _acc00 = vfmv_v_f_f32m1(0.0f, vl);
            vfloat32m1_t _acc10 = vfmv_v_f_f32m1(0.0f, vl);
            vfloat32m1_t _acc20 = vfmv_v_f_f32m1(0.0f, vl);
            vfloat32m1_t _acc30 = vfmv_v_f_f32m1(0.0f, vl);

            for (int c = 0; c < K; c++)
            {
                vfloat32m1_t _b0 = vle32_v_f32m1(b0_ptr, vl);
                b0_ptr += vl;

                _acc00 = vfmacc_vf_f32m1(_acc00, a_ptr[0], _b0, vl);
                _acc10 = vfmacc_vf_f32m1(_acc10, a_ptr[1], _b0, vl);
                _acc20 = vfmacc_vf_f32m1(_acc20, a_ptr[2], _b0, vl);
                _acc30 = vfmacc_vf_f32m1(_acc30, a_ptr[3], _b0, vl);
                a_ptr += 4;
            }

            vse32_v_f32m1(c0_ptr, _acc00, vl);
            vse32_v_f32m1(c0_ptr + N, _acc10, vl);
            vse32_v_f32m1(c0_ptr + N * 2, _acc20, vl);
            vse32_v_f32m1(c0_ptr + N * 3, _acc30, vl);
            j += vl;
        }
    }

    start += ((M - start) >> 2) << 2;
    for (int i = start; i < M - 1; i += 2)
    {
        const float *sa_ptr = sa + i * K;
        int j = 0;
        size_t vl = vsetvl_e32m1(packn);
        for (; j + pack2n - 1 < N; j += pack2n)
        {
            const float *a_ptr = sa_ptr;
            const float *b0_ptr = sb + j * K;
            const float *b1_ptr = b0_ptr + packn;
            float *c0_ptr = dst + i * N + j;
            float *c1_ptr = c0_ptr + packn;

            // [n, 0]
            vfloat32m1_t _acc00 = vfmv_v_f_f32m1(0.0f, vl);
            vfloat32m1_t _acc10 = vfmv_v_f_f32m1(0.0f, vl);
            // [n, 1]
            vfloat32m1_t _acc01 = vmv_v_v_f32m1(_acc00, vl);
            vfloat32m1_t _acc11 = vmv_v_v_f32m1(_acc10, vl);

            for (int c = 0; c < K; c++)
            {
                vfloat32m1_t _b0 = vle32_v_f32m1(b0_ptr, vl);
                vfloat32m1_t _b1 = vle32_v_f32m1(b1_ptr, vl);
                b0_ptr += pack2n;
                b1_ptr += pack2n;

                _acc00 = vfmacc_vf_f32m1(_acc00, a_ptr[0], _b0, vl);
                _acc10 = vfmacc_vf_f32m1(_acc10, a_ptr[1], _b0, vl);
                _acc01 = vfmacc_vf_f32m1(_acc01, a_ptr[0], _b1, vl);
                _acc11 = vfmacc_vf_f32m1(_acc11, a_ptr[1], _b1, vl);
                a_ptr += 2;
            }

            vse32_v_f32m1(c0_ptr, _acc00, vl);
            vse32_v_f32m1(c0_ptr + N, _acc10, vl);
            vse32_v_f32m1(c1_ptr, _acc01, vl);
            vse32_v_f32m1(c1_ptr + N, _acc11, vl);
        }
        while (j < N)
        {
            size_t vl = vsetvl_e32m1(N - j);
            const float *a_ptr = sa_ptr;
            const float *b0_ptr = sb + j * K;
            float *c0_ptr = dst + i * N + j;

            vfloat32m1_t _acc00 = vfmv_v_f_f32m1(0.0f, vl);
            vfloat32m1_t _acc10 = vfmv_v_f_f32m1(0.0f, vl);

            for (int c = 0; c < K; c++)
            {
                vfloat32m1_t _b0 = vle32_v_f32m1(b0_ptr, vl);
                b0_ptr += vl;

                _acc00 = vfmacc_vf_f32m1(_acc00, a_ptr[0], _b0, vl);
                _acc10 = vfmacc_vf_f32m1(_acc10, a_ptr[1], _b0, vl);
                a_ptr += 2;
            }

            vse32_v_f32m1(c0_ptr, _acc00, vl);
            vse32_v_f32m1(c0_ptr + N, _acc10, vl);
            j += vl;
        }
    }

    start += ((M - start) >> 1) << 1;
    for (int i = start; i < M; i++)
    {
        const float *sa_ptr = sa + i * K;
        int j = 0;
        size_t vl = vsetvl_e32m1(packn);
        for (; j + pack2n - 1 < N; j += pack2n)
        {
            const float *a_ptr = sa_ptr;
            const float *b0_ptr = sb + j * K;
            const float *b1_ptr = b0_ptr + packn;
            float *c0_ptr = dst + i * N + j;
            float *c1_ptr = c0_ptr + packn;

            // [n, 0]
            vfloat32m1_t _acc00 = vfmv_v_f_f32m1(0.0f, vl);
            // [n, 1]
            vfloat32m1_t _acc01 = vmv_v_v_f32m1(_acc00, vl);

            for (int c = 0; c < K; c++)
            {
                vfloat32m1_t _b0 = vle32_v_f32m1(b0_ptr, vl);
                vfloat32m1_t _b1 = vle32_v_f32m1(b1_ptr, vl);
                b0_ptr += pack2n;
                b1_ptr += pack2n;

                _acc00 = vfmacc_vf_f32m1(_acc00, a_ptr[0], _b0, vl);
                _acc01 = vfmacc_vf_f32m1(_acc01, a_ptr[0], _b1, vl);
                a_ptr += 1;
            }

            vse32_v_f32m1(c0_ptr, _acc00, vl);
            vse32_v_f32m1(c1_ptr, _acc01, vl);
        }
        while (j < N)
        {
            size_t vl = vsetvl_e32m1(N - j);
            const float *a_ptr = sa_ptr;
            const float *b0_ptr = sb + j * K;
            float *c0_ptr = dst + i * N + j;

            vfloat32m1_t _acc00 = vfmv_v_f_f32m1(0.0f, vl);

            for (int c = 0; c < K; c++)
            {
                vfloat32m1_t _b0 = vle32_v_f32m1(b0_ptr, vl);
                b0_ptr += vl;

                _acc00 = vfmacc_vf_f32m1(_acc00, a_ptr[0], _b0, vl);
                a_ptr += 1;
            }

            vse32_v_f32m1(c0_ptr, _acc00, vl);
            j += vl;
        }
    }
}
#endif
