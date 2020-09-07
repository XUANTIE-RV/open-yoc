/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef CRYPTO_H
#define CRYPTO_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>


typedef enum _tee_crypto_result {
    TEE_CRYPTO_ERROR   = 0xffff0000,    /* 0:  Generic Error */
    TEE_CRYPTO_NOSUPPORT,               /* 1:  Scheme not support */
    TEE_CRYPTO_INVALID_KEY,             /* 2:  Invalid Key in asymmetric scheme: RSA/DSA/ECCP/DH etc */
    TEE_CRYPTO_INVALID_TYPE,            /* 3:  Invalid aes_type/des_type/authenc_type/hash_type/cbcmac_type/cmac_type */
    TEE_CRYPTO_INVALID_CONTEXT,         /* 4:  Invalid context in multi-thread cipher/authenc/mac/hash etc */
    TEE_CRYPTO_INVALID_PADDING,         /* 5:  Invalid sym_padding/rsassa_padding/rsaes_padding */
    TEE_CRYPTO_INVALID_AUTHENTICATION,  /* 6:  Invalid authentication in AuthEnc(AES-CCM/AES-GCM)/asymmetric verify(RSA/DSA/ECCP DSA) */
    TEE_CRYPTO_INVALID_ARG,             /* 7:  Invalid arguments */
    TEE_CRYPTO_INVALID_PACKET,          /* 8:  Invalid packet in asymmetric enc/dec(RSA) */
    TEE_CRYPTO_LENGTH_ERR,              /* 9:  Invalid Length in arguments */
    TEE_CRYPTO_OUTOFMEM,                /* a:  Memory alloc NULL */
    TEE_CRYPTO_BUFFER_OVERFLOW,         /* b:  Output buffer is too short to store result */
    TEE_CRYPTO_NULL,                    /* c:  NULL pointer in arguments */
    TEE_CRYPTO_ERR_STATE,               /* d:  Bad state in mulit-thread cipher/authenc/mac/hash etc */
    TEE_CRYPTO_SUCCESS = 0,             /* Success */
} tee_crypto_result;


#define AES128_KEY_LEN_BYTES    16
#define AES192_KEY_LEN_BYTES    24
#define AES256_KEY_LEN_BYTES    32

#define AES_BLOCK_SIZE       16
#define AES_IV_SIZE          16

#define DES_BLOCK_SIZE       8
#define DES_IV_SIZE          8

#define RSA_KEYBITS          1024
#define RSA2048_KEYBITS      2048
typedef enum _des_type_t {
    DES_ECB     = 0,
    DES_CBC     = 1,
    DES3_ECB    = 2,
    DES3_CBC    = 3,
} des_type_t;

typedef enum _aes_type_t {
    AES_ECB = 0,
    AES_CBC = 1,
    AES_CTR = 2,
    AES_CTS = 3,
    AES_XTS = 4,
} aes_type_t;

typedef enum _sym_padding_t {
    SYM_NOPAD       = 0,
    SYM_PKCS5_PAD   = 1,
    SYM_ZERO_PAD    = 2,
} sym_padding_t;

typedef enum _hash_type_t {
    SHA1    = 0,
    SHA224  = 1,
    SHA256  = 2,
    SHA384  = 3,
    SHA512  = 4,
    MD5     = 5,
} hash_type_t;

enum {
    MD5_HASH_SIZE       = 16,
    SHA1_HASH_SIZE      = 20,
    SHA224_HASH_SIZE    = 28,
    SHA256_HASH_SIZE    = 32,
    SHA384_HASH_SIZE    = 48,
    SHA512_HASH_SIZE    = 64,
    MAX_HASH_SIZE       = 64,
};

enum {
    DES_KEYSIZE         = 8,
    DES3_2KEYSIZE       = 16,
    DES3_3KEYSIZE       = 24,
};

