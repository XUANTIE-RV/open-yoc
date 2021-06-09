/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */
/******************************************************************************
 * @file     seccrypt_rsa.h
 * @brief    Header File for RSA
 * @version  V1.0
 * @date     20. Jul 2020
 * @model    rsa
 ******************************************************************************/
#ifndef _SC_RSA_H_
#define _SC_RSA_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CONFIG_SYSTEM_SECURE
#include "drv/rsa.h"
#endif


#ifdef CONFIG_SEC_CRYPTO_RSA_SW
#include "crypto_rsa.h"
#endif


#include <stdint.h>
#include <stdbool.h>
#include <drv/common.h>
#include <sec_crypto_errcode.h>


//TODO Del this file after updating to sc2.0

/*----- RSA Control Codes: Mode Parameters: Key Bits -----*/
typedef enum {
    SC_RSA_KEY_BITS_192 = 0, ///< 192 Key bits
    SC_RSA_KEY_BITS_256,     ///< 256 Key bits
    SC_RSA_KEY_BITS_512,     ///< 512 Key bits
    SC_RSA_KEY_BITS_1024,    ///< 1024 Key bits
    SC_RSA_KEY_BITS_2048,    ///< 2048 Key bits
    SC_RSA_KEY_BITS_3072,    ///< 3072 Key bits
    SC_RSA_KEY_BITS_4096     ///< 4096 Key bits
} sc_rsa_key_bits_t;

typedef enum {
    SC_RSA_PADDING_MODE_NO = 0,     ///< RSA NO Padding Mode
    SC_RSA_PADDING_MODE_PKCS1,      ///< RSA PKCS1 Padding Mode
    SC_RSA_PADDING_MODE_PKCS1_OAEP, ///< RSA PKCS1 OAEP Padding Mode
    SC_RSA_PADDING_MODE_SSLV23,     ///< RSA SSLV23 Padding Mode
    SC_RSA_PADDING_MODE_X931,       ///< RSA X931 Padding Mode
    SC_RSA_PADDING_MODE_PSS         ///< RSA PSS Padding Mode
} sc_rsa_padding_type_t;

typedef enum {
    SC_RSA_HASH_TYPE_MD5 = 0,
    SC_RSA_HASH_TYPE_SHA1,
    SC_RSA_HASH_TYPE_SHA224,
    SC_RSA_HASH_TYPE_SHA256,
    SC_RSA_HASH_TYPE_SHA384,
    SC_RSA_HASH_TYPE_SHA512
} sc_rsa_hash_type_t;

typedef struct {
// #if (defined(CONFIG_SYSTEM_SECURE) && defined(CONFIG_CSI_V2))
//   csi_rsa_context_t rsa_ctx;
// #else
    void *                 n;            ///< Pointer to the public modulus
    void *                 e;            ///< Pointer to the public exponent
    void *                 d;            ///< Pointer to the private exponent
    uint32_t     key_bits;     ///< RSA KEY BITS
    sc_rsa_padding_type_t padding_type; ///< RSA PADDING TYPE
    sc_rsa_hash_type_t hash_type;
// #endif
} sc_rsa_context_t;

/**
\brief RSA State
*/
typedef struct {
    uint8_t busy : 1;  ///< Calculate busy flag
    uint8_t error : 1; ///< Calculate error flag
} sc_rsa_state_t;

typedef struct {
#ifdef CONFIG_SYSTEM_SECURE
#ifdef CONFIG_CSI_V1
  rsa_handle_t handle;
#endif /* CONFIG_CSI_V1 */
#ifdef CONFIG_CSI_V2
  csi_rsa_t csi_rsa;
#endif
#endif
#if defined(CONFIG_SEC_CRYPTO_RSA_SW)
    sc_mbedtls_rsa_context rsa_ctx;
#endif
  sc_rsa_key_bits_t bits;
} sc_rsa_t;

/****** RSA Event *****/
typedef enum {
    SC_RSA_EVENT_COMPLETE = 0, ///< rsa event completed
    SC_RSA_EVENT_VERIFY_SUCCESS,
    SC_RSA_EVENT_VERIFY_FAILED,
    SC_RSA_EVENT_ERROR, ///< error event
} sc_rsa_event_t;

typedef void (*sc_rsa_callback_t)(
    sc_rsa_t *rsa, sc_rsa_event_t event,
    void *arg); ///< Pointer to \ref sc_rsa_callback_t : RSA Event call back.

// Function documentation

/**
  \brief       Initialize RSA Interface. 1. Initializes the resources needed for the RSA interface 2.registers event callback function
  \param[in]   rsa  rsa handle to operate.
  \param[in]   idx  device id
  \param[in]   data_bits  rsa bit width
  \return      \ref uint32_t
*/
uint32_t sc_rsa_init(sc_rsa_t *rsa, uint32_t idx, sc_rsa_key_bits_t data_bits);

/**
  \brief       De-initialize RSA Interface. stops operation and releases the software resources used by the interface
  \param[in]   rsa  rsa handle to operate.
  \return      none
*/
void sc_rsa_uninit(sc_rsa_t *rsa);

/**
  \brief       attach the callback handler to RSA
  \param[in]   rsa  operate handle.
  \param[in]   cb    callback function
  \param[in]   arg   user can define it by himself as callback's param
  \return      error code
*/
uint32_t sc_rsa_attach_callback(sc_rsa_t *rsa, sc_rsa_callback_t cb, void *arg);

/**
  \brief       detach the callback handler
  \param[in]   rsa  operate handle.
*/
void sc_rsa_detach_callback(sc_rsa_t *rsa);

