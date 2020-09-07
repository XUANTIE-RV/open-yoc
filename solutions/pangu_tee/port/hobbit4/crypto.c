/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <crypto.h>
#include <string.h>
#include <drv_sha.h>
#include <drv_aes.h>
#include <drv_trng.h>
#include <drv_rsa.h>
#include <tee_debug.h>
#include <soc.h>

#if (CONFIG_ALGO_RSA > 0)
#define RSA1024_KEY_BITS 1024
#define RSA2048_KEY_BITS 2048
#endif

tee_crypto_result tee_hash_get_ctx_size(hash_type_t type, size_t *size)
{
    if (size == NULL) {
        return TEE_CRYPTO_INVALID_ARG;
    }

    if (type == MD5) {
#if (CONFIG_ALGO_MD5 > 0)
        *size = sizeof(hash_context);
#endif
    } else if (type == SHA1) {
#if (CONFIG_ALGO_SHA > 0)
        *size = sizeof(hash_context);
#endif
    } else {
        return TEE_CRYPTO_INVALID_TYPE;
    }

    return TEE_CRYPTO_SUCCESS;
}

tee_crypto_result tee_hash_init(hash_type_t type, void *context)
{
    if (context == NULL) {
        return TEE_CRYPTO_INVALID_CONTEXT;
    }

    if (type == MD5) {
#if (CONFIG_ALGO_MD5 > 0)
        ((hash_context *)context)->hashctx = md5_init();
        md5_starts(((hash_context *)context)->hashctx);
#endif
    } else if (type <= SHA512) {
#if (CONFIG_ALGO_SHA > 0)
        sha_handle_t sha;
        int32_t ret;
        sha_mode_e mode;
        sha = ((hash_context *)context)->hashctx;
        sha = csi_sha_initialize(sha, NULL);

        switch (type) {
            case SHA1:
                mode = SHA_MODE_1;
                break;

            case SHA224:
                mode = SHA_MODE_224;
                break;

            case SHA256:
                mode = SHA_MODE_256;
                break;

            case SHA384:
                mode = SHA_MODE_384;
                break;

            case SHA512:
                mode = SHA_MODE_512;
                break;

            default:
                return TEE_CRYPTO_INVALID_TYPE;
        }

        ret = csi_sha_config(sha, mode, SHA_ENDIAN_MODE_BIG);

        if (ret) {
            return TEE_CRYPTO_ERROR;
        }

        csi_sha_start(sha, NULL);
#endif
    } else {
        return TEE_CRYPTO_INVALID_TYPE;
    }

    ((hash_context *)context)->hashtype = type;

    return TEE_CRYPTO_SUCCESS;
}

tee_crypto_result tee_hash_update(const uint8_t *src, size_t size, void *context)
{
    hash_type_t type;
#if (CONFIG_ALGO_MD5 > 0) || (CONFIG_ALGO_SHA > 0)
    void       *ctx = NULL;
#endif
    if (context == NULL) {
        return TEE_CRYPTO_INVALID_CONTEXT;
    }

    if ((src == NULL) || (size == 0)) {
        return TEE_CRYPTO_INVALID_ARG;
    }

    type = ((hash_context *)context)->hashtype;
#if (CONFIG_ALGO_MD5 > 0) || (CONFIG_ALGO_SHA > 0)
    ctx  = ((hash_context *)context)->hashctx;
#endif

    if (type == MD5) {
#if (CONFIG_ALGO_MD5 > 0)

        md5_update(ctx, src, size);
#endif
    } else if (type <= SHA512) {
#if (CONFIG_ALGO_SHA > 0)
        sha_handle_t sha;
        int32_t ret;

        sha = (sha_handle_t)ctx;

        ret = csi_sha_update(sha, NULL, src, size);

        if (ret) {
            return TEE_CRYPTO_ERROR;
        }

#endif
    } else {
        return TEE_CRYPTO_INVALID_TYPE;
    }

    return TEE_CRYPTO_SUCCESS;
}

