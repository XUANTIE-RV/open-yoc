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
#include <stdio.h>
#include <string.h>
#include <riscv_vector.h>

/**************************************************************
 * Data arrangement: Z8 | | |
 **************************************************************/
void shl_c908_reorder_input_z8_fp32(float *b, float *sb, int k, int n, int ldx)
{
    int32_t vl = vsetvl_e32m2(8);
    float *b0 = NULL;
    int i = 0;
    for (; i + 7 < n; i += 8) {
        b0 = b + i;
        for (int j = 0; j < k; j++) {
            vfloat32m2_t _tmp = vle32_v_f32m2(b0, vl);
            b0 += ldx;
            vse32_v_f32m2(sb, _tmp, vl);
            sb += 8;
        }
    }

    for (; i < n; i++) {
        vl = vsetvl_e32m2(8);
        b0 = b + i;
        int j = 0;
        for (; j + 7 < k; j += 8) {
            vfloat32m2_t _tmp = vlse32_v_f32m2(b0, ldx * sizeof(float), vl);
            b0 += 8 * ldx;
            vse32_v_f32m2(sb, _tmp, vl);
            sb += 8;
        }
        if (j < k) {
            vl = vsetvl_e32m2(k & 7);
            vfloat32m2_t _tmp = vlse32_v_f32m2(b0, ldx * sizeof(float), vl);
            vse32_v_f32m2(sb, _tmp, vl);
            sb += vl;
        }
    }
}

/**************************************************************
 * Data arrangement: N8
 **************************************************************/
void shl_c908_reorder_kernel_n8_fp32(float *src, float *dst, int m, int k, int ldx)
{
    int i = 0;
    for (; i + 7 < m; i += 8) {
        for (int j = 0; j < k; j++) {
            float *in_ptr = src + j;
            vfloat32m2_t _input = vlse32_v_f32m2(in_ptr, k * sizeof(float), 8);
            vse32_v_f32m2(dst, _input, 8);
            dst += 8;
        }
        src += 8 * k;
    }
    for (; i + 3 < m; i += 4) {
        for (int j = 0; j < k; j++) {
            float *in_ptr = src + j;
            vfloat32m1_t _input = vlse32_v_f32m1(in_ptr, k * sizeof(float), 4);
            vse32_v_f32m1(dst, _input, 4);
            dst += 4;
        }
        src += 4 * k;
    }
    for (; i + 1 < m; i += 2) {
        for (int j = 0; j < k; j++) {
            float *in_ptr = src + j;
            vfloat32m1_t _input = vlse32_v_f32m1(in_ptr, k * sizeof(float), 2);
            vse32_v_f32m1(dst, _input, 2);
            dst += 2;
        }
        src += 2 * k;
    }
    for (; i < m; i++) {
        memcpy(dst, src, k * sizeof(float));
    }
}

/*************************************************************
 * note: VLEN = 128
 * input matrix and kernel matrix have been reordered
 *************************************************************/
