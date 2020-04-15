/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include "aui_cloud_imp.h"
#include <time.h>
#include <sys/time.h>
#include "mbedtls/base64.h"
#include "mbedtls/md.h"
#include "mbedtls/md5.h"
#include <aos/log.h>
#include <aos/kv.h>
#include <cJSON.h>
#include <http_client.h>

static const char *TAG = "aliyun-cmm";

#define MD5_DIGEST_SIZE 16
#define TOKEN_MAX_SIZE  64
#define REQ_TOKEN_BASE_URL "http://nls-meta.cn-shanghai.aliyuncs.com"

static struct token_mgr {
    char token[TOKEN_MAX_SIZE];
    int expire_time;                // unix timestamp in seconds
} g_token_mgr;

// static int hex2dec(char c)
// {
//     if ('0' <= c && c <= '9')
//     {
//         return c - '0';
//     }
//     else if ('a' <= c && c <= 'f')
//     {
//         return c - 'a' + 10;
//     }
//     else if ('A' <= c && c <= 'F')
//     {
//         return c - 'A' + 10;
//     }
//     else
//     {
//         return -1;
//     }
// }
 
static char dec2hex(short int c)
{
    if (0 <= c && c <= 9)
    {
        return c + '0';
    }
    else if (10 <= c && c <= 15)
    {
        return c + 'A' - 10;
    }
    else
    {
        return -1;
    }
}

static int colon_encode(char url[])
{
    int i = 0;
    int len = strlen(url);
    int res_len = 0;
    char *res = aos_malloc(len * 2);

    if (res == NULL) {
        return -1;
    }

    for (i = 0; i < len; ++i)
    {
        char c = url[i];
        if (c == ':') {
            int j = (short int)c;
            if (j < 0)
                j += 256;
            int i1, i0;
            i1 = j / 16;
            i0 = j - i1 * 16;
            res[res_len++] = '%';
            res[res_len++] = dec2hex(i1);
            res[res_len++] = dec2hex(i0);            
        } else {
            res[res_len++] = c;
        }
    }
    res[res_len] = '\0';
    strcpy(url, res);
    aos_free(res);
    return 0;   
}
 
//编码一个url 
static int urlencode(char url[])
{
    int i = 0;
    int len = strlen(url);
    int res_len = 0;
    char *res = aos_malloc(len * 2);

    if (res == NULL) {
        return -1;
    }

    for (i = 0; i < len; ++i)
    {
        char c = url[i];
        if ( c == '+') {
            res[res_len++] = '%';
            res[res_len++] = '2';
            res[res_len++] = '0';
        }
        else if ( c == '*') {
            res[res_len++] = '%';
            res[res_len++] = '2';
            res[res_len++] = 'A';
        }
        else if (    ('0' <= c && c <= '9') ||
                ('a' <= c && c <= 'z') ||
                ('A' <= c && c <= 'Z') ||
                c == '.' || c == '-' || c == '_')
        {
            res[res_len++] = c;
        }
        else
        {
            int j = (short int)c;
            if (j < 0)
                j += 256;
            int i1, i0;
            i1 = j / 16;
            i0 = j - i1 * 16;
            res[res_len++] = '%';
            res[res_len++] = dec2hex(i1);
            res[res_len++] = dec2hex(i0);
        }
    }
    res[res_len] = '\0';
    strcpy(url, res);
    aos_free(res);
    return 0;
}

