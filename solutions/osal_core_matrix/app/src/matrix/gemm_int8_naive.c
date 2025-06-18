#include <stdio.h>
#include <stdlib.h>

void naive_gemm_int8(int32_t *sc, const int8_t *sa, const int8_t *sb, int m, int k, int n, int ldc)
{
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            int32_t cij = sc[i * n + j];
            for (int z = 0; z < k; z++) {
                cij += sa[i * k + z] * sb[z * n + j];
            }
            sc[i * n + j] = cij;
        }
    }
}

void naive_gemm_int8_4x4(int32_t *sc, const int8_t *sa, const int8_t *sb, int m, int k, int n, int ldc)
{
    for (int i = 0; i < m; i+=4) {
        for (int j = 0; j < n; j+=4) {
            int32_t c00 = sc[(i+0) * n + (j+0)];
            int32_t c01 = sc[(i+0) * n + (j+1)];
            int32_t c02 = sc[(i+0) * n + (j+2)];
            int32_t c03 = sc[(i+0) * n + (j+3)];
            int32_t c10 = sc[(i+1) * n + (j+0)];
            int32_t c11 = sc[(i+1) * n + (j+1)];
            int32_t c12 = sc[(i+1) * n + (j+2)];
            int32_t c13 = sc[(i+1) * n + (j+3)];
            int32_t c20 = sc[(i+2) * n + (j+0)];
            int32_t c21 = sc[(i+2) * n + (j+1)];
            int32_t c22 = sc[(i+2) * n + (j+2)];
            int32_t c23 = sc[(i+2) * n + (j+3)];
            int32_t c30 = sc[(i+3) * n + (j+0)];
            int32_t c31 = sc[(i+3) * n + (j+1)];
            int32_t c32 = sc[(i+3) * n + (j+2)];
            int32_t c33 = sc[(i+3) * n + (j+3)];
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
