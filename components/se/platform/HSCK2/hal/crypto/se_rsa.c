/*
 * Copyright (C) 2017-2023 Alibaba Group Holding Limited
 */
#include <se_rsa.h>
#include <cmd.h>
#include <se_rng.h>
#include <se_dev_internal.h>

typedef enum {
    HS_RSA_KEY_1024  = 0x01,  		
    HS_RSA_KEY_2048  = 0x02,                  
} hs_rsa_key_type_t;

typedef enum {
    RSA_HASH_TYPE_MD5  = 0U,
    RSA_HASH_TYPE_SHA1,
    RSA_HASH_TYPE_SHA224,
    RSA_HASH_TYPE_SHA256,
    RSA_HASH_TYPE_SHA384,
    RSA_HASH_TYPE_SHA512
} rsa_hash_type_e;


#define RSA_1024_BYTE_LEN       (128)
#define RSA_2048_BYTE_LEN       (256)
#define RSA_4096_BYTE_LEN       (512)
#define RSA_TMP_BYTE_LEN         RSA_4096_BYTE_LEN

#define RSA_PKCS1_PADDING_SIZE 11
#define RSA_MD5_OID_LEN         (6 + 8 + 4)
#define RSA_SHA1_OID_LEN        (6 + 5 + 4)
#define RSA_SHA224_OID_LEN      (6 + 9 + 4)
#define RSA_SHA256_OID_LEN      (6 + 9 + 4)
#define RSA_SHA384_OID_LEN      (6 + 9 + 4)
#define RSA_SHA512_OID_LEN      (6 + 9 + 4)

static uint8_t md5_oid[RSA_MD5_OID_LEN] = {
    0x30,0x20,0x30,0x0c,0x06,0x08,0x2a,0x86,0x48,0x86,0xf7,0x0d,0x02,0x05,0x05,0x00,0x04,0x10
};
static uint8_t sha1_oid[RSA_SHA1_OID_LEN] = {
    0x30,0x21,0x30,0x09,0x06,0x05,0x2b,0x0e,0x03,0x02,0x1a,0x05,0x00,0x04,0x14
};
static uint8_t sha224_oid[RSA_SHA224_OID_LEN] = {
    0x30,0x2d,0x30,0x0d,0x06,0x09,0x60,0x86,0x48,0x01,0x65,0x03,0x04,0x02,0x04,0x05,0x00,0x04,0x1c
};
static uint8_t sha256_oid[RSA_SHA256_OID_LEN] = {
    0x30,0x31,0x30,0x0d,0x06,0x09,0x60,0x86,0x48,0x01,0x65,0x03,0x04,0x02,0x01,0x05,0x00,0x04,0x20
};
static uint8_t sha384_oid[RSA_SHA384_OID_LEN] = {
    0x30,0x41,0x30,0x0d,0x06,0x09,0x60,0x86,0x48,0x01,0x65,0x03,0x04,0x02,0x02,0x05,0x00,0x04,0x30
};
static uint8_t sha512_oid[RSA_SHA512_OID_LEN] = {
    0x30,0x51,0x30,0x0d,0x06,0x09,0x60,0x86,0x48,0x01,0x65,0x03,0x04,0x02,0x03,0x05,0x00,0x04,0x40
};

typedef struct {
    se_rsa_hash_type_t  hash_type;
    uint32_t            oid_len;
    uint8_t             *oid;
}RSA_OID;

static RSA_OID g_rsa_oid[] = {
    {RSA_HASH_TYPE_MD5,     RSA_MD5_OID_LEN,    md5_oid},
    {RSA_HASH_TYPE_SHA1,    RSA_SHA1_OID_LEN,   sha1_oid},
    {RSA_HASH_TYPE_SHA224,  RSA_SHA224_OID_LEN, sha224_oid},
    {RSA_HASH_TYPE_SHA256,  RSA_SHA256_OID_LEN, sha256_oid},
    {RSA_HASH_TYPE_SHA384,  RSA_SHA384_OID_LEN, sha384_oid},
    {RSA_HASH_TYPE_SHA512,  RSA_SHA512_OID_LEN, sha512_oid},
};

static uint32_t get_random_byte_nozero(uint8_t *buf, uint32_t count)
{
    uint32_t i = 0;
    uint8_t  tmp = 0;    
    uint32_t ret;

    ret = se_rng_get_bytes(buf, count);
    CHECK_RET_WITH_RET(ret == 0, -1);

    while (i < count) { 
        if (buf[i] == 0) {
            ret = se_rng_get_bytes(&tmp, 1);
            CHECK_RET_WITH_RET(ret == 0, -1);
            buf[i] = tmp;
        }
 
        if(buf[i]) {
            i++;
        }       
    }
    return 0;
}

