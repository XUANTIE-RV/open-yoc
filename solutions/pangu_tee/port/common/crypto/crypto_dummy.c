/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <crypto.h>
#include <string.h>

#define _TEE_WEAK_ __attribute__((weak))

#if (CONFIG_ALGO_RSA > 0)
#define RSA1024_KEY_BITS 1024
#define RSA2048_KEY_BITS 2048
#endif

_TEE_WEAK_ tee_crypto_result tee_hash_get_ctx_size(hash_type_t type, size_t *size)
{
    return TEE_CRYPTO_SUCCESS;
}

_TEE_WEAK_ tee_crypto_result tee_hash_init(hash_type_t type, void *context)
{
    return TEE_CRYPTO_SUCCESS;
}

_TEE_WEAK_ tee_crypto_result tee_hash_update(const uint8_t *src, size_t size, void *context)
{
    return TEE_CRYPTO_SUCCESS;
}

_TEE_WEAK_ tee_crypto_result tee_hash_final(uint8_t *dgst, void *context)
{
    return TEE_CRYPTO_SUCCESS;
}

_TEE_WEAK_ tee_crypto_result tee_hash_reset(void *context)
{
    return TEE_CRYPTO_SUCCESS;
}

_TEE_WEAK_ tee_crypto_result tee_hash_copy_context(void *dst_ctx, void *src_ctx)
{
    return TEE_CRYPTO_SUCCESS;
}

_TEE_WEAK_ tee_crypto_result tee_hash_digest(hash_type_t type, const uint8_t *src, size_t size,
        uint8_t *dgst)
{
    return TEE_CRYPTO_SUCCESS;
}

_TEE_WEAK_ tee_crypto_result tee_des_get_ctx_size(des_type_t type, size_t *size)
{
    return TEE_CRYPTO_SUCCESS;
}

_TEE_WEAK_ tee_crypto_result tee_des_init(des_type_t type, bool is_enc, const uint8_t *key,
        size_t keybytes, const uint8_t *iv, void *context)
{
    return TEE_CRYPTO_SUCCESS;
}

_TEE_WEAK_ tee_crypto_result tee_des_process(const uint8_t *src, uint8_t *dst, size_t size,
        void *context)
{
    return TEE_CRYPTO_SUCCESS;
}

_TEE_WEAK_ tee_crypto_result tee_des_finish(const uint8_t *src, size_t src_size, uint8_t *dst,
        size_t *dst_size, sym_padding_t padding, void *context)
{
    return TEE_CRYPTO_SUCCESS;
}

_TEE_WEAK_ tee_crypto_result tee_des_reset(void *context)
{
    return TEE_CRYPTO_SUCCESS;
}

_TEE_WEAK_ tee_crypto_result tee_des_copy_context(void *dst_ctx, void *src_ctx)
{
    return TEE_CRYPTO_SUCCESS;
}

_TEE_WEAK_ tee_crypto_result tee_aes_get_ctx_size(aes_type_t type, size_t *size)
{
    return TEE_CRYPTO_SUCCESS;
}

_TEE_WEAK_ tee_crypto_result tee_aes_init(aes_type_t type, bool is_enc, const uint8_t *key1,
        const uint8_t *key2, size_t keybytes, uint8_t *iv,
        void *context)
{
    return TEE_CRYPTO_SUCCESS;
}

_TEE_WEAK_ tee_crypto_result tee_aes_process(const uint8_t *src, uint8_t *dst, size_t size,
        void *context)
{
    return TEE_CRYPTO_SUCCESS;
}

_TEE_WEAK_ tee_crypto_result tee_aes_finish(const uint8_t *src, size_t src_size, uint8_t *dst,
        size_t *dst_size, sym_padding_t padding, void *context)
{
    return TEE_CRYPTO_SUCCESS;
}

_TEE_WEAK_ tee_crypto_result tee_aes_reset(void *context)
{
    return TEE_CRYPTO_SUCCESS;
}

_TEE_WEAK_ tee_crypto_result tee_aes_copy_context(void *dst_ctx, void *src_ctx)
{
    return TEE_CRYPTO_SUCCESS;
}

_TEE_WEAK_ tee_crypto_result tee_seed(uint8_t *seed, size_t seed_len)
{
    return TEE_CRYPTO_SUCCESS;
}

_TEE_WEAK_ tee_crypto_result tee_rand_gen(uint8_t *buf, size_t len)
{
    return TEE_CRYPTO_SUCCESS;
}

_TEE_WEAK_ tee_crypto_result tee_rsa_get_keypair_size(size_t keybits, size_t *size)
{
    return TEE_CRYPTO_SUCCESS;
}

_TEE_WEAK_ tee_crypto_result tee_rsa_get_pubkey_size(size_t keybits, size_t *size)
{
    return TEE_CRYPTO_SUCCESS;
}

_TEE_WEAK_ tee_crypto_result tee_rsa_init_keypair(size_t keybits,
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
    return TEE_CRYPTO_SUCCESS;
}

_TEE_WEAK_ tee_crypto_result tee_rsa_init_pubkey(size_t keybits,
        const uint8_t *n, size_t n_size,
        const uint8_t *e, size_t e_size,
        rsa_pubkey_t *pubkey)
{
    return TEE_CRYPTO_SUCCESS;
}

_TEE_WEAK_ tee_crypto_result tee_rsa_gen_keypair(size_t keybits,
        const uint8_t *e, size_t e_size,
        rsa_keypair_t *keypair)
{
    return TEE_CRYPTO_SUCCESS;
}

_TEE_WEAK_ tee_crypto_result tee_rsa_get_key_attr(rsa_key_attr_t attr,
        rsa_keypair_t *keypair, void *buffer, size_t *size)
{
    return TEE_CRYPTO_SUCCESS;
}

_TEE_WEAK_ tee_crypto_result tee_rsa_public_encrypt(const rsa_pubkey_t *pub_key,
        const uint8_t *src, size_t src_size,
        uint8_t *dst, size_t *dst_size,
        tee_rsa_padding_t padding)
{
    return TEE_CRYPTO_SUCCESS;
}

_TEE_WEAK_ tee_crypto_result tee_rsa_private_decrypt(const rsa_keypair_t *priv_key,
        const uint8_t *src, size_t src_size,
        uint8_t *dst, size_t *dst_size,
        tee_rsa_padding_t padding)
{
    return TEE_CRYPTO_SUCCESS;
}

_TEE_WEAK_ tee_crypto_result tee_rsa_sign(const rsa_keypair_t *priv_key,
        const uint8_t *dig, size_t dig_size,
        uint8_t *sig, size_t *sig_size, tee_rsa_padding_t padding)
{
    return TEE_CRYPTO_SUCCESS;
}

_TEE_WEAK_ tee_crypto_result tee_rsa_verify(const rsa_pubkey_t *pub_key,
        const uint8_t *dig, size_t dig_size,
        const uint8_t *sig, size_t sig_size,
        tee_rsa_padding_t padding, bool *result)
{
    return TEE_CRYPTO_SUCCESS;
}

_TEE_WEAK_ int tee_crypto_rsa_sw_exptmod_2_2m(void)
{
    return TEE_CRYPTO_SUCCESS;
}

