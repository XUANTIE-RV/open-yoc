/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <yoc/aui_cloud.h>
#include "cJSON.h"
#include "avutil/url_encode.h"
#include "cloud_qq.h"

#define TAG "MUSIC_QQ"

#define HTTP_HEADER_SIZE  (1024)
#define HTTP_TXT_BUF_SIZE (10 * 1024)

#define QQ_MUSIC_LINKS        "http://114.55.168.53:8018/get_song"
#define QQ_MUSIC_LINKS_FORMAT "http://114.55.168.53:8018/get_song?name=%s"

/**
 * @param  [in] url : http url
 * @param  [out] content : recv buf for http response body
 * @param  [in] size : size of the content
 * @return total read bytes
 */
static uint8_t* _http_get2(const char *url)
{
    int content_length = 0;
    int total_read     = 0;
    int resp_status    = 0;

    struct webclient_session *session = NULL;
    uint8_t *music_buf = NULL;

    session = webclient_session_create(HTTP_HEADER_SIZE);
    if (NULL == session) {
        LOGE(TAG, "session create fail\n");
        goto err;
    }

    webclient_set_timeout(session, 3000); //set timeout to 500ms and retry

    /* send GET request by default header */
    if ((resp_status = webclient_get(session, url)) != 200) {
        LOGD(TAG, "GET ERR %d\n", resp_status);
        goto err;
    }

    content_length = webclient_content_length_get(session);
    if (content_length <= 0) {
        //baidu music goes here, HTTP Chuncked Mode
        music_buf = (uint8_t *)aos_zalloc(HTTP_TXT_BUF_SIZE);
        int cur_read = 0;
        while (total_read < HTTP_TXT_BUF_SIZE) {
            /* read result */
            cur_read = webclient_read(session, music_buf + total_read, HTTP_TXT_BUF_SIZE - total_read);
            if (cur_read <= 0)
                break;

            total_read += cur_read;
        }

        if (total_read >= HTTP_TXT_BUF_SIZE) {
            LOGE(TAG, "total_read is too large: %d\n", total_read);
            goto err;
        }
    } else {
        music_buf = aos_zalloc(content_length);
        total_read = webclient_read(session, music_buf, content_length);
        if (total_read != content_length) {
            LOGE(TAG, "not equal, need read = %d, bytes_read = %d\n", content_length, total_read);
            goto err;
        }
    }

    webclient_close(session);
    return music_buf;

err:
    if (music_buf)
        aos_free(music_buf);
    webclient_close(session);
    return NULL;
}

static int _qq_music_start(char *music_name, char *ourl, size_t osize)
{
    int rc = -1;
    char url[256];
    uint8_t *buf = NULL;
    char qstr[64] = {0}, pstr[64] = {0};
    cJSON *js = NULL, *mp3_l;

    if (strlen(music_name)) {
        snprintf(pstr, sizeof(pstr), "'%s'", music_name);
        url_encode((const char*)pstr, strlen(pstr), qstr, sizeof(qstr));
        snprintf(url, sizeof(url), QQ_MUSIC_LINKS_FORMAT, qstr);
    } else {
        snprintf(url, sizeof(url), "%s", QQ_MUSIC_LINKS);
    }

    buf= _http_get2((const char *)url);
    CHECK_RET_WITH_RET(buf, -1);

    js = cJSON_Parse((const char*)buf);
    CHECK_RET_WITH_GOTO(js, err);

    mp3_l = cJSON_GetObjectItem(js, "mp3_l");
    CHECK_RET_WITH_GOTO(mp3_l && mp3_l->valuestring, err);
    snprintf(ourl, osize, "%s", mp3_l->valuestring);
    rc = 0;

err:
    cJSON_Delete(js);
    aos_free(buf);
    return rc;
}

int qq_music(aui_t *aui, char *text)
{
    int ret = -1;
    char *url1, *url2;

    LOGD(TAG, "music text =%s", text);
    url1 = aos_zalloc(1024);
    url2 = aos_zalloc(1024);
    CHECK_RET_WITH_GOTO(url1 && url2, err);

    ret = _qq_music_start(text + strlen(MUSIC_PREFIX), url1, 1024);
    CHECK_RET_WITH_GOTO(ret == 0, err);
    LOGD(TAG, "Got Music URL=%s", url1);
    snprintf(url2, 1024, MUSIC_URL_NLP, url1);

    if (aui->config.nlp_cb)
        aui->config.nlp_cb(url2); /* example: music://http://mp3.com/1.mp3  */

err:
    aos_free(url1);
    aos_free(url2);
    return ret;
}


