/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>

#include <yoc/aui_cloud.h>
#include <media.h>
#include <aos/ringbuffer.h>
#include <aos/log.h>
#include <aos/debug.h>

#include "cJSON.h"
#include "nui_things.h"
#include "cloud_mit.h"
#include "nls_nui_things_tts.h"

#define TAG "MIT_ASR"

#define MIT_ASR_BUFFER_SIZE 200 * 1024 //50K

#define MIT_ASR_KEY "y5QsLk2A3acWEhCs"
#define MIT_ASR_TOKEN "4a37aa0c0373498ea04f732054841b62"
#define MIT_ASR_URL "wss://nls-gateway-inner.aliyuncs.com/ws/v1"

#define MIT_ASR_TASK_QUIT_EVT (0x01)

static int wwv_enable = 0;
static aui_wwv_cb_t wwv_result_cb = NULL;
static NuiThingsConfig mit_dialog_config = {0};

char *asr_buf;
// long int test_asr_len = 0;

typedef enum mit_status {
    MIT_STATE_INIT = 0,
    MIT_STATE_ONGOING,
    MIT_STATE_FINISH,
    MIT_STATE_RESULT,
    MIT_STATE_CLOUD_RESULT,
    MIT_STATE_END,
} mit_status_t;

typedef enum mit_asr_err_type {
    MIT_ASR_SUCCESS = 0,
    MIT_ASR_NETWORK_ERR,
    MIT_ASR_PARSE_FAIL,
} mit_asr_err_type;

typedef struct mit_context {
    dev_ringbuf_t    rbuf;
    mit_status_t     status;
    mit_asr_err_type err;
    char *           asr_output;
} mit_context_t;

typedef struct mit_kws_data {
    volatile int     do_wwv;         // need to do wwv
    volatile int     data_valid;
    char    *data;
    size_t  len;
    size_t  pos;
} mit_kws_data_t;

static int              bufferLocked = 0;
static mit_kws_data_t   g_mit_kws;
static aos_event_t      asr_task_quit;

static int mit_asr_event_cb(void *user_data, NuiThingsEvent event, int dialog_finish)
{
    int   ret = 0;
    char  nui_things_info[1024];
    int   size;
    char *data;

    aui_t *        aui     = (aui_t *)user_data;
    mit_context_t *context = (mit_context_t *)aui->context;

    LOGD(TAG, "call %s(>>event: %d(%s)<<)", __FUNCTION__, event,
         nui_things_event_get_str(event));

    switch (event) {
        case kNuiThingsEventWwv:
            nui_things_info_print(kNuiThingsInfoTypeWwv);
            memset(nui_things_info, 0, sizeof(nui_things_info));

            ret = nui_things_info_get(kNuiThingsInfoTypeWwv, nui_things_info,
                                      sizeof(nui_things_info));
            if (0 == ret) {
                printf("get wwv info %s\n", nui_things_info);
            } else {
                printf("get wwv info error:%d\n", ret);
            }
            break;
        case kNuiThingsEventVadStart:
            break;
        case kNuiThingsEventVadEnd:
            // context->status = MIT_STATE_END;
            break;

        case kNuiThingsEventAsrPartialResult:
            /* do tts connect in advance to reduce dialog delay */
            aui_cloud_start_tts(aui);
            break;

        case kNuiThingsEventAsrResult:
            context->status = context->status == MIT_STATE_FINISH ? MIT_STATE_RESULT : MIT_STATE_CLOUD_RESULT;
            LOGD(TAG, "asr result %s", context->status == MIT_STATE_RESULT ? "local" : "cloud");

            size = nui_things_info_get_length(kNuiThingsInfoTypeAsr);
            if (size < 0) {
                LOGE(TAG, "nui things len1 error %d", size);
                context->err    = MIT_ASR_PARSE_FAIL;
                return ret;
            }

            data = (char *)aos_malloc(size + 1);
            aos_check_mem(data);

            data[size] = 0;
            ret        = nui_things_info_get(kNuiThingsInfoTypeAsr, data, size);
            if (0 == ret) {
                LOGD(TAG, "get asr info %s\n", data);
                if (aui->config.nlp_cb)
                    aui->config.nlp_cb(data);
            } else {
                LOGE(TAG, "get asr info error:%d\n", ret);
                context->err    = MIT_ASR_PARSE_FAIL;
            }
            // LOGD(TAG, "get asr result %s", data);

            free(data);
            break;

        case kNuiThingsEventDialogResult:
            if (context->status == MIT_STATE_RESULT || context->status == MIT_STATE_CLOUD_RESULT) {
                size = nui_things_info_get_length(kNuiThingsInfoTypeDialog);
                if (size < 0) {
                    LOGE(TAG, "nui things len2 error %d", size);
                    context->err = MIT_ASR_PARSE_FAIL;
                    goto END_CB;
                }

                data = (char *)aos_malloc(size + 1);
                aos_check_mem(data);

                data[size] = 0;
                ret        = nui_things_info_get(kNuiThingsInfoTypeDialog, data, size);
                if (0 == ret) {
                    LOGE(TAG, "get dialog info %s\n", data);
                } else {
                    LOGE(TAG, "get dialog info error:%d\n", ret);
                    context->err = MIT_ASR_PARSE_FAIL;
                    goto END_CB;
                }

                if (aui->config.nlp_cb)
                    aui->config.nlp_cb(data);

                free(data);
            } else {
                context->err = MIT_ASR_PARSE_FAIL;
            }

        END_CB:
            context->status = MIT_STATE_END;
            break;

        case kNuiThingsEventWwvConfirm:
            LOGD(TAG, "kws voice confirmed");
            if (wwv_result_cb) {
                wwv_result_cb(AUI_WWV_CONFIRM);
            }
            break;

        case kNuiThingsEventWwvReject:
            LOGD(TAG, "mit %s event error", "wwvreject");
            if (wwv_result_cb) {
                wwv_result_cb(AUI_WWV_REJECT);
            }
            break;

        case kNuiThingsEventAsrError:
            aui_cloud_stop_tts(aui);

            if (context->status == MIT_STATE_CLOUD_RESULT || context->status == MIT_STATE_ONGOING) {
                aui->config.nlp_cb("{\"aui_result\":-1,\"msg\":\"asr parse error\"}");
            }
            LOGD(TAG, "mit %s event error", "asr");
            context->err    = MIT_ASR_PARSE_FAIL; // TODO assign different error type
            context->status = MIT_STATE_END;
            break;

        default:
            break;
    }

    return ret;
}

