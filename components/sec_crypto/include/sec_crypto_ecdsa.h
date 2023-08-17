/*
 * Copyright (C) 2017-2023 Alibaba Group Holding Limited
 */
/******************************************************************************
 * @file     seccrypt_ecdsa.h
 * @brief    Header File for ECDSA
 * @version  V1.0
 * @date     11. May 2023
 * @model    ecdsa
 ******************************************************************************/
#ifndef _SC_ECDSA_H_
#define _SC_ECDSA_H_

#include <stdint.h>
#include <sec_crypto_errcode.h>


#ifdef CONFIG_SYSTEM_SECURE
#include "drv/ecdsa.h"
#endif

#ifdef CONFIG_SEC_CRYPTO_ECC_SW
#include "crypto_ecc_dsa.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    SC_CURVES_SECP256K1 =  0U,	/* SECG curve over a 256 bit prime field */
    SC_CURVES_SECP384R1, 		/* NIST/SECG curve over a 384 bit prime field */
    SC_CURVES_SECP521R1, 		/* NIST/SECG curve over a 521 bit prime field */
    SC_CURVES_BRAINPOOL256R1, 	/* RFC 5639 curve over a 256 prime field */
    SC_CURVES_BRAINPOOL256T1, 	/* RFC 5639 curve over a 256 prime field */
    SC_CURVES_BRAINPOOL512R1, 	/* RFC 5639 curve over a 512 prime field */
    SC_CURVES_BRAINPOOL512T1, 	/* RFC 5639 curve over a 512 prime field */
} sc_curve_type_t;

#define EC_SECP521R1_PRIVATE_KEY_LEN  32
#define EC_SECP521R1_PUBLIC_KEY_LEN   64
#define EC_SECP521R1_DIGEST_LEN       32
#define EC_SECP521R1_SIGNATURE_LEN    64

/**
\brief ECDSA Ctrl Block
*/
typedef struct {
#ifdef CONFIG_SYSTEM_SECURE
#ifdef CONFIG_CSI_V1
    aes_handle_t  handle;
#endif
#ifdef CONFIG_CSI_V2
    csi_ecdsa_t   csi_ecdsa;
#endif
#endif
#if defined(CONFIG_TEE_CA)
#endif
#if defined(CONFIG_SEC_CRYPTO_ECC_SW)
    uECC_Curve ecdsa_ctx;
#endif
} sc_ecdsa_t;

// Function documentation
/**
  \brief       Initialize ECDSA interface. Initializes the resources needed for the ECDSA interface
  \param[in]   ecdsa    Handle to operate
  \param[in]   idx    Device id
  \return      Error code
*/
uint32_t sc_ecdsa_init(sc_ecdsa_t *ecdsa, uint32_t idx);

/**
  \brief       De-initialize ECDSA interface. Stops operation and releases the software resources used by the interface
  \param[in]   ecdsa    Dandle to operate
  \return      None
*/
void sc_ecdsa_uninit(sc_ecdsa_t *ecdsa);

/**
  \brief       Load curve param to engin
  \param[in]   ecdsa      Handle to operate
  \param[in]   type       Pointer to \ref csi_curve_type_t
  \return      Error code
*/
uint32_t sc_ecdsa_load_curve(sc_ecdsa_t *ecdsa, sc_curve_type_t type);

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
                            const uint8_t *dgst, uint32_t dgst_len, uint8_t *sig, uint32_t *sig_len);

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
                            const uint8_t *dgst, uint32_t gst_len,  const uint8_t *sig,  uint32_t sig_len);


#ifdef __cplusplus
}
#endif
#endif /* _SC_ECDSA_H_ */
