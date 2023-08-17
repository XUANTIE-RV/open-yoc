/*
 * Copyright (C) 2017-2023 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     se_rng.c
 * @brief    rng
 * @version  V1.0
 * @date     26. May 2023
 * @model    sse
 ******************************************************************************/
#ifndef CONFIG_SEC_CRYPTO_RNG_SW
#include <stdint.h>
#include <sec_crypto_rng.h>
#include <sec_crypto_errcode.h>
#include <se_rng.h>

/**
  \brief       Get data from the TRNG engine
  \param[out]  data  Pointer to buffer with data get from TRNG
  \param[in]   num   Number of data items,uinit in uint32
  \return      error code
*/
uint32_t sc_rng_get_multi_word(uint32_t *data, uint32_t num)
{
	int ret;

	ret = se_rng_get_bytes((uint8_t *)data, num * 4);
	if (!ret)
		return SC_OK;
	else
		return SC_RNG_FAILED;
}

/**
  \brief       Get data from the TRNG engine
  \return      error code
*/
uint32_t sc_rng_get_single_word(uint32_t *data)
{
	int ret;

	ret = se_rng_get_bytes((uint8_t *)data, 4);
	if (!ret)
		return SC_OK;
	else
		return SC_RNG_FAILED;
}
#endif
