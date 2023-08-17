/*
 * Copyright (C) 2017-2023 Alibaba Group Holding Limited
 */
/******************************************************************************
 * @file     se_rng.h
 * @brief    Header File for RNG
 * @version  V1.0
 * @date     22. May 2023
 * @model    rng
 ******************************************************************************/
#ifndef _SE_RNG_H_
#define _SE_RNG_H_


#include <stdint.h>
#include "se_device.h"


#ifdef __cplusplus
extern "C" {
#endif

/**
  \brief       Get data from the TRNG engine
  \param[out]  data  Pointer to buffer with data get from TRNG
  \param[in]   blen  Byte length of data items,uinit in uint32
  \return      error code
*/
int se_rng_get_bytes(uint8_t *data, uint32_t blen);

#ifdef __cplusplus
}
#endif

#endif /* _DRV_TRNG_H_ */
