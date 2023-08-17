/*
 * Copyright (C) 2017-2023 Alibaba Group Holding Limited
 */
/******************************************************************************
 * @file     se_device.h
 * @brief    Header File for Device
 * @version  V1.0
 * @date     22. May 2023
 * @model    rng
 ******************************************************************************/
#ifndef _SE_DEVICE_H_
#define _SE_DEVICE_H_

#include <aos/kernel.h>
#include <devices/driver.h>
#include <stdint.h>
#include <stdio.h>
#include "aos/kernel.h"
#include "devices/driver.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SE_FIRMWARE_UPGRADE_BLOCK 512

typedef struct {
    int (*send)(const uint8_t *data, size_t size, uint32_t timeout);
    int (*recv)(uint8_t *data, size_t size, uint32_t timeout);
} se_ops_t;

typedef struct {
    rvm_dev_t   *dev;
    aos_mutex_t mutex;
    void        *priv;
	se_ops_t    *ops;
} se_dev_t;

typedef enum {
	SE_IF_TYPE_SPI		= 0,
} se_if_type_t;
/**
  \brief       Initialize SE Interface. 1. Initializes the resources needed for the RSA interface 2.registers event callback function
  \param[out]  se_dev  	rsa handle to operate.
  \param[in]   idx      device id
  \param[in]   type		the type of se interface
  \param[in]   config	a pointer to the configuraion of se interface
  \return      \ref 0 success, Non-zero failure
*/
int se_init(se_dev_t *se_dev, uint32_t idx, se_if_type_t type, void *config);

/**
  \brief       De-initialize SE Interface. stops operation and releases the software resources used by the interface
  \param[in]   se_dev    handle to operate
  \return      None
*/
void se_uninit(se_dev_t *se_dev);

/**
  \brief       Get se firmware version
  \param[in]   se_dev     handle to operate
  \param[out]  version    Pointer to the version buf
  \return      \ref 0 success, Non-zero failure
*/
int se_get_firmware_version(se_dev_t *se_dev, uint8_t *version);

/**
  \brief       Get se system function test
  \param[in]   se_dev     handle to operate
  \return      \ref 0 success, Non-zero failure
*/
int se_system_selftest(se_dev_t *se_dev);

/**
  \brief       Start SE firmware upgrade
  \return      0 success, Non-zero failured
*/
int se_start_update_firmware(void);

/**
  \brief       Get se system function test
  \param[in]   src        Pointer to the firmware buf
  \param[in]   size       The length of firmware buf
  \param[in]   pos        The position of firmware buf
  \return      \ref 0 success, Non-zero failure
*/
int se_update_firmware(const void * src, uint32_t size, uint32_t pos);

/**
  \brief       Stop SE firmware upgrade
  \return      0 success, Non-zero failured
*/
int se_stop_update_firmware(void);

/*
#define SE_DEVICE_LOG
#define SE_SHA_LOG
#define SE_RSA_LOG
#define SE_SHA_LOG
#define SE_AES_LOG
#define SE_TEST_LOG
*/

#ifdef __cplusplus
}
#endif

#endif /* _DRV_COMMON_H_ */