/*
 * Copyright (C) 2017-2023 Alibaba Group Holding Limited
 */
#include <se_aes.h>
#include <cmd.h>
#include <se_dev_internal.h>

#define SE_AES_KEY_LEN_BYTE_128 16
#define SE_AES_KEY_LEN_BYTE_192 24
#define SE_AES_KEY_LEN_BYTE_256 32

typedef enum {
    HS_AES_ECB = 0x01,
    HS_AES_CBC = 0x02,
} hs_aes_mode_t;

typedef enum {
    HS_AES_KEY_INTERNAL = 0x01,
    HS_AES_KEY_IMPORT = 0x02,
} hs_aes_key_type_t;

typedef struct{
    uint8_t key[SE_AES_KEY_LEN_BYTE_256];
    uint32_t key_len;
} se_aes_context_t;

/**
  \brief       Initialize AES Interface. Initializes the resources needed for the AES interface
  \param[in]   aes    operate handle
  \return      error code \ref uint32_t
*/
int se_aes_init(se_aes_t *aes)
{
    CHECK_PARAM(aes, -1);

    aes->ctx = malloc(sizeof(se_aes_context_t));

    return 0;
}

/**
  \brief       De-initialize AES Interface. stops operation and releases the software resources used by the interface
  \param[in]   aes    handle to operate
  \return      None
*/
void se_aes_uninit(se_aes_t *aes)
{
	if (aes) {
		free(aes->ctx);
		aes->ctx = NULL;
	}
}

/**
  \brief       Set encrypt key
  \param[in]   aes     handle to operate
  \param[in]   key        Pointer to the key buf
  \param[in]   key_len    Pointer to \ref sc_aes_key_bits_t
  \return      error code \ref uint32_t
*/
int se_aes_set_encrypt_key(se_aes_t *aes, void *key, se_aes_key_bits_t key_len)
{
    uint32_t key_byte_len;
    se_aes_context_t *context;
    
    CHECK_PARAM(key, -1);
    CHECK_PARAM(aes->ctx, -1);
    
    context = (se_aes_context_t *)aes->ctx;

    if (key_len == SE_AES_KEY_LEN_BITS_128) {
        key_byte_len = SE_AES_KEY_LEN_BYTE_128;
    } else if (key_len == SE_AES_KEY_LEN_BITS_192) {
        key_byte_len = SE_AES_KEY_LEN_BYTE_192;
    } else if (key_len == SE_AES_KEY_LEN_BITS_256) {
        key_byte_len = SE_AES_KEY_LEN_BYTE_256;
    } else {
        return -1;
    }

	if (key_byte_len > sizeof(context->key))
		return -1;
	memcpy(context->key, key, key_byte_len);
    context->key_len = key_byte_len;

    return 0;
}

/**
  \brief       Set decrypt key
  \param[in]   aes        handle to operate
  \param[in]   key        Pointer to the key buf
  \param[in]   key_len    Pointer to \ref sc_aes_key_bits_t
  \return      error code \ref uint32_t
*/
int se_aes_set_decrypt_key(se_aes_t *aes, void *key, se_aes_key_bits_t key_len)
{
    return se_aes_set_encrypt_key(aes, key, key_len);
}

/**
  \brief       Aes ecb encrypt
  \param[in]   aes  handle to operate
  \param[in]   in      Pointer to the Source data
  \param[out]  out     Pointer to the Result data
  \param[in]   size    the Source data size
  \return      error code \ref uint32_t
*/
int se_aes_ecb_encrypt(se_aes_t *aes, void *in, void *out, uint32_t size, uint32_t key_index)
{
    uint32_t ret;
    uint32_t out_size = 0;
    uint32_t key_flag;
    se_aes_context_t *context;

    CHECK_PARAM(aes, -1);
    CHECK_PARAM(aes->ctx, -1);
    CHECK_PARAM(in, -1);
    CHECK_PARAM(out, -1);
    CHECK_PARAM(size, -1);
    CHECK_PARAM(key_index, -1);

    context = (se_aes_context_t *)aes->ctx;

    if (key_index == 0xFFFFFFFF) {
        key_flag = HS_AES_KEY_IMPORT;
    } else {
        key_flag = HS_AES_KEY_INTERNAL;
    }

	se_dev_lock();
    ret = hs_AES_Encrypt(HS_AES_ECB, key_flag, NULL, key_index, (uint8_t *)context->key,
						 context->key_len, (uint8_t *)in, size, (uint8_t *)out,
						 &out_size );
	se_dev_unlock();
    if(ret != SDR_OK) {
        return -1;
    } else {
        return 0;
    }
}

