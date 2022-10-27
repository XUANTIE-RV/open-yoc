/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     tee_aes.c
 * @brief    aes
 * @version  V1.0
 * @date     21. May 2020
 * @model    sse
 ******************************************************************************/
#ifndef CONFIG_SEC_CRYPTO_RNG_SW
#include <stdint.h>
#include <sec_crypto_rng.h>
#include <sec_crypto_errcode.h>

#include "drv/tee.h"
#include "sec_crypto_common.h"

// Function documentation

typedef enum {
    STATUS_FIRST_ENTRY   = 0,
    STATUS_UNFIRST_ENTRY = 0,
} first_entry_status;
/**
  \brief       Get data from the TRNG engine
  \param[out]  data  Pointer to buffer with data get from TRNG
  \param[in]   num   Number of data items,uinit in uint32
  \return      error code
*/
uint32_t sc_rng_get_multi_word(uint32_t *data, uint32_t num)
{
    return csi_tee_rand_generate((uint8_t *)data, num * sizeof(uint32_t));
}

/**
  \brief       Get data from the TRNG engine
  \return      error code
*/
uint32_t sc_rng_get_single_word(uint32_t *data)
{
    return csi_tee_rand_generate((uint8_t *)data, sizeof(uint32_t));
}
#endif
