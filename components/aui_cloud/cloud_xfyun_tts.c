/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <yoc/aui_cloud.h>
#include "cJSON.h"
#include <avutil/named_straightfifo.h>

#include "aui_cloud_imp.h"

#define MD5_DIGEST_SIZE 16

#define TAG "XFYUN_TTS"

static char *xf_appid;
static char *xf_apikey;


struct tts_arg {
    struct webclient_session *session;
    nsfifo_t *fifo;
};

void xfyun_tts_init()
{
    xf_appid = XF_TTS_APPID1;
    xf_apikey = XF_TTS_API_KEY1;
}

/**
    When the primary appid and apikey is used up,
    change the account for it
*/
void xfyun_tts_toggle_account()
{
    if (0 == strcmp(XF_TTS_APPID1, xf_appid)) {
        xf_appid = XF_TTS_APPID2;
        xf_apikey = XF_TTS_API_KEY2;
    } else {
        xf_appid = XF_TTS_APPID1;
        xf_apikey = XF_TTS_API_KEY1;
    }
}

char *xfyun_tts_get_appid()
{
    return xf_appid;
}

char *xfyun_tts_get_apikey()
{
    return xf_apikey;
}



/**


voice_name  string  是     发音人，可选值：详见发音人列表       xiaoyan
speed       string  否     语速，可选值：[0-100]，默认为50
volume      string  否     音量，可选值：[0-100]，默认为50
pitch       string  否     音高，可选值：[0-100]，默认为50

*/
void xfyun_tts_config_json_gen(aui_t *aui, char **json_out)
{
    aui_config_t *config = &aui->config;
    char speed_str[10] = {0};
    char volume_str[10] = {0};
    char pitch_str[10] = {0};

    sprintf(speed_str, "%d", config->spd);
    sprintf(volume_str, "%d", config->vol);
    sprintf(pitch_str, "%d", config->pit);

    char *json_str = NULL;
    cJSON *json_req = cJSON_CreateObject();
    aos_check_mem(json_req);

    cJSON_AddStringToObject(json_req, "aue", "lame");
    cJSON_AddStringToObject(json_req, "auf", "audio/L16;rate=16000");
    cJSON_AddStringToObject(json_req, "engine_type", "intp65");
    cJSON_AddStringToObject(json_req, "voice_name", "xiaoyan");
    cJSON_AddStringToObject(json_req, "speed", speed_str);
    cJSON_AddStringToObject(json_req, "volume", volume_str);
    cJSON_AddStringToObject(json_req, "pitch", pitch_str);

    json_str = cJSON_Print(json_req);
    *json_out = json_str;

    cJSON_Delete(json_req);

}



void xfyun_tts_to_fifo(struct webclient_session *session, nsfifo_t *fifo, char *buffer, int buffer_size)
{
    uint8_t reof = 0;
    int http_read_size = 0;
    int len = 0;
    char *pos = NULL;

    while (1) {
        nsfifo_get_eof(fifo, &reof, NULL); /** peer read reach to end */
        if (reof) {
            LOGE(TAG, "named fifo read eof");
            break;
        }

        len = nsfifo_get_wpos(fifo, &pos, 4000);
        if (len <= 0) {
            LOGE(TAG, "may be timeout. len = %d", len);
            break;
        }

        http_read_size = webclient_read(session, (unsigned char*)pos, len);
        if (http_read_size <= 0) {
            break;
        } else {
            /* send to player size = http_read_size */
            nsfifo_set_wpos(fifo, http_read_size);
        }
    }

    nsfifo_set_eof(fifo, 0, 1);
    nsfifo_close(fifo);

    return;

}

static void xfyun_tts_service(void *argv)
{
    unsigned char *http_buffer = aos_zalloc(HTTP_BUFFER_SIZE);
    aos_assert(http_buffer);
    struct tts_arg *arg = (struct tts_arg *)argv;

    xfyun_tts_to_fifo(arg->session, arg->fifo, (char *)http_buffer, HTTP_BUFFER_SIZE);
    aos_free(http_buffer);
    webclient_close(arg->session);
    aos_free(arg);
}


/*
    Method:         HTTP Post
    Header:         xfyun header
    Data:            text data
    Response:        Audio PCM Buffer
*/
int xfyun_tts(aui_t *aui, const char* name, char *txt_data)
{
    int ret = 0;
    int post_len = 0;
    int                       resp_status  = 0;
    char *                    post_data    = NULL;
    char *                    content_type = NULL;
    struct webclient_session *session      = NULL;
    aiservice_type_t          ai_type      = AUISRV_TYPE_TTS;

    xfyun_tts_toggle_account();

    if (strlen(txt_data) >= XF_TTS_TXT_SIZE_MAX)
        return -1;

    unsigned char *http_buffer = aos_zalloc(HTTP_BUFFER_SIZE);
    aos_assert(http_buffer);


    /** Init post data and content length */
    post_len  = (strlen(txt_data) * 3) + 16;
    post_data = (char *)aos_zalloc(post_len);
    strcpy(post_data, "text=");
    http_url_encode(post_data + strlen("text="), txt_data);

    session = webclient_session_create(HTTP_HEADER_SIZE);
    if (NULL == session) {
        LOGE(TAG, "webclient_session_create failed\n");
        goto err;
    }

    ret = webclient_build_http_header(aui, session, ai_type, strlen(post_data));
    if (ret < 0) {
        LOGE(TAG, "HTTP Header err=%d\n", ret);
        goto err;
    }

    ret = webclient_post(session, XF_TTS_POST_URI, post_data);
    if (200 != ret) {
        LOGE(TAG, "POST error=%d\n", resp_status);
        goto err;
    }

    /** Content-Type needs be audio/mpeg */
    content_type = (char *)webclient_header_fields_get(session, "Content-Type");
    if (NULL == content_type) {
        LOGE(TAG, "content_type ERR NULL");
        goto err;
    }
    if (0 != strcmp(content_type, "audio/mpeg")) {
        webclient_read(session, http_buffer, HTTP_BUFFER_SIZE);
        LOGE(TAG, "error content = %s\n", content_type, http_buffer);
        goto err;
    }

    //LOGD(TAG, "Audio Content-Length=%s", (char *)webclient_header_fields_get(session, "Content-Length"));


    /** the fifo size is determined by the network jitter */
    nsfifo_t *fifo = nsfifo_open(name, O_CREAT, 64*1024);
    if (NULL == fifo) {
        LOGE(TAG, "%s named fifo open fail", __FUNCTION__);
        goto err;
    }

    struct tts_arg *arg = aos_zalloc(sizeof(struct tts_arg));
    arg->fifo = fifo;
    arg->session = session;
    aos_task_t handle;
    aos_task_new_ext(&handle, "tts_service", xfyun_tts_service, arg, 2 * 1024,
                     AOS_DEFAULT_APP_PRI);

    if (post_data)
        aos_free(post_data);
    if (http_buffer)
        aos_free(http_buffer);
    /** close session in xfyun_tts_service */
    return 0;

err:
    if (session)
        webclient_close(session);
    if (post_data)
        aos_free(post_data);
    if (http_buffer)
        aos_free(http_buffer);

    return -1;
}


