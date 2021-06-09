/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef CONFIG_SEC_CRYPTO_SHA_SW
#ifdef CONFIG_CSI_V2
#include "sec_mcu.h"
#include "sec_crypto_common.h"

/**
  \brief       Initialize SHA Interface. Initializes the resources needed for the SHA interface
*/
uint32_t sc_sha_init(sc_sha_t *sha, uint32_t idx)
{
    csi_error_t ret;
    CHECK_PARAM(sha, SC_PARAM_INV);

    ret = csi_sha_init(&sha->csi_sha, idx);
    if (ret) {
        return SC_DRV_FAILED;
    }

    return SC_OK;
}

/**
  \brief       De-initialize SHA Interface. stops operation and releases the software resources used by the interface
*/
void sc_sha_uninit(sc_sha_t *sha)
{
    if (sha) {
        csi_sha_uninit(&sha->csi_sha);
        memset(sha, 0, sizeof(sc_sha_t));
    }
}

/**
  \brief       attach the callback handler to SHA
*/
uint32_t sc_sha_attach_callback(sc_sha_t *sha, void *callback, void *arg)
{
    return SC_NOT_SUPPORT;
}

/**
  \brief       detach the callback handler
*/
void sc_sha_detach_callback(sc_sha_t *sha)
{
    return;
}

/**
  \brief       start the engine
*/
uint32_t sc_sha_start(sc_sha_t *sha, sc_sha_context_t *context, sc_sha_mode_t mode)
{
    uint32_t   ret;
    CHECK_PARAM(context, SC_PARAM_INV);
    CHECK_PARAM(sha, SC_PARAM_INV);

    ret = csi_sha_start(&sha->csi_sha, &context->ctx, mode);
    if (ret) {
        return SC_CRYPT_FAIL;
    }

    return SC_OK;
}

/**
  \brief       update the engine
*/
uint32_t sc_sha_update(sc_sha_t *sha, sc_sha_context_t *context, const void *input, uint32_t size)
{
    uint32_t ret;
    CHECK_PARAM(sha, SC_PARAM_INV);
    CHECK_PARAM(context, SC_PARAM_INV);
    CHECK_PARAM(input, SC_PARAM_INV);

    ret = csi_sha_update(&sha->csi_sha, &context->ctx, input, size);
    if (ret) {
        return SC_CRYPT_FAIL;
    }

    return SC_OK;
}

/**
  \brief       accumulate the engine (async mode)
*/
uint32_t sc_sha_update_async(sc_sha_t *sha, sc_sha_context_t *context, const void *input,
                             uint32_t size)
{
    return SC_NOT_SUPPORT;
}

/**
  \brief       finish the engine
*/
uint32_t sc_sha_finish(sc_sha_t *sha, sc_sha_context_t *context, void *output, uint32_t *out_size)
{
    uint32_t ret;
    CHECK_PARAM(sha, SC_PARAM_INV);
    CHECK_PARAM(context, SC_PARAM_INV);
    CHECK_PARAM(output, SC_PARAM_INV);

    ret = csi_sha_finish(&sha->csi_sha, &context->ctx, output, out_size);
    if (ret) {
        return SC_CRYPT_FAIL;
    }

    return SC_OK;
}

#endif
#endif