/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <string.h>
#include "tee_client_api.h"
#include "tee_entry.h"
#include "csi_tee_common.h"

#if (CONFIG_ALGO_RSA > 0)
#define RSA_SIGN    0
#define RSA_VERIFY  1
#define RSA_ENC     2
#define RSA_DEC     3
#define RSA_IMG_VERIFY 4

#define RSA_MD5    0
#define RSA_SHA1   1
#define RSA_SHA256 2

#define RSA_1024    1024
#define RSA_2048    2048

#define MD5_SIZE    16
#define SHA1_SIZE   20

const uint8_t rsa_cmd[5] = {
    TEE_CMD_RSA_SIGN,
    TEE_CMD_RSA_VERIFY,
    TEE_CMD_RSA_ENC,
    TEE_CMD_RSA_DEC,
    TEE_CMD_IMG_VERIFY,
};

static int tee_rsa(const uint8_t *key, uint32_t key_len, const uint8_t *in, uint32_t in_len, uint8_t *out, uint32_t out_len, uint8_t type, uint8_t opr)
{
    int ret, verifyflag = 0;
    teec_parameter p[4];
    uint32_t t;

    TEEC_4PARAMS(p, in, in_len, key, key_len, &verifyflag, type, out, out_len);

    t = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_MEMREF_TEMP_INPUT, \
                         TEEC_MEMREF_TEMP_OUTPUT, TEEC_MEMREF_TEMP_OUTPUT);

    ret = tee_send(rsa_cmd[opr], p, t);

    if (ret) {
        return ret;
    }

    if (opr == RSA_VERIFY) {
        if (verifyflag == 0) {
            return 1;
        }
    }

    return 0;
}

int32_t csi_tee_rsa_sign(const uint8_t *in, uint32_t in_len,
                         const uint8_t *key, uint32_t key_len,
                         uint8_t *sign, uint32_t *sign_len,
                         tee_rsa_sign_type_e type)
{
    PARAM_CHECK(in);
    PARAM_CHECK(in_len);
    PARAM_CHECK(sign);
    PARAM_CHECK(sign_len);

    if (type >= TEE_RSA_SIGN_TYPE_MAX) {
        return ERR_TEE(DRV_ERROR_PARAMETER);
    }

    return tee_rsa(key, key_len, in, in_len, sign, (uint32_t)sign_len, type, RSA_SIGN);
}

int32_t csi_tee_rsa_verify(const uint8_t *in, uint32_t in_len,
                           const uint8_t *key, uint32_t key_len,
                           uint8_t *sign, uint32_t sign_len,
                           tee_rsa_sign_type_e type)
{
    PARAM_CHECK(in);
    PARAM_CHECK(in_len);
    PARAM_CHECK(sign);
    PARAM_CHECK(sign_len);

    if (type >= TEE_RSA_SIGN_TYPE_MAX) {
        return ERR_TEE(DRV_ERROR_PARAMETER);
    }

    return tee_rsa(key, key_len, in, in_len, sign, (uint32_t)&sign_len, type, RSA_VERIFY);
}

int32_t csi_tee_rsa_encrypt(const uint8_t *in, uint32_t in_len,
                            const uint8_t *key, uint32_t key_len,
                            uint8_t *out, uint32_t *out_len,
                            tee_rsa_padding_mode_e padding)
{
    PARAM_CHECK(in);
    PARAM_CHECK(in_len);
    PARAM_CHECK(out);
    PARAM_CHECK(out_len);

    if (padding > TEE_RSA_NO_PADDING) {
        return ERR_TEE(DRV_ERROR_PARAMETER);
    }

    return tee_rsa(key, key_len, in, in_len, out, (uint32_t)out_len, padding, RSA_ENC);;
}

int32_t csi_tee_rsa_decrypt(const uint8_t *in, uint32_t in_len,
                            const uint8_t *key, uint32_t key_len,
                            uint8_t *out, uint32_t *out_len,
                            tee_rsa_padding_mode_e padding)
{
    PARAM_CHECK(in);
    PARAM_CHECK(in_len);
    PARAM_CHECK(out);
    PARAM_CHECK(out_len);

    if (padding > TEE_RSA_NO_PADDING) {
        return ERR_TEE(DRV_ERROR_PARAMETER);
    }

    return tee_rsa(key, key_len, in, in_len, out, (uint32_t)out_len, padding, RSA_DEC);;
}

int32_t csi_tee_img_rsa_verify(const uint8_t *in, uint32_t in_len,
                           uint8_t *sign, uint32_t sign_len,
                           tee_rsa_sign_type_e type)
{
    PARAM_CHECK(in);
    PARAM_CHECK(in_len);
    PARAM_CHECK(sign);
    PARAM_CHECK(sign_len);

    if (type >= TEE_RSA_SIGN_TYPE_MAX) {
        return ERR_TEE(DRV_ERROR_PARAMETER);
    }

    return tee_rsa(NULL, 0, in, in_len, sign, (uint32_t)&sign_len, type, RSA_IMG_VERIFY);
}

#endif
