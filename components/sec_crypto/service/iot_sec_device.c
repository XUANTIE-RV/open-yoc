/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */

#include <sec_device.h>
#include <stdio.h>
#include "string.h"
#include "sec_crypto_common.h"
#include "sec_crypto_platform.h"
#include "sec_crypto_sha.h"
#if defined(CONFIG_WITH_SE)
#include <se_aes.h>
#include <se_keystore.h>
#include <se_storage.h>
#endif

#define CHECK_RETURN(ret)                                                      \
        do {                                                                   \
                if (ret != 0) {                                                \
                        while (1)                                              \
                                ;                                              \
                        return -1;                                             \
                }                                                              \
        } while (0);

#define HUK_MAX_SIZE    32
#define GET_AUTH_TYPE(sap)      (sap & 0xff)
#define GET_AUTH_PROTOCOL(sap)      ((sap >> 8) & 0xff)

typedef struct {
    uint8_t huk[HUK_MAX_SIZE];
    uint32_t	huk_size;
} sec_huk_info_t;

static sec_huk_info_t huk_info = {0};
static bool sdk_auth_success = false;

static uint32_t sha256_process(const uint8_t *data_in, size_t size, uint8_t *sha_out)
{
    uint32_t ret = 0;
    sc_sha_t sha;
	sc_sha_context_t context;
    uint32_t out_size = 0;        ///< bytes of calculation result


    CHECK_PARAM(data_in, SC_PARAM_INV);
    CHECK_PARAM(sha_out, SC_PARAM_INV);
    /* STEP 1: init sha */
    ret = sc_sha_init(&sha, 0);
	if (ret != SC_OK)
		return ret;

    /* STEP 2: config sha-1*/
    ret = sc_sha_start(&sha, &context, SC_SHA_MODE_256);
	if (ret != SC_OK) {
		goto __uninit;
	}

    /* STEP 3: first encrypt characters(abc) */
    ret = sc_sha_update(&sha, &context, data_in, size);
	if (ret != SC_OK) {
		goto __uninit;
	}

    /* STEP 4: finish encrypt and output result */
    ret = sc_sha_finish(&sha, &context, sha_out, &out_size);
	if (ret != SC_OK) {
		goto __uninit;
	}

__uninit:
    /* STEP 5: uninit sha */
	sc_sha_uninit(&sha);

    return ret;
}

static uint32_t verify_pubkey(const uint8_t *pubkey_in, size_t pub_size,
							const uint8_t *hash_pubkey)
{
    uint32_t ret = 0;
    uint8_t out[32];            ///< calculation result of sha

    CHECK_PARAM(pubkey_in, SC_PARAM_INV);
    CHECK_PARAM(hash_pubkey, SC_PARAM_INV);
	ret = sha256_process(pubkey_in, pub_size, out);
	if (ret != SC_OK)
		return ret;

	ret = memcmp(out, hash_pubkey, sizeof(out));

	return ret;
}

static uint32_t verify_sig(const uint8_t *message, size_t message_size,
							const uint8_t *pub_in, const uint8_t *ref_sig)
{
    uint32_t ret = 0;
    uint8_t hash_out[32];            ///< calculation result of sha
	sc_rsa_t rsa;
    sc_rsa_context_t context;
    sc_rsa_hash_type_t hash_type = SC_RSA_HASH_TYPE_SHA256;
	const uint8_t publicExponent[] = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01
	};

    CHECK_PARAM(message, SC_PARAM_INV);
    CHECK_PARAM(pub_in, SC_PARAM_INV);
    CHECK_PARAM(ref_sig, SC_PARAM_INV);
	ret = sha256_process(message, message_size, hash_out);
	if (ret != SC_OK)
		return ret;

	ret = sc_rsa_init(&rsa, 0, SC_RSA_KEY_BITS_2048);
	if (ret != SC_OK)
		return ret;
	sc_rsa_detach_callback(&rsa);
	context.padding_type = SC_RSA_PADDING_MODE_PKCS1;
	context.key_bits = 2048;//RSA_KEY_BITS_2048;

	context.n = (void *)pub_in;
	context.e = (void *)publicExponent;
	ret = sc_rsa_verify(&rsa, &context, (void *)hash_out, sizeof(hash_out), 
					(void *)ref_sig, 256,  hash_type);
	if (!ret)
		return ret = SC_VERIFY_FAILED;
	else
		return ret = SC_OK;

	sc_rsa_uninit(&rsa);
	return ret;
}

