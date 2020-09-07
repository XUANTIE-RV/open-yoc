/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <yoc/aui_cloud.h>
#include <string.h>
#include <aos/aos.h>

#define TAG "CLOUD"

int aui_cloud_init(aui_t *aui)
{
    aos_check_param(aui);

    if (aui->ops.asr) {
        if (aui->ops.asr->init) {
            if (aui->ops.asr->init(aui))
                return -1;            
        }
    }
    if (aui->ops.nlp) {
        if (aui->ops.nlp->init) {
            if (aui->ops.nlp->init(aui))
                return -1;
        }
    }
    if (aui->ops.tts) {
        if (aui->ops.tts->init) {
            if (aui->ops.tts->init(aui))
                return -1;
        }
    }

    return 0;
}

int aui_cloud_enable_wwv(aui_t *aui, int enable)
{
    int ret = -1;

    aos_check_param(aui);

    if (aui->ops.asr && aui->ops.asr->enable_wwv)
        ret = aui->ops.asr->enable_wwv(aui, enable);

    return ret;
}

int aui_cloud_start_pcm(aui_t *aui)
{
    int ret = -1;

    aos_check_param(aui);

    if (aui->ops.asr) {
        if (aui->ops.asr->start)
            ret = aui->ops.asr->start(aui);
    }
    return ret;
}

/**
    Start to input PCM buffer to internal buffer
*/
int aui_cloud_push_pcm(aui_t *aui, void *data, size_t size)
{
    int ret = -1;

    aos_check_param(aui);
    aos_check_param(data);
    aos_check_param(size > 0);

    if (aui->ops.asr) {
        if (aui->ops.asr->push_data)
            ret = aui->ops.asr->push_data(aui, data, size);
    }

    return ret;
}

/**
    Finished the buffer input
*/
int aui_cloud_stop_pcm(aui_t *aui)
{
    int ret = -1;

    aos_check_param(aui);

    if (aui->ops.asr) {
        if (aui->ops.asr->stop_push_data)
            ret = aui->ops.asr->stop_push_data(aui);
    }

    return ret;
}

int aui_cloud_set_asr_session_id(aui_t *aui, const char *session_id)
{
    int ret = -1;

    aos_check_param(aui && session_id);

    if (aui->ops.asr && aui->ops.asr->set_session_id)
        ret = aui->ops.asr->set_session_id(aui, session_id);

    return ret;
}

int aui_cloud_stop(aui_t *aui)
{
    if (aui->ops.asr) {
        if (aui->ops.asr->stop)
            aui->ops.asr->stop(aui);
    }
    if (aui->ops.nlp) {
        if (aui->ops.nlp->stop)
            aui->ops.nlp->stop(aui);
    }
    if (aui->ops.tts) {
        if (aui->ops.tts->stop)
            aui->ops.tts->stop(aui);
    }
    return 0;
}

int aui_cloud_init_wwv(aui_t *aui, aui_wwv_cb_t cb)
{
    int ret = -1;

    aos_check_param(aui && cb);

    if (aui->ops.asr && aui->ops.asr->init_wwv)
        ret = aui->ops.asr->init_wwv(aui, cb);

    return ret;
}

int aui_cloud_push_wwv_data(aui_t *aui, void *data, size_t size)
{
    int ret = -1;

    aos_check_param(aui && data && size);

    if (aui->ops.asr && aui->ops.asr->push_wwv_data)
        ret = aui->ops.asr->push_wwv_data(aui, data, size);

    return ret;
}

int aui_cloud_start_nlp(aui_t *aui)
{
    int ret = -1;

    aos_check_param(aui);

    if (aui->ops.nlp && aui->ops.nlp->start)
        ret = aui->ops.nlp->start(aui);
    
    return ret;
}

int aui_cloud_push_text(aui_t *aui, char *text)
{
    int ret = 0;

    aos_check_param(aui);
    aos_check_param(text);

    if (strncasecmp(text, MUSIC_PREFIX, strlen(MUSIC_PREFIX)) == 0) {
        LOGI(TAG, "get music url start");
    } else {
        LOGI(TAG, "nlp start");
        ret = aui_textcmd_matchnlp(aui, text);
        if (ret < 0) {
            /** process normal NLP */
            if (aui->ops.nlp) {
                if (aui->ops.nlp->push_text)
                    ret = aui->ops.nlp->push_text(aui, text);
            }
        }
    }

    return ret;
}

int aui_cloud_stop_nlp(aui_t *aui)
{
    int ret = -1;

    aos_check_param(aui);

    if (aui->ops.nlp && aui->ops.nlp->stop)
        ret = aui->ops.nlp->stop(aui);
    
    return ret;    
}

int aui_cloud_start_tts(aui_t *aui)
{
    int ret = -1;

    aos_check_param(aui);

    if (aui->ops.tts && aui->ops.tts->start)
        ret = aui->ops.tts->start(aui);

    return ret;
}

void aui_cloud_set_tts_status_listener(aui_t *aui, aui_tts_cb_t stat_cb)
{
    aos_check_param(aui);

    if (aui->ops.tts) {
        if (aui->ops.tts->set_status_listener)
            aui->ops.tts->set_status_listener(aui, stat_cb);
    } 
}

int aui_cloud_req_tts(aui_t *aui, const char *text, const char *player_fifo_name)
{
    int ret = -1;
    
    aos_check_param(aui);
    aos_check_param(text);

    if (NULL == player_fifo_name) {
        player_fifo_name = "fifo://tts/1";
    }
    if (aui->ops.tts) {
        if (aui->ops.tts->req_tts)
            ret = aui->ops.tts->req_tts(aui, text, player_fifo_name);
    }
    return ret;
}

int aui_cloud_stop_tts(aui_t *aui)
{
    int ret = -1;

    aos_check_param(aui);

    if (aui->ops.tts && aui->ops.tts->stop)
        ret = aui->ops.tts->stop(aui);

    return ret;
}