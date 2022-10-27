/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */


#ifndef CONFIG_SEC_CRYPTO_RNG_SW
#ifdef CONFIG_CSI_V1

#include "sec_mcu.h"
#include "drv/trng.h"

/**
  \brief       Get data from the TRNG engine
*/
uint32_t sc_rng_get_multi_word(uint32_t *data, uint32_t num)
{
    trng_handle_t trng;

    trng = csi_trng_initialize(0, NULL);

    return csi_trng_get_data(trng, (uint8_t *)data, num * sizeof(uint32_t));
}

/**
  \brief       Get data from the TRNG engine
*/
uint32_t sc_rng_get_single_word(uint32_t *data)
{
    trng_handle_t trng;

    trng = csi_trng_initialize(0, NULL);

    return csi_trng_get_data(trng, (uint8_t *)data, sizeof(uint32_t));
}

#endif
#endif