/**
  \brief       Aes ecb decrypt
  \param[in]   aes  handle to operate
  \param[in]   in      Pointer to the Source data
  \param[out]  out     Pointer to the Result data
  \param[in]   size    the Source data size
  \return      error code \ref uint32_t
*/
int se_aes_ecb_decrypt(se_aes_t *aes, void *in, void *out, uint32_t size, uint32_t key_index)
{
    uint32_t ret;
    uint32_t out_size = 0;
    uint32_t key_flag;
    se_aes_context_t *context;

    CHECK_PARAM(aes, -1);
    CHECK_PARAM(aes->ctx, -1);
    CHECK_PARAM(in, -1);
    CHECK_PARAM(out, -1);
    CHECK_PARAM(size, -1);
    CHECK_PARAM(key_index, -1); 

    context = (se_aes_context_t *)aes->ctx;

    if (key_index == 0xFFFFFFFF) {
        key_flag = HS_AES_KEY_IMPORT;
    } else {
        key_flag = HS_AES_KEY_INTERNAL;
    }

	se_dev_lock();
    ret = hs_AES_Decrypt(HS_AES_ECB, key_flag, NULL, key_index, (uint8_t *)context->key,
						 context->key_len, (uint8_t *)in, size, (uint8_t *)out,
						 &out_size );
	se_dev_unlock();
    if(ret != SDR_OK) {
        return -1;
    } else {
        return 0;
    }
}

/**
  \brief       Aes cbc encrypt
  \param[in]   aes  handle to operate
  \param[in]   in      Pointer to the Source data
  \param[out]  out     Pointer to the Result data
  \param[in]   size    the Source data size
  \param[in]   iv      init vector
  \return      error code \ref uint32_t
*/
int se_aes_cbc_encrypt(se_aes_t *aes, void *in, void *out, uint32_t size, void *iv, uint32_t key_index)
{
    uint32_t ret;
    uint32_t out_size = 0;
    uint32_t key_flag;
    se_aes_context_t *context;
    
    CHECK_PARAM(aes, -1);
    CHECK_PARAM(aes->ctx, -1);
    CHECK_PARAM(iv, -1);
    CHECK_PARAM(in, -1);
    CHECK_PARAM(out, -1);
    CHECK_PARAM(size, -1);
    CHECK_PARAM(key_index, -1);

    context = (se_aes_context_t *)aes->ctx;

    if (key_index == 0xFFFFFFFF) {
        key_flag = HS_AES_KEY_IMPORT;
    } else {
        key_flag = HS_AES_KEY_INTERNAL;
    }

	se_dev_lock();
    ret = hs_AES_Encrypt(HS_AES_CBC, key_flag, iv, key_index, (uint8_t *)context->key,
						context->key_len, (uint8_t *)in, size, (uint8_t *)out,
						&out_size );
	se_dev_unlock();
    if(ret != SDR_OK) {
        return -1;
    } else {
        return 0;
    }
}

/**
  \brief       Aes cbc decrypt
  \param[in]   aes  handle to operate
  \param[in]   in      Pointer to the Source data
  \param[out]  out     Pointer to the Result data
  \param[in]   size    the Source data size
  \param[in]   iv      init vector
  \return      error code \ref uint32_t
*/
int se_aes_cbc_decrypt(se_aes_t *aes, void *in, void *out, uint32_t size, void *iv, uint32_t key_index)
{
    uint32_t ret;
    uint32_t out_size = 0;
    uint32_t key_flag;
    se_aes_context_t *context;
    
    CHECK_PARAM(aes, -1);
    CHECK_PARAM(aes->ctx, -1);
    CHECK_PARAM(iv, -1);
    CHECK_PARAM(in, -1);
    CHECK_PARAM(out, -1);
    CHECK_PARAM(size, -1);
    CHECK_PARAM(key_index, -1);

    context = (se_aes_context_t *)aes->ctx;

    if (key_index == 0xFFFFFFFF) {
        key_flag = HS_AES_KEY_IMPORT;
    } else {
        key_flag = HS_AES_KEY_INTERNAL;
    }

	se_dev_lock();
    ret = hs_AES_Decrypt(HS_AES_CBC, key_flag, iv, key_index, (uint8_t *)context->key,
						context->key_len, (uint8_t *)in, size, (uint8_t *)out,
						&out_size);
	se_dev_unlock();
    if(ret != SDR_OK) {
        return -1;
    } else {
        return 0;
    }
}

