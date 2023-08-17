/*
 * Copyright (C) 2017-2023 Alibaba Group Holding Limited
 */
#include <se_keystore.h>
#include <se_rng.h>
#include <cmd.h>

#define SE_ROOT_KEY_SIZE		16
#define SE_ENC_KEY_SIZE			16
#define SE_ENCRYPT_KEY_INDEX	1

static uint8_t se_rootkey[SE_ROOT_KEY_SIZE] = {0};
static uint8_t se_encryptKey[SE_ENC_KEY_SIZE] =  {
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 
    0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x01
};

typedef enum {
    HS_ROOT_KEY = 0x01,        
    HS_ENCRYPT_KEY = 0x02,       
    HS_ECC_CRYPT_KEY = 0x03,    
    HS_RSA1024_CRYPT_KEY = 0x04,        
    HS_RSA2048_CRYPT_KEY = 0x05,      
    HS_AES_CRYPT_KEY = 0x06, 
} hs_key_type_t;

#define SE_RSA_1024_PRIME_LEN               64
#define SE_RSA_1024_PRIVATE_KEY_LEN         (SE_RSA_1024_PRIME_LEN * 2)
#define SE_RSA_1024_PRIVATE_KEY_CRT_LEN     (SE_RSA_1024_PRIME_LEN * 5)
#define SE_RSA_1024_PUBLIC_KEY_LEN          (SE_RSA_1024_PRIME_LEN * 2)

#define SE_RSA_2048_PRIME_LEN               128
#define SE_RSA_2048_PRIVATE_KEY_LEN         (SE_RSA_2048_PRIME_LEN * 2)
#define SE_RSA_2048_PRIVATE_KEY_CRT_LEN     (SE_RSA_2048_PRIME_LEN * 5)
#define SE_RSA_2048_PUBLIC_KEY_LEN          (SE_RSA_2048_PRIME_LEN * 2)

static int se_import_rsa1024_key(const uint8_t *root_key, const uint8_t *encrypt_key, const se_key_info_t *key_info) 
{
    uint32_t ret;
    uint8_t  key_tmp[512] = {0};
    uint32_t key_size = 0;
    se_rsa_key_info_t *rsa_key_info;

    CHECK_PARAM(key_info, -1);
    CHECK_PARAM(key_info->key, -1);

    rsa_key_info = (se_rsa_key_info_t *)key_info->key;

    if (key_info->key_format == HS_RSA_KEY_PAIR_ND) {
		CHECK_PARAM(rsa_key_info->n, -1);
		CHECK_PARAM(rsa_key_info->d, -1);
        memcpy(key_tmp, rsa_key_info->n, SE_RSA_1024_PUBLIC_KEY_LEN);
		key_size += SE_RSA_1024_PUBLIC_KEY_LEN;
        memcpy(key_tmp + key_size, rsa_key_info->d, SE_RSA_1024_PRIVATE_KEY_LEN);
        key_size += SE_RSA_1024_PRIVATE_KEY_LEN;
    } else if (key_info->key_format == HS_RSA_KEY_PAIR_CRT) {
		CHECK_PARAM(rsa_key_info->n, -1);
		CHECK_PARAM(rsa_key_info->p, -1);
		CHECK_PARAM(rsa_key_info->q, -1);
		CHECK_PARAM(rsa_key_info->dp, -1);
		CHECK_PARAM(rsa_key_info->qinv, -1);
        memcpy(key_tmp, rsa_key_info->n, SE_RSA_1024_PUBLIC_KEY_LEN);
		key_size += SE_RSA_1024_PUBLIC_KEY_LEN;
        memcpy(key_tmp + key_size, rsa_key_info->p, SE_RSA_1024_PRIME_LEN);
		key_size += SE_RSA_1024_PRIME_LEN;
        memcpy(key_tmp + key_size, rsa_key_info->q, SE_RSA_1024_PRIME_LEN);
		key_size += SE_RSA_1024_PRIME_LEN;
        memcpy(key_tmp + key_size, rsa_key_info->dp, SE_RSA_1024_PRIME_LEN);
		key_size += SE_RSA_1024_PRIME_LEN;
        memcpy(key_tmp + key_size, rsa_key_info->dq, SE_RSA_1024_PRIME_LEN);
		key_size += SE_RSA_1024_PRIME_LEN;
        memcpy(key_tmp + key_size, rsa_key_info->qinv, SE_RSA_1024_PRIME_LEN);
		key_size += SE_RSA_1024_PRIME_LEN;
    } else if (key_info->key_format == HS_RSA_PRIVATE_KEY_ONLY) {
		CHECK_PARAM(rsa_key_info->d, -1);
        memcpy(key_tmp , rsa_key_info->d, SE_RSA_1024_PRIVATE_KEY_LEN);
        key_size = SE_RSA_1024_PRIVATE_KEY_LEN;
    } else if (key_info->key_format == HS_RSA_PRIVATE_KEY_ONLY_CRT) {
		CHECK_PARAM(rsa_key_info->p, -1);
		CHECK_PARAM(rsa_key_info->q, -1);
		CHECK_PARAM(rsa_key_info->dp, -1);
		CHECK_PARAM(rsa_key_info->qinv, -1);
        memcpy(key_tmp, rsa_key_info->p, SE_RSA_1024_PRIME_LEN);
		key_size += SE_RSA_1024_PRIME_LEN;
        memcpy(key_tmp + key_size, rsa_key_info->q, SE_RSA_1024_PRIME_LEN);
		key_size += SE_RSA_1024_PRIME_LEN;
        memcpy(key_tmp + key_size, rsa_key_info->dp, SE_RSA_1024_PRIME_LEN);
		key_size += SE_RSA_1024_PRIME_LEN;
        memcpy(key_tmp + key_size, rsa_key_info->dq, SE_RSA_1024_PRIME_LEN);
		key_size += SE_RSA_1024_PRIME_LEN;
        memcpy(key_tmp + key_size, rsa_key_info->qinv, SE_RSA_1024_PRIME_LEN);
		key_size += SE_RSA_1024_PRIME_LEN;
    } else if (key_info->key_format == HS_RSA_PUBLIC_KEY_ONLY) {
		CHECK_PARAM(rsa_key_info->n, -1);
        memcpy(key_tmp, rsa_key_info->n, SE_RSA_1024_PUBLIC_KEY_LEN);
        key_size = SE_RSA_1024_PUBLIC_KEY_LEN;
    } else {
        return -1;
    }

    ret = hs_Import_Key(HS_RSA1024_CRYPT_KEY, (SGD_UCHAR *)root_key, SE_ENCRYPT_KEY_INDEX,
						(SGD_UCHAR *)encrypt_key, key_info->key_usage.usage_rsa, (uint8_t *)key_tmp,
						key_size, key_info->key_format);
	CHECK_RET_WITH_RET(ret == SDR_OK, -1);

    return 0;
}

