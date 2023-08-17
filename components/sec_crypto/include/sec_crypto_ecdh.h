/*
 * Copyright (C) 2017-2023 Alibaba Group Holding Limited
 */
/******************************************************************************
 * @file     seccrypt_ecdh.h
 * @brief    Header File for ECDH
 * @version  V1.0
 * @date     12. May 2023
 * @model    ecdh
 ******************************************************************************/
#ifndef _SC_ECDH_H_
#define _SC_ECDH_H_

#include <stdint.h>
#include <sec_crypto_errcode.h>
#include <sec_crypto_ecdsa.h>

#ifdef CONFIG_SYSTEM_SECURE
#include "drv/ecdh.h"
#endif

#ifdef CONFIG_SEC_CRYPTO_ECC_SW
#include "crypto_ecc_dsa.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define EC_SECP521R1_SECRET_KEY_LEN    32

/**
\brief ECDH Ctrl Block
*/
typedef struct {
#ifdef CONFIG_SYSTEM_SECURE
#ifdef CONFIG_CSI_V1
    aes_handle_t  handle;
#endif
#ifdef CONFIG_CSI_V2
    csi_ecdh_t   csi_ecdh;
#endif
#endif
#if defined(CONFIG_TEE_CA)
#endif
#if defined(CONFIG_SEC_CRYPTO_ECC_SW)
    uECC_Curve ecdh_ctx;
#endif
} sc_ecdh_t;

// Function documentation
/**
  \brief       Initialize ECDH interface. Initializes the resources needed for the ECDH interface
  \param[in]   ecdh   Handle to operate
  \param[in]   idx    Device id
  \return      Error code
*/
uint32_t sc_ecdh_init(sc_ecdh_t *ecdh, uint32_t idx);

/**
  \brief       De-initialize ECDH interface. Stops operation and releases the software resources used by the interface
  \param[in]   ecdh    Dandle to operate
  \return      None
*/
void sc_ecdh_uninit(sc_ecdh_t *ecdh);

/**
  \brief       Load curve param to engin
  \param[in]   ecdh      Handle to operate
  \param[in]   type       Pointer to \ref sc_curve_type_t
  \return      Error code
*/
uint32_t sc_ecdh_load_curve(sc_ecdh_t *ecdh, sc_curve_type_t type);

/**
  \brief       ECDH generate key pairs
  \param[in]   ecdh        Handle to operate
  \param[out]  privkey     Pointer to the private key buf
  \param[out]  pubkey      Pointer to the public key buf
  \return      Error code 
*/
uint32_t sc_ecdh_gen_keypair(sc_ecdh_t *ecdh, uint8_t *prikey, uint8_t *pubkey);

/**
  \brief       ECDH ECDH generate secret key 
  \param[in]   ecdh        Handle to operate
  \param[in]   privkey     Pointer to the private key buf of Alice
  \param[in]   pubkey      Pointer to the public key buf of Bob
  \param[out]  sk          Pointer to the secret key buf
  \param[out]  sk_len      The secret key length
  \return      Error code
*/
uint32_t sc_ecdh_calc_secret(sc_ecdh_t *ecdh, const uint8_t *privkey, const uint8_t *pubkey, 
                                uint8_t *sk, uint32_t *sk_len);

#ifdef __cplusplus
}
#endif
#endif /* _SC_ECDH_H_ */
