/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <string.h>
#include "tee_client_api.h"
#include "tee_entry.h"
#include "csi_tee_common.h"

int32_t csi_tee_dev_info_get(const uint8_t *name_encrypted, uint32_t name_encrypted_len,
                             const uint8_t *product_key_encrypted, uint32_t product_key_encrypted_len,
                             const uint8_t *name, uint32_t
                             *name_len,
                             const uint8_t *product_key, uint32_t *product_key_len)
{
    teec_parameter p[4];
    uint32_t t;

    PARAM_CHECK(name_encrypted);
    PARAM_CHECK(name_encrypted_len);
    PARAM_CHECK(product_key_encrypted);
    PARAM_CHECK(product_key_encrypted_len);
    PARAM_CHECK(name);
    PARAM_CHECK(name_len);
    PARAM_CHECK(*name_len);
    PARAM_CHECK(product_key);
    PARAM_CHECK(product_key_len);
    PARAM_CHECK(*product_key_len);

    TEEC_4PARAMS(p, name_encrypted, name_encrypted_len, \
                 product_key_encrypted, product_key_encrypted_len, \
                 name, name_len, \
                 product_key, product_key_len);

    t = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_MEMREF_TEMP_INPUT, \
                         TEEC_MEMREF_TEMP_OUTPUT, TEEC_MEMREF_TEMP_OUTPUT);

    return tee_send(TEE_CMD_GET_DEV_INFO, p, t);
}

int32_t csi_tee_dev_info_sign(const uint8_t *in, uint32_t in_len,
                              const uint8_t *device_secret, uint32_t device_secret_len,
                              const uint8_t *sign, uint32_t
                              *sign_len)
{
    teec_parameter p[4];
    uint32_t t;

    PARAM_CHECK(in);
    PARAM_CHECK(in_len);
    PARAM_CHECK(device_secret);
    PARAM_CHECK(device_secret_len);
    PARAM_CHECK(sign);
    PARAM_CHECK(sign_len);
    PARAM_CHECK(*sign_len);

    TEEC_4PARAMS(p, in, in_len, \
                 device_secret, device_secret_len, \
                 sign, sign_len, \
                 0, 0);

    t = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_MEMREF_TEMP_INPUT, \
                         TEEC_MEMREF_TEMP_OUTPUT, TEEC_NONE);

    return tee_send(TEE_CMD_DEV_INFO_SIGN, p, t);
}


int32_t csi_tee_dev_info_crypt(const uint8_t *in, uint32_t in_len,
                              uint8_t *out, uint32_t *out_len,
                              uint8_t is_enc)
{
    teec_parameter p[4];
    uint32_t t;

    PARAM_CHECK(in);
    PARAM_CHECK(in_len);
    PARAM_CHECK(out);
    PARAM_CHECK(out_len);
    PARAM_CHECK(*out_len);

    TEEC_4PARAMS(p, in, in_len, \
               out, out_len, \
               is_enc, 0, \
               0, 0);

    t = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_MEMREF_TEMP_OUTPUT, \
                       TEEC_MEMREF_TEMP_INPUT, TEEC_NONE);

    return tee_send(TEE_CMD_DEV_INFO_CRYPT, p, t);
}