static int se_import_rsa2048_key(const uint8_t *root_key, const uint8_t *encrypt_key, const se_key_info_t *key_info) 
{
    uint32_t ret;
    uint8_t  key_tmp[1024] = {0};
    uint32_t key_size = 0;
    se_rsa_key_info_t *rsa_key_info;

    CHECK_PARAM(key_info, -1);
    CHECK_PARAM(key_info->key, -1);

    rsa_key_info = (se_rsa_key_info_t *)key_info->key;

    if (key_info->key_format == HS_RSA_KEY_PAIR_ND) {
		CHECK_PARAM(rsa_key_info->n, -1);
		CHECK_PARAM(rsa_key_info->d, -1);
        memcpy(key_tmp, rsa_key_info->n, SE_RSA_2048_PUBLIC_KEY_LEN);
		key_size += SE_RSA_2048_PUBLIC_KEY_LEN;
        memcpy(key_tmp + key_size, rsa_key_info->d, SE_RSA_2048_PRIVATE_KEY_LEN);
		key_size += SE_RSA_2048_PRIVATE_KEY_LEN;
    } else if (key_info->key_format == HS_RSA_KEY_PAIR_CRT) {
		CHECK_PARAM(rsa_key_info->n, -1);
		CHECK_PARAM(rsa_key_info->p, -1);
		CHECK_PARAM(rsa_key_info->q, -1);
		CHECK_PARAM(rsa_key_info->dp, -1);
		CHECK_PARAM(rsa_key_info->dq, -1);
		CHECK_PARAM(rsa_key_info->qinv, -1);
        memcpy(key_tmp, rsa_key_info->n, SE_RSA_2048_PUBLIC_KEY_LEN);
		key_size += SE_RSA_2048_PUBLIC_KEY_LEN;
        memcpy(key_tmp + key_size, rsa_key_info->p, SE_RSA_2048_PRIME_LEN);
		key_size += SE_RSA_2048_PRIME_LEN;
        memcpy(key_tmp + key_size, rsa_key_info->q, SE_RSA_2048_PRIME_LEN);
		key_size += SE_RSA_2048_PRIME_LEN;
        memcpy(key_tmp + key_size, rsa_key_info->dp, SE_RSA_2048_PRIME_LEN);
		key_size += SE_RSA_2048_PRIME_LEN;
        memcpy(key_tmp + key_size, rsa_key_info->dq, SE_RSA_2048_PRIME_LEN);
		key_size += SE_RSA_2048_PRIME_LEN;
        memcpy(key_tmp + key_size, rsa_key_info->qinv, SE_RSA_2048_PRIME_LEN);
		key_size += SE_RSA_2048_PRIME_LEN;
    } else if (key_info->key_format == HS_RSA_PRIVATE_KEY_ONLY) {
    	CHECK_PARAM(rsa_key_info->d, -1);
        memcpy(key_tmp , rsa_key_info->d, SE_RSA_2048_PRIVATE_KEY_LEN);
        key_size = SE_RSA_2048_PRIVATE_KEY_LEN;
    } else if (key_info->key_format == HS_RSA_PRIVATE_KEY_ONLY_CRT) {
		CHECK_PARAM(rsa_key_info->p, -1);
		CHECK_PARAM(rsa_key_info->q, -1);
		CHECK_PARAM(rsa_key_info->dp, -1);
		CHECK_PARAM(rsa_key_info->dq, -1);
		CHECK_PARAM(rsa_key_info->qinv, -1);
        memcpy(key_tmp, rsa_key_info->p, SE_RSA_2048_PRIME_LEN);
		key_size += SE_RSA_2048_PRIME_LEN;
        memcpy(key_tmp + key_size, rsa_key_info->q, SE_RSA_2048_PRIME_LEN);
		key_size += SE_RSA_2048_PRIME_LEN;
        memcpy(key_tmp + key_size, rsa_key_info->dp, SE_RSA_2048_PRIME_LEN);
		key_size += SE_RSA_2048_PRIME_LEN;
        memcpy(key_tmp + key_size, rsa_key_info->dq, SE_RSA_2048_PRIME_LEN);
		key_size += SE_RSA_2048_PRIME_LEN;
        memcpy(key_tmp + key_size, rsa_key_info->qinv, SE_RSA_2048_PRIME_LEN);
		key_size += SE_RSA_2048_PRIME_LEN;
    } else if (key_info->key_format == HS_RSA_PUBLIC_KEY_ONLY) {
		CHECK_PARAM(rsa_key_info->n, -1);
        memcpy(key_tmp, rsa_key_info->n, SE_RSA_2048_PUBLIC_KEY_LEN);
        key_size = SE_RSA_2048_PUBLIC_KEY_LEN;
    } else {
        return -1;
    }

    ret = hs_Import_Key(HS_RSA2048_CRYPT_KEY, (SGD_UCHAR *)root_key, SE_ENCRYPT_KEY_INDEX,
						(SGD_UCHAR *)encrypt_key, key_info->key_usage.usage_rsa, (uint8_t *)key_tmp,
						key_size, key_info->key_format);
    CHECK_RET_WITH_RET(ret == SDR_OK, -1);

    return 0;
}