static uint32_t sc_device_auth_sdk(sec_device_info_t *sec_dev)
{
	uint32_t ret;
	uint8_t pub_key_tmp[256];

    CHECK_PARAM(sec_dev, SC_PARAM_INV);
	if (GET_AUTH_PROTOCOL(sec_dev->sap) == 0x1) {
		/* autheticate public key */
		memcpy(pub_key_tmp, sec_dev->pub_key, sec_dev->pub_key_size);
		ret = verify_pubkey(pub_key_tmp, sizeof(pub_key_tmp),
							sec_dev->pubkey_hash);
		if (ret != SC_OK) {
			goto __exit;
		}
		/* Autheticate HUK */
		memcpy(huk_info.huk, sec_dev->huk, sec_dev->huk_size);
		huk_info.huk_size = sec_dev->huk_size;
		ret = verify_sig(huk_info.huk, huk_info.huk_size,
						pub_key_tmp, sec_dev->sad);
	
	} else if (GET_AUTH_PROTOCOL(sec_dev->sap) == 0x0) {
			/* Autheticate HUK */
			memcpy(huk_info.huk, sec_dev->huk, sec_dev->huk_size);
			huk_info.huk_size = sec_dev->huk_size;
			ret = SC_OK;
	} else {
			ret = SC_INVALID_FORMAT;
	}

__exit:
	if (ret == SC_OK)
		sdk_auth_success = true;
	else
		sdk_auth_success = false;

	return ret;
}


/**
  \brief       The interface of the authentication of secure device
  \param[in]   sec_dev  A pointer to the struct of secure device infomation.
  \return      0: succfull other: error code
*/
uint32_t sc_device_auth_probe(sec_device_info_t *sec_dev)
{
	uint32_t ret;
	
    CHECK_PARAM(sec_dev, SC_PARAM_INV);
	if (GET_AUTH_TYPE(sec_dev->sap) == 0xE0) {
		return sc_device_auth_sdk(sec_dev);
	} else {
			ret = SC_FEATURE_UNAVAILABLE;
			goto __exit;
	}

__exit:
	return ret;
}

/**
  \brief       The interface of the initialization of secure storage
  \param[in]   sec_storage  A pointer to the struct of secure storage infomation.
  \return      0: successfull other: error code
*/
uint32_t sc_device_storage_init(sc_dev_storage_t *sec_storage)
{
    CHECK_PARAM(sec_storage, SC_PARAM_INV);
	/* Check whether SDK is autheticated successfully */
	if (!sdk_auth_success)
		return SC_FEATURE_UNAVAILABLE;

	return sc_aes_init(&sec_storage->aes, sec_storage->idx);

}

/**
  \brief       The interface of the unnitialization of secure storage
  \param[in]   sec_storage  A pointer to the struct of secure storage infomation.
  \return      0: successfull other: error code
*/
void sc_device_storage_uninit(sc_dev_storage_t *sec_storage)
{
	sc_aes_uninit(&sec_storage->aes);
}

static uint32_t derive_sc_device_storage_key(uint8_t *message, size_t size, uint8_t *key)
{
	uint32_t ret;
	uint8_t buffer[48] = {0};
	size_t  hash_size;
	uint32_t counter = 1;
	uint8_t key_tmp[32] = {0};
	uint8_t *p_src;
	uint8_t const_val[] = {0xb4, 0x96, 0x4f, 0x37, 0xe8, 0x87, 0x4c, 0x3b,
							  0x85, 0x1d, 0xc7, 0x97, 0xd0, 0x72, 0x73, 0xc8};

    CHECK_PARAM(message, SC_PARAM_INV);
    CHECK_PARAM(key, SC_PARAM_INV);

	hash_size = sizeof(const_val) + sizeof(counter) + size;
	if (hash_size > sizeof(buffer))
		return SC_BUFFER_TOO_SMALL;

	p_src = buffer;
	memcpy(p_src, const_val, sizeof(const_val));
	p_src += sizeof(const_val);
	memcpy(p_src, &counter, sizeof(counter));
	p_src += sizeof(counter);
	memcpy(p_src, message, size);

	ret = sha256_process(buffer, hash_size, key_tmp);
	if (ret != SC_OK)
		return ret;
	
	/* Use the lowest 16bytes as key */
	memcpy(key, key_tmp, 16);

	return ret;
}