static int mit_wwv_provide_data_cb(void *user_data, NuiThingsVoiceData *data)
{
    int ret;

    if (!g_mit_kws.do_wwv) {
        return -1;
    }

    if (!g_mit_kws.data_valid) {
        for (int i = 0; i < 30; i ++) {
            aos_msleep(20);
            if (g_mit_kws.data_valid) {
                break;
            }
        }

        if (!g_mit_kws.data_valid) {
            g_mit_kws.do_wwv = 0;
            LOGE(TAG, "get wwv data failed");
            return -1;
        }
    }

    if (data->mode == kNuiThingsVoiceModeKws && data->buffer && data->len > 0) {
        //   LOGD(TAG, "wwv data cb: %d total:%d, offset:%d, need_len:%d", g_mit_kws.valid, g_mit_kws.len, g_mit_kws.pos,  data->len);
        if (g_mit_kws.pos + data->len < g_mit_kws.len) {
            memcpy(data->buffer, g_mit_kws.data + g_mit_kws.pos, data->len);
            g_mit_kws.pos += data->len;
            data->finish = 0;
            ret          = data->len;
        } else {
            memcpy(data->buffer, g_mit_kws.data + g_mit_kws.pos, g_mit_kws.len - g_mit_kws.pos);
            data->finish = 1;
            ret          = g_mit_kws.len - g_mit_kws.pos;

            g_mit_kws.data_valid = 0;
            g_mit_kws.do_wwv    = 0;
            LOGD(TAG, "wwv data end.");
        }
    } else {
        g_mit_kws.data_valid    = 0;
        g_mit_kws.do_wwv        = 0;
        ret             = -1;
        LOGW(TAG, "wwv provide data cb err");
    }

    return ret;
}

static int mit_asr_provide_data_cb(void *user_data, char *buffer, int len)
{
    int            ret     = 0;
    aui_t *        aui     = (aui_t *)user_data;
    mit_context_t *context = (mit_context_t *)aui->context;
    //LOGD(TAG, "call %s(%p, %d) [start]",__FUNCTION__, buffer, len);

    if (context->status == MIT_STATE_ONGOING) {
        for (int i = 0; i < 2 && ringbuffer_available_read_space(&context->rbuf) < len; i++) {
            aos_msleep(40);
        }

        if (ringbuffer_available_read_space(&context->rbuf) < len) {
            return 0;
        }
    }

    ret = ringbuffer_read(&context->rbuf, (uint8_t *)buffer, len);
    // LOGD(TAG, "provide data cb read %d bytes", len);

    return ret;
}

