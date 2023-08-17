/*
 * Copyright (C) 2017-2023 Alibaba Group Holding Limited
 */
/******************************************************************************
 * @file     se_aes.h
 * @brief    Header File for AES engine
 * @version  V1.0
 * @date     22. May 2023
 * @model    aes
 ******************************************************************************/
#ifndef _SE_AES_H_
#define _SE_AES_H_


#include <stdint.h>
#include "se_device.h"


#ifdef __cplusplus
extern "C" {
#endif

/************/

typedef enum {
    SE_AES_KEY_LEN_BITS_128 = 0U, ///< 128 Data bits
    SE_AES_KEY_LEN_BITS_192,      ///< 192 Data bits
    SE_AES_KEY_LEN_BITS_256       ///< 256 Data bits
} se_aes_key_bits_t;

typedef struct{
    aos_mutex_t mutex;
    void      *ctx;
} se_aes_t;

/**
  \brief       Initialize AES Interface. Initializes the resources needed for the AES interface
  \param[in]   aes    operate handle
  \return      error code \ref uint32_t
*/
int se_aes_init(se_aes_t *aes);

/**
  \brief       De-initialize AES Interface. stops operation and releases the software resources used by the interface
  \param[in]   aes    handle to operate
  \return      None
*/
void se_aes_uninit(se_aes_t *aes);

/**
  \brief       Set encrypt key
  \param[in]   aes     handle to operate
  \param[in]   key        Pointer to the key buf
  \param[in]   key_len    Pointer to \ref sc_aes_key_bits_t
  \return      error code \ref uint32_t
*/
int se_aes_set_encrypt_key(se_aes_t *aes, void *key, se_aes_key_bits_t key_len);

/**
  \brief       Set decrypt key
  \param[in]   aes        handle to operate
  \param[in]   key        Pointer to the key buf
  \param[in]   key_len    Pointer to \ref sc_aes_key_bits_t
  \return      error code \ref uint32_t
*/
int se_aes_set_decrypt_key(se_aes_t *aes, void *key, se_aes_key_bits_t key_len);


/**
  \brief       Aes ecb encrypt
  \param[in]   aes  handle to operate
  \param[in]   in      Pointer to the Source data
  \param[out]  out     Pointer to the Result data
  \param[in]   size    The Source data size
  \param[in]   key_index    The index of key stored in key store, 0xffffffff indicate the key in input by user
  \return      error code \ref uint32_t
*/
int se_aes_ecb_encrypt(se_aes_t *aes, void *in, void *out, uint32_t size, uint32_t key_index);


/**
  \brief       Aes ecb decrypt
  \param[in]   aes  handle to operate
  \param[in]   in      Pointer to the Source data
  \param[out]  out     Pointer to the Result data
  \param[in]   size    the Source data size
  \param[in]   key_index    The index of key stored in key store, 0xffffffff indicate the key in input by user
  \return      error code \ref uint32_t
*/
int se_aes_ecb_decrypt(se_aes_t *aes, void *in, void *out, uint32_t size, uint32_t key_index);

/**
  \brief       Aes cbc encrypt
  \param[in]   aes  handle to operate
  \param[in]   in      Pointer to the Source data
  \param[out]  out     Pointer to the Result data
  \param[in]   size    the Source data size
  \param[in]   iv      init vector
  \param[in]   key_index    The index of key stored in key store, 0xffffffff indicate the key in input by user
  \return      error code \ref uint32_t
*/
int se_aes_cbc_encrypt(se_aes_t *aes, void *in, void *out, uint32_t size, void *iv, uint32_t key_index);

/**
  \brief       Aes cbc decrypt
  \param[in]   aes  handle to operate
  \param[in]   in      Pointer to the Source data
  \param[out]  out     Pointer to the Result data
  \param[in]   size    the Source data size
  \param[in]   iv      init vector
  \param[in]   key_index    The index of key stored in key store, 0xffffffff indicate the key in input by user
  \return      error code \ref uint32_t
*/
int se_aes_cbc_decrypt(se_aes_t *aes, void *in, void *out, uint32_t size, void *iv, uint32_t key_index);


/************/

#ifdef __cplusplus
}
#endif

#endif /* _DRV_AES_H_ */
