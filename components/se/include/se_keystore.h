/*
 * Copyright (C) 2017-2023 Alibaba Group Holding Limited
 */
/******************************************************************************
 * @file     se_keystore.h
 * @brief    Header File for key storage
 * @version  V1.0
 * @date     22. May 2023
 * @model    keystore
 ******************************************************************************/
#ifndef _SE_KEYSTORE_H_
#define _SE_KEYSTORE_H_

#include <stdint.h>
#include "se_device.h"

#ifdef __cplusplus
extern "C" {
#endif

/* aes key: up to 16 keys can be imported*/
typedef enum {
    SE_AES_KEY_INDEX_1 = 1U,     
    SE_AES_KEY_INDEX_2 = 2U,      
    SE_AES_KEY_INDEX_3 = 3U,      
    SE_AES_KEY_INDEX_4 = 4U,     
    SE_AES_KEY_INDEX_5 = 5U,      
    SE_AES_KEY_INDEX_6 = 6U,     
    SE_AES_KEY_INDEX_7 = 7U,      
    SE_AES_KEY_INDEX_8 = 8U,    
    SE_AES_KEY_INDEX_9 = 9U,   
    SE_AES_KEY_INDEX_10 = 10U,     
    SE_AES_KEY_INDEX_11 = 11U,    
    SE_AES_KEY_INDEX_12 = 12U,      
    SE_AES_KEY_INDEX_13 = 13U,      
    SE_AES_KEY_INDEX_14 = 14U,      
    SE_AES_KEY_INDEX_15 = 15U,      
    SE_AES_KEY_INDEX_16 = 16U,      
} se_aes_key_index_t;

/* rsa 1024: up to 16 keys can be imported*/
typedef enum {
	SE_RSA_1024_KEY_SBOOT   = 1U,
    SE_RSA_1024_KEY_INDEX_2 = 2U,      
    SE_RSA_1024_KEY_INDEX_3 = 3U,      
    SE_RSA_1024_KEY_INDEX_4 = 4U,     
    SE_RSA_1024_KEY_INDEX_5 = 5U,      
    SE_RSA_1024_KEY_INDEX_6 = 6U,     
    SE_RSA_1024_KEY_INDEX_7 = 7U,      
    SE_RSA_1024_KEY_INDEX_8 = 8U,    
    SE_RSA_1024_KEY_INDEX_9 = 9U,   
    SE_RSA_1024_KEY_INDEX_10 = 10U,     
    SE_RSA_1024_KEY_INDEX_11 = 11U,    
    SE_RSA_1024_KEY_INDEX_12 = 12U,      
    SE_RSA_1024_KEY_INDEX_13 = 13U,      
    SE_RSA_1024_KEY_INDEX_14 = 14U,      
    SE_RSA_1024_KEY_INDEX_15 = 15U,      
    SE_RSA_1024_KEY_INDEX_16 = 16U,      
} se_rsa_1024_key_index_t;

/* rsa 2048: up to 16 keys can be imported*/
typedef enum {
	SE_RSA_2048_KEY_SBOOT   = 1U,  
    SE_RSA_2048_KEY_INDEX_2 = 2U,      
    SE_RSA_2048_KEY_INDEX_3 = 3U,      
    SE_RSA_2048_KEY_INDEX_4 = 4U,     
    SE_RSA_2048_KEY_INDEX_5 = 5U,      
    SE_RSA_2048_KEY_INDEX_6 = 6U,     
    SE_RSA_2048_KEY_INDEX_7 = 7U,      
    SE_RSA_2048_KEY_INDEX_8 = 8U,    
    SE_RSA_2048_KEY_INDEX_9 = 9U,   
    SE_RSA_2048_KEY_INDEX_10 = 10U,     
    SE_RSA_2048_KEY_INDEX_11 = 11U,    
    SE_RSA_2048_KEY_INDEX_12 = 12U,      
    SE_RSA_2048_KEY_INDEX_13 = 13U,      
    SE_RSA_2048_KEY_INDEX_14 = 14U,      
    SE_RSA_2048_KEY_INDEX_15 = 15U,      
    SE_RSA_2048_KEY_INDEX_16 = 16U,      
} se_rsa_2048_key_index_t;


/************/
typedef enum {
    KEY_TYPE_SM4 = 1,
    KEY_TYPE_AES,
    KEY_TYPE_DES,
    KEY_TYPE_TDES,
    KEY_TYPE_PIN,
    KEY_TYPE_PUK,
    KEY_TYPE_SM2_KPAIR,
    KEY_TYPE_ECC_KPAIR,
    KEY_TYPE_RSA1024_KPAIR,
    KEY_TYPE_RSA2048_KPAIR,
    KEY_TYPE_HMAC,
    KEY_TYPE_ENCRYPT,
} se_key_type_t;

typedef enum {
    HS_PARAMETER_IGNORABLE = 0x00,         /* Except import RSA1024 ang RSA2048 key, the parameter can be ignorable */
    HS_RSA_PRIVATE_KEY_ONLY = 0x01,        /* Private key only exists */
    HS_RSA_PRIVATE_KEY_ONLY_CRT = 0x02,    /* CRT format private key only exists */
    HS_RSA_PUBLIC_KEY_ONLY = 0x10,         /* Public key only exists */
    HS_RSA_KEY_PAIR_ND = 0x11,             /* ND format key pair */
    HS_RSA_KEY_PAIR_CRT = 0x12,            /* CRT format key pair */
} se_key_format_t;

/* the private key is the quintuplet(p, q, qinv, dp, dq) with CRT */
typedef struct{
    uint8_t       *p;
    uint8_t       *q;
    uint8_t       *dp;
    uint8_t       *dq;
    uint8_t       *qinv;
    uint8_t       *n;
    uint8_t       *d;
} se_rsa_key_info_t;
/**
 * enum define for the key usage in cipher
 */
typedef enum {
	KEY_USAGE_SSTORAGE 	= 1,				/* Used for secure storage */
} se_kusage_cipher_t;

/**
 * enum define for the key usage for rsa
 */
typedef enum {
	KEY_USAGE_DATA_EN_DE 	= 1,			/* Used for encrypt and decrypt */
	KEY_USAGE_SBOOT_VERIFY 	   ,			/* Used for secure storage */
} se_kusage_rsa_t;

typedef union {
	se_kusage_cipher_t usage_cipher;
	se_kusage_rsa_t	usage_rsa;
} se_kusage_t;

typedef struct{
    se_key_type_t       key_type;
    se_kusage_t      	key_usage; 
    void                *key;
    uint32_t            key_size;
    se_key_format_t     key_format;
} se_key_info_t;

/**
  \brief       import key
  \param[in]   key_info  Pointer to key information
  \return      error code
*/
int se_import_key(const uint8_t *rootkey, const uint8_t *encrypt_key, const se_key_info_t *key_info);

/**
  \brief       import root key
  \param[in]   rootkey  Pointer to root key
  \param[in]   size  The size of root key
  \return      error code
*/
int se_import_root_key(const uint8_t *old_root_key, const uint8_t *new_root_key, uint32_t size);

/**
  \brief       import encrypt key
  \param[in]   root_key  Pointer to root key 
  \param[in]   encrypt_key  Pointer to encrypt key
  \param[in]   encrypt_key_len  Length of encrypt key
  \return      error code
*/
int se_import_encrypt_key(const uint8_t *root_key, const uint8_t *encrypt_key, uint32_t encrypt_key_len);


/************/

#ifdef __cplusplus
}
#endif

#endif /* _DRV_KEYSTORE_H_ */