/**
  \brief       Initialize RSA Interface. 1. Initializes the resources needed for the RSA interface 2.registers event callback function
  \param[in]   rsa  rsa handle to operate.
  \return      \ref 0 success, Non-zero failure
*/
int se_rsa_init(se_rsa_t *rsa)
{
    return 0;
}

/**
  \brief       De-initialize RSA Interface. stops operation and releases the software resources used by the interface
  \param[in]   rsa  rsa handle to operate.
  \return      none
*/
void se_rsa_uninit(se_rsa_t *rsa)
{
}

/**
  \brief       ras encrypt padding of pkcs1
  \param[in]   to           The filled data.
  \param[in]   tlen         The total length of the filled data
  \param[in]   from         Pointer to the source data.
  \param[in]   flen         the source data len
  \return      \ref error code
*/
static int rsaes_pkcs1_enc(unsigned char *to, int tlen, const unsigned char *from, int flen)
{
    int            j;
    unsigned char *p;
    uint32_t       ret;

    CHECK_PARAM(flen <= (tlen - RSA_PKCS1_PADDING_SIZE), -1);

    p = (unsigned char *)to;

    *(p++) = 0;
    *(p++) = 2; /* Public Key BT (Block Type) */

    /* pad out with non-zero random data */
    j = tlen - 3 - flen;

    ret = get_random_byte_nozero(p, j);
    CHECK_RET_WITH_RET(ret == 0, ret);

    p += j;
    
    *(p++) = '\0';

    memcpy(p, from, (unsigned int)flen);

    return 0;
}

/**
  \brief       ras padding decrypt of pkcs1
  \param[in]   to           Pointer to the source data.
  \param[in]   tlen         the source data len 
  \param[in]   from         Pointer to the filled data.
  \param[in]   flen         The total length of the filled data
  \return      \ref error code
*/
static int rsaes_pkcs1_dec(unsigned char *to, uint32_t *tlen, const unsigned char *from, int flen)
{
    int      i;
    int      index = 1;
    int      len   = flen;
    int      padc;
    uint32_t pc;
    int ret = -1;

    CHECK_PARAM(from[1] == 2, -1);

    for (i = index; i < len; i++) {
        if (from[i] == 0) {
            ret  = 0;
            padc = i - index;
            break;
        }
    }

    CHECK_RET_WITH_RET(ret == 0, -1);
    CHECK_RET_WITH_RET(padc >= 8, -1);

    pc = len - padc - 3 + index;
    CHECK_RET_WITH_RET(pc > 0, -1);

    memcpy(to, from + 2 - index + padc + 1, pc);
    *tlen = pc;

    return 0;
}

/**
  \brief       encrypt
  \param[in]   rsa    rsa handle to operate.
  \param[in]   context   Pointer to the rsa context
  \param[in]   src       Pointer to the source data.
  \param[in]   src_size  the source data len
  \param[out]  out       Pointer to the result buffer
  \param[in]   key_index the key index
  \return      \ref 0 success, Non-zero failure
*/
int se_rsa_encrypt(se_rsa_t *rsa, se_rsa_context_t *context, void *src, uint32_t src_size, 
                        void *out, uint32_t *out_size, uint32_t key_index)
{
    uint32_t    ret;
    uint8_t     src_tmp[RSA_TMP_BYTE_LEN]   = {0};
    uint32_t    byte_length = RSA_1024_BYTE_LEN;
    uint32_t    hs_type = HS_RSA_KEY_1024;

    CHECK_PARAM(rsa , -1);
    CHECK_PARAM(context , -1);
    CHECK_PARAM(src , -1);
    CHECK_PARAM(src_size , -1);
    CHECK_PARAM(out , -1);
    CHECK_PARAM((key_index >= 1) &&  (key_index <= 0x10), -1);
    CHECK_PARAM((context->padding_type == SE_RSA_PADDING_MODE_PKCS1)
				|| (context->padding_type == SE_RSA_PADDING_MODE_NO), -1);
    CHECK_PARAM((context->key_bits == SE_RSA_KEY_BITS_1024) ||
                       (context->key_bits == SE_RSA_KEY_BITS_2048), -1);

	if (context->key_bits == SE_RSA_KEY_BITS_1024) {
        byte_length = RSA_1024_BYTE_LEN;
        hs_type =  HS_RSA_KEY_1024;
	} else if (context->key_bits == SE_RSA_KEY_BITS_2048) {
        byte_length = RSA_2048_BYTE_LEN;
        hs_type =  HS_RSA_KEY_2048;
    } else
		return -1;

	if (context->padding_type == SE_RSA_PADDING_MODE_NO) {
		if (src_size != byte_length)
			return -1;
		se_dev_lock();
    	ret = hs_RSA_Encrypt(hs_type, key_index, src_tmp, byte_length, out, out_size);
		se_dev_unlock();
	} else if (context->padding_type == SE_RSA_PADDING_MODE_PKCS1) {
		ret = rsaes_pkcs1_enc(src_tmp, byte_length, src, src_size);
		CHECK_RET_WITH_RET(ret == 0, -1);
		se_dev_lock();
		ret = hs_RSA_Encrypt(hs_type, key_index, src_tmp, byte_length, out, out_size);
		se_dev_unlock();
	} else
		return -1;
    CHECK_RET_WITH_RET(ret == 0, -1);

    return 0;
}