tee_crypto_result tee_hash_final(uint8_t *dgst, void *context)
{
    hash_type_t type;
#if (CONFIG_ALGO_MD5 > 0) || (CONFIG_ALGO_SHA > 0)
    void       *ctx = NULL;
#endif
    if (context == NULL) {
        return TEE_CRYPTO_INVALID_CONTEXT;
    }

    if (dgst == NULL) {
        return TEE_CRYPTO_INVALID_ARG;
    }

    type = ((hash_context *)context)->hashtype;
#if (CONFIG_ALGO_MD5 > 0) || (CONFIG_ALGO_SHA > 0)
    ctx  = ((hash_context *)context)->hashctx;
#endif
    if (type == MD5) {
#if (CONFIG_ALGO_MD5 > 0)
        md5_finish(ctx, dgst);
#endif
    } else if (type <= SHA512) {
#if (CONFIG_ALGO_SHA > 0)
        sha_handle_t sha;
        int32_t ret;

        sha = (sha_handle_t)ctx;

        ret = csi_sha_finish(sha, NULL, dgst);

        if (ret) {
            csi_sha_uninitialize(sha);
            return TEE_CRYPTO_ERROR;
        }

        csi_sha_uninitialize(sha);
#endif
    } else {
        return TEE_CRYPTO_INVALID_TYPE;
    }

    return TEE_CRYPTO_SUCCESS;
}

tee_crypto_result tee_hash_reset(void *context)
{
    hash_type_t type;
#if (CONFIG_ALGO_MD5 > 0) || (CONFIG_ALGO_SHA > 0)
    void       *ctx;
#endif
    if (context == NULL) {
        return TEE_CRYPTO_INVALID_CONTEXT;
    }

    type = ((hash_context *)context)->hashtype;
#if (CONFIG_ALGO_MD5 > 0) || (CONFIG_ALGO_SHA > 0)
    ctx  = ((hash_context *)context)->hashctx;
#endif
    if (type == MD5) {
#if (CONFIG_ALGO_MD5 > 0)
        md5_free(ctx);
#endif
    } else if (type <= SHA512) {
#if (CONFIG_ALGO_SHA > 0)
        /*do nothing*/
        (void)ctx;
#endif
    }

    return TEE_CRYPTO_SUCCESS;
}

tee_crypto_result tee_hash_copy_context(void *dst_ctx, void *src_ctx)
{
    return TEE_CRYPTO_SUCCESS;
}

tee_crypto_result tee_hash_digest(hash_type_t type, const uint8_t *src, size_t size,
                                  uint8_t *dgst)
{
    void *ctx;

    if ((size == 0) || (src == NULL)) {
        return TEE_CRYPTO_INVALID_ARG;
    }

    if (type == MD5) {
#if (CONFIG_ALGO_MD5 > 0)
        ctx = md5_init();
        md5_starts(ctx);
        md5_update(ctx, src, size);
        md5_finish(ctx, dgst);
#else
        (void)ctx;
#endif
    } else if (type <= SHA512) {
#if (CONFIG_ALGO_SHA > 0)
        uint8_t sha_ctx[SHA_CONTEXT_SIZE];
        sha_handle_t sha;
        sha_mode_e mode;
        int32_t ret;

        sha = csi_sha_initialize(sha_ctx, NULL);

        switch (type) {
            case SHA1:
                mode = SHA_MODE_1;
                break;

            case SHA224:
                mode = SHA_MODE_224;
                break;

            case SHA256:
                mode = SHA_MODE_256;
                break;

            case SHA384:
                mode = SHA_MODE_384;
                break;

            case SHA512:
                mode = SHA_MODE_512;
                break;

            default:
                return TEE_CRYPTO_INVALID_TYPE;
        }

        ret = csi_sha_config(sha, mode, SHA_ENDIAN_MODE_BIG);

        if (ret) {
            goto err;
        }

        ret = csi_sha_start(sha, NULL);

        if (ret) {
            goto err;
        }

        ret = csi_sha_update(sha, NULL, src, size);

        if (ret) {
            goto err;
        }

        ret = csi_sha_finish(sha, NULL, dgst);

        if (ret) {
            goto err;
        }

        csi_sha_uninitialize(sha);
        return TEE_CRYPTO_SUCCESS;
err:
        csi_sha_uninitialize(sha);
        return TEE_CRYPTO_ERROR;

#endif
    } else {
        return TEE_CRYPTO_INVALID_TYPE;
    }

    return TEE_CRYPTO_SUCCESS;
}

tee_crypto_result tee_des_get_ctx_size(des_type_t type, size_t *size)
{
#if (CONFIG_ALGO_DES3 > 0)

    if (size == NULL) {
        return TEE_CRYPTO_INVALID_ARG;
    }

    if (type == DES_ECB) {
        *size = sizeof(des_context);
    } else {
        return TEE_CRYPTO_INVALID_TYPE;
    }

#endif
    return TEE_CRYPTO_SUCCESS;
}

