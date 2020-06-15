/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <string.h>
#include <aos/aos.h>
#include <yoc/aui_cloud.h>
#include "cloud_xunfei.h"


#define TAG "XFYUN_NLP"


/**
    Text in, text out
*/
static int xfyun_nlp(aui_t *aui, char *text_in)
{
    int                       ret     = 0;
    struct webclient_session *session = NULL;
    aiservice_type_t          ai_type = AUISRV_TYPE_NLP;

    int http_buffer_size = HTTP_BUFFER_SIZE;
    unsigned char *http_buffer = aos_zalloc(http_buffer_size);
    aos_assert(http_buffer);

    /** HTTP header is built inside webclient */
    session = webclient_session_create(HTTP_HEADER_SIZE);
    if (NULL == session) {
        LOGE(TAG, "webclient_session_create failed\n");
        goto err;
    }

    ret = webclient_build_http_header(aui, session, ai_type, strlen(text_in));
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

    int write_len = webclient_write(session, (const unsigned char *)text_in, strlen(text_in));
    if (write_len != strlen(text_in)) {
        LOGE(TAG, "write err, dat_size = %d, write_len = %d\n", strlen(text_in), write_len);
        goto err;
    }

    /* resolve response data, get http status code */
    ret = webclient_handle_response(session);
    if (ret != 200) {
        webclient_read(session, http_buffer, http_buffer_size);
        LOGE(TAG, "HTTP error %d content = %s\n", ret, http_buffer);
        goto err;
    }

    /** realloc http buffer for http content */
    char* content_length = (char *)webclient_header_fields_get(session, "Content-Length");
    http_buffer_size = atoi(content_length) + 10; //10 byte is margin
    http_buffer = aos_realloc(http_buffer, http_buffer_size);

    webclient_read_response(session, (char *)http_buffer, http_buffer_size);
    webclient_close(session);

    aui->config.nlp_cb((char*)http_buffer);
    aos_free(http_buffer);

    return 0;

err:
    webclient_close(session);
    aos_free(http_buffer);

    return -1;

}

static aui_nlp_cls_t xunfei_nlp_cls = {
    .push_text = xfyun_nlp,
};

void aui_nlp_register_xunfei(aui_t *aui)
{
    if (aui) {
        aui->ops.nlp = &xunfei_nlp_cls;
    }
}