/**
  \brief       decrypt
  \param[in]   rsa    rsa handle to operate.
  \param[in]   context   Pointer to the rsa context
  \param[in]   src       Pointer to the source data.
  \param[in]   src_size  the source data len
  \param[out]  out       Pointer to the result buffer
  \param[in]   key_index the key index
  \return      \ref 0 success, Non-zero failure
*/
int se_rsa_decrypt(se_rsa_t *rsa, se_rsa_context_t *context, void *src, uint32_t src_size, 
                        void *out, uint32_t *out_size, uint32_t key_index)
{
    uint32_t    ret;
    uint8_t     out_tmp[RSA_TMP_BYTE_LEN]   = {0};
    uint32_t    out_tmp_size = 0;
    uint32_t    hs_type = HS_RSA_KEY_1024;
    uint32_t    byte_length = RSA_1024_BYTE_LEN;

    CHECK_PARAM(rsa , -1);
    CHECK_PARAM(context , -1);
    CHECK_PARAM(src , -1);
    CHECK_PARAM(src_size , -1);
    CHECK_PARAM(out , -1);
    CHECK_PARAM((key_index >= 1) &&  (key_index <= 0x10), -1);
    CHECK_PARAM((context->padding_type == SE_RSA_PADDING_MODE_PKCS1)
				|| (context->padding_type == SE_RSA_PADDING_MODE_NO), -1);
    CHECK_PARAM((context->key_bits == SE_RSA_KEY_BITS_1024) ||
                       (context->key_bits == SE_RSA_KEY_BITS_2048), -1);

	if (context->key_bits == SE_RSA_KEY_BITS_1024) {
        byte_length = RSA_1024_BYTE_LEN;
        hs_type =  HS_RSA_KEY_1024;
	} else if (context->key_bits == SE_RSA_KEY_BITS_2048) {
        byte_length = RSA_2048_BYTE_LEN;
        hs_type =  HS_RSA_KEY_2048;
    } else
		return -1;

	if (src_size != byte_length)
		return -1;

	se_dev_lock();
    ret = hs_RSA_Decrypt(hs_type, key_index, src, src_size, out_tmp, &out_tmp_size);
	se_dev_unlock();
    CHECK_RET_WITH_RET(ret == 0, -1);

	if (context->padding_type == SE_RSA_PADDING_MODE_NO) {
		memcpy(out, out_tmp, out_tmp_size);
	} else if (context->padding_type == SE_RSA_PADDING_MODE_PKCS1) {
    	ret = rsaes_pkcs1_dec(out, out_size, out_tmp, out_tmp_size);
    	CHECK_RET_WITH_RET(ret == 0, -1);
	}

    return 0;
}

