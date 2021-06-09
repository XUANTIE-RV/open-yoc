/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     ck_crc_v1.c
 * @brief    CSI Source File for CRC Driver
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/

#include <stdio.h>
#include <string.h>
#include "drv/crc.h"
#include "soc.h"
#include "wm_crypto_hard.h"

#define ERR_CRC(errno) (CSI_DRV_ERRNO_CRC_BASE | errno)
#define CRC_NULL_PARAM_CHK(para)  HANDLE_PARAM_CHK(para, ERR_CRC(DRV_ERROR_PARAMETER))

typedef struct {
    crc_event_cb_t cb;
    crc_status_t status;

    uint32_t initv;
    CRYPTO_CRC_TYPE crc_type;
    uint8_t mode;
} ck_crc_priv_t;


static ck_crc_priv_t crc_handle[CONFIG_CRC_NUM];
/* Driver Capabilities */
static const crc_capabilities_t driver_capabilities = {
    .XMODEM = 1, /* XMODEM mode */
    .MODBUS = 1  /* MODBUS mode */
};

//
// Functions
//

/**
  \brief       Initialize CRC Interface. 1. Initializes the resources needed for the CRC interface 2.registers event callback function
  \param[in]   idx must not exceed return value of csi_crc_get_handle_count()
  \param[in]   cb_event  Pointer to \ref crc_event_cb_t
  \return      return crc handle if success
*/
crc_handle_t csi_crc_initialize(int32_t idx, crc_event_cb_t cb_event)
{
    if (idx < 0 || idx >= CONFIG_CRC_NUM) {
        return NULL;
    }

    ck_crc_priv_t *crc_priv = &crc_handle[idx];

    crc_priv->cb = cb_event;
    crc_priv->status.busy = 0;

    return (crc_handle_t)crc_priv;
}

/**
  \brief       De-initialize CRC Interface. stops operation and releases the software resources used by the interface
  \param[in]   handle  crc handle to operate.
  \return      error code
*/
int32_t csi_crc_uninitialize(crc_handle_t handle)
{
    CRC_NULL_PARAM_CHK(handle);

    ck_crc_priv_t *crc_priv = handle;
    crc_priv->cb = NULL;

    return 0;
}

/**
  \brief       control crc power.
  \param[in]   handle  crc handle to operate.
  \param[in]   state   power state.\ref csi_power_stat_e.
  \return      error code
*/
int32_t csi_crc_power_control(crc_handle_t handle, csi_power_stat_e state)
{
    CRC_NULL_PARAM_CHK(handle);
    return ERR_CRC(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief       Get driver capabilities.
  \param[in]   idx device id.
  \return      \ref crc_capabilities_t
*/
crc_capabilities_t csi_crc_get_capabilities(int32_t idx)
{
    if (idx < 0 || idx >= CONFIG_CRC_NUM) {
        crc_capabilities_t ret;
        memset(&ret, 0, sizeof(crc_capabilities_t));
        return ret;
    }

    return driver_capabilities;
}

/**
  \brief       config crc mode.
  \param[in]   handle  crc handle to operate.
  \param[in]   mode      \ref crc_mode_e
  \param[in]   standard  \ref crc_standard_crc_e
  \return      error code
*/
int32_t csi_crc_config(crc_handle_t handle, crc_mode_e mode, crc_standard_crc_e standard)
{
    CRC_NULL_PARAM_CHK(handle);

    ck_crc_priv_t *crc_priv = handle;

    if (CRC_MODE_CRC8 == mode)
    {
        if (CRC_STANDARD_CRC_NONE == standard)
        {
            crc_priv->crc_type = CRYPTO_CRC_TYPE_8;
            crc_priv->initv = 0xFFFFFFFF;
            crc_priv->mode = 3;
        }
        else
            return ERR_CRC(DRV_ERROR_UNSUPPORTED);
    }
    else if (CRC_MODE_CRC16 == mode)
    {
        if (CRC_STANDARD_CRC_MODBUS == standard)
        {
            crc_priv->crc_type = CRYPTO_CRC_TYPE_16_MODBUS;
            crc_priv->initv = 0xFFFFFFFF;
            crc_priv->mode = 3;
        }
        else if (CRC_STANDARD_CRC_XMODEM == standard)
        {
            crc_priv->crc_type = CRYPTO_CRC_TYPE_16_CCITT;
            crc_priv->initv = 0x0;
            crc_priv->mode = 0;
        }
        else
            return ERR_CRC(DRV_ERROR_UNSUPPORTED);
    }
    else if (CRC_MODE_CRC32 == crc_priv->mode)
    {
        if (CRC_STANDARD_CRC_NONE == standard)
        {
            crc_priv->crc_type = CRYPTO_CRC_TYPE_32;
            crc_priv->initv = 0xFFFFFFFF;
            crc_priv->mode = 3;
        }
        else
            return ERR_CRC(DRV_ERROR_UNSUPPORTED);
    }
    else
        return ERR_CRC(DRV_ERROR_UNSUPPORTED);

    return 0;
}

/**
  \brief       calculate crc.
  \param[in]   handle  crc handle to operate.
  \param[in]   in      Pointer to the input data
  \param[out]  out     Pointer to the result.
  \param[in]   len     intpu data len.
  \return      error code
*/
int32_t csi_crc_calculate(crc_handle_t handle, const void *in, void *out, uint32_t len)
{
    CRC_NULL_PARAM_CHK(handle);
    CRC_NULL_PARAM_CHK(in);
    CRC_NULL_PARAM_CHK(out);

    if (len <= 0) {
        return ERR_CRC(DRV_ERROR_PARAMETER);
    }

    ck_crc_priv_t *crc_priv = handle;
    psCrcContext_t ctx;

    crc_priv->status.busy = 1;

    tls_crypto_crc_init(&ctx, crc_priv->initv, crc_priv->crc_type, crc_priv->mode);
    tls_crypto_crc_update(&ctx, (unsigned char *)in, len);
    tls_crypto_crc_final(&ctx, (u32 *)out);

    crc_priv->status.busy = 0;

    return 0;
}

/**
  \brief       Get CRC status.
  \param[in]   handle  crc handle to operate.
  \return      CRC status \ref crc_status_t
*/
crc_status_t csi_crc_get_status(crc_handle_t handle)
{
    if (handle == NULL) {
        crc_status_t ret;
        memset(&ret, 0, sizeof(crc_status_t));
        return ret;
    }

    ck_crc_priv_t *crc_priv = handle;
    return crc_priv->status;
}
