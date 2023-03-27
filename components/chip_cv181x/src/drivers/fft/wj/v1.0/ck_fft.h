#ifndef __CK_FFT_H__
#define __CK_FFT_H__

#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include "soc.h"
////////////////////////////////////////////////////////////////////////////////
// MCA FFT MODULE

// Base address of the MCA FFT module

typedef enum {
	// 512-point FFT
	CSKY_MCA_FFT_LEN_512 = 0x1,
	// 256-point FFT
	CSKY_MCA_FFT_LEN_256 = 0x2,
	// 128-point FFT
	CSKY_MCA_FFT_LEN_128 = 0x4,
	// 64-point FFT
	CSKY_MCA_FFT_LEN_64 = 0x8,
	// 32-point FFT
	CSKY_MCA_FFT_LEN_32 = 0x10,
	// 16-point FFT
	CSKY_MCA_FFT_LEN_16 = 0x20,
} csky_mca_fft_len_t;

/* 8-bit fixed-point numeric type in user-defined format */
typedef int8_t fxp8_t;
/* 16-bit fixed-point numeric type in user-defined format */
typedef int16_t fxp16_t;
/* 24-bit fixed-point numeric type in user-defined format */
typedef int32_t fxp24_t;
/* 32-bit fixed-point numeric type in user-defined format */
typedef int32_t fxp32_t;
/* 64-bit fixed-point numeric type in user-defined format */
typedef int64_t fxp64_t;

/* 8-bit fixed-point numeric type in 1.0.7 format */
typedef fxp8_t q7_t;
/* 16-bit fixed-point numeric type in 1.0.15 format */
typedef fxp16_t q15_t;
/* 32-bit fixed-point numeric type in 1.15.16 format */
typedef fxp32_t q16_t;


// FFT function selection
typedef enum {
    DRV_FFT_FUNC_SEL_REAL_FFT = 0x1,
    DRV_FFT_FUNC_SEL_COMPLEX_FFT = 0x2,
    DRV_FFT_FUNC_SEL_REAL_IFFT = 0x4,
    DRV_FFT_FUNC_SEL_COMPLEX_IFFT = 0x8,
    DRV_FFT_FUNC_SEL_POWER_SPECTRUM = 0x10,
} drv_fft_func_sel_t;

// FFT mode selection
typedef enum {
    DRV_FFT_MODE_SEL_512 = 0x1,
    DRV_FFT_MODE_SEL_256 = 0x2,
    DRV_FFT_MODE_SEL_128 = 0x4,
    DRV_FFT_MODE_SEL_64 = 0x8,
    DRV_FFT_MODE_SEL_32 = 0x10,
    DRV_FFT_MODE_SEL_16 = 0x20,
} drv_fft_mode_sel_t;

// FFT Register: start
#define DRV_FFT_START_POS  0U
#define DRV_FFT_START_MASK (1U << DRV_FFT_START_POS)

// FFT Register: mode_sel
#define DRV_FFT_FUNC_SEL_POS 0U
#define DRV_FFT_MODE_SEL_POS 8U

// Registers of the FFT module
typedef struct {
    volatile uint32_t start; // offset 0x00
    volatile uint32_t in_addr; // offset 0x04
    volatile uint32_t out_addr; // offset 0x08
    volatile uint32_t mode_sel; // offset 0x0C
    volatile uint32_t in_num; // offset 0x10
} drv_fft_regs_t;

// Pointer to the FFT registers
#define drv_fft_regs ((drv_fft_regs_t *)WJ_FFT_BASE)

// Returns FFT length from mode selection.
static inline uint32_t get_fft_len_from_mode(drv_fft_mode_sel_t mode) {
    return 512 / mode;
}

// Assertion: FFT mode selection must be valid.
#define DRV_FFT_ASSERT_FFT_MODE_SEL_VALID(mode) assert(( \
    mode == DRV_FFT_MODE_SEL_512 || \
    mode == DRV_FFT_MODE_SEL_256 || \
    mode == DRV_FFT_MODE_SEL_128 || \
    mode == DRV_FFT_MODE_SEL_64 || \
    mode == DRV_FFT_MODE_SEL_32 || \
    mode == DRV_FFT_MODE_SEL_16) && \
    "FFT length selection is out of range.")

// Assertion: FFT input length must be in range.
#define DRV_FFT_ASSERT_INPUT_LEN_IN_RANGE(len, mode) assert( \
    (len) > 0 && (len) <= get_fft_len_from_mode(mode) && \
    "FFT input length is out of range.")

// Waits the asynchronous FFT operation to be completed.
#ifdef CSKY_MCA_SIMULATION
void drv_fft_await();
#else
static inline
void drv_fft_await() {
    while (drv_fft_regs->start & DRV_FFT_START_MASK) {
        ;
    }
}
#endif

#endif