tee_crypto_result tee_des_init(des_type_t type, bool is_enc, const uint8_t *key,
                               size_t keybytes, const uint8_t *iv, void *context)
{
#if (CONFIG_ALGO_DES3 > 0)
    static uint8_t des_ctx[sizeof(mbedtls_des3_context)];

    if (context == NULL) {
        return TEE_CRYPTO_INVALID_CONTEXT;
    }

    if (key == NULL) {
        return TEE_CRYPTO_INVALID_ARG;
    }

    if (type == DES3_ECB) {

        mbedtls_des3_init((mbedtls_des3_context *)des_ctx);
        ((des_context *)context)->destype = type;
        ((des_context *)context)->desctx  = des_ctx;

        if (keybytes == DES3_2KEYSIZE) {
            if (is_enc) {
                mbedtls_des3_set2key_enc((mbedtls_des3_context *)des_ctx, key);
            } else {
                mbedtls_des3_set2key_dec((mbedtls_des3_context *)des_ctx, key);
            }
        } else if (keybytes == DES3_3KEYSIZE) {
            if (is_enc) {
                mbedtls_des3_set3key_enc((mbedtls_des3_context *)des_ctx, key);
            } else {
                mbedtls_des3_set3key_dec((mbedtls_des3_context *)des_ctx, key);
            }
        } else {
            return TEE_CRYPTO_INVALID_ARG;
        }
    } else if (type == DES_ECB) {
        mbedtls_des_init((mbedtls_des_context *)des_ctx);

        ((des_context *)context)->destype = type;
        ((des_context *)context)->desctx  = des_ctx;

        if (keybytes == DES_KEYSIZE) {
            if (is_enc) {
                mbedtls_des_setkey_enc((mbedtls_des_context *)des_ctx, key);
            } else {
                mbedtls_des_setkey_dec((mbedtls_des_context *)des_ctx, key);
            }
        } else {
            return TEE_CRYPTO_INVALID_ARG;
        }
    } else {
        return TEE_CRYPTO_INVALID_TYPE;
    }

#endif
    return TEE_CRYPTO_SUCCESS;
}

tee_crypto_result tee_des_process(const uint8_t *src, uint8_t *dst, size_t size,
                                  void *context)
{
#if (CONFIG_ALGO_DES3 > 0)
    int i;

    if (context == NULL) {
        return TEE_CRYPTO_INVALID_CONTEXT;
    }

    if ((size == 0) && ((src == NULL) && (dst == NULL))) {
        return TEE_CRYPTO_SUCCESS;
    }

    if (size % DES_BLOCK_SIZE) {
        return TEE_CRYPTO_INVALID_ARG;
    }

    if (((des_context *)context)->destype == DES3_ECB) {
        for (i = 0; i < size / DES_BLOCK_SIZE; i++) {
            mbedtls_des3_crypt_ecb((mbedtls_des3_context *)((des_context *)context)->desctx, src + i * DES_BLOCK_SIZE, dst + i * DES_BLOCK_SIZE);
        }
    }

    if (((des_context *)context)->destype == DES_ECB) {
        for (i = 0; i < size / DES_BLOCK_SIZE; i++) {
            mbedtls_des_crypt_ecb((mbedtls_des_context *)((des_context *)context)->desctx, src + i * DES_BLOCK_SIZE, dst + i * DES_BLOCK_SIZE);
        }
    } else {
        return TEE_CRYPTO_INVALID_TYPE;
    }

#endif
    return TEE_CRYPTO_SUCCESS;
}

tee_crypto_result tee_des_finish(const uint8_t *src, size_t src_size, uint8_t *dst,
                                 size_t *dst_size, sym_padding_t padding, void *context)
{
#if (CONFIG_ALGO_DES3 > 0)

    if (context == NULL) {
        return TEE_CRYPTO_INVALID_CONTEXT;
    }

    if ((src_size == 0) && ((src == NULL) && (dst == NULL))) {
        return TEE_CRYPTO_SUCCESS;
    }

    if (padding == SYM_PKCS5_PAD) {
        uint8_t  tmpblock[DES_BLOCK_SIZE];
        uint32_t padding;
        uint32_t block_num;
        uint32_t block_byte;

        block_num = src_size / DES_BLOCK_SIZE;
        block_byte = src_size % DES_BLOCK_SIZE;

        if (block_byte != 0) {
            padding = DES_BLOCK_SIZE - block_byte;

            memcpy(tmpblock, src + block_num * DES_BLOCK_SIZE, block_byte);
            memset(tmpblock + block_byte, (uint8_t)padding, padding);

            if (((des_context *)context)->destype == DES3_ECB) {
                mbedtls_des3_crypt_ecb((mbedtls_des3_context *)((des_context *)context)->desctx, tmpblock, dst + block_num * DES_BLOCK_SIZE);
            } else if (((des_context *)context)->destype == DES_ECB) {
                mbedtls_des_crypt_ecb((mbedtls_des_context *)((des_context *)context)->desctx, tmpblock, dst + block_num * DES_BLOCK_SIZE);
            }

            *dst_size = src_size + padding;
        } else {
            *dst_size = src_size;
        }
    } else if (padding == SYM_NOPAD) {
        if (src_size % DES_BLOCK_SIZE) {
            return TEE_CRYPTO_INVALID_ARG;
        }

        *dst_size = src_size;
    } else if (padding == SYM_ZERO_PAD) {

    } else {
        return TEE_CRYPTO_INVALID_TYPE;
    }

#endif
    return TEE_CRYPTO_SUCCESS;
}

