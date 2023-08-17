/*
 * Copyright (C) 2017-2023 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     softse_ecdh.c
 * @brief    ecdh
 * @version  V1.0
 * @date     11. May 2023
 * @model    ecdh
 ******************************************************************************/
#ifdef CONFIG_SEC_CRYPTO_ECC_SW

#include <stdint.h>
#include <sec_crypto_ecdh.h>
#include <sec_crypto_errcode.h>

#include "crypto_ecc_dh.h"
#include "sec_crypto_common.h"

// Function documentation

/**
  \brief       Initialize ECDH interface. Initializes the resources needed for the ECDH interface
  \param[in]   ecdh    Handle to operate
  \param[in]   idx    Device id
  \return      Error code 
*/
uint32_t sc_ecdh_init(sc_ecdh_t *ecdh, uint32_t idx)
{
    CHECK_PARAM(ecdh, SC_PARAM_INV);
    memset(ecdh, 0, sizeof(sc_ecdh_t));
    return SC_OK;

}


/**
  \brief       De-initialize ECDH interface. Stops operation and releases the software resources used by the interface
  \param[in]   ecdh    Dandle to operate
  \return      None
*/
void sc_ecdh_uninit(sc_ecdh_t *ecdh)
{
    memset(ecdh, 0, sizeof(sc_ecdh_t));
}

/**
  \brief       Load curve param to engin
  \param[in]   ecdh      Handle to operate
  \param[in]   type       Pointer to \ref sc_curve_type_t
  \return      Error code \ref 
*/
uint32_t sc_ecdh_load_curve(sc_ecdh_t *ecdh, sc_curve_type_t type)
{
    uint32_t ret = SC_OK;
    CHECK_PARAM(ecdh, SC_PARAM_INV);
    
    switch(type) {
        case SC_CURVES_SECP521R1:
        {
            ecdh->ecdh_ctx = &curve_secp256r1;
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
  \brief       ECDH generate key pairs
  \param[in]   ecdh        Handle to operate
  \param[out]  privkey     Pointer to the private key buf
  \param[out]  pubkey      Pointer to the public key buf
  \return      Error code 
*/
uint32_t sc_ecdh_gen_keypair(sc_ecdh_t *ecdh, uint8_t *prikey, uint8_t *pubkey)
{
    uint32_t ret = SC_OK;
    CHECK_PARAM(ecdh, SC_PARAM_INV);
    CHECK_PARAM(prikey, SC_PARAM_INV);
    CHECK_PARAM(pubkey, SC_PARAM_INV);

    ret = uECC_make_key(pubkey, prikey, ecdh->ecdh_ctx);

    if ( ret == 0) {
        ret = SC_DRV_FAILED;
    } else {
        ret = SC_OK;
    }
    
    return ret;
}

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
                                uint8_t *sk, uint32_t *sk_len)
{
    uint32_t ret = SC_OK;
    CHECK_PARAM(ecdh, SC_PARAM_INV);
    CHECK_PARAM(pubkey, SC_PARAM_INV);
    CHECK_PARAM(privkey, SC_PARAM_INV);
    CHECK_PARAM(sk, SC_PARAM_INV);

    ret = uECC_shared_secret(pubkey, privkey, sk, ecdh->ecdh_ctx);

    *sk_len = EC_SECP521R1_SECRET_KEY_LEN;
    
    if ( ret == 0) {
        ret = SC_DRV_FAILED;
    } else {
        ret = SC_OK;
    }
    
    return ret;
}

#endif
