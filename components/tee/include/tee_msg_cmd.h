/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef TEE_CLIENT_CMD_H
#define TEE_CLIENT_CMD_H

/*
 * TEE MSG ID
 */
#define TEE_MSG_INIT_CONTEXT            0x0
#define TEE_MSG_FINAL_CONTEXT           0x1
#define TEE_MSG_OPEN_SESSION            0x2
#define TEE_MSG_CLOSE_SESSION           0x3
#define TEE_MSG_INVOKE_COMMAND          0x4

/*
 * TEE REQUEST
 */
#define TEE_COM_TYPE_QUERY              0x0
#define TEE_COM_TYPE_RESPONSE           0x1

/*
 * TEE CMD ID
 */
#define TEE_CMD_GET_ID                  0x0
#define TEE_CMD_RSA_SIGN                0x1
#define TEE_CMD_RSA_VERIFY              0x2
#define TEE_CMD_RSA_ENC                 0x3
#define TEE_CMD_RSA_DEC                 0x4
#define TEE_CMD_AES_CBC_ENC             0x5
#define TEE_CMD_AES_CBC_DEC             0x6
#define TEE_CMD_RANDOM_GET              0x7
#define TEE_CMD_MD5_DIGEST              0x8
#define TEE_CMD_SHA256_DIGEST           0x9
#define TEE_CMD_DES3_ECB_ENC            0xa
#define TEE_CMD_DES3_ECB_DEC            0xb
#define TEE_CMD_SHA_DIGEST              0xc
#define TEE_CMD_LPM                     0xd
#define TEE_CMD_ALGO_CAP                0xe
#define TEE_CMD_IMG_INFO                0xf
#define TEE_CMD_AES_ECB_ENC             0x10
#define TEE_CMD_AES_ECB_DEC             0x11
#define TEE_CMD_MANIFEST_INFO           0x12
#define TEE_CMD_HMAC                    0x13
#define TEE_CMD_GET_DEV_INFO            0x14
#define TEE_CMD_DEV_INFO_SIGN           0x15
#define TEE_CMD_IMG_VERIFY              0x16
#define TEE_CMD_DEV_INFO_CRYPT          0x17
#define TEE_CMD_SYS_FREQ                0x18

#define TEE_CMD_XOR                     0xE0
#endif