static void tsk_wait_for_mit_consuming(void *arg)
{
    aui_t *        aui         = (aui_t *)arg;
    mit_context_t *context     = (mit_context_t *)aui->context;
    int            timeout_cnt = 0;

    // wait for data streaming finish, timeout in 5 s
    LOGD(TAG, "Wait consumig thread");
    while (context->status != MIT_STATE_END && context->err == MIT_ASR_SUCCESS &&
           ringbuffer_available_read_space(&context->rbuf) > 0) {
        aos_msleep(50);

        if (++timeout_cnt > 200)
            break;
    }

    if (context->err == MIT_ASR_SUCCESS) {
        // stop the mit asr to get the final result
        LOGD(TAG, "stop mit asr");
        nui_things_stop(0);

        timeout_cnt = 0;
        while (context->status != MIT_STATE_END) {
            aos_msleep(200);
            if (++timeout_cnt == 100) {
                // wait timeout, deem as error
                context->err = MIT_ASR_PARSE_FAIL;
                break;
            }
        }
    }

    context->status = MIT_STATE_END;
    if (context->err != MIT_ASR_SUCCESS) {
        // LOGE(TAG, "MIT asr error %d", context->err);
        nui_things_stop(1);

        LOGD(TAG, "result error %d", context->err);
        switch (context->err) {
            case MIT_ASR_NETWORK_ERR:
                aui->config.nlp_cb(
                    "{\"aui_result\":-101,\"msg\":\"mit cloud connect fail(ENETUNREACH)\"}");
                break;
            case MIT_ASR_PARSE_FAIL:
                aui->config.nlp_cb("{\"aui_result\":-1,\"msg\":\"asr parse error\"}");
                break;
            default:
                break;
        }
    }

    LOGD(TAG, "consumig thread end");
    aos_event_set(&asr_task_quit, MIT_ASR_TASK_QUIT_EVT, AOS_EVENT_OR);
    return;
}

static int nui_things_return_data_main(void *              user_data,
                                       NuiThingsVoiceData *data /*char * buffer, int len*/)
{
    int ret = 0;
    LOGD(TAG, "call %s(len=%d finish=%d) [start]", __FUNCTION__, data->len, data->finish);

    switch (data->mode) {
        case kNuiThingsVoiceModeKws:
            break;
        case kNuiThingsVoiceModeAsr:
            break;
        default:
            break;
    }
    LOGD(TAG, "call %s() [done]", __FUNCTION__);
    return ret;
}

mit_account_info_t g_mit_account_info;
static int mit_asr_set_account(aui_t *aui)
{
    cJSON *j_info = NULL;
    char *json_account_info = NULL;

    aos_check_param(aui);

    json_account_info = (char *)aui->config.js_account;

    j_info                  = cJSON_Parse(json_account_info);
    cJSON *device_uuid      = cJSON_GetObjectItem(j_info, "device_uuid");
    cJSON *asr_app_key      = cJSON_GetObjectItem(j_info, "asr_app_key");
    cJSON *asr_token        = cJSON_GetObjectItem(j_info, "asr_token");
    cJSON *asr_url          = cJSON_GetObjectItem(j_info, "asr_url");
    cJSON *tts_app_key      = cJSON_GetObjectItem(j_info, "tts_app_key");
    cJSON *tts_token        = cJSON_GetObjectItem(j_info, "tts_token");
    cJSON *tts_url          = cJSON_GetObjectItem(j_info, "tts_url");
    cJSON *tts_key_id       = cJSON_GetObjectItem(j_info, "tts_key_id");
    cJSON *tts_key_secret   = cJSON_GetObjectItem(j_info, "tts_key_secret");

    CHECK_RET_TAG_WITH_GOTO(
        j_info &&
        device_uuid && cJSON_IsString(device_uuid) &&
        asr_app_key && cJSON_IsString(asr_app_key) &&
        asr_token && cJSON_IsString(asr_token) &&
        asr_url && cJSON_IsString(asr_url) &&
        tts_app_key && cJSON_IsString(tts_app_key) &&
        tts_token && cJSON_IsString(tts_token) &&
        tts_url && cJSON_IsString(tts_url),
        ERR
    );

    g_mit_account_info.device_uuid  = device_uuid->valuestring;
    g_mit_account_info.asr_app_key  = asr_app_key->valuestring;
    g_mit_account_info.asr_token    = asr_token->valuestring;
    g_mit_account_info.asr_url      = asr_url->valuestring;
    g_mit_account_info.tts_app_key  = tts_app_key->valuestring;
    g_mit_account_info.tts_token    = tts_token->valuestring;
    g_mit_account_info.tts_url      = tts_url->valuestring;

    if ((tts_key_id && cJSON_IsString(tts_key_id)) && (tts_key_secret && cJSON_IsString(tts_key_secret))) {
        g_mit_account_info.tts_key_id      = tts_key_id->valuestring;
        g_mit_account_info.tts_key_secret  = tts_key_secret->valuestring;
    }

    mit_dialog_config.device_uuid       = g_mit_account_info.device_uuid;
    mit_dialog_config.app_key           = g_mit_account_info.asr_app_key;
    mit_dialog_config.token             = g_mit_account_info.asr_token;
    mit_dialog_config.url               = g_mit_account_info.asr_url;
	mit_dialog_config.enable_vad_cloud  = aui->config.cloud_vad;//1;
    return 0;

ERR:
    if (j_info) {
        cJSON_Delete(j_info);
    }
    
    return -1; 
}

