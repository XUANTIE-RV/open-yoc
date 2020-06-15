/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <string.h>
#include <tee_internel_api.h>
#include <tee_crypto_api.h>
#include <tee_storage_api.h>
#include <tee_msg_cmd.h>

#include <crypto.h>

#if (CONFIG_TB_KP > 0)
#include <mtb.h>
#include <kp.h>
#endif

#if (CONFIG_ALGO_RSA > 0)

#define PKCS1_PADDING   0x01
#define NO_PADDING      0x02

#define MD5_PADDING     0x00
#define SHA1_PADDING    0x01
#define SHA256_PADDING    0x03

extern const uint8_t test_key_n[];
extern const uint8_t test_key_e[];
extern const uint8_t test_key_d[];

/*load rsa key from KP if key is NULL,otherwise from param key*/
static int tee_rsa_load_key(uint8_t *key, uint32_t key_len, uint32_t key_type, void *keys, uint8_t internel_key)
{
    uint8_t *n = NULL, *e = NULL, *d = NULL;
    uint32_t keybits = 0;

    int ret = 0;

    if (key_type == 0) {
        keybits = key_len / 2 * 8;
    } else if (key_type == 1) {
        keybits = key_len / 3 * 8;
    }

    if (!key) {

        /*KP RSA key size is 1024 bits*/
        keybits = RSA_KEYBITS;
#if (CONFIG_TB_KP > 0)
        // boot public key, only used for verify
        if (internel_key == 1)
        {
            key_info_t info;
            ret = mtb_get_pubkey_info(&info);
            if (ret)
            {
                return ret;
            }

            n = info.n;
            e = info.e;
        }
        else
        {
            uint32_t keyaddr;

            ret = kp_get_key(KEY_CIDPRIVKEY, &keyaddr);

            if (ret < 0) {
                TEE_LOGE("find KEY_CIDPRIVKEY fail ret %d\n", ret);
                return ret;
            }

            if (ret != RSA_KEYBITS >> 2)
            {
                TEE_LOGE("KEY_CIDPRIVKEY size (%d) err, unsupport in rsa\n", ret);
                return TEE_ERROR_GENERIC;
            }
            n = (uint8_t *)keyaddr;
            e = (uint8_t *)test_key_e;
            d = (uint8_t *)(keyaddr + (RSA_KEYBITS >> 3));
        }
#else
        n = (uint8_t *)test_key_n;
        e = (uint8_t *)test_key_e;
        d = (uint8_t *)test_key_d;
#endif

    } else {
        n = key;
        e = key + (keybits >> 3);
        d = key + (keybits >> 3) * 2;
    }

    if (key_type == 0) {
        ret = tee_rsa_init_pubkey(keybits,
                                  n, keybits >> 3,
                                  e, keybits >> 3,
                                  (rsa_pubkey_t *)keys);
    } else {
        ret = tee_rsa_init_keypair(keybits,
                                   n, keybits >> 3,
                                   e, keybits >> 3,
                                   d, keybits >> 3,
                                   NULL, 0,
                                   NULL, 0,
                                   NULL, 0,
                                   NULL, 0,
                                   NULL, 0,
                                   (rsa_keypair_t *)keys);
    }

    return ret;
}

/* internel_key: 0 default cid key ,1 boot public key */
int tee_core_rsa(tee_param params[4], uint32_t algo, uint32_t mode, uint8_t internel_key)
{
    tee_rsa_padding_t padding;
    uint8_t ver_ok;
    int ret = 0;

    rsa_keypair_t keypair = {0};
    rsa_pubkey_t  pubkey = {0};

    uint8_t *msg        = params[0].memref.buffer;
    uint32_t msglen     = params[0].memref.size;
    uint8_t *key        =  params[1].memref.buffer;
    uint32_t keylen     = params[1].memref.size;
    uint32_t ver_result = params[2].value.a;
    uint32_t type       = params[2].value.b;
    uint8_t *out        = params[3].memref.buffer;
    uint32_t outlen     = params[3].memref.size;

    TEE_LOGD("msg %x, msglen:%d, out %x, outlen:%d, key 0x%x, keylen:%d, alg:%x, mode: %d, type: %d\n", \
             msg, msglen, out, *(uint32_t *)outlen, key, keylen, algo, mode, type);

    if (mode == TEE_MODE_SIGN || mode == TEE_MODE_DECRYPT) {
        ret = tee_rsa_load_key(key, keylen, 1, (void *)&keypair, internel_key);
    } else {
        ret = tee_rsa_load_key(key, keylen, 0, (void *)&pubkey, internel_key);
    }

    if (ret) {
        TEE_LOGE("load key fail,ret %x\n", ret);
        return ret;
    }

    if (mode == TEE_MODE_SIGN || mode == TEE_MODE_VERIFY) {
        padding.type = RSASSA_PKCS1_V1_5;

        switch (type) {
            case MD5_PADDING:
                padding.pad.rsassa_v1_5.type = MD5;
                break;

            case SHA1_PADDING:
                padding.pad.rsassa_v1_5.type = SHA1;
                break;

            case SHA256_PADDING:
                padding.pad.rsassa_v1_5.type = SHA256;
                break;

            default:
                return TEE_ERROR_BAD_PARAMETERS;
        }
    } else {
        padding.type = (type == PKCS1_PADDING) ? RSAES_PKCS1_V1_5 : RSA_NOPAD;
    }

    switch (mode) {
        case TEE_MODE_SIGN:
            ret = tee_rsa_sign(&keypair, msg, msglen, out, (uint32_t *)outlen, padding);
            break;

        case TEE_MODE_VERIFY:
            ret = tee_rsa_verify(&pubkey, (uint8_t *)msg, msglen, out, *(uint32_t *)outlen, padding, (bool *)&ver_ok);
            *(uint32_t *)ver_result = (uint32_t)ver_ok;
            break;

        case TEE_MODE_ENCRYPT:
            ret = tee_rsa_public_encrypt(&pubkey, (uint8_t *)msg, msglen, out, (uint32_t *)outlen, padding);

            break;

        case TEE_MODE_DECRYPT:
            ret = tee_rsa_private_decrypt(&keypair, msg, msglen, out, (uint32_t *)outlen, padding);
            break;

        default:
            TEE_LOGE("mode %d\n", mode);
            ret = TEE_ERROR_BAD_PARAMETERS;
            break;
    }

    if (ret) {
        TEE_LOGE("tee core rsa error,ret 0x%x\n", ret);
    }

    return ret;
}
#endif