static int se_import_aes_key(const uint8_t *root_key, const uint8_t *encrypt_key, const se_key_info_t *key_info) 
{
    uint32_t ret;

	CHECK_PARAM(key_info, -1);
	CHECK_PARAM(key_info->key, -1);

    ret = hs_Import_Key(HS_AES_CRYPT_KEY, (SGD_UCHAR *)root_key, SE_ENCRYPT_KEY_INDEX,
						(SGD_UCHAR *)encrypt_key, key_info->key_usage.usage_cipher, (uint8_t *)key_info->key,
						key_info->key_size, key_info->key_format);
    CHECK_RET_WITH_RET(ret == SDR_OK, -1);

	return 0;
}

/**
  \brief       import encrypt key
  \param[in]   root_key  Pointer to root key 
  \param[in]   encrypt_key  Pointer to encrypt key
  \param[in]   encrypt_key_len  Length of encrypt key
  \return      error code
*/
int se_import_encrypt_key(const uint8_t *root_key, const uint8_t *encrypt_key, uint32_t encrypt_key_len)
{
    uint32_t ret;
	CHECK_PARAM(root_key, -1);
	CHECK_PARAM(root_key, -1);
	CHECK_PARAM(encrypt_key_len == 16, -1);

    ret = hs_Import_Key(HS_ENCRYPT_KEY, (SGD_UCHAR *)root_key, 0xFF, NULL, SE_ENCRYPT_KEY_INDEX,
						(SGD_UCHAR *)encrypt_key, encrypt_key_len, 0xFF);

    CHECK_RET_WITH_RET(ret == SDR_OK, -1);
	/* Update encrypt key */
	memcpy(se_encryptKey, encrypt_key, encrypt_key_len);

	return 0;
}

/**
  \brief       import root key
  \param[in]   root_key  Pointer to root key
  \return      error code
*/
int se_import_root_key(const uint8_t *old_root_key, const uint8_t *new_root_key, uint32_t size)
{
    uint32_t ret;
	CHECK_PARAM(old_root_key, -1);
    CHECK_PARAM(new_root_key, -1);
	CHECK_PARAM(size == 16, -1);

    ret = hs_Import_Key(HS_ROOT_KEY, (SGD_UCHAR *)old_root_key, 0xFF, NULL, 0xFF, (SGD_UCHAR *)new_root_key, size, 0xFF);
    CHECK_RET_WITH_RET(ret == SDR_OK, -1);

	/* Update ROOT key */
	memcpy(se_rootkey, new_root_key, size);

    return 0;
}

/**
  \brief       import key
  \param[in]   key_info  Pointer to key information
  \return      error code
*/
int se_import_key(const uint8_t *root_key, const uint8_t *encrypt_key, const se_key_info_t *key_info)
{
    int ret = -1;
	CHECK_PARAM(key_info, -1);

    switch (key_info->key_type) {
        case KEY_TYPE_RSA1024_KPAIR:
            ret = se_import_rsa1024_key(root_key, encrypt_key, key_info);
			break;
        case KEY_TYPE_RSA2048_KPAIR:
            ret = se_import_rsa2048_key(root_key, encrypt_key, key_info);
			break;
        case KEY_TYPE_AES:
            ret = se_import_aes_key(root_key, encrypt_key, key_info);
			break;
        default:
            return -1;
    }

    return ret;
}