/** init only once with multiple talks */
static int mit_asr_init(aui_t *aui)
{
    aos_check_return_einval(aui);

    mit_context_t *context = aos_malloc(sizeof(mit_context_t));
    aos_check_mem(context);

    asr_buf = aos_zalloc(MIT_ASR_BUFFER_SIZE);
    aos_check_mem(asr_buf);

    ringbuffer_create(&context->rbuf, asr_buf, MIT_ASR_BUFFER_SIZE);
    context->err    = MIT_ASR_SUCCESS;
    aui->context    = context;
    context->status = MIT_STATE_END;

    g_mit_kws.do_wwv    = 0;
    g_mit_kws.data_valid = 0;
    g_mit_kws.pos       = 0;

    /* mit sdk struct */
    NuiThingsListener   mit_listener;
    NuiThingsInitConfig mit_init_config; //init 配置

    memset(&mit_init_config, 0, sizeof(mit_init_config));
    mit_listener.on_event_callback      = mit_asr_event_cb;
    mit_listener.need_data_callback     = mit_asr_provide_data_cb;
    mit_listener.put_data_callback      = nui_things_return_data_main;
    mit_listener.need_data_callback_wwv = mit_wwv_provide_data_cb;
    mit_listener.need_data_callback_nls = NULL;
    mit_listener.user_data              = aui;
    mit_init_config.mode                = kNuiThingsModeNls;
    mit_init_config.listener            = &mit_listener;
    mit_init_config.enable_fe           = 0;
    mit_init_config.enable_kws          = 0;
    mit_init_config.enable_vad          = 1;
    mit_init_config.log_level           = 4;
	mit_init_config.log_link_enable   = 0;
	
    mit_asr_set_account(aui);

    // log_setLevel(1);//Warning

    int ret;
    ret = aos_event_new(&asr_task_quit, 0);
    CHECK_RET_TAG_WITH_RET(ret == 0, -1);

    return nui_things_init(&mit_init_config);
}

static int mit_set_session_id(aui_t *aui, const char *session_id)
{
    aos_check_return_einval(aui && session_id);

    if (!mit_dialog_config.session_id) {
        mit_dialog_config.session_id = (char *)aos_zalloc_check(32 + 1);
    }

    memcpy(mit_dialog_config.session_id, session_id, 32);
    return 0;
}

static int mit_enable_wwv(aui_t *aui, int enable)
{
    mit_dialog_config.enable_wwv = wwv_enable && enable;
    if (1 == mit_dialog_config.enable_wwv) {
        mit_dialog_config.kws_format = "pcm";
        mit_dialog_config.kws_model  = "gushiji-baola";
        mit_dialog_config.kws_word   = "宝拉宝拉";
    }

    g_mit_kws.do_wwv    = enable;
    g_mit_kws.data_valid = 0;

    return 0;
}

