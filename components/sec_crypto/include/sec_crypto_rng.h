/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */
/******************************************************************************
 * @file     seccrypt_rng.h
 * @brief    Header File for RNG
 * @version  V1.0
 * @date     20. Jul 2020
 * @model    rng
 ******************************************************************************/
#ifndef _SC_RNG_H_
#define _SC_RNG_H_


#include <stdint.h>
#include <sec_crypto_errcode.h>


#ifdef __cplusplus
extern "C" {
#endif

/**
  \brief       Get data from the TRNG engine
  \param[out]  data  Pointer to buffer with data get from TRNG
  \param[in]   num   Number of data items,uinit in uint32
  \return      error code
*/
uint32_t sc_rng_get_multi_word(uint32_t *data, uint32_t num);

/**
  \brief       Get data from the TRNG engine
  \return      error code
*/
uint32_t sc_rng_get_single_word(uint32_t *data);

#ifdef __cplusplus
}
#endif

#endif /* _DRV_TRNG_H_ */
