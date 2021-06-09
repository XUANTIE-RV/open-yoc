/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     ck_sha_v1.c
 * @brief    CSI Source File for SHA Driver
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "drv/sha.h"
#include "soc.h"
#include "wm_crypto_hard.h"

typedef struct {
    sha_event_cb_t cb;
    sha_status_t status;

    hsDigestContext_t ctx;
} ck_sha_priv_t;

static ck_sha_priv_t sha_handle[CONFIG_SHA_NUM];

/* Driver Capabilities */
static const sha_capabilities_t driver_capabilities = {
    .sha1 = 1, /* sha1 mode */
};

#define ERR_SHA(errno) (CSI_DRV_ERRNO_SHA_BASE | errno)
#define SHA_NULL_PARAM_CHK(para)                         \
    do {                                        \
        if (para == NULL) {                     \
            return ERR_SHA(DRV_ERROR_PARAMETER);   \
        }                                       \
    } while (0)

//
// Functions
//

/**
  \brief       Initialize SHA Interface. 1. Initializes the resources needed for the SHA interface 2.registers event callback function
  \param[in]   context Pointer to the buffer store the sha context
  \param[in]   cb_event  Pointer to \ref sha_event_cb_t
  \return      return sha handle if success
*/
sha_handle_t csi_sha_initialize(int32_t idx, void *context, sha_event_cb_t cb_event)
{
    if (idx < 0 || idx >= CONFIG_SHA_NUM) {
        return NULL;
    }

    ck_sha_priv_t *sha_priv = &sha_handle[idx];

    /* initialize the sha context */
    sha_priv->cb = cb_event;
    sha_priv->status.busy = 0;

    return (sha_handle_t)sha_priv;
}

/**
  \brief       De-initialize SHA Interface. stops operation and releases the software resources used by the interface
  \param[in]   handle  sha handle to operate.
  \return      error code
*/
int32_t csi_sha_uninitialize(sha_handle_t handle)
{
    SHA_NULL_PARAM_CHK(handle);

    ck_sha_priv_t *sha_priv = handle;
    sha_priv->cb = NULL;

    return 0;
}

/**
  \brief       control sha power.
  \param[in]   handle  sha handle to operate.
  \param[in]   state   power state.\ref csi_power_stat_e.
  \return      error code
*/
int32_t csi_sha_power_control(sha_handle_t handle, csi_power_stat_e state)
{
    SHA_NULL_PARAM_CHK(handle);

    return ERR_SHA(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief       Get driver capabilities.
  \param[in]   idx device id
  \return      \ref sha_capabilities_t
*/
sha_capabilities_t csi_sha_get_capabilities(int32_t idx)
{
    if (idx < 0 || idx >= CONFIG_SHA_NUM) {
        sha_capabilities_t ret;
        memset(&ret, 0, sizeof(sha_capabilities_t));
        return ret;
    }

    return driver_capabilities;
}

/**
  \brief       config sha mode.
  \param[in]   handle  sha handle to operate.
  \param[in]   mode      \ref sha_mode_e
  \param[in]   endian    \ref sha_endian_mode_e
  \return      error code
*/
int32_t csi_sha_config(sha_handle_t handle, sha_mode_e mode, sha_endian_mode_e endian_mode)
{
    SHA_NULL_PARAM_CHK(handle);

    if (SHA_MODE_1 != mode)
        return ERR_SHA(SHA_ERROR_MODE);

    if (SHA_ENDIAN_MODE_LITTLE != endian_mode)
        return ERR_SHA(SHA_ERROR_ENDIAN);

    return 0;
}

/**
  \brief       start the engine
  \param[in]   handle  sha handle to operate.
  \param[in]   context  Pointer to the sha context.
  \return      error code
*/
int32_t csi_sha_start(sha_handle_t handle, void *context)
{
    SHA_NULL_PARAM_CHK(handle);

    ck_sha_priv_t *sha_priv = handle;
    sha_priv->status.busy = 1;

    tls_crypto_sha1_init(&sha_priv->ctx);

    return 0;
}

/**
  \brief       updata the engine
  \param[in]   handle  sha handle to operate.
  \param[in]   context  Pointer to the sha context.
  \param[in]   input   Pointer to the Source data
  \param[in]   len    the data len
  \return      error code
*/
int32_t csi_sha_update(sha_handle_t handle, void *context, const void *input, uint32_t len)
{
    SHA_NULL_PARAM_CHK(handle);
    SHA_NULL_PARAM_CHK(input);

    if (len <= 0) {
        return ERR_SHA(DRV_ERROR_PARAMETER);
    }

    ck_sha_priv_t *sha_priv = handle;

    tls_crypto_sha1_update(&sha_priv->ctx, input, len);

    sha_priv->status.busy = 0;

    return 0;
}

/**
  \brief       finish the engine
  \param[in]   handle  sha handle to operate.
  \param[in]   context  Pointer to the sha context.
  \param[out]  output   Pointer to the dest data
  \return      error code
*/
int32_t csi_sha_finish(sha_handle_t handle, void *context, void *output)
{
    SHA_NULL_PARAM_CHK(handle);
    SHA_NULL_PARAM_CHK(output);

    ck_sha_priv_t *sha_priv = handle;

    tls_crypto_sha1_final(&sha_priv->ctx, output);

    return 0;
}

/**
  \brief       Get SHA status.
  \param[in]   handle  sha handle to operate.
  \return      SHA status \ref sha_status_t
*/
sha_status_t csi_sha_get_status(sha_handle_t handle)
{
    if (handle == NULL) {
        sha_status_t ret;
        memset(&ret, 0, sizeof(sha_status_t));
        return ret;
    }

    ck_sha_priv_t *sha_priv = handle;
    return sha_priv->status;
}
