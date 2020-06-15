/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <stdbool.h>
#include <webclient.h>
#include "mbedtls/base64.h"
#include "mbedtls/md.h"
#include "mbedtls/md5.h"
#include "avutil/url_encode.h"
#include "cJSON.h"
#include <time.h>

#include <yoc/aui_cloud.h>
#include "cloud_xunfei.h"


//#define XF_GET_HEADER_BUFSZ    (1024)
//#define XF_GET_RESP_BUFSZ      (10*1024)

#define MD5_DIGEST_SIZE 16

#define TAG "ASR_CORE"



/**
 * @brief  binary to base16
 * @param  [out] out
 * @param  [in] digest : origin binary data
 * @param  [in] n : the len of digest
 * @return
 */
static void hex_to_str(uint8_t *out, const uint8_t *digest, size_t n)
{
    char const encode[] = "0123456789abcdef";
    int        j        = 0;
    int        i        = 0;

    for (i = 0; i < n; i++) {
        int a    = digest[i];
        out[j++] = encode[(a >> 4) & 0xf];
        out[j++] = encode[a & 0xf];
    }
}

static char *XF_NLP_PARAM_FORMAT()
{
    static char *fmt = NULL;

    if (fmt == NULL)
        asprintf(&fmt, XF_NLP_PARAM_FORMATX, get_latitude(), get_longitude());

    return fmt;
}


static char *XF_ASR_PARAM_FORMAT()
{
    static char *fmt = NULL;

    if (fmt == NULL)
        asprintf(&fmt, XF_ASR_PARAM_FORMATX, get_latitude(), get_longitude());

    return fmt;
}

char *xfyun_rasr_get_handshake_params(void)
{
    int rc;
    time_t tt;
    unsigned char out[MD5_DIGEST_SIZE];
    char  buffer[48] = {0};
    char *appid      = NULL;
    char *api_key    = NULL;
    char appid_buf[16], apikey_buf[48];
    static uint8_t digest[128] = {0};

    rc = aos_kv_getstring("xf_rasr_appid", appid_buf, sizeof(appid_buf));
    appid = rc > 0 ? appid_buf : XF_RASR_APPID;
    rc = aos_kv_getstring("xf_rasr_apikey", apikey_buf, sizeof(apikey_buf));
    api_key = rc > 0 ? apikey_buf : XF_RASR_API_KEY;

    tt = time(NULL);
    snprintf(buffer, sizeof(buffer), "%s%d", appid, tt);

    //md5(API_KEY + curTime)
    mbedtls_md5((const unsigned char *)buffer, strlen(buffer), out);

    memset(buffer, 0, sizeof(buffer));
    hex_to_str((uint8_t *)buffer, out, MD5_DIGEST_SIZE);

    // mbedtls_md_context_t sha_ctx;
    memset(digest, 0, sizeof(digest));
    snprintf((char *)digest, sizeof(digest), "appid=%s&ts=%d&signa=", appid, tt);
    int len = strlen((char *)digest);
    unsigned char hmac1out[20];
    mbedtls_md_hmac(mbedtls_md_info_from_type(MBEDTLS_MD_SHA1),
                    (uint8_t *)api_key, strlen(api_key),
                    (const uint8_t *)buffer, MD5_DIGEST_SIZE * 2,
                    hmac1out);

    size_t olen;
    size_t slen = 20;
    char qstr[64] = {0}, b64[64] = {0};

    mbedtls_base64_encode((unsigned char *)b64, sizeof(b64), &olen, hmac1out, slen);
    url_encode((const char*)b64, strlen(b64), qstr, sizeof(qstr));

    snprintf((char*)&digest[len], sizeof(digest) - len, "%s", qstr);
    //mbedtls_base64_encode(&digest[len], sizeof(digest) - len, &olen, hmac1out, slen);
    LOGD(TAG, "handshake params:%s\n", digest);
    return (char *)digest;
}


