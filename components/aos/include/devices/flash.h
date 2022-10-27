/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef DEVICE_FLASH_PAI_H
#define DEVICE_FLASH_PAI_H

#ifdef __cplusplus
extern "C" {
#endif

#include <devices/device.h>

typedef struct {
    uint32_t    start_addr;
    uint32_t    block_size;
    uint32_t    block_count;
} flash_dev_info_t;

#define flash_open(name) device_open(name)
#define flash_open_id(name, id) device_open_id(name, id)
#define flash_close(dev) device_close(dev)

/**
  \brief       Notify a flash to read data.
  \param[in]   dev      Pointer to device object.
  \param[in]   addroff  address to read flash.
  \param[out]  buff     buffer address to store data read.
  \param[in]   bytesize data length expected to read.
  \return      0 on success, -1 on fail.
*/
int flash_read(aos_dev_t *dev, int32_t addroff, void *buff, int32_t bytesize);

/**
  \brief       Program flash at specified address.
  \param[in]   dev      Pointer to device object.
  \param[in]   dstaddr  address to program flash.
  \param[in]   srcbuf   buffer storing data to be programmed.
  \param[in]   bytesize data length to be programmed.
  \return      0 on success, -1 on fail.
*/
int flash_program(aos_dev_t *dev, int32_t dstaddr, const void *srcbuf, int32_t bytesize);

/**
  \brief       Notify a flash to fetch data.
  \param[in]   dev      Pointer to device object.
  \param[in]   addroff  a flash section including the address will be erased.
  \param[in]   blkcnt   erased block count
  \return      0 on success, -1 on fail.
*/
int flash_erase(aos_dev_t *dev, int32_t addroff, int32_t blkcnt);

/**
  \brief       Get info from a flash device.
  \param[in]   dev  Pointer to device object.
  \param[out]  info return flash info
  \return      0 on success, -1 on fail.
*/
int flash_get_info(aos_dev_t *dev, flash_dev_info_t *info);

#ifdef __cplusplus
}
#endif

#endif
