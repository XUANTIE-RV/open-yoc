/*
 * Copyright (C) 2017-2023 Alibaba Group Holding Limited
 */
/******************************************************************************
 * @file     se_storage.h
 * @brief    Header File for RNG
 * @version  V1.0
 * @date     22. May 2023
 * @model    storage
 ******************************************************************************/
#ifndef _SE_STORAGE_H_
#define _SE_STORAGE_H_


#include <stdint.h>
#include "se_device.h"


#ifdef __cplusplus
extern "C" {
#endif

/************/
/**
  \brief       Get the capacity the secure storage
  \param[out]   size  A pointer to the buffer which will be stored the size of storage.
  \return      0 success, Non-zero failure
*/
int se_get_avail_storage_capacity(uint32_t *size);

/**
  \brief       Get the capacity the secure storage
  \param[in]   data  A pointer to the buffer which stores the data need to be written.
  \param[in]   size  The size of data which will be written.
  \param[in]   pos  The position which the data will be written to.
  \return      0 success, Non-zero failure
*/
int se_write_storage_data(const uint8_t *data, uint32_t size, uint32_t pos);

/**
  \brief       Get the capacity the secure storage
  \param[out]   data  A pointer to the buffer which stores the data read out.
  \param[in]   size  The size of data which will be read.
  \param[in]   pos  The position which the data will be read.
  \return      0 success, Non-zero failure
*/
int se_read_storage_data(uint8_t *data , uint32_t size, uint32_t pos);


/************/

#ifdef __cplusplus
}
#endif

#endif /* _DRV_STORAGE_H_ */