tee_crypto_result tee_des_reset(void *context)
{
    return TEE_CRYPTO_SUCCESS;
}

tee_crypto_result tee_des_copy_context(void *dst_ctx, void *src_ctx)
{
    return TEE_CRYPTO_SUCCESS;
}

tee_crypto_result tee_aes_get_ctx_size(aes_type_t type, size_t *size)
{
#if (CONFIG_ALGO_AES > 0)

    if (size == NULL) {
        return TEE_CRYPTO_INVALID_ARG;
    }

    if (type == AES_CBC) {
        *size = sizeof(aes_context);
    } else {
        return TEE_CRYPTO_INVALID_TYPE;
    }

#endif
    return TEE_CRYPTO_SUCCESS;
}

tee_crypto_result tee_aes_init(aes_type_t type, bool is_enc, const uint8_t *key1,
                               const uint8_t *key2, size_t keybytes, uint8_t *iv,
                               void *context)
{
#if (CONFIG_ALGO_AES > 0)
    int32_t ret;
    aes_handle_t aes;

    if (context == NULL) {
        return TEE_CRYPTO_INVALID_CONTEXT;
    }

    if (key1 == NULL) {
        return TEE_CRYPTO_INVALID_ARG;
    }

    if (type == AES_CBC && !iv) {
        return TEE_CRYPTO_INVALID_ARG;
    }

    if (type == AES_CBC || type == AES_ECB) {
        aes_crypto_mode_e enc_mode;
        aes_mode_e mode;
        aes_key_len_bits_e key_len_bits;

        ((aes_context *)context)->aestype = type;

        aes = csi_aes_initialize(0, NULL);
        ((aes_context *)context)->aesctx = (void *)aes;

        if (type == AES_CBC) {
            mode = AES_MODE_CBC;
            ((aes_context *)context)->iv = iv;
        } else if (type == AES_ECB) {
            mode = AES_MODE_ECB;
        }

        switch (keybytes) {
            case AES128_KEY_LEN_BYTES:
                key_len_bits = AES_KEY_LEN_BITS_128;
                break;

            case AES192_KEY_LEN_BYTES:
                key_len_bits = AES_KEY_LEN_BITS_192;
                break;

            case AES256_KEY_LEN_BYTES:
                key_len_bits = AES_KEY_LEN_BITS_256;
                break;

            default:
                return TEE_CRYPTO_INVALID_ARG;
        }

        enc_mode = is_enc ? AES_CRYPTO_MODE_ENCRYPT : AES_CRYPTO_MODE_DECRYPT;

        ret = csi_aes_config(aes, mode, key_len_bits, AES_ENDIAN_LITTLE, (uint32_t)((aes_context *)context)->iv);

        if (ret) {
            return TEE_CRYPTO_ERROR;
        }

        ret = csi_aes_set_key(aes, NULL, (void *)key1, key_len_bits, enc_mode);

        if (ret) {
            return TEE_CRYPTO_ERROR;
        }
    } else {
        return TEE_CRYPTO_INVALID_TYPE;
    }

#endif
    return TEE_CRYPTO_SUCCESS;
}

tee_crypto_result tee_aes_process(const uint8_t *src, uint8_t *dst, size_t size,
                                  void *context)
{
#if (CONFIG_ALGO_AES > 0)
    int32_t ret;
    aes_handle_t aes;

    if (context == NULL) {
        return TEE_CRYPTO_INVALID_CONTEXT;
    }

    if ((size == 0) && ((src == NULL) && (dst == NULL))) {
        return TEE_CRYPTO_SUCCESS;
    }

    if (size % AES_BLOCK_SIZE) {
        return TEE_CRYPTO_INVALID_ARG;
    }

    if (((aes_context *)context)->aestype == AES_CBC
        || ((aes_context *)context)->aestype == AES_ECB) {
        aes = (aes_handle_t)((aes_context *)context)->aesctx;
        ret = csi_aes_crypto(aes, NULL, (void *)src, (void *)dst, size, AES_PADDING_MODE_NO);

        if (ret) {
            return TEE_CRYPTO_ERROR;
        }

    } else {
        return TEE_CRYPTO_INVALID_TYPE;
    }

#endif
    return TEE_CRYPTO_SUCCESS;
}

