/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <yoc/aui_cloud.h>
#include "pcm_buffer.h"
#include "aui_cloud_imp.h"

#define TAG "CLOUD"

// #define CLOUD_MUSIC_QQ
#define CLOUD_MUSIC_BAIDU

int aui_cloud_init(aui_t *aui)
{
    aos_check_param(aui);

    xfyun_tts_init();

    switch (aui->asr_type)
    {
    default:
    case CLOUD_RASR_XFYUN:
        xfyun_rasr_init(aui);
        break;
    case CLOUD_ASR_XFYUN:
        xfyun_asr_init();
        break;
    case CLOUD_ASR_BAIDU:
        baidu_asr_init(aui);
        break;
    case CLOUD_ASR_PROXY_WS:
        proxy_ws_asr_init(aui);
        break;
    case CLOUD_RASR_ALIYUN:
        aliyun_rasr_init(aui);
        break;
    }

    return 0;
}

int aui_cloud_enable_wwv(aui_t *aui, int enable)
{
    return 0;
}

int aui_cloud_start_pcm(aui_t *aui)
{
    return 0;
}

/**
    Start to input PCM buffer to internal buffer
*/
int aui_cloud_push_pcm(aui_t *aui, void *data, size_t size)
{
    aos_check_param(aui);
    aos_check_param(data);
    aos_check_param(size > 0);

    if (size > PCM_BUFFER_SIZE) {
        LOGE(TAG, "pcm size out of range %d", size);
        return -ERANGE;
    }

    switch (aui->asr_type)
    {
    case CLOUD_RASR_XFYUN:
        return xfyun_rasr_source_pcm(aui, data, size);
    case CLOUD_ASR_XFYUN:
        return xfyun_asr_source_pcm(data, size);
    case CLOUD_ASR_BAIDU:
        return baidu_asr_source_pcm(aui, data, size);
    case CLOUD_ASR_PROXY_WS:
        return proxy_ws_asr_source_pcm(aui, data, size);
    case CLOUD_RASR_ALIYUN:
        return aliyun_rasr_source_pcm(aui, data, size);
    default:
        break;
    }
    return -1;
}

/**
    Finished the buffer input
*/
int aui_cloud_stop_pcm(aui_t *aui)
{
    int ret = -1;

    aos_check_param(aui);

    switch (aui->asr_type)
    {
    case CLOUD_RASR_XFYUN:
        ret = xfyun_rasr_source_pcm_finish(aui);
        break;
    case CLOUD_ASR_XFYUN:
        ret = xfyun_asr_source_pcm_finish(aui);
        break;
    case CLOUD_ASR_BAIDU:
        ret = baidu_asr_source_pcm_finish(aui);
        break;
    case CLOUD_ASR_PROXY_WS:
        ret = proxy_ws_asr_source_pcm_finish(aui);
        break;
    case CLOUD_RASR_ALIYUN:
        ret = aliyun_rasr_source_pcm_finish(aui);
        break;
    default:
        break;
    }

    return ret;
}

int aui_cloud_set_account(const char *json_account_info)
{
    return 0;
}

int aui_cloud_set_session_id(const char *session_id)
{
    return 0;
}

int aui_cloud_force_stop(aui_t *aui)
{
    return 0;
}

int aui_cloud_init_wwv(aui_t *aui, aui_wwv_cb_t cb)
{
    return 0;
}

int aui_cloud_push_wwv_data(aui_t *aui, void *data, size_t size)
{
    return 0;
}

int aui_cloud_push_text(aui_t *aui, char *text)
{
    int ret = 0;

    aos_check_param(aui);
    aos_check_param(text);

    if (strncasecmp(text, MUSIC_PREFIX, strlen(MUSIC_PREFIX)) == 0) {
        LOGI(TAG, "get music url start");
#ifdef CLOUD_MUSIC_QQ
        ret = qq_music(aui, text);
#elif defined CLOUD_MUSIC_BAIDU
        ret = baidu_music(aui, text);
#endif
    } else {
        LOGI(TAG, "nlp start");
        ret = aui_textcmd_matchnlp(aui, text);
        if (ret < 0) {
            /** process normal NLP */
            ret = xfyun_nlp(aui, text);
        }
    }

    return ret;
}

int aui_cloud_start_tts(aui_t *aui)
{
    return 0;
}

int aui_cloud_stop_tts(aui_t *aui)
{
    return 0;
}

int aui_cloud_req_tts(aui_t *aui, const char *player_fifo_name, const char *text, aui_tts_cb_t stat_cb)
{

    aos_check_param(aui);
    aos_check_param(text);

    if (NULL == player_fifo_name) {
        player_fifo_name = "fifo://tts/1";
    }
    return xfyun_tts(aui, player_fifo_name, (char *)text);
}