/**
  \brief       The interface of  encrypting secure storage data
  \param[in]   sec_storage  A pointer to the struct of secure storage infomation.
  \param[in]   data_info  A pointer to the struct of data infomation.
  \return      0: successfull other: error code
*/
uint32_t sc_device_storage_encrypt(sc_dev_storage_t *sec_storage, 
                                  sc_dev_data_info_t *data_info)
{
	uint32_t ret;
	uint8_t key[16] = {0};
	uint8_t iv[16] = {0};

    CHECK_PARAM(sec_storage, SC_PARAM_INV);
    CHECK_PARAM(data_info, SC_PARAM_INV);

	/* Check whether SDK is autheticated successfully */
	if (!sdk_auth_success)
		return SC_FEATURE_UNAVAILABLE;

	ret = derive_sc_device_storage_key(huk_info.huk, huk_info.huk_size, key);
	if (ret != SC_OK)
		return ret;

    /* config encrypt key */
    ret = sc_aes_set_encrypt_key(&sec_storage->aes, (void *)key, SC_AES_KEY_LEN_BITS_128);
	if (ret != SC_OK)
		return ret;

    /* do encryption process */
    ret = sc_aes_cbc_encrypt(&sec_storage->aes, (void *)data_info->data_in,
							(void *)data_info->data_out, data_info->data_size, iv);
    
	return ret;
}


/**
  \brief       The interface of decrypting secure storage data
  \param[in]   sec_storage  A pointer to the struct of secure storage infomation.
  \param[in]   data_info  A pointer to the struct of data infomation.
  \return      0: successfull other: error code
*/
uint32_t sc_device_storage_decrypt(sc_dev_storage_t *sec_storage, 
                                  sc_dev_data_info_t *data_info)
{
	uint32_t ret;
	uint8_t key[16] = {0};
	uint8_t iv[16] = {0};

    CHECK_PARAM(sec_storage, SC_PARAM_INV);
    CHECK_PARAM(data_info, SC_PARAM_INV);

	/* Check whether SDK is autheticated successfully */
	if (!sdk_auth_success)
		return SC_FEATURE_UNAVAILABLE;

	ret = derive_sc_device_storage_key(huk_info.huk, huk_info.huk_size, key);
	if (ret != SC_OK)
		return ret;

    /* config decrypt key */
    ret = sc_aes_set_decrypt_key(&sec_storage->aes, (void *)key, SC_AES_KEY_LEN_BITS_128);
	if (ret != SC_OK)
		return ret;

    /* do decryption process */
    ret = sc_aes_cbc_decrypt(&sec_storage->aes, (void *)data_info->data_in,
							(void *)data_info->data_out, data_info->data_size, iv);

	return ret;
}

#if defined(CONFIG_WITH_SE)
/**
  \brief       The interface of the initialization of secure storage with key storage
  \param[in]   sec_storage  A pointer to the struct of secure storage infomation.
  \return      0: successfull other: error code
*/
uint32_t sc_device_storage_init_with_ks(sc_dev_storage_t *sec_storage)
{
	int ret;
    CHECK_PARAM(sec_storage, SC_PARAM_INV);

	ret = se_aes_init(&sec_storage->aes.se_aes);
	if (ret)
		return SC_DRV_FAILED;

	return SC_OK;
}

/**
  \brief       The interface of the unnitialization of secure storage with key storage
  \param[in]   sec_storage  A pointer to the struct of secure storage infomation.
  \return      0: successfull other: error code
*/
void sc_device_storage_uninit_with_ks(sc_dev_storage_t *sec_storage)
{
	se_aes_uninit(&sec_storage->aes.se_aes);
}

/**
  \brief       The interface of  encrypting secure storage data with key storage
  \param[in]   sec_storage  A pointer to the struct of secure storage infomation.
  \param[in]   data_info  A pointer to the struct of data infomation.
  \return      0: successfull other: error code
*/
uint32_t sc_device_storage_encrypt_with_ks(sc_dev_storage_t *sec_storage, 
                                  sc_dev_data_info_t *data_info)
{
	int ret;
	uint8_t iv[16] = {0};

    CHECK_PARAM(sec_storage, SC_PARAM_INV);
    CHECK_PARAM(data_info, SC_PARAM_INV);

    /* do encryption process */
    ret = se_aes_cbc_encrypt(&sec_storage->aes.se_aes, (void *)data_info->data_in,
							 (void *)data_info->data_out, data_info->data_size, iv,
							 KEY_USAGE_SSTORAGE);
	if (ret)
		return SC_DRV_FAILED;

	return SC_OK;
}

