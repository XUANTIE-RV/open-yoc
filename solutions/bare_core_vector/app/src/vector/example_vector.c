/*
 * Copyright (C) 2019-2024 Alibaba Group Holding Limited
 */

#ifdef __riscv_vector
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <float.h>
#include <time.h>
#include <riscv_vector.h>
#include <drv/tick.h>

#define min(i, j) ((i) < (j) ? (i): (j))
#define max(i, j) ((i) > (j) ? (i): (j))

void naive_gemm_fp32(float *sc, const float *sa, const float *sb, int m, int k, int n, int ldc);
void naive_gemm_fp32_4x4(float *sc, const float *sa, const float *sb, int m, int k, int n, int ldc);

void shl_c908_reorder_input_z8_fp32(float *b, float *sb, int k, int n, int ldx);
void shl_c908_reorder_kernel_n8_fp32(float *src, float *dst, int m, int k, int ldx);
void shl_c908_gemm_8x8_fp32(float *sc, const float *sa, const float *sb, int m, int k, int n, int ldc);

void shl_c908_reorder_input_z16_fp32_v256(float *b, float *sb, int k, int n, int ldx);
void shl_c908_reorder_kernel_n8_fp32_v256(float *src, float *dst, int m, int k, int ldx);
void shl_c908_gemm_8x16_fp32_v256(float *sc, const float *sa, const float *sb, int m, int k, int n, int ldc);

static uint32_t _get_current_ms(void)
{
    return csi_tick_get_ms();
}

void* fastmalloc(int size)
{
    void* ptr = 0;
    // int iRet = posix_memalign(&ptr, 64, size);
    ptr = malloc(size);
    if (!ptr) {
        printf("oom size=%d\n", size);
        while(1);
    }
    //printf("ptr=%p\n", ptr);
    return ptr;
}

void random_matrix(int m, int n, float *a, int lda)
{
    double drand48();

    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            a[i * lda + j] = (float)drand48();
        }
    }
}

float compare_matrices(int m, int n, float *a, int lda, float *b, int ldb)
{
    float max_diff = 0.0, diff;
    for (int i = 0; i < m; i++ ) {
        for (int j = 0; j < n; j++ ) {
            diff = abs(a[i * lda + j] - b[i * ldb + j]);

            max_diff = max(diff, max_diff);

            if(max_diff > FLT_EPSILON || max_diff < -FLT_EPSILON) {
                printf("\n error: i %d  j %d diff %f", i, j, max_diff);
            }
        }
    }
    return max_diff;
}

int csrr_vlenb()
{
    int a = 0;
    __asm volatile("csrr %0, vlenb" : "=r"(a) : : "memory");
    return a;
}

// matrix A: m x k
// matrix B: k x n
// matrix C: m x n
int gemm_perf(int m, int k, int n, int loop_cnt)
{
    int rc = 0;
    uint32_t start_time, stop_time, diff_ms0, diff_ms1;
    float *sc_ptr = (float *)fastmalloc(m * n * sizeof(float));
    float *ref = (float *)fastmalloc(m * n * sizeof(float));

    float *sa_ptr = (float *)fastmalloc(m * k * sizeof(float));
    float *sa_data = (float *)fastmalloc(m * k * sizeof(float));
    float *sb_ptr = (float *)fastmalloc((k * n) * sizeof(float));
    float *sb_data = (float *)fastmalloc(k * n * sizeof(float));
    float *bias_ptr = (float *)fastmalloc(m * sizeof(float));

    //double cals = (m * n * k * 2) * 0.001 * 0.001;  // MFOPS
    random_matrix(m, k, sa_data, k);
    random_matrix(k, n, sb_data, n);
    random_matrix(m, n, sc_ptr, n);

    memset(bias_ptr, 0, m * sizeof(float));
    memset(ref, 0, m * n * sizeof(float));
    memset(sc_ptr, 0, m * n * sizeof(float));

    // naive_gemm_fp32(ref, sa_data, sb_data, m, k, n, n);
    printf("=====>>native test start.\r\n");
    start_time = _get_current_ms();
    for (int i = 0; i < loop_cnt; i++) {
        memset(ref, 0, m * n * sizeof(float));
        naive_gemm_fp32_4x4(ref, sa_data, sb_data, m, k, n, n);
    }
    printf("=====>>native test end.\r\n");
    stop_time = _get_current_ms();
    diff_ms0 = stop_time - start_time;

    printf("=====>>vector test start.\r\n");
    const int vlen = csrr_vlenb() * 8;
    printf("===vlen=%d\n", vlen);

    //uint64_t time_best = 0;
    start_time = _get_current_ms();
    for (int i = 0; i < loop_cnt; i++) {
        // 每次循环执行要清零结果矩阵
        memset(sc_ptr, 0, m * n * sizeof(float));

        if (128 == vlen) {
            shl_c908_reorder_kernel_n8_fp32(sa_data, sa_ptr, m, k, k);
            shl_c908_reorder_input_z8_fp32(sb_data, sb_ptr, k, n, n);
            shl_c908_gemm_8x8_fp32(sc_ptr, sa_ptr, sb_ptr, m, k, n, n);
        } else if (256 == vlen) {
            shl_c908_reorder_kernel_n8_fp32_v256(sa_data, sa_ptr, m, k, k);
            shl_c908_reorder_input_z16_fp32_v256(sb_data, sb_ptr, k, n, n);
            shl_c908_gemm_8x16_fp32_v256(sc_ptr, sa_ptr, sb_ptr, m, k, n, n);
        } else {
            printf("Unsupport vlen\n");
        }

    }
    printf("=====>>vector test end.\r\n");
    stop_time = _get_current_ms();
    diff_ms1 = stop_time - start_time;

    float diff = compare_matrices(m, n, ref, n, sc_ptr, n);
    if(diff > FLT_EPSILON || diff < -FLT_EPSILON) {
        rc = -1;
    }
    printf("native time = %ums, vector time = %ums. diff = %ums\n", diff_ms0, diff_ms1, diff_ms0 - diff_ms1);

    free(sc_ptr);
    free(sa_ptr);
    free(sa_data);
    free(sb_ptr);
    free(sb_data);
    free(bias_ptr);
    free(ref);

    return rc;
}

// ./c_runtime 160 160 160 100
// m % 8 == 0  &&  n % 12 == 0
static int vector_main(int argc, char **argv)
{
    int rc;
    int m = 64;
    int k = 64;
    int n = 64;
    int cnt = 10;
    uint32_t start_time, stop_time, diff_ms;

    if (argc == 5) {
        m = atoi(argv[1]);
        k = atoi(argv[2]);
        n = atoi(argv[3]);
        cnt = atoi(argv[4]);
    }
    printf("[fp32] gemm start\n");
    start_time = _get_current_ms();

    rc = gemm_perf(m, k, n, cnt);

    stop_time = _get_current_ms();
    diff_ms = stop_time - start_time;
    printf("[fp32] gemm end, total diff time = %ums\n", diff_ms);
    return rc;
}

static void vector_thread(void)
{
    vector_main(0, NULL);
}

int example_core_vector()
{
    int rc;

    rc = vector_main(0, NULL);
    if (rc == 0)
        printf("vector runs successfully!\n");
    else
        printf("vector runs fail!\n");

    return rc;
}
#endif


