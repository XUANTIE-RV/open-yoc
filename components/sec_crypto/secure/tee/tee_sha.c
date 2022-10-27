/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     tee_sha.c
 * @brief    sha
 * @version  V1.0
 * @date     21. May 2020
 * @model    sse
 ******************************************************************************/
#ifndef CONFIG_SEC_CRYPTO_SHA_SW
#include <stdint.h>
#include <sec_crypto_sha.h>
#include <sec_crypto_errcode.h>

#include "drv/tee.h"
#include "sec_crypto_common.h"

// Function documentation

/**
  \brief       Initialize SHA Interface. Initializes the resources needed for the SHA interface
  \param[in]   sha  operate handle.
  \param[in]   idx index of sha
  \return      error code \ref uint32_t
*/
uint32_t sc_sha_init(sc_sha_t *sha, uint32_t idx)
{
    CHECK_PARAM(sha, SC_PARAM_INV);

    memset(sha, 0, sizeof(sc_sha_t));

    return SC_OK;
}

/**
  \brief       De-initialize SHA Interface. stops operation and releases the software resources used by the interface
  \param[in]   sha  sha handle to operate.
  \return      none
*/
void sc_sha_uninit(sc_sha_t *sha)
{
    if (!sha) {
        return;
    }
    memset(sha, 0, sizeof(sc_sha_t));
}

/**
  \brief       attach the callback handler to SHA
  \param[in]   sha  operate handle.
  \param[in]   callback callback function
  \param[in]   arg      callback's param
  \return      error code
*/
uint32_t sc_sha_attach_callback(sc_sha_t *sha, void *callback, void *arg)
{
    return SC_NOT_SUPPORT;
}

/**
  \brief       detach the callback handler
  \param[in]   sha  operate handle.
*/
void sc_sha_detach_callback(sc_sha_t *sha)
{
    return;
}

/**
  \brief       start the engine
  \param[in]   sha     sha handle to operate.
  \param[in]   context Pointer to the sha context \ref sc_sha_context_t
  \param[in]   mode    sha mode \ref sc_sha_mode_t
  \return      error code \ref uint32_t
*/
uint32_t sc_sha_start(sc_sha_t *sha, sc_sha_context_t *context, sc_sha_mode_t mode)
{
    CHECK_PARAM(sha, SC_PARAM_INV);
    CHECK_PARAM(context, SC_PARAM_INV);

    if (mode == SC_SHA_MODE_224) {
        return csi_tee_sha_start(TEE_SHA224, (void *)context);
    } else if (mode == SC_SHA_MODE_256) {
        return csi_tee_sha_start(TEE_SHA256, (void *)context);
    } else if (mode == SC_SHA_MODE_1) {
        return csi_tee_sha_start(TEE_SHA1, (void *)context);
    } else {
        return SC_NOT_SUPPORT;
    }
}

/**
  \brief       update the engine
  \param[in]   sha     sha handle to operate.
  \param[in]   context Pointer to the sha context \ref sc_sha_context_t
  \param[in]   input   Pointer to the Source data
  \param[in]   size    the data size
  \return      error code \ref uint32_t
*/
uint32_t sc_sha_update(sc_sha_t *sha, sc_sha_context_t *context, const void *input, uint32_t size)
{
    CHECK_PARAM(sha, SC_PARAM_INV);
    CHECK_PARAM(context, SC_PARAM_INV);
    CHECK_PARAM(input, SC_PARAM_INV);
    return csi_tee_sha_update(input, size, (void *)context);
}

/**
  \brief       accumulate the engine (async mode)
  \param[in]   sha     sha handle to operate.
  \param[in]   context Pointer to the sha context \ref sc_sha_context_t
  \param[in]   input   Pointer to the Source data
  \param[in]   size    the data size
  \return      error code \ref uint32_t
*/
uint32_t sc_sha_update_async(sc_sha_t *sha, sc_sha_context_t *context, const void *input,
                             uint32_t size)
{
    return SC_NOT_SUPPORT;
}

/**
  \brief       finish the engine
  \param[in]   sha      sha handle to operate.
  \param[in]   context  Pointer to the sha context \ref sc_sha_context_t
  \param[out]  output   Pointer to the result data
  \param[out]  out_size Pointer to the result data size(bytes)
  \return      error code \ref uint32_t
*/
uint32_t sc_sha_finish(sc_sha_t *sha, sc_sha_context_t *context, void *output, uint32_t *out_size)
{
    CHECK_PARAM(sha, SC_PARAM_INV);
    uint32_t lens[] = {20, 32, 28, 48, 64};
    CHECK_PARAM(context, SC_PARAM_INV);
    CHECK_PARAM(output, SC_PARAM_INV);
    CHECK_PARAM(out_size, SC_PARAM_INV);

    *out_size = lens[1];

    return csi_tee_sha_finish(output, (void *)context);
}

/**
  \brief       Get SHA state.
  \param[in]   sha      sha handle to operate.
  \param[out]  state    sha state \ref sc_sha_state_t.
  \return      error code \ref uint32_t
*/
uint32_t sc_sha_get_state(sc_sha_t *sha, sc_sha_state_t *state)
{
    state->busy = 0;
    return SC_OK;
}

/**
  \brief       enable sha power manage
  \param[in]   sha  sha handle to operate.
  \return      error code \ref uint32_t
*/
uint32_t sc_sha_enable_pm(sc_sha_t *sha)
{
    return SC_OK;
}

/**
  \brief       disable sha power manage
  \param[in]   sha  sha handle to operate.
*/
void sc_sha_disable_pm(sc_sha_t *sha)
{
    return;
}
#endif
