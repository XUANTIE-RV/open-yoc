#include <stdlib.h>
#include <stdio.h>
#include <string.h>
// #include <thead_matrix.h>
#include <riscv_vector.h>
#include <thead_matrix.h>

int csrr_xrlenb()
{
    int a = 0;
    asm volatile("csrr %0, xrlenb" : "=r"(a) : : "memory");
    return a;
}

static void transpose_matrix(int8_t *dst, const int8_t *src, int rows, int cols)
{
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            dst[j * rows + i] = src[i * cols + j];
        }
    }
}

static void align_input_int8(int8_t *dst, const int8_t *src, int m, int k, int alinged_k)
{
    for (int i = 0; i < m; i++) {
        int size = k;
        while (size > 0) {
            int vl = vsetvl_e16m1(size);
            vint8m1_t _input = vle8_v_i8m1(src, vl);
            src += vl;
            vse8_v_i8m1(dst, _input, vl);
            dst += vl;
            size -= vl;
        }
        dst += alinged_k - k;
    }
}

static void reorder_kernel_int8(int8_t *dst, int8_t *src, int k, int n)
{
    const int col = csrr_xrlenb();
    const int row = col / 4;
    //int K = k;
    int oc = 0;
    for (; oc + 2 * row <= n; oc += 2 * row) {
        int8_t *src_m = src + oc * k;
        int j = 0;
        for (; j + col <= k; j += col) {
            int8_t *src_n = src_m + j;
            for (int i = 0; i < 2 * row; i++) {
                int8_t *src_i = src_n + i * k;
                memcpy(dst, src_i, col * sizeof(int8_t));
                dst += col;
            }
        }
        // k_tail
        if (j < k) {
            int8_t *src_n = src_m + j;
            for (int i = 0; i < 2 * row; i++) {
                int8_t *src_i = src_n + i * k;
                memcpy(dst, src_i, (k - j) * sizeof(int8_t));
                dst += col;
            }
        }
    }
    for (; oc + row <= n; oc += row) {
        int8_t *src_m = src + oc * k;
        int j = 0;
        for (; j + col <= k; j += col) {
            int8_t *src_n = src_m + j;
            for (int i = 0; i < row; i++) {
                int8_t *src_i = src_n + i * k;
                memcpy(dst, src_i, col * sizeof(int8_t));
                dst += col;
            }
        }
        if (j < k) {
            int8_t *src_n = src_m + j;
            for (int i = 0; i < row; i++) {
                int8_t *src_i = src_n + i * k;
                memcpy(dst, src_i, (k - j) * sizeof(int8_t));
                dst += col;
            }
        }
    }
    // oc_tail
    if (oc < n) {
        int8_t *src_m = src + oc * k;
        int j = 0;
        for (; j + col <= k; j += col) {
            int8_t *src_n = src_m + j;
            for (int i = 0; i < (n - oc); i++) {
                int8_t *src_i = src_n + i * k;
                memcpy(dst, src_i, col * sizeof(int8_t));
                dst += col;
            }
            dst += (oc + row - n) * col;  // padding
        }
        if (j < k) {
            int8_t *src_n = src_m + j;
            for (int i = 0; i < (n - oc); i++) {
                int8_t *src_i = src_n + i * k;
                memcpy(dst, src_i, (k - j) * sizeof(int8_t));
                dst += col;
            }
        }
    }
}

// src bytes: m * k * sizeof(int8_t)
// dst bytes: m * k_align * sizeof(int8_t)
void shl_rvv_reorder_input_int8(int8_t *src, int8_t *dst, int m, int k, int ldc)
{
    int32_t k_align = ((k - 1) & -csrr_xrlenb()) + csrr_xrlenb();
    align_input_int8(dst, src, m, k, k_align);
}

// src bytes: k * n * sizeof(int8_t)
// dst bytes: k_align * n_align * sizeof(int8_t)
// k_align = ((k - 1) & -csrr_xrlenb()) + csrr_xrlenb();
// n_align = ((n - 1) & -(csrr_xrlenb() / 4)) + csrr_xrlenb() / 4;
void shl_rvv_reorder_kernel_int8(int8_t *src, int8_t *dst, int k, int n, int ldx)
{
    int8_t *trans_buff = (int8_t *)malloc(k * n * sizeof(int8_t));
    transpose_matrix(trans_buff, src, k, n);
    reorder_kernel_int8(dst, trans_buff, k, n);
    free(trans_buff);
}

void gemm_int8_to_int32_2rowxn_matrix(const int32_t *output, const int8_t *kernel, const int8_t *input, const int32_t *bias, int m, int k, int n);
void gemm_int8_to_int32_rowxn_matrix(const int32_t *output, const int8_t *kernel, const int8_t *input, const int32_t *bias, int m, int k, int n);
void gemm_int8_to_int32_row_tailxn_matrix(const int32_t *output, const int8_t *kernel, const int8_t *input, const int32_t *bias, int m, int k, int n);

void shl_rvm_gemm_int8_to_int32(int32_t *dst, const int8_t *sa, const int8_t *sb, int m, int k, int n, int ldc)
{
    int mrows = csrr_xrlenb() / 4;
    int m2rows = mrows * 2;
    int i = 0;

    for (; i + m2rows - 1 < m; i += m2rows) {
        const int8_t *b_ptr = sb + i * k;
        int32_t *c_ptr = dst + i * n;
        gemm_int8_to_int32_2rowxn_matrix(c_ptr, sa, b_ptr, NULL, mrows, k, n);
    }
    for (; i + mrows - 1 < m; i += mrows) {
        const int8_t *b_ptr = sb + i * k;
        int32_t *c_ptr = dst + i * n;
        gemm_int8_to_int32_rowxn_matrix(c_ptr, sa, b_ptr, NULL, mrows, k, n);
    }
    if (i < m) {
        const int8_t *b_ptr = sb + i * k;
        int32_t *c_ptr = dst + i * n;
        gemm_int8_to_int32_row_tailxn_matrix(c_ptr, sa, b_ptr, NULL, m - i, k, n);
    }
}
