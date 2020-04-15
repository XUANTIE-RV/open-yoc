/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <yoc/aui_cloud.h>
#include "aui_cloud_imp.h"

#include "pcm_buffer.h"

#define TAG "CLOUD_XFYUN_ASR"



static struct pcm_buffer_list pcm_buf_list;



/**
    Directly write 80KB will cause CPU Exception
    Directly write 32KB Should change lwip setting

*/
int webclient_send_http_buffer(struct webclient_session *session, char *dat, int dat_size)
{

    aos_check_param(session);
    aos_check_param(dat);
    
    int left_size = dat_size;

    do {
        int per_size = 1 * 1024;                                    //every write size
        int cur_size = left_size > per_size ? per_size : left_size; //current write size

        int write_len = webclient_write(
                            session, (const unsigned char *)(dat + (dat_size - left_size)), cur_size);
        if (write_len != cur_size) {
            LOGE(TAG, "write err, dat_size = %d, write_len = %d\n", dat_size, write_len);
            return -1;
        }

        left_size -= write_len;
        // LOGD(TAG, "Written=%d Left=%d", write_len, left_size);

    } while (left_size > 0);

    return 0;
}



/**
    Audio in, text out(both txt and nlp out)

    Method:         HTTP Post
    Header:         xfyun header
    Data:             Audio PCM data
    Response:         text
*/
int xfyun_asr_seq_buffer(aui_t *aui, char *dat, int dat_size)
{
    int                       ret     = 0;
    struct webclient_session *session = NULL;
    aiservice_type_t          ai_type = AUISRV_TYPE_ASR_NLP;

    unsigned char *http_buffer = aos_zalloc(HTTP_BUFFER_SIZE);
    aos_assert(http_buffer);

    /** HTTP header is built inside webclient */
    session = webclient_session_create(HTTP_HEADER_SIZE);
    if (NULL == session) {
        LOGE(TAG, "webclient_session_create failed\n");
        goto err;
    }

    ret = webclient_build_http_header(aui, session, ai_type, dat_size);
    if (ret < 0) {
        LOGE(TAG, "build http header err\n");
        goto err;
    }

    ret = webclient_connect(session, XF_ASR_POST_URI);
    if (ret != WEBCLIENT_OK) {
        LOGE(TAG, "connect to webclient server failed.\n");
        goto err;
    }

    ret = webclient_send_header(session, WEBCLIENT_POST);
    if (ret != WEBCLIENT_OK) {
        LOGE(TAG, "send header to webclient server failed. \n");
        goto err;
    }

    ret = webclient_send_http_buffer(session, dat, dat_size);
    if (ret < 0) {
        LOGE(TAG, "send http buffer error\n");
        goto err;
    }

    /* resolve response data, get http status code */
    ret = webclient_handle_response(session);
    if (ret != 200) {
        LOGE(TAG, "HTTP error %d", ret);
        webclient_read(session, http_buffer, HTTP_BUFFER_SIZE);
        LOGE(TAG, "HTTP error content = %s\n", http_buffer);
        goto err;
    }

    //int total_read =
    webclient_read_response(session, (char *)http_buffer, HTTP_BUFFER_SIZE);
    //LOGD(TAG, "Response Size=%d Strlen=%d Buffer=%s", total_read, strlen(http_buffer), http_buffer);
    webclient_close(session);


    aui->config.nlp_cb((char*)http_buffer);

    
    aos_free(http_buffer);

    return 0;

err:
    webclient_close(session);
    aos_free(http_buffer);

    return -1;
}

/**
    Audio in, text out(both txt and nlp out)

    Method:         HTTP Post
    Header:         xfyun header
    Data:             Audio PCM data
    Response:         text
*/
int xfyun_asr_pcm_buffer(aui_t *aui)
{
    int                       ret     = 0;
    struct webclient_session *session = NULL;
    aiservice_type_t          ai_type = AUISRV_TYPE_ASR_NLP;

    int http_buffer_size = HTTP_BUFFER_SIZE;
    unsigned char *http_buffer = aos_zalloc(http_buffer_size);
    aos_assert(http_buffer);

    /** HTTP header is built inside webclient */
    session = webclient_session_create(HTTP_HEADER_SIZE);
    if (NULL == session) {
        LOGE(TAG, "webclient_session_create failed\n");
        goto err;
    }

    ret = webclient_build_http_header(aui, session, ai_type, pcm_buffer_total_size(&pcm_buf_list));
    if (ret < 0) {
        LOGE(TAG, "build http header err\n");
        goto err;
    }

    ret = webclient_connect(session, XF_ASR_POST_URI);
    if (ret != WEBCLIENT_OK) {
        LOGE(TAG, "connect to webclient server failed.\n");
        goto err;
    }

    ret = webclient_send_header(session, WEBCLIENT_POST);
    if (ret != WEBCLIENT_OK) {
        LOGE(TAG, "send header to webclient server failed. \n");
        goto err;
    }

    /** loop the buffer and send */
    {
        int                     i    = 0;
        struct pcm_buffer_list *list = &pcm_buf_list;
        for (i = 0; i < sizeof(list->buffer) / sizeof(list->buffer[0]); i++) {
            struct pcm_buffer *buffer = &list->buffer[i];
            if (buffer->valid) {
                LOGD(TAG, "Will write %d bytes, Total=%d bytes", buffer->used,
                     pcm_buffer_total_size(&pcm_buf_list));
                ret = webclient_send_http_buffer(session, buffer->buffer, buffer->used);
                if (ret < 0) {
                    LOGE(TAG, "send http buffer error\n");
                    pcm_buffer_free(&pcm_buf_list);
                    goto err;
                }
            }
        }
        pcm_buffer_free(&pcm_buf_list);
    }


    /* resolve response data, get http status code */
    ret = webclient_handle_response(session);
    if (ret != 200) {
        LOGE(TAG, "HTTP error %d", ret);
        webclient_read(session, http_buffer, http_buffer_size);
        LOGE(TAG, "HTTP error content = %s\n", http_buffer);
        goto err;
    }

    /** realloc http buffer for http content */
    char* content_length = (char *)webclient_header_fields_get(session, "Content-Length");
    http_buffer_size = atoi(content_length) + 10;//10 byte is margin;
    LOGD(TAG, "Content length= %s",  content_length);
    http_buffer = aos_realloc(http_buffer, http_buffer_size);


    webclient_read_response(session, (char *)http_buffer, http_buffer_size);
    //LOGD(TAG, " Strlen=%d Buffer=%s", strlen((char*)http_buffer), http_buffer);
    webclient_close(session);

    
    aui->config.nlp_cb((char*)http_buffer);

    aos_free(http_buffer);
    return 0;

err:
    webclient_close(session);
    aos_free(http_buffer);

    return -1;
}


void xfyun_asr_init()
{
    pcm_buffer_init(&pcm_buf_list);
}


int xfyun_asr_source_pcm_finish(aui_t *aui)
{
    xfyun_asr_pcm_buffer(aui);
    return 0;
}


int xfyun_asr_source_pcm(char *data, int size)
{
    return pcm_buffer_copy(&pcm_buf_list, data, size);
}
