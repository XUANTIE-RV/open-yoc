/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     ck_trng.c
 * @brief    CSI Source File for TRNG Driver
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "drv/trng.h"
#include "soc.h"
#include "wm_osal.h"
#include "wm_crypto_hard.h"


#define ERR_TRNG(errno) (CSI_DRV_ERRNO_TRNG_BASE | errno)
#define TRNG_NULL_PARAM_CHK(para) HANDLE_PARAM_CHK(para, ERR_TRNG(DRV_ERROR_PARAMETER))

typedef struct {
    trng_event_cb_t cb;
    trng_status_t status;
} ck_trng_priv_t;

static ck_trng_priv_t trng_handle[CONFIG_TRNG_NUM];

/* Driver Capabilities */
static const trng_capabilities_t driver_capabilities = {
    .lowper_mode = 0 /* low power mode */
};

//
// Functions
//

/**
  \brief       Initialize TRNG Interface. 1. Initializes the resources needed for the TRNG interface 2.registers event callback function
  \param[in]   idx device id
  \param[in]   cb_event  Pointer to \ref trng_event_cb_t
  \return      pointer to trng handle
*/
trng_handle_t csi_trng_initialize(int32_t idx, trng_event_cb_t cb_event)
{

    if (idx < 0 || idx >= CONFIG_TRNG_NUM) {
        return NULL;
    }

    ck_trng_priv_t *trng_priv = &trng_handle[idx];

    /* initialize the trng context */
    trng_priv->cb = cb_event;
    trng_priv->status.busy = 0;
    trng_priv->status.data_valid = 0;

    return (trng_handle_t)trng_priv;
}

/**
  \brief       De-initialize TRNG Interface. stops operation and releases the software resources used by the interface
  \param[in]   handle  trng handle to operate.
  \return      error code
*/
int32_t csi_trng_uninitialize(trng_handle_t handle)
{
    TRNG_NULL_PARAM_CHK(handle);

    ck_trng_priv_t *trng_priv = handle;
    trng_priv->cb = NULL;

    return 0;
}

/**
  \brief       control trng power.
  \param[in]   handle  trng handle to operate.
  \param[in]   state   power state.\ref csi_power_stat_e.
  \return      error code
*/
int32_t csi_trng_power_control(trng_handle_t handle, csi_power_stat_e state)
{
    TRNG_NULL_PARAM_CHK(handle);

    return ERR_TRNG(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief       Get driver capabilities.
  \param[in]   idx device id.
  \return      \ref trng_capabilities_t
*/
trng_capabilities_t csi_trng_get_capabilities(int32_t idx)
{
    if (idx < 0 || idx >= CONFIG_TRNG_NUM) {
        trng_capabilities_t ret;
        memset(&ret, 0, sizeof(trng_capabilities_t));
        return ret;
    }

    return driver_capabilities;
}

/**
  \brief       Get data from the TRNG.
  \param[in]   handle  trng handle to operate.
  \param[out]  data  Pointer to buffer with data get from TRNG
  \param[in]   num   Number of data items to obtain
  \return      error code
*/
int32_t csi_trng_get_data(trng_handle_t handle, void *data, uint32_t num)
{
    volatile int32_t i;
    volatile int32_t j;
    uint32_t seed = 0;
    TRNG_NULL_PARAM_CHK(handle);
    TRNG_NULL_PARAM_CHK(data);
    TRNG_NULL_PARAM_CHK(num);

    ck_trng_priv_t *trng_priv = handle;

    trng_priv->status.busy = 1U;
    trng_priv->status.data_valid = 0U;

    seed = tls_random_seed_generation();
    tls_crypto_random_init(seed, CRYPTO_RNG_SWITCH_32);
    for(i=0;i<32;i++)
    {
        for(j=0;j<3;j++)
        {}
    }
    tls_crypto_random_bytes(data, num);
    tls_crypto_random_stop();

    trng_priv->status.busy = 0U;
    trng_priv->status.data_valid = 1U;

    if (trng_priv->cb) {
        trng_priv->cb(0, TRNG_EVENT_DATA_GENERATE_COMPLETE);
    }

    return 0;
}

/**
  \brief       Get TRNG status.
  \param[in]   handle  trng handle to operate.
  \return      TRNG status \ref trng_status_t
*/
trng_status_t csi_trng_get_status(trng_handle_t handle)
{
    if (handle == NULL) {
        trng_status_t ret;
        memset(&ret, 0, sizeof(trng_status_t));
        return ret;
    }

    ck_trng_priv_t *trng_priv = handle;
    return trng_priv->status;
}