#define HASH_SIZE(type) (((type) == SHA1) ? (SHA1_HASH_SIZE) : (     \
                         ((type) == SHA224) ? (SHA224_HASH_SIZE) : ( \
                                 ((type) == SHA256) ? (SHA256_HASH_SIZE) : ( \
                                         ((type) == SHA384) ? (SHA384_HASH_SIZE) : ( \
                                                 ((type) == SHA512) ? (SHA512_HASH_SIZE) : ( \
                                                         ((type) == MD5) ? (MD5_HASH_SIZE) : (0)))))))

typedef enum _rsa_key_attr_t {
    RSA_MODULUS          = 0x130,
    RSA_PUBLIC_EXPONENT  = 0x230,
    RSA_PRIVATE_EXPONENT = 0x330,
    RSA_PRIME1           = 0x430,
    RSA_PRIME2           = 0x530,
    RSA_EXPONENT1        = 0x630,
    RSA_EXPONENT2        = 0x730,
    RSA_COEFFICIENT      = 0x830,
} rsa_key_attr_t;

typedef enum _rsa_pad_type_t {
    RSA_NOPAD               = 0,

    RSAES_PKCS1_V1_5        = 10,
    RSAES_PKCS1_OAEP_MGF1   = 11,

    RSASSA_PKCS1_V1_5       = 20,
    RSASSA_PKCS1_PSS_MGF1   = 21,
} rsa_pad_type_t;

typedef struct _tee_rsa_padding_t {
    rsa_pad_type_t type;
    union {
        struct {
            hash_type_t type;
            const uint8_t *lparam;
            size_t lparamlen;
        } rsaes_oaep;
        struct {
            hash_type_t type;   /* md5/sha1/sha224/sha256/sha384/sha512 */
        } rsassa_v1_5;
        struct {
            hash_type_t type;   /* sha1/sha224/sha256/sha384/sha512 */
            size_t salt_len;
        } rsassa_pss;
    } pad;
} tee_rsa_padding_t;


typedef struct {
    hash_type_t hashtype;
    void       *hashctx;
} hash_context;

typedef struct {
    des_type_t destype;
    void       *desctx;
} des_context;

typedef struct {
    aes_type_t aestype;
    void       *aesctx;
    uint8_t  *iv;
} aes_context;

struct __rsa_keypair {
    void    *e;
    void    *d;
    void    *n;
    uint32_t keybits;
};

struct __rsa_pubkey {
    void    *e;
    void    *n;
    uint32_t keybits;
};

typedef struct __rsa_keypair  rsa_keypair_t;
typedef struct __rsa_pubkey   rsa_pubkey_t;
/*
 * hash porting api
 */
tee_crypto_result tee_hash_get_ctx_size(hash_type_t type, size_t *size);
tee_crypto_result tee_hash_init(hash_type_t type, void *context);
tee_crypto_result tee_hash_update(const uint8_t *src, size_t size, void *context);
tee_crypto_result tee_hash_final(uint8_t *dgst, void *context);
tee_crypto_result tee_hash_reset(void *context);
tee_crypto_result tee_hash_copy_context(void *dst_ctx, void *src_ctx);
tee_crypto_result tee_hash_digest(hash_type_t type, const uint8_t *src, size_t size,
                                  uint8_t *dgst);

/*
 * aes porting api
 */
tee_crypto_result tee_aes_get_ctx_size(aes_type_t type, size_t *size);
tee_crypto_result tee_aes_init(aes_type_t type, bool is_enc, const uint8_t *key1,
                               const uint8_t *key2, size_t keybytes, uint8_t *iv,
                               void *context);
tee_crypto_result tee_aes_process(const uint8_t *src, uint8_t *dst, size_t size,
                                  void *context);
tee_crypto_result tee_aes_finish(const uint8_t *src, size_t src_size, uint8_t *dst,
                                 size_t *dst_size, sym_padding_t padding, void *context);
tee_crypto_result tee_aes_reset(void *context);
tee_crypto_result tee_aes_copy_context(void *dst_ctx, void *src_ctx);

/*
 * des porting api
 */
