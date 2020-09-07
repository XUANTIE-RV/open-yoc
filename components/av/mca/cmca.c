/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#include "mca/mca.h"
#include "mca/cmca.h"

#define TAG                  "cmca"

static mcax_t *_mca_iir;

static int _mca_iir_init()
{
    if (!_mca_iir) {
        _mca_iir = mcax_new(MCA_TYPE_IIR);
        CHECK_RET_TAG_WITH_RET(_mca_iir, -1);
    }

    return _mca_iir ? 0 : -1;
}

/**
 * Configures IIR filter with 32-bit input/ouput and 32-bit coefficients.
 *
 * Note:
 *   # coeff is 32-bit (Q1.7.24), with size of 5.
 *   # Input/output are 32-bit (Q1.31.0).
 * @return -1/0
 */
int cmca_iir_fxp32_coeff32_config(const fxp32_t *coeff)
{
    CHECK_PARAM(coeff, -1);
    _mca_iir_init();

    return mcax_iir_fxp32_coeff32_config(_mca_iir, coeff);
}

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
int cmca_iir_fxp32(const fxp32_t *input, size_t input_size, fxp32_t yn1, fxp32_t yn2,
                   fxp32_t *output)
{
    CHECK_PARAM(input && input_size && output, -1);
    _mca_iir_init();

    return mcax_iir_fxp32(_mca_iir, input, input_size, yn1, yn2, output);
}