tee_crypto_result tee_aes_finish(const uint8_t *src, size_t src_size, uint8_t *dst,
                                 size_t *dst_size, sym_padding_t padding, void *context)
{
#if (CONFIG_ALGO_AES > 0)
    aes_handle_t aes;

    if (context == NULL) {
        return TEE_CRYPTO_INVALID_CONTEXT;
    }

    aes = (aes_handle_t)((aes_context *)context)->aesctx;

    if ((src_size == 0) && ((src == NULL) && (dst == NULL))) {
        csi_aes_uninitialize(aes);
        return TEE_CRYPTO_SUCCESS;
    }

    if (padding == SYM_PKCS5_PAD) {
        uint8_t  tmpblock[AES_BLOCK_SIZE];
        uint32_t padding;
        uint32_t block_num;
        uint32_t block_byte;
        int32_t ret;

        block_num = src_size / AES_BLOCK_SIZE;
        block_byte = src_size % AES_BLOCK_SIZE;

        if (block_byte != 0) {
            padding = AES_BLOCK_SIZE - block_byte;

            memcpy(tmpblock, src + block_num * AES_BLOCK_SIZE, block_byte);
            memset(tmpblock + block_byte, (uint8_t)padding, padding);

            if (((aes_context *)context)->aestype == AES_CBC) {
                ret = csi_aes_crypto(aes, NULL, (void *)tmpblock, (void *)dst, AES_BLOCK_SIZE, AES_PADDING_MODE_NO);

                if (ret) {
                    csi_aes_uninitialize(aes);
                    return TEE_CRYPTO_ERROR;
                }
            }

            *dst_size = src_size + padding;
        } else {
            *dst_size = src_size;
        }
    } else if (padding == SYM_NOPAD) {
        if (src_size % AES_BLOCK_SIZE) {
            csi_aes_uninitialize(aes);
            return TEE_CRYPTO_INVALID_ARG;
        }

        *dst_size = src_size;
    } else if (padding == SYM_ZERO_PAD) {

    } else {
        csi_aes_uninitialize(aes);
        return TEE_CRYPTO_INVALID_TYPE;
    }

    csi_aes_uninitialize(aes);
#endif

    return TEE_CRYPTO_SUCCESS;
}

tee_crypto_result tee_aes_reset(void *context)
{
    return TEE_CRYPTO_SUCCESS;
}

tee_crypto_result tee_aes_copy_context(void *dst_ctx, void *src_ctx)
{
    return TEE_CRYPTO_SUCCESS;
}

tee_crypto_result tee_seed(uint8_t *seed, size_t seed_len)
{
    return TEE_CRYPTO_SUCCESS;
}

tee_crypto_result tee_rand_gen(uint8_t *buf, size_t len)
{
#if (CONFIG_ALGO_TRNG > 0)
    int err;
    trng_handle_t trng_handle;

    trng_handle = csi_trng_initialize(0, NULL);

    if (!trng_handle) {
        return TEE_CRYPTO_NULL;
    }

    err = csi_trng_get_data(trng_handle, buf, len);

    if (err) {
        csi_trng_uninitialize(trng_handle);
        return TEE_CRYPTO_ERROR;
    }

    csi_trng_uninitialize(trng_handle);
#endif
    return TEE_CRYPTO_SUCCESS;
}

tee_crypto_result tee_rsa_get_keypair_size(size_t keybits, size_t *size)
{
#if (CONFIG_ALGO_RSA > 0)

    if (keybits == RSA1024_KEY_BITS || keybits == RSA2048_KEY_BITS) {
        *size = sizeof(rsa_keypair_t) - sizeof(uint32_t);
    } else {
        return TEE_CRYPTO_INVALID_ARG;
    }

#endif
    return TEE_CRYPTO_SUCCESS;
}

tee_crypto_result tee_rsa_get_pubkey_size(size_t keybits, size_t *size)
{
#if (CONFIG_ALGO_RSA > 0)

    if (keybits == RSA1024_KEY_BITS || keybits == RSA2048_KEY_BITS) {
        *size = sizeof(rsa_pubkey_t) - sizeof(uint32_t);
    } else {
        return TEE_CRYPTO_INVALID_ARG;
    }

#endif
    return TEE_CRYPTO_SUCCESS;
}

