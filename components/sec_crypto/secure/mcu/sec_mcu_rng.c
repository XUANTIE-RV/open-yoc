/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */


#ifndef CONFIG_SEC_CRYPTO_RNG_SW
#ifdef CONFIG_CSI_V2

#include "sec_mcu.h"
#include "drv/rng.h"

/**
  \brief       Get data from the TRNG engine
*/
uint32_t sc_rng_get_multi_word(uint32_t *data, uint32_t num)
{
    uint32_t ret;

    ret = csi_rng_get_multi_word(data, num);
    if (ret) {
        return SC_DRV_FAILED;
    }

    return SC_OK;
}

/**
  \brief       Get data from the TRNG engine
*/
uint32_t sc_rng_get_single_word(uint32_t *data)
{
    uint32_t ret;
    ret = csi_rng_get_single_word(data);
    if (ret) {
        return SC_DRV_FAILED;
    }

    return SC_OK;
}

#endif
#endif