/**
  \brief       generate rsa key pair.
  \param[in]   rsa       rsa handle to operate.
  \param[out]  context   Pointer to the rsa context
  \return      \ref uint32_t
*/
uint32_t sc_rsa_gen_key(sc_rsa_t *rsa, sc_rsa_context_t *context);

/**
  \brief       encrypt
  \param[in]   rsa       rsa handle to operate.
  \param[in]   context   Pointer to the rsa context
  \param[in]   src       Pointer to the source data.
  \param[in]   src_size  the source data len
  \param[out]  out       Pointer to the result buffer
  \return      \ref uint32_t
*/
uint32_t sc_rsa_encrypt(sc_rsa_t *rsa, sc_rsa_context_t *context, void *src,
                            uint32_t src_size, void *out);

/**
  \brief       decrypt
  \param[in]   rsa       rsa handle to operate.
  \param[in]   context   Pointer to the rsa context
  \param[in]   src       Pointer to the source data.
  \param[in]   src_size  the source data len
  \param[out]  out       Pointer to the result buffer
  \param[out]  out_size  the result size
  \return      \ref uint32_t
*/
uint32_t sc_rsa_decrypt(sc_rsa_t *rsa, sc_rsa_context_t *context, void *src,
                            uint32_t src_size, void *out, uint32_t *out_size);

/**
  \brief       rsa sign
  \param[in]   rsa       rsa handle to operate.
  \param[in]   context   Pointer to the rsa context
  \param[in]   src       Pointer to the source data.
  \param[in]   src_size  the source data len
  \param[out]  signature Pointer to the signature
  \param[in]   hash_type the source data hash type
  \return      \ref uint32_t
*/
uint32_t sc_rsa_sign(sc_rsa_t *rsa, sc_rsa_context_t *context, void *src, uint32_t src_size,
                         void *signature, sc_rsa_hash_type_t hash_type);

/**
  \brief       rsa verify
  \param[in]   rsa       rsa handle to operate.
  \param[in]   context   Pointer to the rsa context
  \param[in]   src       Pointer to the source data.
  \param[in]   src_size  the source data len
  \param[in]   signature Pointer to the signature
  \param[in]   sig_size  the signature size
  \param[in]   hash_type the source data hash type
  \return      verify result
*/
bool sc_rsa_verify(sc_rsa_t *rsa, sc_rsa_context_t *context, void *src, uint32_t src_size,
                    void *signature, uint32_t sig_size, sc_rsa_hash_type_t hash_type);

/**
  \brief       encrypt(async mode)
  \param[in]   rsa       rsa handle to operate.
  \param[in]   context   Pointer to the rsa context
  \param[in]   src       Pointer to the source data.
  \param[in]   src_size  the source data len
  \param[out]  out       Pointer to the result buffer
  \return      \ref uint32_t
*/
uint32_t sc_rsa_encrypt_async(sc_rsa_t *rsa, sc_rsa_context_t *context, void *src,
                                  uint32_t src_size, void *out);

/**
  \brief       decrypt(async mode)
  \param[in]   rsa       rsa handle to operate.
  \param[in]   context   Pointer to the rsa context
  \param[in]   src       Pointer to the source data.
  \param[in]   src_size  the source data len
  \param[out]  out       Pointer to the result buffer
  \param[out]  out_size  the result size
  \return      \ref uint32_t
*/
uint32_t sc_rsa_decrypt_async(sc_rsa_t *rsa, sc_rsa_context_t *context, void *src,
                                  uint32_t src_size, void *out, uint32_t *out_size);

/**
  \brief       rsa sign(async mode)
  \param[in]   rsa       rsa handle to operate.
  \param[in]   context   Pointer to the rsa context
  \param[in]   src       Pointer to the source data.
  \param[in]   src_size  the source data len
  \param[out]  signature Pointer to the signature
  \param[in]   hash_type the source data hash type
  \return      \ref uint32_t
*/
uint32_t sc_rsa_sign_async(sc_rsa_t *rsa, sc_rsa_context_t *context, void *src,
                               uint32_t src_size, void *signature, sc_rsa_hash_type_t hash_type);

/**
  \brief       rsa verify(async mode)
  \param[in]   rsa       rsa handle to operate.
  \param[in]   context   Pointer to the rsa context
  \param[in]   src       Pointer to the source data.
  \param[in]   src_size  the source data len
  \param[in]   signature Pointer to the signature
  \param[in]   sig_size  the signature size
  \param[in]   hash_type the source data hash type
  \return      verify result
*/
uint32_t sc_rsa_verify_async(sc_rsa_t *rsa, sc_rsa_context_t *context, void *src,
                                 uint32_t src_size, void *signature, uint32_t sig_size,
                                 sc_rsa_hash_type_t hash_type);

/**
  \brief       Get RSA state.
  \param[in]   rsa      rsa handle to operate.
  \param[out]  state    rsa state \ref sc_rsa_state_t.
  \return      \ref uint32_t
*/
uint32_t sc_rsa_get_state(sc_rsa_t *rsa, sc_rsa_state_t *state);

/**
  \brief       Get big prime data
  \param[in]   rsa          rsa handle to operate.
  \param[in]   p            Pointer to the prime
  \param[in]   bit_length   Pointer to the prime bit length
  \return      \ref uint32_t
*/
uint32_t sc_rsa_get_prime(sc_rsa_t *rsa, void *p, uint32_t bit_length);

/**
  \brief       enable rsa power manage
  \param[in]   rsa  rsa handle to operate.
  \return      error code
*/
uint32_t sc_rsa_enable_pm(sc_rsa_t *rsa);

/**
  \brief       disable rsa power manage
  \param[in]   rsa  rsa handle to operate.
*/
void sc_rsa_disable_pm(sc_rsa_t *rsa);

#ifdef __cplusplus
}
#endif

#endif /* _SC_RSA_H_ */
