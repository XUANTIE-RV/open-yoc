/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */
/******************************************************************************
 * @file     seccrypt_sha.h
 * @brief    Header File for SHA
 * @version  V1.0
 * @date     20. Jul 2020
 * @model    sha
 ******************************************************************************/
#ifndef _SC_SHA_H_
#define _SC_SHA_H_

#include <stdint.h>
#ifdef CONFIG_SYSTEM_SECURE
#include "drv/sha.h"
#include "soc.h"
#endif
#include <sec_crypto_errcode.h>


#ifdef CONFIG_SEC_CRYPTO_SHA_SW
#include "crypto_sha1.h"
#include "crypto_sha256.h"
#endif
#if defined(CONFIG_WITH_SE)
#include <se_sha.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*----- SHA Control Codes: Mode -----*/
typedef enum {
    SC_SHA_MODE_1 = 1U,  ///< SHA_1 mode
    SC_SHA_MODE_256,     ///< SHA_256 mode
    SC_SHA_MODE_224,     ///< SHA_224 mode
    SC_SHA_MODE_512,     ///< SHA_512 mode
    SC_SHA_MODE_384,     ///< SHA_384 mode
    SC_SHA_MODE_512_256, ///< SHA_512_256 mode
    SC_SHA_MODE_512_224  ///< SHA_512_224 mode
} sc_sha_mode_t;

/**
\brief SHA State
*/
typedef struct {
    uint32_t busy : 1;  ///< calculate busy flag
    uint32_t error : 1; ///< calculate error flag
} sc_sha_state_t;

typedef struct {
#ifdef CONFIG_SYSTEM_SECURE
#ifdef CONFIG_CSI_V1
uint8_t ctx[SHA_CONTEXT_SIZE];
#endif /* CONFIG_CSI_V1 */
#ifdef CONFIG_CSI_V2
  csi_sha_context_t ctx;
#endif
#endif
#if defined(CONFIG_TEE_CA)
    uint8_t ctx[224+8];
#endif
#if defined(CONFIG_SEC_CRYPTO_SHA_SW)
  sc_mbedtls_sha1_context sha1_ctx;
  sc_mbedtls_sha256_context sha2_ctx;
#endif
    sc_sha_mode_t mode;        ///< sha mode
} sc_sha_context_t;

/****** SHA Event *****/
typedef enum {
    SC_SHA_EVENT_COMPLETE = 0U, ///< calculate completed
    SC_SHA_EVENT_ERROR          ///< calculate error
} sc_sha_event_t;

typedef struct sc_sha {
#ifdef CONFIG_SYSTEM_SECURE
#ifdef CONFIG_CSI_V1
  sha_handle_t handle;
  sc_sha_context_t ctx;
  sc_sha_mode_t mode;        ///< sha mode
#endif /* CONFIG_CSI_V1 */
#ifdef CONFIG_CSI_V2
  csi_sha_t csi_sha;
#endif
#endif
#if defined(CONFIG_TEE_CA)
#endif
#if defined(CONFIG_WITH_SE)
	se_sha_t se_sha;
#endif
  void *user;   ///< CID 68702（1 的 1 数)：编码标准冲突 (MISRA C-2012 Rule 21.18)
} sc_sha_t;

// Function documentation

/**
  \brief       Initialize SHA Interface. Initializes the resources needed for the SHA interface
  \param[in]   sha  operate handle.
  \param[in]   idx index of sha
  \return      error code \ref uint32_t
*/
uint32_t sc_sha_init(sc_sha_t *sha, uint32_t idx);

/**
  \brief       De-initialize SHA Interface. stops operation and releases the software resources used by the interface
  \param[in]   sha  sha handle to operate.
  \return      none
*/
void sc_sha_uninit(sc_sha_t *sha);

/**
  \brief       attach the callback handler to SHA
  \param[in]   sha  operate handle.
  \param[in]   callback callback function
  \param[in]   arg      callback's param
  \return      error code
*/
uint32_t sc_sha_attach_callback(sc_sha_t *sha, void *callback, void *arg);

/**
  \brief       detach the callback handler
  \param[in]   sha  operate handle.
*/
void sc_sha_detach_callback(sc_sha_t *sha);

/**
  \brief       start the engine
  \param[in]   sha     sha handle to operate.
  \param[in]   context Pointer to the sha context \ref sc_sha_context_t
  \param[in]   mode    sha mode \ref sc_sha_mode_t
  \return      error code \ref uint32_t
*/
uint32_t sc_sha_start(sc_sha_t *sha, sc_sha_context_t *context, sc_sha_mode_t mode);

/**
  \brief       update the engine
  \param[in]   sha     sha handle to operate.
  \param[in]   context Pointer to the sha context \ref sc_sha_context_t
  \param[in]   input   Pointer to the Source data
  \param[in]   size    the data size
  \return      error code \ref uint32_t
*/
uint32_t sc_sha_update(sc_sha_t *sha, sc_sha_context_t *context, const void *input, uint32_t size);

/**
  \brief       accumulate the engine (async mode)
  \param[in]   sha     sha handle to operate.
  \param[in]   context Pointer to the sha context \ref sc_sha_context_t
  \param[in]   input   Pointer to the Source data
  \param[in]   size    the data size
  \return      error code \ref uint32_t
*/
uint32_t sc_sha_update_async(sc_sha_t *sha, sc_sha_context_t *context, const void *input,
                             uint32_t size);

/**
  \brief       finish the engine
  \param[in]   sha      sha handle to operate.
  \param[in]   context  Pointer to the sha context \ref sc_sha_context_t
  \param[out]  output   Pointer to the result data
  \param[out]  out_size Pointer to the result data size(bytes)
  \return      error code \ref uint32_t
*/
uint32_t sc_sha_finish(sc_sha_t *sha, sc_sha_context_t *context, void *output, uint32_t *out_size);

#ifdef __cplusplus
}
#endif

#endif /* _sc_SHA_H_ */