static int mit_start_pcm(aui_t *aui)
{
    aos_check_return_einval(aui);

    mit_context_t *context = (mit_context_t *)aui->context;

    aos_event_set(&asr_task_quit, 0, AOS_EVENT_AND);

    mit_status_t stat = context->status;
    if (stat != MIT_STATE_END) {
        unsigned int flag;
        
        context->status = MIT_STATE_END;
        nui_things_stop(1);

        if (stat == MIT_STATE_FINISH) {
            aos_event_get(&asr_task_quit, MIT_ASR_TASK_QUIT_EVT, AOS_EVENT_OR_CLEAR, &flag,
                          AOS_WAIT_FOREVER);
        }
    }

    // clear the ring buffer space
    ringbuffer_clear(&context->rbuf);

    LOGD(TAG, "session_id=%s", mit_dialog_config.session_id);

    if (0 != nui_things_start(&mit_dialog_config)) {
        LOGW(TAG, "nui_things_start return error");
        context->err    = MIT_ASR_NETWORK_ERR;
        context->status = MIT_STATE_END;
        nui_things_stop(1);
        return -1;
    }

    aui_cloud_stop_tts(aui);

    context->status     = MIT_STATE_ONGOING;
    context->err        = MIT_ASR_SUCCESS;
    bufferLocked        = 0;

    LOGD(TAG, "nui_things_start success");

    return 0;
}

static int mit_push_pcm(aui_t *aui, void *data, size_t size)
{
    aos_check_return_einval(aui);
    int            ret     = -1;
    mit_context_t *context = (mit_context_t *)aui->context;

    if (context->status != MIT_STATE_ONGOING) {
        return 0;
    }

    if (context->err != MIT_ASR_SUCCESS) {
        ret = -1;
        goto END_SOURCE_PCM;
    }

    if (bufferLocked || ringbuffer_full(&context->rbuf)) {
        LOGD(TAG, "buffer locked");
        bufferLocked = 1;
        return 0;
    }

    ringbuffer_write(&context->rbuf, (uint8_t *)data, size);
    // LOGD(TAG, "mit asr buf left %d", mit_rbuf_available_space(&context->rbuf));

    return 0;

END_SOURCE_PCM:
    if (ret < 0)
        LOGE(TAG, "Source PCM Error\n");
    return ret;
}

static int mit_stop_pcm(aui_t *aui)
{
    aos_check_return_einval(aui);
    mit_context_t *context = (mit_context_t *)aui->context;

    if (context->status == MIT_STATE_RESULT) {
        return 0;
    } else if (context->status != MIT_STATE_ONGOING) {
        return -1;
    }

    context->status = MIT_STATE_FINISH;

    // create a task to wait for streaming the rest of the buffer
    aos_task_t task_handle;
    if (0 != aos_task_new_ext(&task_handle, "wait_mit", tsk_wait_for_mit_consuming, aui, 2 * 1024,
                              AOS_DEFAULT_APP_PRI)) {
        LOGE(TAG, "Create tsk_wait_for_mit_consuming failed.");
        return -1;
    }

    LOGD(TAG, "MIT source pcm finish");

    return 0;
}

static int mit_force_stop(aui_t *aui)
{
    aos_check_return_einval(aui);
    mit_context_t *context = (mit_context_t *)aui->context;
    mit_status_t stat = context->status;
    unsigned int flags;

    g_mit_kws.do_wwv = 0;
    g_mit_kws.data_valid = 0;
    context->status = MIT_STATE_END;
    nui_things_stop(1);

    if (stat == MIT_STATE_FINISH) {
        aos_event_get(&asr_task_quit, MIT_ASR_TASK_QUIT_EVT, AOS_EVENT_OR_CLEAR, &flags, AOS_WAIT_FOREVER);
    }

    return 0;
}

static int mit_init_wwv(aui_t *aui, aui_wwv_cb_t cb)
{
    aos_check_return_einval(cb);

    wwv_result_cb = cb;
    wwv_enable = 1;
    return 0;
}

static int mit_push_wwv_data(aui_t *aui, void *data, size_t size)
{
    aos_check_return_einval(data);
    g_mit_kws.data_valid    = 1;
    g_mit_kws.data          = data;
    g_mit_kws.len           = size;
    g_mit_kws.pos           = 0;    

    return 0;
}

#if 1
// FIXME: just for compile
// 因为mit库里面需要用到这个函数
#include <mbedtls/ssl.h>
void mbedtls_ssl_init_ext( mbedtls_ssl_context *ssl, int len)
{
    mbedtls_ssl_init(ssl);
}
#endif

static aui_asr_cls_t mit_asr_cls = {
    .init = mit_asr_init,
    .set_session_id = mit_set_session_id,
    .start = mit_start_pcm,
    .enable_wwv = mit_enable_wwv,
    .push_data = mit_push_pcm,
    .stop_push_data = mit_stop_pcm,
    .stop = mit_force_stop,
    .init_wwv = mit_init_wwv,
    .push_wwv_data = mit_push_wwv_data,
};

void aui_asr_register_mit(aui_t *aui)
{
    if (aui) {
        aui->ops.asr = &mit_asr_cls;
    }
}
