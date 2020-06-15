/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <tee_internel_api.h>
#include <tee_msg_cmd.h>
#include <string.h>
#include <mtb.h>
#include <crypto.h>
#include <hmac.h>
#include <tee_debug.h>

static int generate_key(uint8_t key[32])
{
#if (CONFIG_TB_KP > 0)
    key_info_t key_info;
    int ret;

    memset(key, 0, 32);

    ret = mtb_get_key_info(&key_info);

    if (ret) {
        return ret;
    }

    tee_hash_digest(SHA256, (uint8_t *)key_info.d, key_info.size, key);
#else
    memset(key, 0, 32);
#endif
    return 0;
}

static int dev_info_decrypt(const uint8_t *in, uint32_t in_len, uint8_t *out, uint32_t *out_len)
{
    uint8_t tmp_key[32] = {0};
    uint8_t tmp_out[512]  = {0};
    uint32_t tmp_out_len = 0;
    int ret = 0;
    aes_context aes_ctx;

    TEE_HEX_DUMP("dev_info_decrypt in", in, in_len);

    if (in_len > sizeof(tmp_out))
    {
        return TEE_ERROR_BAD_PARAMETERS;
    }

    ret = generate_key(tmp_key);
    if (ret)
    {
        return ret;
    }

    TEE_HEX_DUMP("tmp_key:", tmp_key, 32);

    ret = tee_aes_init(AES_ECB, 0, tmp_key, NULL, 32, NULL, (void *)&aes_ctx);
    if (ret)
    {
        return ret;
    }

    ret = tee_aes_process(in, tmp_out, in_len, (void *)&aes_ctx);
    if (ret)
    {
        return ret;
    }

    ret = tee_aes_finish(in, in_len, tmp_out, &tmp_out_len, SYM_NOPAD, (void *)&aes_ctx);
    if (ret)
    {
        return ret;
    }

    if (tmp_out_len != in_len || tmp_out[in_len - 1] > AES_BLOCK_SIZE || tmp_out[in_len - 1] >= in_len)
    {
        return TEE_ERROR_GENERIC;
    }

    if (*out_len < tmp_out_len - tmp_out[in_len - 1])
    {
        return TEE_ERROR_BAD_PARAMETERS;
    }

    *out_len = tmp_out_len - tmp_out[in_len - 1];
    memcpy(out, tmp_out, *out_len);

    memset(tmp_key, 0, 32);

    TEE_HEX_DUMP("dev_info_decrypted out", out, *out_len);

    return TEE_SUCCESS;
}

static int dev_info_encrypt(const uint8_t *in, uint32_t in_len, uint8_t *out, uint32_t *out_len)
{
    uint8_t tmp_key[32] = {0};
    uint8_t tmp_in[512] = {0};
    uint32_t tmp_in_len = 0;
    int ret = 0;
    aes_context aes_ctx;

    TEE_HEX_DUMP("dev_info_encrypted in", in, in_len);

    tmp_in_len = ((in_len / AES_BLOCK_SIZE) + 1) * AES_BLOCK_SIZE;

    if (tmp_in_len >= 512 || tmp_in_len > *out_len)
    {
        return TEE_ERROR_BAD_PARAMETERS;
    }

    memcpy(tmp_in, in, in_len);

    /* paddding */
    memset(tmp_in + in_len, tmp_in_len - in_len, tmp_in_len);

    ret = generate_key(tmp_key);
    if (ret)
    {
        return ret;
    }

    TEE_HEX_DUMP("tmp_key:", tmp_key, 32);

    ret = tee_aes_init(AES_ECB, 1, tmp_key, NULL, 32, NULL, (void *)&aes_ctx);
    if (ret)
    {
        return ret;
    }

    ret = tee_aes_process(tmp_in, out, tmp_in_len, (void *)&aes_ctx);
    if (ret)
    {
        return ret;
    }

    ret = tee_aes_finish(tmp_in, tmp_in_len, out, out_len, SYM_NOPAD, (void *)&aes_ctx);
    if (ret)
    {
        return ret;
    }

    *out_len = tmp_in_len;

    memset(tmp_key, 0, 32);

    TEE_HEX_DUMP("dev_info_encrypted out", out, *out_len);

    return TEE_SUCCESS;
}

