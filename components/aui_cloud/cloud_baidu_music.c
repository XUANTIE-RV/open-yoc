/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <yoc/aui_cloud.h>
#include "cJSON.h"
#include "aui_cloud_imp.h"

#define TAG "CLOUD_BAIDU_MUSIC"

#define HTTP_HEADER_SIZE (1024)
#define BAIDU_HTTP_TXT_BUF_SIZE (10 * 1024)

//#define BD_MUSIC_LINKS_FORMAT  "http://ting.baidu.com/data/music/links?songIds=%s"
#define BD_MUSIC_LINKS_FORMAT "http://music.taihe.com/data/music/links?songIds=%s"
#define BD_MUSIC_SEARCH_FORMAT                                                                     \
    "http://tingapi.ting.baidu.com/v1/restserver/"                                                 \
    "ting?from=webapp_music&method=baidu.ting.search.catalogSug&format=json&callback=&query=%s"


/**
 * @param  [in] url : http url
 * @param  [out] content : recv buffer for http response body
 * @param  [in] size : size of the content
 * @return total read bytes
 */
uint8_t* http_get2(const char *url)
{
    int content_length = 0;
    int total_read     = 0;
    int resp_status    = 0;

    struct webclient_session *session = NULL;
    uint8_t *music_buffer = NULL;
     
    session = webclient_session_create(HTTP_HEADER_SIZE);
    if (NULL == session) {
        LOGE(TAG, "session create fail\n");
        goto err;
    }

    /* send GET request by default header */
    if ((resp_status = webclient_get(session, url)) != 200) {
        LOGD(TAG, "GET ERR %d\n", resp_status);
        goto err;
    }

    webclient_set_timeout(session, 500); //set timeout to 500ms and retry

    content_length = webclient_content_length_get(session);
    if (content_length <= 0) {
        //baidu music goes here, HTTP Chuncked Mode
        music_buffer = (uint8_t *)aos_zalloc(BAIDU_HTTP_TXT_BUF_SIZE);
        int cur_read = 0;
        while (total_read < BAIDU_HTTP_TXT_BUF_SIZE) {
            /* read result */
            cur_read = webclient_read(session, music_buffer + total_read, BAIDU_HTTP_TXT_BUF_SIZE - total_read);
            if (cur_read <= 0)
                break;

            total_read += cur_read;
        }

        if (total_read >= BAIDU_HTTP_TXT_BUF_SIZE) {
            LOGE(TAG, "total_read is too large: %d\n", total_read);
            goto err;
        }
    } else {
        music_buffer = aos_zalloc(content_length);
        total_read = webclient_read(session, music_buffer, content_length);
        if (total_read != content_length) {
            LOGE(TAG, "not equal, need read = %d, bytes_read = %d\n", content_length, total_read);
            goto err;
        }
    }

    webclient_close(session);
    return music_buffer;

err:
    if (music_buffer)
        aos_free(music_buffer);
    webclient_close(session);
    return NULL;
}

int baidu_music_extract_song_id(char *json, char *out_songid)
{
    int ret = AUI_ERR_OK;

    cJSON *json_root = cJSON_Parse(json);
    if (NULL == json_root) {
        LOGE(TAG, "JSON Root error buffer=%s", json);
        ret = AUI_ERR_JSON;
        goto out;
    }

    cJSON *artist = cJSON_GetObjectItemByPath(json_root, "song[0].artistname");
    cJSON *songid = cJSON_GetObjectItemByPath(json_root, "song[0].songid");
    cJSON *songname = cJSON_GetObjectItemByPath(json_root, "song[0].songname");

    if ((artist == NULL) ||
        (songid == NULL) ||
        (songname == NULL))
        goto out;

    LOGD(TAG, "Song info:Artist:%s Songid:%s Songname:%s", 
        artist->valuestring, 
        songid->valuestring,
        songname->valuestring);
    strcpy(out_songid, songid->valuestring);

out:
    if (json_root)
        cJSON_Delete(json_root);

    return ret;
}



int baidu_music_extract_auto_song_id(char *json, char *out_songid)
{
    int ret = AUI_ERR_OK;

    cJSON *json_root = cJSON_Parse(json);
    if (NULL == json_root) {
        LOGE(TAG, "JSON Root error buffer=%s", json);
        ret = AUI_ERR_JSON;
        goto out;
    }

    cJSON *songid = cJSON_GetObjectItemByPath(json_root, "song_list[0].song_id");

    if (songid == NULL)
        goto out;

    LOGD(TAG, "Song info:Songid:%s", songid->valuestring);
    strcpy(out_songid, songid->valuestring);

out:
    if (json_root)
        cJSON_Delete(json_root);

    return ret;
}