/**
  \brief       rsa sign/verify padding of pkcs1
  \param[in]   to           The filled data.
  \param[in]   tlen         The total length of the filled data
  \param[in]   from         Pointer to the source data.
  \param[in]   flen         the source data len
  \param[in]   hash_type    the source data hash type
  \return      error code
*/
static int rsassa_pkcs1(unsigned char *to, int tlen, const unsigned char *from,
                            int flen, se_rsa_hash_type_t hash_type)
{
    int            j;
    unsigned char *p;

    CHECK_PARAM(hash_type == g_rsa_oid[hash_type].hash_type, -1);
    CHECK_PARAM(flen <= (tlen - RSA_PKCS1_PADDING_SIZE), -1);

    p = (unsigned char *)to;

    *(p++) = 0;
    *(p++) = 1;
    j      = tlen - 3 - flen - g_rsa_oid[hash_type].oid_len;
    memset(p, 0xff, j);
    p += j;
    *(p++) = '\0';
    memcpy(p, g_rsa_oid[hash_type].oid, g_rsa_oid[hash_type].oid_len);
    p += g_rsa_oid[hash_type].oid_len;
    memcpy(p, from, (unsigned int)flen);

    return 0;
}

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
                    void *signature, se_rsa_hash_type_t hash_type, uint32_t key_index)
{
    uint32_t    ret;
    uint8_t     src_tmp[RSA_TMP_BYTE_LEN]   = {0};
    uint32_t    byte_length = RSA_1024_BYTE_LEN;
    uint32_t    hs_type = HS_RSA_KEY_1024;

    CHECK_PARAM(rsa , -1);
    CHECK_PARAM(context , -1);
    CHECK_PARAM(src , -1);
    CHECK_PARAM(src_size , -1);
    CHECK_PARAM(signature , -1);
    CHECK_PARAM((key_index >= 0x01) &&  (key_index <= 0x10), -1);
    CHECK_PARAM(hash_type == SE_RSA_HASH_TYPE_SHA256,     -1);
    CHECK_PARAM((context->padding_type == SE_RSA_PADDING_MODE_PKCS1)
				|| (context->padding_type == SE_RSA_PADDING_MODE_NO), -1);
    CHECK_PARAM((context->key_bits == SE_RSA_KEY_BITS_1024) ||
                       (context->key_bits == SE_RSA_KEY_BITS_2048), -1);

	if (context->key_bits == SE_RSA_KEY_BITS_1024) {
        byte_length = RSA_1024_BYTE_LEN;
        hs_type =  HS_RSA_KEY_1024;
	} else if (context->key_bits == SE_RSA_KEY_BITS_2048) {
        byte_length = RSA_2048_BYTE_LEN;
        hs_type =  HS_RSA_KEY_2048;
    } else
		return -1;

	if (context->padding_type == SE_RSA_PADDING_MODE_NO) {
		if (src_size != byte_length)
			return -1;
		se_dev_lock();
		ret = hs_RSA_Sign(hs_type, key_index, (uint8_t *)src, src_size, (uint8_t *)signature);
		se_dev_unlock();
	} else if (context->padding_type == SE_RSA_PADDING_MODE_PKCS1) {
		ret = rsassa_pkcs1(src_tmp, byte_length, src, src_size, hash_type);
		CHECK_RET_WITH_RET(ret == 0, -1);
		se_dev_lock();
		ret = hs_RSA_Sign(hs_type, key_index, (uint8_t *)src_tmp, byte_length, (uint8_t *)signature);
		se_dev_unlock();
	}
	CHECK_RET_WITH_RET(ret == 0, -1);

    return 0;
}

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
                        void *signature, se_rsa_hash_type_t hash_type, uint32_t key_index)
{
    uint32_t    ret;
    uint8_t     src_tmp[RSA_TMP_BYTE_LEN]   = {0};
    uint32_t    byte_length = RSA_1024_BYTE_LEN;
    uint32_t    hs_type = HS_RSA_KEY_1024;

    CHECK_PARAM(rsa , false);
    CHECK_PARAM(context , false);
    CHECK_PARAM(src , false);
    CHECK_PARAM(src_size , false);
    CHECK_PARAM(signature , false);
    CHECK_PARAM((key_index >= 0x01) &&  (key_index <= 0x10), false);
    CHECK_PARAM((hash_type == SE_RSA_HASH_TYPE_SHA256)
				|| (hash_type == SE_RSA_HASH_TYPE_SHA1), false);
    CHECK_PARAM(context->padding_type == SE_RSA_PADDING_MODE_PKCS1, false);
    CHECK_PARAM((context->key_bits == SE_RSA_KEY_BITS_1024) ||
                       (context->key_bits == SE_RSA_KEY_BITS_2048), false);

	if (context->key_bits == SE_RSA_KEY_BITS_1024) {
        byte_length = RSA_1024_BYTE_LEN;
        hs_type =  HS_RSA_KEY_1024;
	} else if (context->key_bits == SE_RSA_KEY_BITS_2048) {
        byte_length = RSA_2048_BYTE_LEN;
        hs_type =  HS_RSA_KEY_2048;
    } else
		return false;

	if (context->padding_type == SE_RSA_PADDING_MODE_NO) {
		if (src_size != byte_length)
			return false;
		se_dev_lock();
		ret = hs_RSA_Verify(hs_type, key_index, (uint8_t *)src, src_size, (uint8_t *)signature);
		se_dev_unlock();
	} else if (context->padding_type == SE_RSA_PADDING_MODE_PKCS1) {
		ret = rsassa_pkcs1(src_tmp, byte_length, src, src_size, hash_type);
		CHECK_RET_WITH_RET(ret == 0, false);
		se_dev_lock();
		ret = hs_RSA_Verify(hs_type, key_index, (uint8_t *)src_tmp, byte_length, (uint8_t *)signature);
		se_dev_unlock();
	}
	CHECK_RET_WITH_RET(ret == 0, false);
    return true;
}