tee_crypto_result tee_rsa_init_keypair(size_t keybits,
                                       const uint8_t *n, size_t n_size,
                                       const uint8_t *e, size_t e_size,
                                       const uint8_t *d, size_t d_size,
                                       const uint8_t *p, size_t p_size,
                                       const uint8_t *q, size_t q_size,
                                       const uint8_t *dp, size_t dp_size,
                                       const uint8_t *dq, size_t dq_size,
                                       const uint8_t *qp, size_t qp_size,
                                       rsa_keypair_t *keypair)
{
#if (CONFIG_ALGO_RSA > 0)

    if (((keybits == RSA1024_KEY_BITS &&
          (n_size == (RSA1024_KEY_BITS >> 3)) &&
          (e_size == (RSA1024_KEY_BITS >> 3)) &&
          (d_size == (RSA1024_KEY_BITS >> 3))) ||
         ((keybits == RSA2048_KEY_BITS &&
           (n_size == (RSA2048_KEY_BITS >> 3)) &&
           (e_size == (RSA2048_KEY_BITS >> 3)) &&
           (d_size == (RSA2048_KEY_BITS >> 3))))) &&
        ((n != NULL) &&
         (e != NULL) &&
         (d != NULL))) {
        keypair->e = (void *)e;
        keypair->n = (void *)n;
        keypair->d = (void *)d;
        keypair->keybits = keybits;
        return TEE_CRYPTO_SUCCESS;
    }

    return TEE_CRYPTO_INVALID_ARG;


#endif
    return TEE_CRYPTO_SUCCESS;
}

tee_crypto_result tee_rsa_init_pubkey(size_t keybits,
                                      const uint8_t *n, size_t n_size,
                                      const uint8_t *e, size_t e_size,
                                      rsa_pubkey_t *pubkey)
{
#if (CONFIG_ALGO_RSA > 0)

    if (((keybits == RSA1024_KEY_BITS &&
          (n_size == (RSA1024_KEY_BITS >> 3)) &&
          (e_size == (RSA1024_KEY_BITS >> 3))) ||
         (keybits == RSA2048_KEY_BITS &&
          (n_size == (RSA2048_KEY_BITS >> 3)) &&
          (e_size == (RSA2048_KEY_BITS >> 3)))) &&
        ((n != NULL) &&
         (e != NULL))) {
        pubkey->e = (void *)e;
        pubkey->n = (void *)n;
        pubkey->keybits = keybits;
        return TEE_CRYPTO_SUCCESS;
    }

    return TEE_CRYPTO_INVALID_ARG;

#endif
    return TEE_CRYPTO_SUCCESS;
}

tee_crypto_result tee_rsa_gen_keypair(size_t keybits,
                                      const uint8_t *e, size_t e_size,
                                      rsa_keypair_t *keypair)
{
    return TEE_CRYPTO_SUCCESS;
}

tee_crypto_result tee_rsa_get_key_attr(rsa_key_attr_t attr,
                                       rsa_keypair_t *keypair, void *buffer, size_t *size)
{
#if (CONFIG_ALGO_RSA > 0)

    if ((buffer == NULL) || (keypair == NULL) || (size == NULL)) {
        return TEE_CRYPTO_INVALID_ARG;
    }

    if (attr == RSA_MODULUS) {
        buffer = keypair->n;
        *size = keypair->keybits;
    } else if (attr == RSA_PUBLIC_EXPONENT) {
        buffer = keypair->e;
        *size = keypair->keybits;
    } else if (attr == RSA_PRIVATE_EXPONENT) {
        buffer = keypair->d;
        *size = keypair->keybits;
    } else {
        return TEE_CRYPTO_INVALID_TYPE;
    }

#endif
    return TEE_CRYPTO_SUCCESS;
}

