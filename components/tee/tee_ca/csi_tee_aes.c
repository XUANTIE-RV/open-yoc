/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include "tee_client_api.h"
#include "tee_entry.h"
#include "csi_tee_common.h"

#if (CONFIG_ALGO_AES > 0)

#define AES_ENC        0
#define AES_DEC        1
#define AES_ECB        0
#define AES_CBC        1

const uint8_t aes_cmd[2][2] = {
    {TEE_CMD_AES_ECB_ENC, TEE_CMD_AES_CBC_ENC},
    {TEE_CMD_AES_ECB_DEC, TEE_CMD_AES_CBC_DEC},
};

static int hal_aes(const uint8_t *in, uint32_t in_len,
                   const uint8_t *key, uint32_t key_len,
                   const uint8_t *iv,
                   uint8_t *out, uint32_t *out_len,
                   uint32_t mode, uint32_t enc)
{
    teec_parameter p[4];
    uint32_t t;

    TEEC_4PARAMS(p, in, in_len, key, key_len, iv, 16, out, out_len);

    t = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_MEMREF_TEMP_INPUT, \
                         TEEC_MEMREF_TEMP_INOUT, TEEC_MEMREF_TEMP_OUTPUT);

    return tee_send(aes_cmd[enc][mode], p, t);
}

int32_t csi_tee_aes_encrypt(const uint8_t *in, uint32_t in_len,
                            const uint8_t *key, uint32_t key_len,
                            uint8_t iv[16],
                            uint8_t *out,
                            tee_aes_mode_e mode)
{
    uint32_t out_len;
    PARAM_CHECK(in);
    PARAM_CHECK(in_len);
    PARAM_CHECK(key);
    PARAM_CHECK(key_len);
    PARAM_CHECK(out);

    if (mode >= TEE_AES_MODE_MAX) {
        return ERR_TEE(DRV_ERROR_PARAMETER);
    }

    if (mode == TEE_AES_MODE_CBC && iv == NULL) {
        return ERR_TEE(DRV_ERROR_PARAMETER);
    }

    if (key_len != 16 && key_len != 24 && key_len != 32) {
        return ERR_TEE(DRV_ERROR_PARAMETER);
    }

    if (in_len % 16) {
        return ERR_TEE(DRV_ERROR_PARAMETER);
    }

    return hal_aes(in, in_len, key, key_len, iv, out, &out_len, mode, AES_ENC);
}

int32_t csi_tee_aes_decrypt(const uint8_t *in, uint32_t in_len,
                            const uint8_t *key, uint32_t key_len,
                            uint8_t iv[16],
                            uint8_t *out,
                            tee_aes_mode_e mode)
{
    uint32_t out_len;

    PARAM_CHECK(in);
    PARAM_CHECK(in_len);
    PARAM_CHECK(key);
    PARAM_CHECK(key_len);
    PARAM_CHECK(out);

    if (mode >= TEE_AES_MODE_MAX) {
        return ERR_TEE(DRV_ERROR_PARAMETER);
    }

    if (mode == TEE_AES_MODE_CBC && iv == NULL) {
        return ERR_TEE(DRV_ERROR_PARAMETER);
    }

    if (key_len != 16 && key_len != 24 && key_len != 32) {
        return ERR_TEE(DRV_ERROR_PARAMETER);
    }

    if (in_len % 16) {
        return ERR_TEE(DRV_ERROR_PARAMETER);
    }

    return hal_aes(in, in_len, key, key_len, iv, out, &out_len, mode, AES_DEC);
}

#endif