tee_crypto_result tee_des_get_ctx_size(des_type_t type, size_t *size);
tee_crypto_result tee_des_init(des_type_t type, bool is_enc, const uint8_t *key,
                               size_t keybytes, const uint8_t *iv, void *context);
tee_crypto_result tee_des_process(const uint8_t *src, uint8_t *dst, size_t size,
                                  void *context);
tee_crypto_result tee_des_finish(const uint8_t *src, size_t src_size, uint8_t *dst,
                                 size_t *dst_size, sym_padding_t padding, void *context);
tee_crypto_result tee_des_reset(void *context);
tee_crypto_result tee_des_copy_context(void *dst_ctx, void *src_ctx);

/*
 * random generator porting api
 */
tee_crypto_result tee_seed(uint8_t *seed, size_t seed_len);
tee_crypto_result tee_rand_gen(uint8_t *buf, size_t len);

/*
 * rsa poring api
 */

tee_crypto_result tee_rsa_get_keypair_size(size_t keybits, size_t *size);
tee_crypto_result tee_rsa_get_pubkey_size(size_t keybits, size_t *size);
tee_crypto_result tee_rsa_init_keypair(size_t keybits,
                                       const uint8_t *n, size_t n_size,
                                       const uint8_t *e, size_t e_size,
                                       const uint8_t *d, size_t d_size,
                                       const uint8_t *p, size_t p_size,
                                       const uint8_t *q, size_t q_size,
                                       const uint8_t *dp, size_t dp_size,
                                       const uint8_t *dq, size_t dq_size,
                                       const uint8_t *qp, size_t qp_size,
                                       rsa_keypair_t *keypair);
tee_crypto_result tee_rsa_init_pubkey(size_t keybits,
                                      const uint8_t *n, size_t n_size,
                                      const uint8_t *e, size_t e_size,
                                      rsa_pubkey_t *pubkey);
tee_crypto_result tee_rsa_gen_keypair(size_t keybits,
                                      const uint8_t *e, size_t e_size,
                                      rsa_keypair_t *keypair);
tee_crypto_result tee_rsa_get_key_attr(rsa_key_attr_t attr,
                                       rsa_keypair_t *keypair, void *buffer, size_t *size);
tee_crypto_result tee_rsa_public_encrypt(const rsa_pubkey_t *pub_key,
        const uint8_t *src, size_t src_size,
        uint8_t *dst, size_t *dst_size,
        tee_rsa_padding_t padding);
tee_crypto_result tee_rsa_private_decrypt(const rsa_keypair_t *priv_key,
        const uint8_t *src, size_t src_size,
        uint8_t *dst, size_t *dst_size,
        tee_rsa_padding_t padding);
tee_crypto_result tee_rsa_sign(const rsa_keypair_t *priv_key,
                               const uint8_t *dig, size_t dig_size,
                               uint8_t *sig, size_t *sig_size, tee_rsa_padding_t padding);
tee_crypto_result tee_rsa_verify(const rsa_pubkey_t *pub_key,
                                 const uint8_t *dig, size_t dig_size,
                                 const uint8_t *sig, size_t sig_size,
                                 tee_rsa_padding_t padding, bool *result);

tee_crypto_result tee_rsa_sign_id2_fips(const uint8_t *dig, size_t dig_size,
                                        uint8_t *sig, size_t *sig_size, tee_rsa_padding_t padding);
tee_crypto_result tee_rsa_verify_id2_fips(const uint8_t *dig, size_t dig_size,
        const uint8_t *sig, size_t sig_size,
        tee_rsa_padding_t padding, bool *result);
tee_crypto_result tee_rsa_public_encrypt_id2_fips(
    const uint8_t *src, size_t src_size,
    uint8_t *dst, size_t *dst_size,
    tee_rsa_padding_t padding);
tee_crypto_result tee_rsa_private_decrypt_id2_fips(
    const uint8_t *src, size_t src_size,
    uint8_t *dst, size_t *dst_size,
    tee_rsa_padding_t padding);

int tee_crypto_rsa_sw_exptmod_2_2m(void);


#endif