static char hex[16] = {'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};
void aliyun_gen_uuid(char uuid[33])
{
    // 5b1ff0604d6911ea9456e86a6492b10f
    int i;
    struct timeval tv;

    for (i = 0; i < 32; i++) {
        gettimeofday(&tv, NULL);
        uint64_t now = (uint64_t)tv.tv_sec * 1000 + tv.tv_usec / 1000;
        uint64_t ui = rand() * now;
        int mod = ui % 0xF;  
        uuid[i] = hex[mod];
    }
    uuid[32] = 0;
    LOGD(TAG, "uuid: %s", uuid);
}

void aliyun_gen_uuid_split(char uuid[37])
{
    // 5b1ff060-4d69-11ea-9456-e86a6492b10f
    int i;
    struct timeval tv;

    for (i = 0; i < 36; i++) {
        gettimeofday(&tv, NULL);
        uint64_t now = (uint64_t)tv.tv_sec * 1000 + tv.tv_usec / 1000;
        uint64_t ui = rand() * now;
        int mod = ui % 0xF;
        uuid[i] = hex[mod];
    }
        
    uuid[8] = '-';
    uuid[13] = '-';
    uuid[18] = '-';
    uuid[23] = '-';
    uuid[36] = 0;
    LOGD(TAG, "split-uuid: %s", uuid);
}

static int request_token_from_aliyun(const char *full_url)
{
#define RECV_BUF_SIZE 1024
    cJSON *js = NULL;
    char *buffer = NULL;
    http_client_handle_t client;
    http_errors_t err;
    http_client_config_t config = {
        .url = full_url,
        .method = HTTP_METHOD_GET,
    };

    client = http_client_init(&config);
    if (!client) {
        LOGE(TAG, "http cllient init e");
        goto fail;
    }
    if ((err = http_client_open(client, 0)) != HTTP_CLI_OK) {
        LOGE(TAG, "Failed to open HTTP connection: 0x%x @#@@@@@@", (err));
        goto fail;
    }
    int content_length =  http_client_fetch_headers(client);
    int total_read_len = 0, read_len;
    buffer = aos_malloc(RECV_BUF_SIZE + 1);
    if (total_read_len < content_length && content_length <= RECV_BUF_SIZE) {
        read_len = http_client_read(client, buffer, content_length);
        if (read_len <= 0) {
            LOGE(TAG, "Error read data");
            goto fail;
        }
        buffer[read_len] = 0;
        LOGD(TAG, "read_len = %d", read_len);
    }
    LOGD(TAG, "===> %s\r\n", buffer);
    js = cJSON_Parse(buffer);
    if (js == NULL) {
        goto fail;
    }
    cJSON *Token = cJSON_GetObjectItem(js, "Token");
    if (!(Token && cJSON_IsObject(Token))) {
        goto fail;
    }
    cJSON *ExpireTime = cJSON_GetObjectItem(Token, "ExpireTime");
    if (!(ExpireTime && cJSON_IsNumber(ExpireTime))) {
        goto fail;
    }
    g_token_mgr.expire_time = ExpireTime->valueint;
    cJSON *Id = cJSON_GetObjectItem(Token, "Id");
    if (!(Id && cJSON_IsString(Id))) {
        goto fail;
    }
    strncpy(g_token_mgr.token, Id->valuestring, sizeof(g_token_mgr.token) - 1);
    cJSON_Delete(js);
    http_client_close(client);
    http_client_cleanup(client);
    aos_free(buffer);

    LOGD(TAG, "token:%s, expiretime:%d", g_token_mgr.token, g_token_mgr.expire_time);
    return 0;
fail:
    if (js) cJSON_Delete(js);
    if (buffer) aos_free(buffer);
    return -1;
}

const char *aliyun_rasr_get_token(void)
{
#define BUF_SIZE 512
    int rc;
    struct timeval tv;
    struct tm tm;
    int now;
    char *buffer = NULL;
    char *query_string = NULL;
    char uuid[40];
    char tt[24];
    char *access_id = NULL;
    char *access_sec = NULL;
    char access_id_buf[48], access_sec_buf[48];
    const char *hd = "GET&%2F&";
    
    gettimeofday(&tv, NULL);
    now = tv.tv_sec + tv.tv_usec / 1000000;
    LOGD(TAG, "now：%d", now);
    if (g_token_mgr.expire_time > now) {
        LOGD(TAG, "token is valid.....");
        goto out;
    }

    buffer = aos_zalloc(BUF_SIZE);
    if (!buffer) goto out;
    query_string = aos_zalloc(BUF_SIZE);
    if (!query_string) goto out;

    rc = aos_kv_getstring("aliyun_accessid", access_id_buf, sizeof(access_id_buf));
    access_id = rc > 0 ? access_id_buf : ALIYUN_RASR_ACCESS_ID;
    rc = aos_kv_getstring("aliyun_accesssec", access_sec_buf, sizeof(access_sec_buf));
    access_sec = rc > 0 ? access_sec_buf : ALIYUN_RASR_ACCESS_SEC;

    localtime_r(&tv.tv_sec, &tm);
    strftime(tt, sizeof(tt), "%Y-%m-%dT%H:%M:%SZ", &tm);
    aliyun_gen_uuid_split(uuid);
    snprintf(buffer, BUF_SIZE,
             "AccessKeyId=%s&Action=CreateToken&Format=JSON&RegionId=cn-shanghai&SignatureMethod="
             "HMAC-SHA1&SignatureNonce=%s&SignatureVersion=1.0&Timestamp=%s&Version=2019-02-28",
             access_id, uuid, tt);
    colon_encode(buffer);
    strcpy(query_string, buffer); 
    urlencode(buffer);
    memmove(&buffer[8], buffer, strlen(buffer));
    memcpy(buffer, hd, strlen(hd));
    
    unsigned char hmac1out[20];
    snprintf(access_sec_buf, sizeof(access_sec_buf), "%s&", access_sec);
    mbedtls_md_hmac(mbedtls_md_info_from_type(MBEDTLS_MD_SHA1),
                    (uint8_t *)access_sec_buf, strlen(access_sec_buf),
                    (const uint8_t *)buffer, strlen(buffer),
                    hmac1out);
    size_t olen;
    size_t slen = 20;
    char signature[40] = {0};             
    mbedtls_base64_encode((unsigned char *)signature, sizeof(signature), &olen, hmac1out, slen);
    urlencode(signature);
    snprintf(buffer, BUF_SIZE, "%s/?Signature=%s&%s", REQ_TOKEN_BASE_URL, signature, query_string);
    LOGD(TAG, "fullurl[%d]=====> \n%s\n", strlen(buffer), buffer);
    rc = request_token_from_aliyun(buffer);
out:
    if (buffer) aos_free(buffer);
    if (query_string) aos_free(query_string);

    return (char *)g_token_mgr.token;
}