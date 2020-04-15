/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <yoc/netio.h>
#include <yoc/fota.h>
#include <aos/network.h>
#include <aos/kv.h>
#include <aos/version.h>
#include <yoc/sysinfo.h>
#include "../http/http.h"
#include <aos/log.h>

#define COP_IMG_URL "cop_img_url"
#define COP_VERSION "cop_version"
#define TO_URL      "flash://misc"

#define TAG "fota"

char *json_getvalue(char *body, char *key, int *len)
{
    char key_buf[16];
    char *start_ptr, *end_ptr;

    snprintf(key_buf, 16, "\"%s\":", key);

    start_ptr = strstr(body, key_buf);

    if (start_ptr == NULL || body == start_ptr) {
        LOGD(TAG, "key");
        return NULL;
    }

    start_ptr += strlen(key_buf);

    while ((*start_ptr == ' ') || (*start_ptr == '\"') || (*start_ptr == 0)) {
        start_ptr++;
    }

    end_ptr = start_ptr;

    while (((*end_ptr != ',') && (*end_ptr != '\"')) && (*end_ptr != 0)) {
        end_ptr++;
    }

    if (start_ptr == end_ptr) {
        return NULL;
    }

    if (*end_ptr == '\"') {
        end_ptr --;
    }

    *len = end_ptr - start_ptr + 1;

    return start_ptr;
}

static int cop_get_ota_url(char *ota_url, int len)
{
    int ret = -1;

    ret = aos_kv_getstring("otaurl", ota_url, len);
    if (ret < 0) {
        strcpy(ota_url, "http://occ.t-head.cn/api/image/ota/pull");
    }

    return ret;
}

static int cop_version_check(fota_info_t *info) {
    http_t *http;
    char *payload;
    char *body;
    int value_len;
    char *cptr;
    char getvalue[64];
    int rc;

    if ((payload = malloc(156)) == NULL) {
        return -1;
    }

    snprintf(payload, 156, "{\"cid\":\"%s\",\"model\":\"%s\",\"version\":\"%s\"}",
             aos_get_device_id(), aos_get_product_model(), aos_get_app_version());
    //snprintf(payload, 100, "{\"cid\":\"%s\",\"model\":\"%s\",\"version\":\"%s\"}",
    //         "00A2C6FB32241D9423F5AF00", "hlb_test", "1.0.0-20181125.1321-R-hlb_tes");

    LOGI(TAG, "cop_update: %s", payload);

    memset(getvalue, 0, sizeof(getvalue));
    cop_get_ota_url(getvalue, sizeof(getvalue));

    if ((http = http_init(getvalue)) == NULL) {
        LOGD(TAG, "e cop http init");
        free(payload);
        return -1;
    }
    memset(http->buffer, 0, BUFFER_SIZE);
    http->buffer_offset = 0;
    http_head_sets(http, "Host", http->host);
    http_head_seti(http, "Content-Length", strlen(payload));
    http_head_sets(http, "Connection", "keep-alive");
    http_head_sets(http, "Content-Type", "application/json");
    http_head_sets(http, "Cache-Control", "no-cache");

    http_post(http, payload, 10000);

    if (http_wait_resp(http, &body, 10000) < 0) {
        http_deinit(http);
        free(payload);
        LOGD(TAG, "recv failed");
        return -1;
    }

    free(payload);
    LOGD(TAG, "resp body: %s", body);
    cptr = json_getvalue(body, "code", &value_len);

    if ((cptr == NULL) || (atoi(cptr) < 0)) {
        LOGW(TAG, "rsp format");
        http_deinit(http);
        return -1;
    }

    cptr = json_getvalue(body, "version", &value_len);
    if (cptr == NULL) {
        LOGD(TAG, "rsp format %s", "version");
        http_deinit(http);
        return -1;
    }
    char *ver = malloc(value_len + 1);
    if (!ver) {
        return -1;
    }
    memcpy(ver, cptr, value_len);
    ver[value_len] = 0;
    LOGD(TAG, "%s: %s", COP_VERSION, ver);
    aos_kv_setstring(COP_VERSION, ver);
    free(ver);

    cptr = json_getvalue(body, "url", &value_len);

    if (cptr == NULL) {
        LOGD(TAG, "rsp format %s", "url");
        http_deinit(http);
        return -1;
    }

    if (http->url) {
        free(http->url);
        http->url = NULL;
    }
    http->url = calloc(value_len + 1, 1);

    if (http->url == NULL) {
        LOGD(TAG, "realloc failed");
        http_deinit(http);
        return -1;
    }

    if (strstr(cptr, "https://")) {
        snprintf(http->url, value_len, "http://%s", cptr + strlen("https://"));
    } else {
        memcpy(http->url, cptr, value_len);
    }

    char *buffer = malloc(156);

    if (buffer == NULL) {
        http_deinit(http);
        return -1;
    }

    rc = aos_kv_getstring(COP_IMG_URL, buffer, 156);

    if (rc <= 0) {
        aos_kv_setstring(COP_IMG_URL, http->url);
    } else {
        if (strcmp(http->url, buffer) == 0) {
            aos_kv_getint("fota_offset", &rc);
            LOGI(TAG, "continue fota :%d", rc);
        } else {
            aos_kv_setstring(COP_IMG_URL, http->url);
            aos_kv_setint("fota_offset", 0);
            LOGI(TAG, "restart fota");
        }
    }
    free(buffer);
    if (info->fota_url) {
        free(info->fota_url);
        info->fota_url = NULL;
    }
    info->fota_url = strdup(http->url);
    LOGD(TAG, "get url: %s", info->fota_url);
    http_deinit(http);
    return 0;
}

static int cop_finish(void)
{
    return 0;
}

const fota_cls_t fota_cop_cls = {
    "cop",
    NULL,
    cop_version_check,
    cop_finish,
    NULL,
};

int fota_register_cop(void)
{
    return fota_register(&fota_cop_cls);
}
