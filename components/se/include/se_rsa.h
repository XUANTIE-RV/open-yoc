/*
 * Copyright (C) 2017-2023 Alibaba Group Holding Limited
 */
/******************************************************************************
 * @file     se_rsa.h
 * @brief    Header File for RNG
 * @version  V1.0
 * @date     22. May 2023
 * @model    rsa
 ******************************************************************************/
#ifndef _SE_RSA_H_
#define _SE_RSA_H_


#include <stdint.h>
#include <stdbool.h>
#include "se_device.h"


#ifdef __cplusplus
extern "C" {
#endif

/************/

typedef enum {
    SE_RSA_KEY_BITS_192  = 0U,  		  /* 192 Key bits */ 
    SE_RSA_KEY_BITS_256,                  /* 256 Key bits */ 
    SE_RSA_KEY_BITS_512,                  /* 512 Key bits */ 
    SE_RSA_KEY_BITS_1024,                 /* 1024 Key bits */ 
    SE_RSA_KEY_BITS_2048,                 /* 2048 Key bits */ 
    SE_RSA_KEY_BITS_3072,                 /* 3072 Key bits */ 
    SE_RSA_KEY_BITS_4096                  /* 4096 Key bits */ 
} se_rsa_key_bits_t;

typedef enum {
    SE_RSA_HASH_TYPE_MD5  = 0,
    SE_RSA_HASH_TYPE_SHA1,
    SE_RSA_HASH_TYPE_SHA224,
    SE_RSA_HASH_TYPE_SHA256,
    SE_RSA_HASH_TYPE_SHA384,
    SE_RSA_HASH_TYPE_SHA512
} se_rsa_hash_type_t;

typedef enum {
    SE_RSA_PADDING_MODE_NO  = 0, 		   /* RSA NO Padding Mode */ 
    SE_RSA_PADDING_MODE_PKCS1,             /* RSA PKCS1 Padding Mode */ 
    SE_RSA_PADDING_MODE_PKCS1_OAEP,        /* RSA PKCS1 OAEP Padding Mode */ 
    SE_RSA_PADDING_MODE_SSLV23,            /* RSA SSLV23 Padding Mode */ 
    SE_RSA_PADDING_MODE_X931,              /* RSA X931 Padding Mode */ 
    SE_RSA_PADDING_MODE_PSS                /* RSA PSS Padding Mode */ 
} se_rsa_padding_type_t;

typedef struct {
    void *n;                                /* Pointer to the public modulus */
    void *e;                                /* Pointer to the public exponent */
    void *d;                                /* Pointer to the private exponent */
    se_rsa_key_bits_t  key_bits;            /* RSA KEY BITS */
    se_rsa_padding_type_t padding_type;     /* RSA PADDING TYPE */
} se_rsa_context_t;

typedef struct{
    aos_mutex_t mutex;
} se_rsa_t;

/**
  \brief       Initialize RSA Interface. 1. Initializes the resources needed for the RSA interface 2.registers event callback function
  \param[in]   rsa  rsa handle to operate.
  \return      \ref 0 success, Non-zero failure
*/
int se_rsa_init(se_rsa_t *rsa);

/**
  \brief       De-initialize RSA Interface. stops operation and releases the software resources used by the interface
  \param[in]   rsa  rsa handle to operate.
  \return      none
*/
void se_rsa_uninit(se_rsa_t *rsa);

/**
  \brief       encrypt
  \param[in]   rsa    rsa handle to operate.
  \param[in]   context   Pointer to the rsa context
  \param[in]   src       Pointer to the source data.
  \param[in]   src_size  the source data len
  \param[out]  out       Pointer to the result buffer
  \param[out]  out_size  the result buffer len
  \param[in]   key_index the key index
  \return      \ref 0 success, Non-zero failure
*/
int se_rsa_encrypt(se_rsa_t *rsa, se_rsa_context_t *context, void *src, uint32_t src_size, 
                        void *out, uint32_t *out_size, uint32_t key_index);

/**
  \brief       decrypt
  \param[in]   rsa    rsa handle to operate.
  \param[in]   context   Pointer to the rsa context
  \param[in]   src       Pointer to the source data.
  \param[in]   src_size  the source data len
  \param[out]  out       Pointer to the result buffer
  \param[out]  out_size  the result buffer len
  \param[in]   key_index the key index
  \return      \ref 0 success, Non-zero failure
*/
int se_rsa_decrypt(se_rsa_t *rsa, se_rsa_context_t *context, void *src, uint32_t src_size, 
                        void *out, uint32_t *out_size, uint32_t key_index);

/**
  \brief       rsa sign
  \param[in]   rsa    rsa handle to operate.
  \param[in]   context   Pointer to the rsa context
  \param[in]   src       Pointer to the source data.
  \param[in]   src_size  the source data len
  \param[out]  signature Pointer to the signature
  \param[in]   hash_type the source data hash type
  \param[in]   key_index the key index
  \return      \ref 0 success, Non-zero failure
*/
int se_rsa_sign(se_rsa_t *rsa, se_rsa_context_t *context, void *src, uint32_t src_size, 
                    void *signature, se_rsa_hash_type_t hash_type, uint32_t key_index);

/**
  \brief       rsa verify
  \param[in]   rsa    rsa handle to operate.
  \param[in]   context   Pointer to the rsa context
  \param[in]   src       Pointer to the source data.
  \param[in]   src_size  the source data len
  \param[in]   signature Pointer to the signature
  \param[in]   sig_size  the signature size
  \param[in]   hash_type the source data hash type
  \return      \ref true success, false failure.
*/
bool se_rsa_verify(se_rsa_t *rsa, se_rsa_context_t *context, void *src, uint32_t src_size, 
                        void *signature, se_rsa_hash_type_t hash_type, uint32_t key_index);


/************/

#ifdef __cplusplus
}
#endif

#endif /* _DRV_RSA_H_ */
