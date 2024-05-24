#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <float.h>
#include <math.h>
#include <time.h>
#include <riscv_vector.h>
#include <drv/tick.h>

#define min(i, j) ((i) < (j) ? (i): (j))
#define max(i, j) ((i) > (j) ? (i): (j))

void naive_gemm_int8(int32_t *sc, const int8_t *sa, const int8_t *sb, int m, int k, int n, int ldc);
void naive_gemm_int8_4x4(int32_t *sc, const int8_t *sa, const int8_t *sb, int m, int k, int n, int ldc);

int csrr_xrlenb();
void shl_rvv_reorder_input_int8(int8_t *src, int8_t *dst, int m, int k, int ldc);
void shl_rvv_reorder_kernel_int8(int8_t *src, int8_t *dst, int k, int n, int ldx);
void shl_rvm_gemm_int8_to_int32(int32_t *dst, const int8_t *sa, const int8_t *sb, int m, int k, int n, int ldc);

#define BILLION 1000000000

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


void random_matrix(int m, int n, int8_t *a, int lda)
{
    srand(_get_current_ms());
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            a[i * lda + j] = (int8_t)(rand() % 256 - 128);
        }
    }
}

int32_t compare_matrices(int m, int n, int32_t *ref, int lda, int32_t *out, int ldb)
{
#define ERROR 0
    int32_t max_diff = 0, diff;
    for (int i = 0; i < m; i++ ) {
        for (int j = 0; j < n; j++ ) {
            int32_t f_ref = ref[i * lda + j];
            int32_t f_out = out[i * ldb + j];
            diff = abs(f_ref - f_out);
            max_diff = max(diff, max_diff);

            if(diff > ERROR || diff < -ERROR) {
                printf("\n error: [%d][%d] ref=%d out=%d diff=%d", i, j, f_ref, f_out, diff);
            }
        }
    }
    return max_diff;
}

// matrix A: m x k
// matrix B: k x n
// matrix C: m x n
int gemm_perf(int m, int k, int n, int loop_cnt)
{
    int rc = 0;
    int32_t k_align = ((k - 1) & -csrr_xrlenb()) + csrr_xrlenb();
    int32_t n_align = ((n - 1) & -(csrr_xrlenb() / 4)) + csrr_xrlenb() / 4;

    int32_t *sc_ptr = (int32_t *)fastmalloc(m * n * sizeof(int32_t));
    int32_t *ref = (int32_t *)fastmalloc(m * n * sizeof(int32_t));

    int8_t *sa_ptr = (int8_t *)fastmalloc(m * k_align * sizeof(int8_t));
    int8_t *sa_data = (int8_t *)fastmalloc(m * k * sizeof(int8_t));
    int8_t *sb_ptr = (int8_t *)fastmalloc((k_align * n_align) * sizeof(int8_t));
    int8_t *sb_data = (int8_t *)fastmalloc(k * n * sizeof(int8_t));
    int32_t *bias_ptr = (int32_t *)fastmalloc(m * sizeof(int32_t));

    double cals = (m * n * k * 2) * 0.001 * 0.001;  // MFOPS

    random_matrix(m, k, sa_data, k);
    random_matrix(k, n, sb_data, n);
    // random_matrix(m, n, sc_ptr, n);

    memset(bias_ptr, 0, m * sizeof(int32_t));
    memset(ref, 0, m * n * sizeof(int32_t));
    memset(sc_ptr, 0, m * n * sizeof(int32_t));
    memset(sa_ptr, 0, m * k_align * sizeof(int8_t));
    memset(sb_ptr, 0, (k_align * n_align) * sizeof(int8_t));

    uint64_t time_best = 0;
    for (int i = 0; i < loop_cnt; i++) {
        // 每次循环执行要清零结果矩阵
        // memset(sc_ptr, 0, m * n * sizeof(int32_t));

        naive_gemm_int8(ref, sa_data, sb_data, m, k, n, n);

        uint64_t time0 = _get_current_ms();

        shl_rvv_reorder_kernel_int8(sb_data, sb_ptr, k, n, n);

        int8_t *sa_ptr_real = sa_ptr;
        if (k_align != k) {
            shl_rvv_reorder_input_int8(sa_data, sa_ptr, m, k, k);
        } else {
            sa_ptr_real = sa_data;
        }

        shl_rvm_gemm_int8_to_int32(sc_ptr, sb_ptr, sa_ptr_real, m, k_align, n, n);

        uint64_t time1 = _get_current_ms();
        uint64_t time_tmp = time1 - time0;
        if (i == 0) {
            time_best = time_tmp;
        } else {
            time_best = min(time_best, time_tmp);
        }
    }

    int32_t diff = compare_matrices(m, n, ref, n, sc_ptr, n);
    if(diff > 0 || diff < 0) {
        rc = -1;
        printf("compare error\n");
        goto error;
    }

    float t0 = ((float)time_best);///1000000.0f;
    printf("[m * k * n = %d * %d * %d] best execution time: %.3fms, %.2f GFLOPS\n", m, k, n, t0, cals/t0);

error:
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
int matrix_main(int argc, char **argv)
{
    int rc = 0;
    int m = 160;
    int k = 160;
    int n = 160;
    int cnt = 3;

    if (argc == 5) {
        m = atoi(argv[1]);
        k = atoi(argv[2]);
        n = atoi(argv[3]);
        cnt = atoi(argv[4]);
    }
    printf("[int8] gemm start\n");

    //for (int i = 48; i <= 960; i += 48) {
    for (int i = 48; i <= 240; i += 48) {
        m = k = n = i;
        rc = gemm_perf(m, k, n, cnt);
        if (rc != 0)
            return rc;
    }

    printf("[int8] gemm end\n");
    return rc;
}