int tee_core_get_dev_info(tee_param params[4])
{
    uint8_t *ct_dev_name = (uint8_t *)params[0].memref.buffer;
    uint32_t ct_dev_name_len = params[0].memref.size;
    uint8_t *ct_pkey = (uint8_t *)params[1].memref.buffer;
    uint32_t ct_pkey_len = params[1].memref.size;
    uint8_t *dev_name = (uint8_t *)params[2].memref.buffer;
    uint32_t *dev_name_len = (uint32_t *)params[2].memref.size;
    uint8_t *pkey = (uint8_t *)params[3].memref.buffer;
    uint32_t *pkey_len = (uint32_t *)params[3].memref.size;
    int rc = 0;

    TEE_HEX_DUMP("ct_dev_name:", ct_dev_name, ct_dev_name_len);
    TEE_HEX_DUMP("ct_pkey:", ct_pkey, ct_pkey_len);

    rc = dev_info_decrypt(ct_dev_name, ct_dev_name_len, dev_name, dev_name_len);
    if (rc)
        return TEE_ERROR_GENERIC;
    rc = dev_info_decrypt(ct_pkey, ct_pkey_len, pkey, pkey_len);
    if (rc)
        return TEE_ERROR_GENERIC;

    TEE_HEX_DUMP("dev_name:", dev_name, *dev_name_len);
    TEE_HEX_DUMP("pkey:", pkey, *pkey_len);

    return TEE_SUCCESS;
}

#if (CONFIG_ALGO_HMAC > 0)
int tee_core_dev_info_sign(tee_param params[4])
{
    int ret = 0;
    uint8_t *in = (uint8_t *)params[0].memref.buffer;
    uint32_t in_len = params[0].memref.size;
    uint8_t *ct_dev_secret = (uint8_t *)params[1].memref.buffer;
    uint32_t ct_dev_secret_len = params[1].memref.size;
    uint8_t *sign = (uint8_t *)params[2].memref.buffer;
    uint32_t *sign_len = (uint32_t *)params[2].memref.size;

    /*device secret max length is 32, add PKCS#5 padding size 16, 32 + 16 = 48 */
    uint8_t dev_secret[48] = {0};
    uint32_t dev_secret_len = 48;
    TEE_LOGD("in %x, in_len %d, ct_dev_secret %x, len %d\n", in, in_len, ct_dev_secret, ct_dev_secret_len);
    TEE_HEX_DUMP("sign in:",in, in_len);
    TEE_HEX_DUMP("ct_dev_secret:",ct_dev_secret,ct_dev_secret_len);
    ret = dev_info_decrypt(ct_dev_secret, ct_dev_secret_len, dev_secret, &dev_secret_len);
    if (ret)
    {
        return ret;
    }
    TEE_HEX_DUMP("device securet:", dev_secret, dev_secret_len);
    hmac_context_t hmac_ctx;
    hmac_init(&hmac_ctx, HMAC_SHA1);
    hmac_start(&hmac_ctx, dev_secret, dev_secret_len);
    hmac_update(&hmac_ctx, in, in_len);
    hmac_finish(&hmac_ctx, sign);
    *sign_len = 20;
    TEE_HEX_DUMP("dev signed:", sign, *sign_len);
    return TEE_SUCCESS;
}
#endif

int tee_core_dev_info_crypt(tee_param params[4])
{
    uint8_t *in = (uint8_t *)params[0].memref.buffer;
    uint32_t in_len = params[0].memref.size;
    uint8_t *out = (uint8_t *)params[1].memref.buffer;
    uint32_t *out_len = (uint32_t *)params[1].memref.size;
    uint32_t is_enc = (uint32_t)params[2].memref.buffer;

    if (is_enc)
    {
        return dev_info_encrypt(in, in_len, out, out_len);
    }
    else
    {
        return dev_info_decrypt(in, in_len, out, out_len);
    }

    return TEE_SUCCESS;
}

