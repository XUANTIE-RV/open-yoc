/*
 * Copyright (C) 2017-2023 Alibaba Group Holding Limited
 */
#include <cmd.h>
#include <se_storage.h>
#include <se_dev_internal.h>

/**
  \brief       Get the capacity the secure storage
  \param[out]   size  A pointer to the buffer which will be stored the size of storage.
  \return      0 success, Non-zero failure
*/
int se_get_avail_storage_capacity(uint32_t *size)
{
	*size =  1024;

	return 0;
}

/**
  \brief       Get the capacity the secure storage
  \param[in]   data  A pointer to the buffer which stores the data need to be written.
  \param[in]   size  The size of data which will be written.
  \param[in]   pos  The position which the data will be written to.
  \return      0 success, Non-zero failure
*/
int se_write_storage_data(const uint8_t *data, uint32_t size, uint32_t pos)
{
	SGD_RV ret;

	se_dev_lock();
	ret = hs_Write_user_info(pos, size, (uint8_t *)data);
	se_dev_unlock();
	if (ret != SDR_OK)
		return -1;

	return 0;
}

/**
  \brief       Get the capacity the secure storage
  \param[out]   data  A pointer to the buffer which stores the data read out.
  \param[in]   size  The size of data which will be read.
  \param[in]   pos  The position which the data will be read.
  \return      0 success, Non-zero failure
*/
int se_read_storage_data(uint8_t *data , uint32_t size, uint32_t pos)
{
	SGD_RV ret;

	se_dev_lock();
	ret = hs_Read_user_info(pos, size, data);
	se_dev_unlock();
	if (ret != SDR_OK)
		return -1;

	return 0;
}