void shl_c908_gemm_8x8_fp32(float *sc, const float *sa, const float *sb, int m, int k, int n, int ldc)
{
    float *kernel_data = (float *)sa;
    float *input_data = (float *)sb;
    float *output_data = sc;

    int vl;

    int i = 0;
    // m8 loop
    vl = vsetvl_e32m2(8);
    for (; i + 7 < m; i += 8) {
        float *in_ptr = input_data;

        float *out_ptr0 = output_data;
        float *out_ptr1 = out_ptr0 + ldc;
        float *out_ptr2 = out_ptr1 + ldc;
        float *out_ptr3 = out_ptr2 + ldc;
        float *out_ptr4 = out_ptr3 + ldc;
        float *out_ptr5 = out_ptr4 + ldc;
        float *out_ptr6 = out_ptr5 + ldc;
        float *out_ptr7 = out_ptr6 + ldc;

        int j = 0;
        // m8n8 loop
        for (; j + 7 < n; j += 8) {
            float *kernel_ptr = kernel_data;
            vfloat32m2_t _acc0 = vle32_v_f32m2(out_ptr0, vl);
            vfloat32m2_t _acc1 = vle32_v_f32m2(out_ptr1, vl);
            vfloat32m2_t _acc2 = vle32_v_f32m2(out_ptr2, vl);
            vfloat32m2_t _acc3 = vle32_v_f32m2(out_ptr3, vl);
            vfloat32m2_t _acc4 = vle32_v_f32m2(out_ptr4, vl);
            vfloat32m2_t _acc5 = vle32_v_f32m2(out_ptr5, vl);
            vfloat32m2_t _acc6 = vle32_v_f32m2(out_ptr6, vl);
            vfloat32m2_t _acc7 = vle32_v_f32m2(out_ptr7, vl);  // init acc with bias_data

            for (int c = 0; c < k; c++) {
                vfloat32m2_t _input = vle32_v_f32m2(in_ptr, vl);

                float k0 = kernel_ptr[0];
                float k1 = kernel_ptr[1];
                float k2 = kernel_ptr[2];
                float k3 = kernel_ptr[3];
                float k4 = kernel_ptr[4];
                float k5 = kernel_ptr[5];
                float k6 = kernel_ptr[6];
                float k7 = kernel_ptr[7];

                _acc0 = vfmacc_vf_f32m2(_acc0, k0, _input, vl);
                _acc1 = vfmacc_vf_f32m2(_acc1, k1, _input, vl);
                _acc2 = vfmacc_vf_f32m2(_acc2, k2, _input, vl);
                _acc3 = vfmacc_vf_f32m2(_acc3, k3, _input, vl);
                _acc4 = vfmacc_vf_f32m2(_acc4, k4, _input, vl);
                _acc5 = vfmacc_vf_f32m2(_acc5, k5, _input, vl);
                _acc6 = vfmacc_vf_f32m2(_acc6, k6, _input, vl);
                _acc7 = vfmacc_vf_f32m2(_acc7, k7, _input, vl);

                kernel_ptr += 8;
                in_ptr += 8;
            }
            vse32_v_f32m2(out_ptr0, _acc0, vl);
            vse32_v_f32m2(out_ptr1, _acc1, vl);
            vse32_v_f32m2(out_ptr2, _acc2, vl);
            vse32_v_f32m2(out_ptr3, _acc3, vl);
            vse32_v_f32m2(out_ptr4, _acc4, vl);
            vse32_v_f32m2(out_ptr5, _acc5, vl);
            vse32_v_f32m2(out_ptr6, _acc6, vl);
            vse32_v_f32m2(out_ptr7, _acc7, vl);
            out_ptr0 += 8;
            out_ptr1 += 8;
            out_ptr2 += 8;
            out_ptr3 += 8;
            out_ptr4 += 8;
            out_ptr5 += 8;
            out_ptr6 += 8;
            out_ptr7 += 8;
        }
        // m8n4
        for (; j + 3 < n; j += 4) {
            float *kernel_ptr = kernel_data;

            float *in_ptr0 = in_ptr;
            float *in_ptr1 = in_ptr0 + k;
            float *in_ptr2 = in_ptr1 + k;
            float *in_ptr3 = in_ptr2 + k;

            out_ptr1 = out_ptr0 + 1;
            out_ptr2 = out_ptr0 + 2;
            out_ptr3 = out_ptr0 + 3;

            vfloat32m2_t _acc0 = vlse32_v_f32m2(out_ptr0, ldc * sizeof(float), vl);
            vfloat32m2_t _acc1 = vlse32_v_f32m2(out_ptr1, ldc * sizeof(float), vl);
            vfloat32m2_t _acc2 = vlse32_v_f32m2(out_ptr2, ldc * sizeof(float), vl);
            vfloat32m2_t _acc3 = vlse32_v_f32m2(out_ptr3, ldc * sizeof(float), vl);  // init acc with bias_data

            for (int c = 0; c < k; c++) {
                vfloat32m2_t _kernel = vle32_v_f32m2(kernel_ptr, vl);
                _acc0 = vfmacc_vf_f32m2(_acc0, in_ptr0[c], _kernel, vl);
                _acc1 = vfmacc_vf_f32m2(_acc1, in_ptr1[c], _kernel, vl);
                _acc2 = vfmacc_vf_f32m2(_acc2, in_ptr2[c], _kernel, vl);
                _acc3 = vfmacc_vf_f32m2(_acc3, in_ptr3[c], _kernel, vl);
                kernel_ptr += 8;
            }
            vsse32_v_f32m2(out_ptr0, ldc * sizeof(float), _acc0, vl);
            vsse32_v_f32m2(out_ptr1, ldc * sizeof(float), _acc1, vl);
            vsse32_v_f32m2(out_ptr2, ldc * sizeof(float), _acc2, vl);
            vsse32_v_f32m2(out_ptr3, ldc * sizeof(float), _acc3, vl);
            out_ptr0 += 4;
            in_ptr += 4 * k;
        }
        // m8n2
        for (; j + 1 < n; j += 2) {
            float *kernel_ptr = kernel_data;

            float *in_ptr0 = in_ptr;
            float *in_ptr1 = in_ptr0 + k;

            out_ptr1 = out_ptr0 + 1;
            vfloat32m2_t _acc0 = vlse32_v_f32m2(out_ptr0, ldc * sizeof(float), vl);
            vfloat32m2_t _acc1 = vlse32_v_f32m2(out_ptr1, ldc * sizeof(float), vl);

            for (int c = 0; c < k; c++) {
                vfloat32m2_t _kernel = vle32_v_f32m2(kernel_ptr, vl);
                _acc0 = vfmacc_vf_f32m2(_acc0, in_ptr0[c], _kernel, vl);
                _acc1 = vfmacc_vf_f32m2(_acc1, in_ptr1[c], _kernel, vl);
                kernel_ptr += 8;
            }
            vsse32_v_f32m2(out_ptr0, ldc * sizeof(float), _acc0, vl);
            vsse32_v_f32m2(out_ptr1, ldc * sizeof(float), _acc1, vl);
            out_ptr0 += 2;
            in_ptr += 2 * k;
        }
        // m8n1
        for (; j < n; j++) {
            float *kernel_ptr = kernel_data;
            float *in_ptr0 = in_ptr;
            vfloat32m2_t _acc0 = vlse32_v_f32m2(out_ptr0, ldc * sizeof(float), vl);

            for (int c = 0; c < k; c++) {
                vfloat32m2_t _kernel = vle32_v_f32m2(kernel_ptr, vl);
                _acc0 = vfmacc_vf_f32m2(_acc0, in_ptr0[c], _kernel, vl);
                kernel_ptr += 8;
            }
            vsse32_v_f32m2(out_ptr0, ldc * sizeof(float), _acc0, vl);
        }
        kernel_data += 8 * k;
        output_data += 8 * ldc;
    }
    // m4
    for (; i + 3 < m; i += 4) {
        vl = vsetvl_e32m2(8);
        float *in_ptr = input_data;

        float *out_ptr0 = output_data;
        float *out_ptr1 = out_ptr0 + ldc;
        float *out_ptr2 = out_ptr1 + ldc;
        float *out_ptr3 = out_ptr2 + ldc;

        int j = 0;
        // m4n8 loop
        for (; j + 7 < n; j += 8) {
            float *kernel_ptr = kernel_data;
            vfloat32m2_t _acc0 = vle32_v_f32m2(out_ptr0, vl);
            vfloat32m2_t _acc1 = vle32_v_f32m2(out_ptr1, vl);
            vfloat32m2_t _acc2 = vle32_v_f32m2(out_ptr2, vl);
            vfloat32m2_t _acc3 = vle32_v_f32m2(out_ptr3, vl);   // init acc with bias_data

            for (int c = 0; c < k; c++) {
                vfloat32m2_t _input = vle32_v_f32m2(in_ptr, vl);
                float k0 = kernel_ptr[0];
                float k1 = kernel_ptr[1];
                float k2 = kernel_ptr[2];
                float k3 = kernel_ptr[3];

                _acc0 = vfmacc_vf_f32m2(_acc0, k0, _input, vl);
                _acc1 = vfmacc_vf_f32m2(_acc1, k1, _input, vl);
                _acc2 = vfmacc_vf_f32m2(_acc2, k2, _input, vl);
                _acc3 = vfmacc_vf_f32m2(_acc3, k3, _input, vl);

                kernel_ptr += 4;
                in_ptr += 8;
            }
            vse32_v_f32m2(out_ptr0, _acc0, vl);
            vse32_v_f32m2(out_ptr1, _acc1, vl);
            vse32_v_f32m2(out_ptr2, _acc2, vl);
            vse32_v_f32m2(out_ptr3, _acc3, vl);
            out_ptr0 += 8;
            out_ptr1 += 8;
            out_ptr2 += 8;
            out_ptr3 += 8;
        }
        vl = vsetvl_e32m1(4);
        // m4n4
        for (; j + 3 < n; j += 4) {
            float *kernel_ptr = kernel_data;

            float *in_ptr0 = in_ptr;
            float *in_ptr1 = in_ptr0 + k;
            float *in_ptr2 = in_ptr1 + k;
            float *in_ptr3 = in_ptr2 + k;

            out_ptr1 = out_ptr0 + 1;
            out_ptr2 = out_ptr0 + 2;
            out_ptr3 = out_ptr0 + 3;
            vfloat32m1_t _acc0 = vlse32_v_f32m1(out_ptr0, ldc * sizeof(float), vl);
            vfloat32m1_t _acc1 = vlse32_v_f32m1(out_ptr1, ldc * sizeof(float), vl);
            vfloat32m1_t _acc2 = vlse32_v_f32m1(out_ptr2, ldc * sizeof(float), vl);
            vfloat32m1_t _acc3 = vlse32_v_f32m1(out_ptr3, ldc * sizeof(float), vl);  // init acc with bias_data

            for (int c = 0; c < k; c++) {
                vfloat32m1_t _kernel = vle32_v_f32m1(kernel_ptr, vl);
                _acc0 = vfmacc_vf_f32m1(_acc0, in_ptr0[c], _kernel, vl);
                _acc1 = vfmacc_vf_f32m1(_acc1, in_ptr1[c], _kernel, vl);
                _acc2 = vfmacc_vf_f32m1(_acc2, in_ptr2[c], _kernel, vl);
                _acc3 = vfmacc_vf_f32m1(_acc3, in_ptr3[c], _kernel, vl);
                kernel_ptr += 4;
            }
            vsse32_v_f32m1(out_ptr0, ldc * sizeof(float), _acc0, vl);
            vsse32_v_f32m1(out_ptr1, ldc * sizeof(float), _acc1, vl);
            vsse32_v_f32m1(out_ptr2, ldc * sizeof(float), _acc2, vl);
            vsse32_v_f32m1(out_ptr3, ldc * sizeof(float), _acc3, vl);
            out_ptr0 += 4;
            in_ptr += 4 * k;
        }
        // m4n2
        for (; j + 1 < n; j += 2) {
            float *kernel_ptr = kernel_data;
            float *in_ptr0 = in_ptr;
            float *in_ptr1 = in_ptr0 + k;
            out_ptr1 = out_ptr0 + 1;
            vfloat32m1_t _acc0 = vlse32_v_f32m1(out_ptr0, ldc * sizeof(float), vl);
            vfloat32m1_t _acc1 = vlse32_v_f32m1(out_ptr1, ldc * sizeof(float), vl);

            for (int c = 0; c < k; c++) {
                vfloat32m1_t _kernel = vle32_v_f32m1(kernel_ptr, vl);
                _acc0 = vfmacc_vf_f32m1(_acc0, in_ptr0[c], _kernel, vl);
                _acc1 = vfmacc_vf_f32m1(_acc1, in_ptr1[c], _kernel, vl);
                kernel_ptr += 4;
            }
            vsse32_v_f32m1(out_ptr0, ldc * sizeof(float), _acc0, vl);
            vsse32_v_f32m1(out_ptr1, ldc * sizeof(float), _acc1, vl);
            out_ptr0 += 2;
            in_ptr += 2 * k;
        }
        // m4n1
        for (; j < n; j++) {
            float *kernel_ptr = kernel_data;
            float *in_ptr0 = in_ptr;
            vfloat32m1_t _acc0 = vlse32_v_f32m1(out_ptr0, ldc * sizeof(float), vl);

            for (int c = 0; c < k; c++) {
                vfloat32m1_t _kernel = vle32_v_f32m1(kernel_ptr, vl);
                _acc0 = vfmacc_vf_f32m1(_acc0, in_ptr0[c], _kernel, vl);
                kernel_ptr += 4;
            }
            vsse32_v_f32m1(out_ptr0, ldc * sizeof(float), _acc0, vl);
        }
        kernel_data += 4 * k;
        output_data += 4 * ldc;
    }

    // m2
    for (; i + 1 < m; i += 2) {
        vl = vsetvl_e32m2(8);
        float *in_ptr = input_data;
        float *out_ptr0 = output_data;
        float *out_ptr1 = out_ptr0 + ldc;

        int j = 0;
        // m2n8 loop
        for (; j + 7 < n; j += 8) {
            float *kernel_ptr = kernel_data;
            vfloat32m2_t _acc0 = vle32_v_f32m2(out_ptr0, vl);
            vfloat32m2_t _acc1 = vle32_v_f32m2(out_ptr1, vl);   // init acc with bias_data

            for (int c = 0; c < k; c++) {
                vfloat32m2_t _input = vle32_v_f32m2(in_ptr, vl);
                float k0 = kernel_ptr[0];
                float k1 = kernel_ptr[1];
                _acc0 = vfmacc_vf_f32m2(_acc0, k0, _input, vl);
                _acc1 = vfmacc_vf_f32m2(_acc1, k1, _input, vl);
                kernel_ptr += 2;
                in_ptr += 8;
            }
            vse32_v_f32m2(out_ptr0, _acc0, vl);
            vse32_v_f32m2(out_ptr1, _acc1, vl);
            out_ptr0 += 8;
            out_ptr1 += 8;
        }

        // TODO: rvv opt
        for (; j < n; j++) {
            float acc0 = out_ptr0[0];
            float acc1 = out_ptr1[1];
            for (int c = 0; c < k; c++) {
                acc0 += kernel_data[2 * c] * in_ptr[c];
                acc1 += kernel_data[2 * c + 1] * in_ptr[c];
            }
            *out_ptr0++ = acc0;
            *out_ptr1++ = acc1;
            in_ptr += k;
        }
        kernel_data += 2 * k;
        output_data += 2 * ldc;
    }

    // m1
    for (; i < m; i++) {
        vl = vsetvl_e32m2(8);
        float *in_ptr = input_data;
        float *out_ptr0 = output_data;

        int j = 0;
        // m1n8 loop
        for (; j + 7 < n; j += 8) {
            float *kernel_ptr = kernel_data;
            vfloat32m2_t _acc0 = vle32_v_f32m2(out_ptr0, vl);
            for (int c = 0; c < k; c++) {
                vfloat32m2_t _input = vle32_v_f32m2(in_ptr, vl);
                float k0 = kernel_ptr[0];
                _acc0 = vfmacc_vf_f32m2(_acc0, k0, _input, vl);
                kernel_ptr += 1;
                in_ptr += 8;
            }
            vse32_v_f32m2(out_ptr0, _acc0, vl);
            out_ptr0 += 8;
        }

        // TODO: rvv opt
        for (; j < n; j++) {
            float acc0 = out_ptr0[0];
            for (int c = 0; c < k; c++) {
                acc0 += kernel_data[c] * in_ptr[c];
            }
            *out_ptr0++ = acc0;
            in_ptr += k;
        }
    }
}
#endif