int baidu_music_extract_song_url(char *json, char *out_songurl)
{
    int ret = AUI_ERR_OK;

    /** 
        cJSON Process 
    */
    cJSON *json_root = cJSON_Parse(json);
    if (NULL == json_root) {
        ret = AUI_ERR_JSON;
        LOGE(TAG, "JSON Root error buffer=%s", json);
        goto out;
    }

    LOGD(TAG, "Song:%s\n", json);

#if 0
    cJSON *errcode = cJSON_GetObjectItem(json_root, "errorCode");
    if (NULL == errcode) {
        ret = AUI_ERR_JSON;
        LOGE(TAG, "No errcode");
        goto out;
    }

    if (errcode->valueint != 22000) {
        ret = AUI_ERR_JSON;
        LOGE(TAG, "Errcode failed: %d", errcode->valueint);
        goto out;
    }
#endif
    cJSON *songLink = cJSON_GetObjectItemByPath(json_root, "data.songList[0].songLink");
    if (NULL == songLink) {
        ret = AUI_ERR_JSON;
        LOGE(TAG, "No songLink");
        goto out;
    }

    if (strstr(songLink->valuestring, "http://") == NULL) {
        ret = AUI_ERR_JSON;
        LOGE(TAG, "Song link format error:%s\n", songLink->valuestring);
        goto out;
    }

    strcpy(out_songurl, songLink->valuestring);

out:
    if (json_root)
        cJSON_Delete(json_root);

    return ret;
}

/**
    Provide song id, get song url
*/
int baidu_music_step2(char *songid, char *out_url)
{

    int      ret   = AUI_ERR_OK;
    char     url[256] = {0};
    int try = 5;

    snprintf(url, sizeof(url), BD_MUSIC_LINKS_FORMAT, songid);
    //LOGD(TAG, "baidu music download url: %s\n", url);

    uint8_t *buffer;
    while (try-- > 1) {
        buffer = http_get2((const char *)url);
        if (buffer)
            break;
    }

    if (buffer == NULL)
        return AUI_ERR_FAIL;

    ret = baidu_music_extract_song_url((char*)buffer, out_url);
    if (AUI_ERR_OK != ret) {
        LOGE(TAG, "extract song url failed");
        goto err;
    }

    aos_free(buffer);
    return AUI_ERR_OK;
err:
    aos_free(buffer);
    return AUI_ERR_FAIL;
}


/**
    Provide empty song name, get song id
*/
int baidu_music_step_auto(const char *music_name, char *out_songid)
{
    int song_index = aos_now_ms() % 500;
    //int ret = -1;
    int try = 5;
    char url[150] = {0};
    sprintf(url, "http://tingapi.ting.baidu.com/v1/restserver/ting?format=json&method=baidu.ting.billboard.billList&type=2&size=1&offset=%d", song_index);

    uint8_t *buffer;
    while (try-- > 1) {
        buffer = http_get2((const char *)url);
        if (buffer)
            break;
    }

    if (buffer == NULL)
        return AUI_ERR_FAIL;

    baidu_music_extract_auto_song_id((char*)buffer, out_songid);

    aos_free(buffer);
    return AUI_ERR_OK;
}


/**
    Provide song name, get song id
*/
int baidu_music_step1(const char *music_name, char *out_songid)
{
    int      ret                     = 0;
    char     url[256]                = {0};
    char     music_name_encoded[128] = {0};
    int try = 5;

    http_url_encode(music_name_encoded, music_name);
    snprintf(url, sizeof(url), BD_MUSIC_SEARCH_FORMAT, music_name_encoded);
    //LOGD(TAG, "Require Music=%s URL=%s", music_name, url);
    
    uint8_t *buffer;
    while (try-- > 1) {
        buffer = http_get2((const char *)url);
        if (buffer)
            break;
    }

    if (buffer == NULL)
        return AUI_ERR_FAIL;

    /***
        Translate from:    ({a:b});
        To:                {a:b}
    */
    buffer[strlen((const char *)buffer) - 2] = 0x00;
    ret = baidu_music_extract_song_id((char *)(buffer + 1), out_songid);
    if (ret != AUI_ERR_OK) {
        LOGE(TAG, "extract song id failed");
        goto err;
    }

    aos_free(buffer);
    return AUI_ERR_OK;
err:
    aos_free(buffer);
    return AUI_ERR_FAIL;
}

int baidu_music_start(char *music_name, char *out_url)
{
    char song_id[100] = {0};

    if (strlen(music_name) > 0) {
        if (AUI_ERR_FAIL == baidu_music_step1(music_name, song_id))
            return AUI_ERR_FAIL;
    } else {
        if (AUI_ERR_FAIL == baidu_music_step_auto(music_name, song_id))
            return AUI_ERR_FAIL;
    }

    return baidu_music_step2(song_id, out_url);
}

int baidu_music(aui_t *aui, char *text)
{
    int ret = 0;
    
    char *song_url = aos_zalloc(1024);
    char *song_url_out = aos_zalloc(1024);
    aos_assert(song_url);
    aos_assert(song_url_out);
    if (AUI_ERR_OK == baidu_music_start(text + strlen(MUSIC_PREFIX), song_url)) {
        LOGD(TAG, "Got BaiduMusic URL=%s", song_url);
        snprintf(song_url_out, 1024, MUSIC_URL_NLP, song_url);
        if (aui->config.nlp_cb)
            aui->config.nlp_cb(song_url_out); /* example: music://http://mp3.com/1.mp3  */
    } else {
        //LOGE(TAG, "Got BaiduMusic URL failed");
        ret = AUI_ERR_FAIL;
    }
    aos_free(song_url);
    aos_free(song_url_out);

    
    return ret;

}