tee_crypto_result tee_rsa_public_encrypt(const rsa_pubkey_t *pub_key,
        const uint8_t *src, size_t src_size,
        uint8_t *dst, size_t *dst_size,
        tee_rsa_padding_t padding)
{
#if (CONFIG_ALGO_RSA > 0)
    int32_t ret;
    rsa_padding_t rsa_padding;
    rsa_handle_t rsa;
    rsa_data_bits_e keybits;

    rsa_padding.hash_type = RSA_HASH_TYPE_SHA1;

    if ((pub_key == NULL) || (src == NULL) || (dst == NULL)) {
        return TEE_CRYPTO_INVALID_ARG;
    }

    if (padding.type == RSAES_PKCS1_V1_5) {
        rsa_padding.padding_type = RSA_PADDING_MODE_PKCS1;
    } else if (padding.type == RSA_NOPAD) {
        rsa_padding.padding_type = RSA_PADDING_MODE_NO;
    } else {
        return TEE_CRYPTO_INVALID_ARG;
    }

    rsa = csi_rsa_initialize(0, NULL);

    if (!rsa) {
        return TEE_CRYPTO_NULL;
    }

    if (pub_key->keybits == 1024) {
        keybits = RSA_DATA_BITS_1024;
    } else if (pub_key->keybits == 2048) {
        keybits = RSA_DATA_BITS_2048;
    } else {
        return TEE_CRYPTO_ERROR;
    }

    ret = csi_rsa_config(rsa, keybits, RSA_ENDIAN_MODE_LITTLE, NULL);

    if (ret) {
        return TEE_CRYPTO_ERROR;
    }

    ret = csi_rsa_encrypt(rsa, pub_key->n, pub_key->e, (void *)src, src_size, (void *)dst, dst_size, rsa_padding);

    if (ret) {
        return TEE_CRYPTO_ERROR;
    }

    ret = csi_rsa_uninitialize(rsa);

    if (ret) {
        return TEE_CRYPTO_ERROR;
    }

#endif
    return TEE_CRYPTO_SUCCESS;
}

tee_crypto_result tee_rsa_private_decrypt(const rsa_keypair_t *priv_key,
        const uint8_t *src, size_t src_size,
        uint8_t *dst, size_t *dst_size,
        tee_rsa_padding_t padding)
{
#if (CONFIG_ALGO_RSA > 0)
    int32_t ret;
    rsa_padding_t rsa_padding;
    rsa_handle_t rsa;
    rsa_data_bits_e keybits;

    rsa_padding.hash_type = RSA_HASH_TYPE_SHA1;

    if ((priv_key == NULL) || (src == NULL) || (dst == NULL)) {
        return TEE_CRYPTO_INVALID_ARG;
    }

    if (padding.type == RSAES_PKCS1_V1_5) {
        rsa_padding.padding_type = RSA_PADDING_MODE_PKCS1;
    } else if (padding.type == RSA_NOPAD) {
        rsa_padding.padding_type = RSA_PADDING_MODE_NO;
    } else {
        return TEE_CRYPTO_INVALID_ARG;
    }

    rsa = csi_rsa_initialize(0, NULL);

    if (!rsa) {
        return TEE_CRYPTO_NULL;
    }

    if (priv_key->keybits == 1024) {
        keybits = RSA_DATA_BITS_1024;
    } else if (priv_key->keybits == 2048) {
        keybits = RSA_DATA_BITS_2048;
    } else {
        return TEE_CRYPTO_ERROR;
    }

    ret = csi_rsa_config(rsa, keybits, RSA_ENDIAN_MODE_LITTLE, NULL);

    if (ret) {
        return TEE_CRYPTO_ERROR;
    }

    ret = csi_rsa_decrypt(rsa, priv_key->n, priv_key->d, (void *)src, src_size, (void *)dst, dst_size, rsa_padding);

    if (ret) {
        return TEE_CRYPTO_ERROR;
    }

    ret = csi_rsa_uninitialize(rsa);

    if (ret) {
        return TEE_CRYPTO_ERROR;
    }

#endif
    return TEE_CRYPTO_SUCCESS;
}

