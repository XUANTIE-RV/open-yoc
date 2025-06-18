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

void naive_gemm_fp32(float *sc, const float *sa, const float *sb, int m, int k, int n, int ldc)
{
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            float cij = sc[i * n + j];
            for (int z = 0; z < k; z++) {
                cij += sa[i * k + z] * sb[z * n + j];
            }
            sc[i * n + j] = cij;
        }
    }
}


void naive_gemm_fp32_4x4(float *sc, const float *sa, const float *sb, int m, int k, int n, int ldc)
{
    for (int i = 0; i < m; i+=4) {
        for (int j = 0; j < n; j+=4) {
            float c00 = sc[(i+0) * n + (j+0)];
            float c01 = sc[(i+0) * n + (j+1)];
            float c02 = sc[(i+0) * n + (j+2)];
            float c03 = sc[(i+0) * n + (j+3)];
            float c10 = sc[(i+1) * n + (j+0)];
            float c11 = sc[(i+1) * n + (j+1)];
            float c12 = sc[(i+1) * n + (j+2)];
            float c13 = sc[(i+1) * n + (j+3)];
            float c20 = sc[(i+2) * n + (j+0)];
            float c21 = sc[(i+2) * n + (j+1)];
            float c22 = sc[(i+2) * n + (j+2)];
            float c23 = sc[(i+2) * n + (j+3)];
            float c30 = sc[(i+3) * n + (j+0)];
            float c31 = sc[(i+3) * n + (j+1)];
            float c32 = sc[(i+3) * n + (j+2)];
            float c33 = sc[(i+3) * n + (j+3)];
            for (int z = 0; z < k; z++) {
                c00 += sa[(i+0) * k + z] * sb[z * n + (j+0)];
                c01 += sa[(i+0) * k + z] * sb[z * n + (j+1)];
                c02 += sa[(i+0) * k + z] * sb[z * n + (j+2)];
                c03 += sa[(i+0) * k + z] * sb[z * n + (j+3)];
                c10 += sa[(i+1) * k + z] * sb[z * n + (j+0)];
                c11 += sa[(i+1) * k + z] * sb[z * n + (j+1)];
                c12 += sa[(i+1) * k + z] * sb[z * n + (j+2)];
                c13 += sa[(i+1) * k + z] * sb[z * n + (j+3)];
                c20 += sa[(i+2) * k + z] * sb[z * n + (j+0)];
                c21 += sa[(i+2) * k + z] * sb[z * n + (j+1)];
                c22 += sa[(i+2) * k + z] * sb[z * n + (j+2)];
                c23 += sa[(i+2) * k + z] * sb[z * n + (j+3)];
                c30 += sa[(i+3) * k + z] * sb[z * n + (j+0)];
                c31 += sa[(i+3) * k + z] * sb[z * n + (j+1)];
                c32 += sa[(i+3) * k + z] * sb[z * n + (j+2)];
                c33 += sa[(i+3) * k + z] * sb[z * n + (j+3)];
            }
            sc[(i+0) * n + (j+0)] = c00;
            sc[(i+0) * n + (j+1)] = c01;
            sc[(i+0) * n + (j+2)] = c02;
            sc[(i+0) * n + (j+3)] = c03;
            sc[(i+1) * n + (j+0)] = c10;
            sc[(i+1) * n + (j+1)] = c11;
            sc[(i+1) * n + (j+2)] = c12;
            sc[(i+1) * n + (j+3)] = c13;
            sc[(i+2) * n + (j+0)] = c20;
            sc[(i+2) * n + (j+1)] = c21;
            sc[(i+2) * n + (j+2)] = c22;
            sc[(i+2) * n + (j+3)] = c23;
            sc[(i+3) * n + (j+0)] = c30;
            sc[(i+3) * n + (j+1)] = c31;
            sc[(i+3) * n + (j+2)] = c32;
            sc[(i+3) * n + (j+3)] = c33;
        }
    }
}
#endif

