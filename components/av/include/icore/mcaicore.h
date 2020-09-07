/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __MCAICORE_H__
#define __MCAICORE_H__

#include <aos/types.h>
#include <aos/aos.h>
#include "mca/csky_mca.h"

__BEGIN_DECLS__

typedef struct mcaicore mcaicore_t;

/**
 * @brief  init the icore inter-core mca
 * @return 0/-1
 */
int mcaicore_init();

/**
 * @brief  alloc a inter-core mca
 * @param  [in] type : MCA_TYPE_XXX
 * @return NULL on error
 */
mcaicore_t* mcaicore_new(int type);

/**
 * Configures IIR filter with 32-bit input/ouput and 32-bit coefficients.
 *
 * Note:
 *   # coeff is 32-bit (Q1.7.24), with size of 5.
 *   # Input/output are 32-bit (Q1.31.0).
 * @return -1/0
 */
int mcaicore_iir_fxp32_coeff32_config(mcaicore_t *hdl, const fxp32_t *coeff);

/**
 * IIR filter with 32-bit input/output.
 *
 * Note:
 *   # input is 32-bit (Q1.31.0), output also 32-bit but its precision is
 *     determined by configuration.
 *   # input_size must be greater than order, and size of output is
 *     (input_size - 2).
 *   # yn1 and yn2 are the 1st and 2nd samples from the last output.
 * @return -1/0
 */
int mcaicore_iir_fxp32(mcaicore_t *hdl, const fxp32_t *input, size_t input_size, fxp32_t yn1, fxp32_t yn2,
                       fxp32_t *output);

/**
 * @brief  free the inter-core mca
 * @param  [in] hdl
 * @return 0/-1
 */
int mcaicore_free(mcaicore_t *hdl);

__END_DECLS__

#endif /* __MCAICORE_H__ */

