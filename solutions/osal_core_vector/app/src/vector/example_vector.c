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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <float.h>
#include <time.h>
#include <riscv_vector.h>
#include <aos/aos.h>

#define VECTOR_THREAD_NUM (5)
static volatile int g_vector_cnt;
static aos_task_t g_vector_handles[VECTOR_THREAD_NUM];
static volatile int g_vector_ret[VECTOR_THREAD_NUM];

#define min(i, j) ((i) < (j) ? (i): (j))
#define max(i, j) ((i) > (j) ? (i): (j))

extern void naive_gemm_fp32(float *sc, const float *sa, const float *sb, int m, int k, int n, int ldc);
extern void naive_gemm_fp32_4x4(float *sc, const float *sa, const float *sb, int m, int k, int n, int ldc);

extern void shl_c908_reorder_input_z8_fp32(float *b, float *sb, int k, int n, int ldx);
extern void shl_c908_reorder_kernel_n8_fp32(float *src, float *dst, int m, int k, int ldx);
extern void shl_c908_gemm_8x8_fp32(float *sc, const float *sa, const float *sb, int m, int k, int n, int ldc);

extern void shl_c908_reorder_input_z16_fp32_v256(float *b, float *sb, int k, int n, int ldx);
extern void shl_c908_reorder_kernel_n8_fp32_v256(float *src, float *dst, int m, int k, int ldx);
extern void shl_c908_gemm_8x16_fp32_v256(float *sc, const float *sa, const float *sb, int m, int k, int n, int ldc);

extern void reordered_A(float *src, float *dst, int M, int K);
extern void reordered_B(float *src, float *dst, int K, int N);
extern void vdot_compute(float *dst, float *sa, float *sb, int M, int K, int N);

static uint32_t _get_current_ms(void)
{
    return (uint32_t)aos_now_ms();
}

#if CONFIG_USE_FASTMEM
#include <umm_heap.h>
struct mm_heap_s g_fast_mmheap;
void fast_mm_init(void)
{
    mm_initialize(&g_fast_mmheap, (void *)CONFIG_FASTMEM_ADDR, (size_t)CONFIG_FASTMEM_SIZE);
}
#endif

void* fastmalloc(size_t size)
{
#if CONFIG_USE_FASTMEM
    void *ptr = mm_malloc(&g_fast_mmheap, size, __builtin_return_address(0));
    if (!ptr) {
        printf("oom size=%d\n", (int)size);
        while(1);
    }
    memset(ptr, 0, size);
    return ptr;
#else
    void* ptr = 0;
    // int iRet = posix_memalign(&ptr, 64, size);
    ptr = malloc(size);
    if (!ptr) {
        printf("oom size=%d\n", (int)size);
        while(1);
    }
    //printf("ptr=%p\n", ptr);
    return ptr;
#endif
}

void fastfree(void *ptr)
{
#if CONFIG_USE_FASTMEM
    mm_free(&g_fast_mmheap, ptr, __builtin_return_address(0));
#else
    free(ptr);
#endif
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
        aos_msleep(10);
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

        /* for vector context save/restore test */
        aos_msleep(20);
        reordered_A( sa_data, sa_ptr, m, k);
        aos_msleep(10);
        reordered_B( sb_data, sb_ptr, k, n);
        aos_msleep(20);
        vdot_compute(sc_ptr, sa_ptr, sb_ptr, m, k, n);
    }
    printf("=====>>vector test end.\r\n");
    stop_time = _get_current_ms();
    diff_ms1 = stop_time - start_time;

    float diff = compare_matrices(m, n, ref, n, sc_ptr, n);
    if(diff > FLT_EPSILON || diff < -FLT_EPSILON) {
        rc = -1;
    }
    printf("native time = %ums, vector time = %ums. diff = %ums\n", diff_ms0, diff_ms1, diff_ms0 - diff_ms1);

    fastfree(sc_ptr);
    fastfree(sa_ptr);
    fastfree(sa_data);
    fastfree(sb_ptr);
    fastfree(sb_data);
    fastfree(bias_ptr);
    fastfree(ref);

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

static void vector_thread(void *arg)
{
    g_vector_ret[g_vector_cnt] = vector_main(0, NULL);
    g_vector_cnt++;
}

int example_core_vector()
{
    int rc;

#if CONFIG_USE_FASTMEM
    fast_mm_init();
#endif

    for (int i = 0; i < VECTOR_THREAD_NUM; i++) {
        rc = aos_task_new_ext(&g_vector_handles[i], "app_task", vector_thread,
                              NULL, 16*1024, AOS_DEFAULT_APP_PRI);
        if (rc) {
            printf("may be oom! rc = %d\n", rc);
            goto error;
        }
    }

    while (g_vector_cnt < VECTOR_THREAD_NUM) {
        aos_msleep(50);
    }
    for (int i = 0; i < VECTOR_THREAD_NUM; i++) {
        if (g_vector_ret[i])
            goto error;
    }
    printf("vector runs successfully!\n");
    return 0;

error:
    printf("vector runs fail!\n");
    return -1;
}
#endif


