/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <tee_internel_api.h>
#include <tee_crypto_api.h>
#include <tee_storage_api.h>
#include <tee_msg_cmd.h>
#include <tee_srv.h>
#include <string.h>

extern int tee_core_sys_freq(tee_param params[4]);
extern int tee_core_cid(tee_param params[4]);
extern int tee_core_random(tee_param params[4]);
extern int tee_core_md5(tee_param params[4]);
extern int tee_core_sha(tee_param params[4]);
extern int tee_core_aes(tee_param params[4], uint32_t algo, uint32_t mode);
extern int tee_core_des3(tee_param params[4], uint32_t algo, uint32_t mode);
extern int tee_core_rsa(tee_param params[4], uint32_t algo, uint32_t mode, uint8_t internel_key);
extern int tee_core_lpm(tee_param params[4]);
extern int tee_core_manifest_info(tee_param params[4]);
extern int tee_core_hmac(tee_param params[4]);
extern int tee_core_get_dev_info(tee_param params[4]);
extern int tee_core_dev_info_sign(tee_param params[4]);
extern int tee_core_dev_info_crypt(tee_param params[4]);

#define  CID_TA_UUID   {0x5445455f, 0x4944, 0x325f,                             \
                       {0x53, 0x52, 0x56, 0x5f, 0x55, 0x55, 0x49, 0x44 }        \
                       }

static tee_result _cid_ta_createentrypoint(void)
{
    return TEE_SUCCESS;
}

static void _cid_ta_destroyentrypoint(void)
{

}

static tee_result _cid_ta_opensessionentrypoint(uint32_t  paramtypes,
                                                tee_param params[4],
                                                void    **sessioncontext)
{
    return TEE_SUCCESS;
}

static void _cid_ta_closesessionentrypoint(void *sessioncontext)
{

}

static tee_result _cid_ta_invokecommandentrypoint(void     *sessioncontext,
                                                  uint32_t  commandid,
                                                  uint32_t  paramtypes,
                                                  tee_param params[4])
{
    int ret = TEE_SUCCESS;

    TEE_LOGI("TEE get cmd id: %d\n", commandid);

    switch (commandid) {
#if (CONFIG_DEV_CID > 0)

        case TEE_CMD_GET_ID:
            ret = tee_core_cid(params);
            break;
#endif
#if (CONFIG_ALGO_TRNG > 0)

        case TEE_CMD_RANDOM_GET:
            ret = tee_core_random(params);
            break;
#endif
#if (CONFIG_ALGO_MD5 > 0)

        case TEE_CMD_MD5_DIGEST:
            ret = tee_core_md5(params);
            break;
#endif
#if (CONFIG_ALGO_SHA > 0)

        case TEE_CMD_SHA_DIGEST:
            ret = tee_core_sha(params);
            break;
#endif
#if (CONFIG_ALGO_DES3 > 0)

        case TEE_CMD_DES3_ECB_ENC:
            ret = tee_core_des3(params, TEE_ALG_DES3_ECB_NOPAD, TEE_MODE_ENCRYPT);
            break;

        case TEE_CMD_DES3_ECB_DEC:
            ret = tee_core_des3(params, TEE_ALG_DES3_ECB_NOPAD, TEE_MODE_DECRYPT);
            break;
#endif
#if (CONFIG_ALGO_AES > 0)

        case TEE_CMD_AES_CBC_ENC:
            ret = tee_core_aes(params, TEE_ALG_AES_CBC_NOPAD, TEE_MODE_ENCRYPT);
            break;

        case TEE_CMD_AES_CBC_DEC:
            ret = tee_core_aes(params, TEE_ALG_AES_CBC_NOPAD, TEE_MODE_DECRYPT);
            break;

        case TEE_CMD_AES_ECB_ENC:
            ret = tee_core_aes(params, TEE_ALG_AES_ECB_NOPAD, TEE_MODE_ENCRYPT);
            break;

        case TEE_CMD_AES_ECB_DEC:
            ret = tee_core_aes(params, TEE_ALG_AES_ECB_NOPAD, TEE_MODE_DECRYPT);
            break;
#endif
#if (CONFIG_ALGO_RSA > 0)

        case TEE_CMD_RSA_SIGN:
            ret = tee_core_rsa(params, TEE_ALG_RSASSA_PKCS1_V1_5_SHA1, TEE_MODE_SIGN, 0);
            break;

        case TEE_CMD_RSA_VERIFY:
            ret = tee_core_rsa(params, TEE_ALG_RSASSA_PKCS1_V1_5_SHA1, TEE_MODE_VERIFY, 0);
            break;

        case TEE_CMD_RSA_ENC:
            ret = tee_core_rsa(params, TEE_ALG_RSASSA_PKCS1_V1_5_SHA1, TEE_MODE_ENCRYPT, 0);
            break;

        case TEE_CMD_RSA_DEC:
            ret = tee_core_rsa(params, TEE_ALG_RSASSA_PKCS1_V1_5_SHA1, TEE_MODE_DECRYPT, 0);
            break;
        case TEE_CMD_IMG_VERIFY:
            ret = tee_core_rsa(params, TEE_ALG_RSASSA_PKCS1_V1_5_SHA1, TEE_MODE_VERIFY, 1);
            break;
#endif
#if (CONFIG_ALGO_LPM > 0)

        case TEE_CMD_LPM:
            ret = tee_core_lpm(params);
            break;
#endif
#if (CONFIG_SYS_FREQ > 0)

        case TEE_CMD_SYS_FREQ:
            ret = tee_core_sys_freq(params);
            break;
#endif
#if (CONFIG_ALGO_HMAC > 0)

        case TEE_CMD_HMAC:
            ret = tee_core_hmac(params);
            break;
#endif

        case TEE_CMD_MANIFEST_INFO:
            ret = tee_core_manifest_info(params);
            break;

        case TEE_CMD_GET_DEV_INFO:
            ret = tee_core_get_dev_info(params);
            break;
#if (CONFIG_ALGO_HMAC > 0 )

        case TEE_CMD_DEV_INFO_SIGN:
            ret = tee_core_dev_info_sign(params);
            break;
#endif
        default:
            ret = TEE_ERROR_NOT_SUPPORTED;
            break;
    }

    return ret;
}

TEE_SRV_DATA_START(_cid_ta_createentrypoint,
                   _cid_ta_destroyentrypoint,
                   _cid_ta_opensessionentrypoint,
                   _cid_ta_closesessionentrypoint,
                   _cid_ta_invokecommandentrypoint)
TEE_SRV_UUID_PROPERTY("gpd.ta.appID", CID_TA_UUID)
TEE_SRV_BOOL_PROPERTY("gpd.ta.singleInstance", true)
TEE_SRV_BOOL_PROPERTY("gpd.ta.multiSession", true)
TEE_SRV_BOOL_PROPERTY("gpd.ta.instanceKeepAlive", true)
TEE_SRV_INT_PROPERTY("gpd.ta.dataSize", 0x1000)
TEE_SRV_INT_PROPERTY("gpd.ta.stackSize", 0x1000)
TEE_SRV_DATA_END

