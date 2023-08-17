/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     softse_ecdsa.c
 * @brief    ecdsa
 * @version  V1.0
 * @date     11. May 2023
 * @model    ecdsa
 ******************************************************************************/
#ifdef CONFIG_SEC_CRYPTO_ECC_SW

#include <stdint.h>
#include <sec_crypto_ecdsa.h>
#include <sec_crypto_errcode.h>

#include "crypto_ecc_dsa.h"
#include "sec_crypto_common.h"

// Function documentation

/**
  \brief       Initialize ECDSA interface. Initializes the resources needed for the ECDSA interface
  \param[in]   ecdsa    Handle to operate
  \param[in]   idx    Device id
  \return      Error code 
*/
uint32_t sc_ecdsa_init(sc_ecdsa_t *ecdsa, uint32_t idx)
{
    CHECK_PARAM(ecdsa, SC_PARAM_INV);
    memset(ecdsa, 0, sizeof(sc_ecdsa_t));
    return SC_OK;

}


/**
  \brief       De-initialize ECDSA interface. Stops operation and releases the software resources used by the interface
  \param[in]   ecdsa    Dandle to operate
  \return      None
*/
void sc_ecdsa_uninit(sc_ecdsa_t *ecdsa)
{
    memset(ecdsa, 0, sizeof(sc_ecdsa_t));
}

/**
  \brief       Load curve param to engin
  \param[in]   ecdsa      Handle to operate
  \param[in]   type       Pointer to \ref sc_curve_type_t
  \return      Error code \ref 
*/
uint32_t sc_ecdsa_load_curve(sc_ecdsa_t *ecdsa, sc_curve_type_t type)
{
    uint32_t ret = SC_OK;
    CHECK_PARAM(ecdsa, SC_PARAM_INV);
    
    switch(type) {
        case SC_CURVES_SECP521R1:
        {
            ecdsa->ecdsa_ctx = &curve_secp256r1;
            ret = SC_OK;
            break;
        }
        case SC_CURVES_SECP256K1:
        case SC_CURVES_SECP384R1:
        case SC_CURVES_BRAINPOOL256R1:
        case SC_CURVES_BRAINPOOL256T1:
        case SC_CURVES_BRAINPOOL512R1:
        case SC_CURVES_BRAINPOOL512T1:
        {
            ret = SC_NOT_SUPPORT;
            break;
        }
        default:
        {
            ret = SC_NOT_SUPPORT;
        }
    }

    return ret;
}

/**
  \brief       Ecdsa Sign
  \param[in]   ecdsa        Handle to operate
  \param[in]   prikey       Pointer to the private key buf
  \param[in]   prikey_len   The private key length
  \param[in]   dgst         Pointer to the digest buf
  \param[in]   dgst_len     The digest length
  \param[out]  sig          Pointer to the signature buf
  \param[out]  sig_len      The signature length
  \return      Error code 
*/
uint32_t sc_ecdsa_sign(sc_ecdsa_t *ecdsa,  const uint8_t *prikey,  uint32_t prikey_len, 
                            const uint8_t *dgst, uint32_t dgst_len, uint8_t *sig, uint32_t *sig_len)
{
    uint32_t ret = SC_OK;
    CHECK_PARAM(ecdsa, SC_PARAM_INV);
    CHECK_PARAM(prikey, SC_PARAM_INV);
    CHECK_PARAM(prikey_len == EC_SECP521R1_PRIVATE_KEY_LEN, SC_PARAM_INV);
    CHECK_PARAM(dgst, SC_PARAM_INV);
    CHECK_PARAM(dgst_len == EC_SECP521R1_DIGEST_LEN, SC_PARAM_INV);
    CHECK_PARAM(sig, SC_PARAM_INV);
    CHECK_PARAM(sig_len, SC_PARAM_INV);

    ret = uECC_sign(prikey, dgst, dgst_len, sig, ecdsa->ecdsa_ctx);

    *sig_len = EC_SECP521R1_SIGNATURE_LEN;

    if ( ret == 0) {
        ret = SC_DRV_FAILED;
    } else {
        ret = SC_OK;
    }
    
    return ret;
}

/**
  \brief       Ecdsa Verify
  \param[in]   ecdsa        Handle to operate
  \param[in]   pubkey       Pointer to the public key buf
  \param[in]   prikey_len   The public key length
  \param[in]   dgst         Pointer to the digest buf
  \param[in]   dgst_len     The digest length
  \param[in]   sig          Pointer to the signature buf
  \param[in]   sig_len      The signature length
  \return      Error code 
*/
uint32_t sc_ecdsa_verify(sc_ecdsa_t *ecdsa, const uint8_t *pubkey, uint32_t pubkey_len, 
                            const uint8_t *dgst, uint32_t dgst_len,  const uint8_t *sig,  uint32_t sig_len)
{
    uint32_t ret = SC_OK;
    CHECK_PARAM(ecdsa, SC_PARAM_INV);
    CHECK_PARAM(pubkey, SC_PARAM_INV);
    CHECK_PARAM(pubkey_len == EC_SECP521R1_PUBLIC_KEY_LEN, SC_PARAM_INV);
    CHECK_PARAM(dgst, SC_PARAM_INV);
    CHECK_PARAM(dgst_len == EC_SECP521R1_DIGEST_LEN, SC_PARAM_INV);
    CHECK_PARAM(sig, SC_PARAM_INV);
    CHECK_PARAM(sig_len == EC_SECP521R1_SIGNATURE_LEN, SC_PARAM_INV);

    ret = uECC_verify(pubkey, dgst, dgst_len, sig, ecdsa->ecdsa_ctx);
    
    if ( ret == 0) {
        ret = SC_DRV_FAILED;
    } else {
        ret = SC_OK;
    }
    
    return ret;
}

#endif