tee_crypto_result tee_rsa_sign(const rsa_keypair_t *priv_key,
                               const uint8_t *dig, size_t dig_size,
                               uint8_t *sig, size_t *sig_size, tee_rsa_padding_t padding)
{
#if (CONFIG_ALGO_RSA > 0)
    int32_t ret;
    rsa_padding_t rsa_padding;
    rsa_handle_t rsa;
    rsa_data_bits_e keybits;

    rsa_padding.padding_type = RSA_PADDING_MODE_PKCS1;

    if ((priv_key == NULL) || (dig == NULL) || (sig == NULL)) {
        return TEE_CRYPTO_INVALID_ARG;
    }

    if (padding.type == RSASSA_PKCS1_V1_5) {
        if (padding.pad.rsassa_v1_5.type == SHA1) {
            rsa_padding.hash_type = RSA_HASH_TYPE_SHA1;
        } else if (padding.pad.rsassa_v1_5.type == MD5) {
            rsa_padding.hash_type = RSA_HASH_TYPE_MD5;
        } else if (padding.pad.rsassa_v1_5.type == SHA256) {
            rsa_padding.hash_type = RSA_HASH_TYPE_SHA256;
        } else {
            return TEE_CRYPTO_INVALID_TYPE;
        }
    } else {
        return TEE_CRYPTO_INVALID_ARG;
    }

    rsa = csi_rsa_initialize(0, NULL);

    if (!rsa) {
        return TEE_CRYPTO_NULL;
    }

    if (priv_key->keybits == 1024) {
        keybits = RSA_DATA_BITS_1024;
    } else if (priv_key->keybits == 2048) {
        keybits = RSA_DATA_BITS_2048;
    } else {
        return TEE_CRYPTO_ERROR;
    }

    ret = csi_rsa_config(rsa, keybits, RSA_ENDIAN_MODE_LITTLE, NULL);

    if (ret) {
        return TEE_CRYPTO_ERROR;
    }

    ret = csi_rsa_sign(rsa, priv_key->n, priv_key->d, (void *)dig, dig_size, (void *)sig, sig_size, rsa_padding);

    if (ret) {
        return TEE_CRYPTO_ERROR;
    }

    ret = csi_rsa_uninitialize(rsa);

    if (ret) {
        return TEE_CRYPTO_ERROR;
    }

#endif
    return TEE_CRYPTO_SUCCESS;
}

tee_crypto_result tee_rsa_verify(const rsa_pubkey_t *pub_key,
                                 const uint8_t *dig, size_t dig_size,
                                 const uint8_t *sig, size_t sig_size,
                                 tee_rsa_padding_t padding, bool *result)
{
#if (CONFIG_ALGO_RSA > 0)
    int32_t ret;
    rsa_padding_t rsa_padding;
    rsa_handle_t rsa;
    rsa_data_bits_e keybits;

    if ((pub_key == NULL) || (dig == NULL) || (sig == NULL)) {
        return TEE_CRYPTO_INVALID_ARG;
    }

    if (padding.type == RSASSA_PKCS1_V1_5) {
        rsa_padding.padding_type = RSA_PADDING_MODE_PKCS1;
        if (padding.pad.rsassa_v1_5.type == SHA1) {
            rsa_padding.hash_type = RSA_HASH_TYPE_SHA1;
        } else if (padding.pad.rsassa_v1_5.type == MD5) {
            rsa_padding.hash_type = RSA_HASH_TYPE_MD5;
        } else if (padding.pad.rsassa_v1_5.type == SHA256) {
            rsa_padding.hash_type = RSA_HASH_TYPE_SHA256;
        } else {
            return TEE_CRYPTO_INVALID_TYPE;
        }
    } else {
        return TEE_CRYPTO_INVALID_ARG;
    }

    rsa = csi_rsa_initialize(0, NULL);

    if (!rsa) {
        return TEE_CRYPTO_NULL;
    }

    if (pub_key->keybits == 1024) {
        keybits = RSA_DATA_BITS_1024;
    } else if (pub_key->keybits == 2048) {
        keybits = RSA_DATA_BITS_2048;
    } else {
        return TEE_CRYPTO_INVALID_KEY;
    }

    ret = csi_rsa_config(rsa, keybits, RSA_ENDIAN_MODE_LITTLE, NULL);

    if (ret) {
        TEE_LOGE("csi_rsa_config %x\n", ret);
        return TEE_CRYPTO_ERROR;
    }

    ret = csi_rsa_verify(rsa, pub_key->n, pub_key->e, (void *)dig, dig_size, (void *)sig, sig_size, (uint8_t *)result, rsa_padding);

    if (ret) {
        TEE_LOGE("csi_rsa_verify %x\n", ret);
        return TEE_CRYPTO_ERROR;
    }

    ret = csi_rsa_uninitialize(rsa);

#endif
    return TEE_CRYPTO_SUCCESS;
}

int tee_crypto_rsa_sw_exptmod_2_2m(void)
{
#if 0
#if (CONFIG_ALGO_RSA > 0)
    extern int rsa_sw_exptmod_2_2m(uint8_t *modulus);

#if (CONFIG_TB_KP > 0)
    uint32_t keyaddr;
    int ret_keysearh;
    ret_keysearh = key_search(MANIFEST_TB_IDX_BASE, KEY_CIDPRIVKEY, &keyaddr, NULL_TB);

    if (ret_keysearh) {
        TEE_LOGE("find KEY_CIDPRIVKEY fail ret %d\n", ret_keysearh);
        return TEE_CRYPTO_ERROR;
    }

    rsa_sw_exptmod_2_2m((uint8_t *)keyaddr);
#else
    rsa_sw_exptmod_2_2m(modulus);
#endif

#endif
#endif
    return 0;
}

