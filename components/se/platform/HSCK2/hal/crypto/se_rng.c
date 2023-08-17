/*
 * Copyright (C) 2017-2023 Alibaba Group Holding Limited
 */
#include "se_rng.h"
#include "cmd.h"
#include <se_dev_internal.h>

/**
  \brief       Get data from the TRNG engine
  \param[in]   se_dev  Pointer to se device handle
  \param[out]  data  Pointer to buffer with data get from TRNG
  \param[in]   blen  Byte length of data items,uinit in uint32
  \return      error code
*/
int se_rng_get_bytes(uint8_t *data, uint32_t blen)
{
    uint32_t ret;
    CHECK_PARAM(data, -1);

	se_dev_lock();
    ret = hs_rng_get_bytes(blen, data);
	se_dev_unlock();
    if(ret != 0) {
        return -1;
    } else {
        return 0;
    }
}