/**
    Build the xunfei headers
*/
int webclient_build_http_header(aui_t *aui, struct webclient_session *session, aiservice_type_t ai_type,
                                int content_length)
{
    //int                 ret;
    time_t              tt;
    uint8_t *           b64 = NULL; //json to base64
    size_t              dlen, olen, slen;
    mbedtls_md5_context ctx;
    unsigned char       out[MD5_DIGEST_SIZE];
    char                curtime[48] = {0};
    uint8_t             digest[48]  = {0};
    char *              param       = NULL;
    char *              appid       = NULL;
    char *              api_key     = NULL;

    switch (ai_type) {
    case AUISRV_TYPE_ASR:
    case AUISRV_TYPE_ASR_NLP:
        appid = XF_APPID;
        api_key = XF_ASR_API_KEY;
        param   = XF_ASR_PARAM_FORMAT();
        break;
    case AUISRV_TYPE_NLP:
        appid = XF_APPID;
        api_key = XF_ASR_API_KEY;
        param   = XF_NLP_PARAM_FORMAT();
        break;
    case AUISRV_TYPE_TTS:
        appid = xfyun_tts_get_appid();
        api_key = xfyun_tts_get_apikey();
        /** generate tts config */
        xfyun_tts_config_json_gen(aui, &param); /** warning need free memory 'param' */
        break;
    default:
        return -1;
    }

    //LOGD(TAG, "APIKEY=%s PARAM=%s", api_key, param);

    slen = strlen(param);
    dlen = slen * 3 / 2 + 4; //large enough to hold output
    b64  = aos_zalloc(dlen);

    //Encode Json string to base64 array
    mbedtls_base64_encode(b64, dlen, &olen, (unsigned char *)param, slen);
    //LOGD(TAG, "ret = %d, b64 = %s, dlen = %d olen = %d", ret, b64, dlen, olen);
    aos_assert(dlen >= olen);

    /* construct post param */
    tt = time(NULL);
    snprintf(curtime, sizeof(curtime), "%d", tt);
    //LOGD(TAG, "system time:%s", ctime(&tt)); //will  cause memory bug

    //md5(API_KEY + curTime + paramBase64)
    mbedtls_md5_init(&ctx);
    mbedtls_md5_starts(&ctx);
    mbedtls_md5_update(&ctx, (const unsigned char *)api_key, strlen(api_key));
    mbedtls_md5_update(&ctx, (const unsigned char *)curtime, strlen(curtime));
    mbedtls_md5_update(&ctx, b64, olen);
    mbedtls_md5_finish(&ctx, out);
    mbedtls_md5_free(&ctx);
    //md5 to base16 string
    hex_to_str(digest, out, MD5_DIGEST_SIZE);

    /* build header for upload */
    webclient_header_fields_add(session, "X-CurTime: %s\r\n", curtime);
    webclient_header_fields_add(session, "X-Param: %s\r\n", b64);
    webclient_header_fields_add(session, "X-Appid: %s\r\n", appid);
    webclient_header_fields_add(session, "X-CheckSum: %s\r\n", digest);
    webclient_header_fields_add(session, "Content-Length: %d\r\n", content_length);
    if (AUISRV_TYPE_TTS == ai_type)
        webclient_header_fields_add(
            session, "Content-Type: application/x-www-form-urlencoded; charset=utf-8\r\n");
    //webclient_header_fields_add(session, "Content-Type: application/octet-stream\r\n");

    if (ai_type == AUISRV_TYPE_TTS)
        aos_free(param);
    aos_free(b64);

    return 0;
}



/**
    If buffer size is too small, need call webclient_read_response multiple times
*/
int webclient_read_response(struct webclient_session *session, char *buffer, int buffer_size)
{
    int total_read = 0;
    int cur_read   = 0;

    do {
        cur_read = webclient_read(session, (unsigned char *)(buffer + total_read),
                                  buffer_size - total_read);
        if (cur_read <= 0) {
            break;
        } else {
            total_read += cur_read;
            //LOGD(TAG, ">>> current read size=%d/%d", cur_read, buffer_size);
        }

    } while (1);

    return total_read;
}