/**
  \brief       The interface of decrypting secure storage data with key storage
  \param[in]   sec_storage  A pointer to the struct of secure storage infomation.
  \param[in]   data_info  A pointer to the struct of data infomation.
  \return      0: successfull other: error code
*/
uint32_t sc_device_storage_decrypt_with_ks(sc_dev_storage_t *sec_storage, 
                                  sc_dev_data_info_t *data_info)
{
	uint32_t ret;
	uint8_t iv[16] = {0};

    CHECK_PARAM(sec_storage, SC_PARAM_INV);
    CHECK_PARAM(data_info, SC_PARAM_INV);

    /* do encryption process */
    ret = se_aes_cbc_decrypt(&sec_storage->aes.se_aes, (void *)data_info->data_in,
							 (void *)data_info->data_out, data_info->data_size, iv,
							 KEY_USAGE_SSTORAGE);
	if (ret)
		return SC_DRV_FAILED;

	return SC_OK;
}
/**
  \brief       Get the capacity the secure storage
  \param[out]   size  A pointer to the buffer which will be stored the size of storage.
  \return      0 success, Non-zero failure
*/
uint32_t sc_get_avail_storage_capacity(uint32_t *size)
{
	int ret;

	ret = se_get_avail_storage_capacity(size);
	if (ret)
		return SC_DRV_FAILED;
	return SC_OK;
}

/**
  \brief       Get the capacity the secure storage
  \param[in]   data  A pointer to the buffer which stores the data need to be written.
  \param[in]   size  The size of data which will be written.
  \param[in]   pos  The position which the data will be written to.
  \return      0 success, Non-zero failure
*/
uint32_t sc_write_storage_data(const uint8_t *data, uint32_t size, uint32_t pos)
{
	int ret;

	ret = se_write_storage_data(data, size, pos);
	if (ret)
		return SC_DRV_FAILED;
	return SC_OK;
}

/**
  \brief       Get the capacity the secure storage
  \param[out]   data  A pointer to the buffer which stores the data read out.
  \param[in]   size  The size of data which will be read.
  \param[in]   pos  The position which the data will be read.
  \return      0 success, Non-zero failure
*/
uint32_t  sc_read_storage_data(uint8_t *data , uint32_t size, uint32_t pos)
{
	int ret;

	ret = se_read_storage_data(data, size, pos);
	if (ret)
		return SC_DRV_FAILED;
	return SC_OK;
}
#else
/**
  \brief       The interface of the initialization of secure storage with key storage
  \param[in]   sec_storage  A pointer to the struct of secure storage infomation.
  \return      0: successfull other: error code
*/
uint32_t sc_device_storage_init_with_ks(sc_dev_storage_t *sec_storage)
{
	return SC_NOT_SUPPORT;
}

/**
  \brief       The interface of the unnitialization of secure storage with key storage
  \param[in]   sec_storage  A pointer to the struct of secure storage infomation.
  \return      0: successfull other: error code
*/
void sc_device_storage_uninit_with_ks(sc_dev_storage_t *sec_storage)
{
}

/**
  \brief       The interface of  encrypting secure storage data with key storage
  \param[in]   sec_storage  A pointer to the struct of secure storage infomation.
  \param[in]   data_info  A pointer to the struct of data infomation.
  \return      0: successfull other: error code
*/
uint32_t sc_device_storage_encrypt_with_ks(sc_dev_storage_t *sec_storage, 
                                  sc_dev_data_info_t *data_info)
{
	return SC_NOT_SUPPORT;
}

/**
  \brief       The interface of decrypting secure storage data with key storage
  \param[in]   sec_storage  A pointer to the struct of secure storage infomation.
  \param[in]   data_info  A pointer to the struct of data infomation.
  \return      0: successfull other: error code
*/
uint32_t sc_device_storage_decrypt_with_ks(sc_dev_storage_t *sec_storage, 
                                  sc_dev_data_info_t *data_info)
{
	return SC_NOT_SUPPORT;
}
/**
  \brief       Get the capacity the secure storage
  \param[out]   size  A pointer to the buffer which will be stored the size of storage.
  \return      0 success, Non-zero failure
*/
uint32_t sc_get_avail_storage_capacity(uint32_t *size)
{
	return SC_NOT_SUPPORT;
}

/**
  \brief       Get the capacity the secure storage
  \param[in]   data  A pointer to the buffer which stores the data need to be written.
  \param[in]   size  The size of data which will be written.
  \param[in]   pos  The position which the data will be written to.
  \return      0 success, Non-zero failure
*/
uint32_t sc_write_storage_data(const uint8_t *data, uint32_t size, uint32_t pos)
{
	return SC_NOT_SUPPORT;
}

/**
  \brief       Get the capacity the secure storage
  \param[out]   data  A pointer to the buffer which stores the data read out.
  \param[in]   size  The size of data which will be read.
  \param[in]   pos  The position which the data will be read.
  \return      0 success, Non-zero failure
*/
uint32_t  sc_read_storage_data(uint8_t *data , uint32_t size, uint32_t pos)
{
	return SC_NOT_SUPPORT;
}
#endif