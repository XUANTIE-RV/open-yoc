/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <string.h>
#include <aos/aos.h>

#include <yoc/aui_cloud.h>

#define TAG "CLOUD"

static aui_t *g_default_handle = NULL;

aui_t *aui_cloud_init(void)
{
    aui_t *aui = aos_zalloc_check(sizeof(aui_t));

    if(g_default_handle == NULL) {
        g_default_handle = aui;
    }

    return aui;
}

int aui_cloud_deinit(aui_t *aui)
{
    if (aui == NULL) {
        aui = g_default_handle;
    }

    aos_check_return_einval(aui);
    
    aos_free(aui);

    return 0;
}

int aui_cloud_start_audio(aui_t *aui, aui_audio_req_type_e type)
{
    int ret = -1;

    if (aui == NULL) {
        aui = g_default_handle;
    }

    aos_check_return_einval(aui);

    aui->audio_req_type = type;
    if (aui->ops.nlp) {
        if (aui->ops.nlp->start)
            ret = aui->ops.nlp->start(aui);
    }

    return ret;
}

/**
    Start to input audio buffer to internal buffer
*/
int aui_cloud_push_audio(aui_t *aui, void *data, size_t size)
{
    int ret = -1;

    if (aui == NULL) {
        aui = g_default_handle;
    }

    aos_check_return_einval(aui);
    aos_check_return_einval(data);
    aos_check_return_einval(size > 0);

    if (aui->ops.nlp) {
        if (aui->ops.nlp->push_data)
            ret = aui->ops.nlp->push_data(aui, data, size);
    }

    return ret;
}

/**
    Finished the buffer input
*/
int aui_cloud_stop_audio(aui_t *aui)
{
    int ret = -1;

    if (aui == NULL) {
        aui = g_default_handle;
    }

    aos_check_return_einval(aui);

    if (aui->ops.nlp) {
        if (aui->ops.nlp->stop_push_data)
            ret = aui->ops.nlp->stop_push_data(aui);
    }

    return ret;
}

int aui_cloud_stop(aui_t *aui)
{
    int ret = -1;

    if (aui == NULL) {
        aui = g_default_handle;
    }

    aos_check_return_einval(aui);

    if (aui->ops.nlp) {
        if (aui->ops.nlp->stop)
            ret = aui->ops.nlp->stop(aui);
    }
    if (aui->ops.tts) {
        if (aui->ops.tts->stop)
            ret = aui->ops.tts->stop(aui);
    }

    return ret;
}

int aui_cloud_push_text(aui_t *aui, char *text)
{
    int ret = 0;

    if (aui == NULL) {
        aui = g_default_handle;
    }

    aos_check_return_einval(aui);
    aos_check_return_einval(text);

    if (aui->ops.nlp && aui->ops.nlp->push_text) {
        ret = aui->ops.nlp->push_text(aui, text);
    }

    return ret;
}

int aui_cloud_start_tts(aui_t *aui)
{
    int ret = -1;

    if (aui == NULL) {
        aui = g_default_handle;
    }

    aos_check_return_einval(aui);

    if (aui->ops.tts && aui->ops.tts->start)
        ret = aui->ops.tts->start(aui);

    return ret;
}

int aui_cloud_req_tts(aui_t *aui, const char *text)
{
    int ret = -1;

    if (aui == NULL) {
        aui = g_default_handle;
    }

    aos_check_return_einval(aui);
    aos_check_return_einval(text);

    if (aui->ops.tts && aui->ops.tts->req_tts) {
        ret = aui->ops.tts->req_tts(aui, text);
    }

    return ret;
}

int aui_cloud_stop_tts(aui_t *aui)
{
    int ret = -1;

    if (aui == NULL) {
        aui = g_default_handle;
    }

    aos_check_return_einval(aui);

    if (aui->ops.tts && aui->ops.tts->stop)
        ret = aui->ops.tts->stop(aui);

    return ret;
}

int aui_cloud_nlp_unregister(aui_t *aui)
{
    int ret = 0;

    if (aui == NULL) {
        aui = g_default_handle;
    }

    aos_check_return_einval(aui);

    if (aui->ops.nlp && aui->ops.nlp->deinit) {
        ret = aui->ops.nlp->deinit(aui);
    }

    aui->ops.nlp = NULL;

    return ret;
}

int aui_cloud_tts_unregister(aui_t *aui)
{
    if (aui == NULL) {
        aui = g_default_handle;
    }

    aos_check_return_einval(aui);

    int ret = 0;

    if (aui->ops.tts && aui->ops.tts->deinit) {
        ret = aui->ops.tts->deinit(aui);
    }

    aui->ops.tts = NULL;

    return ret;
}

int aui_cloud_nlp_register(aui_t *aui, aui_nlp_cls_t *ops, aui_nlp_cb_t cb, void *priv)
{
    if (aui == NULL) {
        aui = g_default_handle;
    }

    aos_check_return_einval(aui && ops && cb);

    aui->ops.nlp = ops;
    aui->cb.nlp_cb = cb;
    aui->cb.nlp_priv = priv;
    
    if (aui->ops.nlp->init) {
        return aui->ops.nlp->init(aui);
    }

    return -1;
}

int aui_cloud_tts_register(aui_t *aui, aui_tts_cls_t *ops, aui_tts_cb_t cb, void *priv)
{
    if (aui == NULL) {
        aui = g_default_handle;
    }

    aos_check_return_einval(aui && ops && cb);

    aui->ops.tts = ops;
    aui->cb.tts_cb = cb;
    aui->cb.tts_priv = priv;

    if (aui->ops.tts->init) {
        return aui->ops.tts->init(aui);
    }
    return -1;
}
