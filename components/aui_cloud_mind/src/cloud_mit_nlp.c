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
#include <aos/ringbuffer.h>
#include <aos/debug.h>
#include <devices/wifi.h>
#include <aos/kv.h>

#include <cJSON.h>

#include "nui_things.h"
#include "cloud_mit.h"
#include "nls_nui_things_tts.h"

#define TAG "MinDCloud"

#define MIT_ASR_BUFFER_SIZE 200 * 1024 //50K

#define MIT_ASR_TASK_QUIT_EVT (0x01)

mit_account_info_t g_mit_account_info;

static NuiThingsConfig mit_dialog_config = {0};
static char *          asr_buf           = NULL;
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
    volatile int do_wwv; // need to do wwv
    volatile int data_valid;
    char *       data;
    size_t       len;
    size_t       pos;
} mit_kws_data_t;

static int            g_buffer_locked = 0;
static mit_kws_data_t g_mit_kws;
static aos_event_t    g_asr_task_quit = NULL;

static void mit_nlp_handle(void *data, int len, void *priv)
{
    aui_audio_req_ack_info_t req_ack_info;
    aui_t *aui = (aui_t *)priv;

    LOGD(TAG, "%s normal asr\n", __func__);
    req_ack_info.data_type = g_mit_kws.do_wwv ? AUI_AUDIO_REQ_TYPE_WWV : AUI_AUDIO_REQ_TYPE_NLP;
    req_ack_info.data     = data;
    req_ack_info.len      = len;

    if (aui->cb.nlp_cb) {
        aui->cb.nlp_cb((void *)&req_ack_info, sizeof(aui_audio_req_ack_info_t), aui->cb.nlp_priv);
    }
    return;

#if 0
    char *json_text = (char *)data;
    cJSON *js = cJSON_Parse(json_text);
    cJSON *state = cJSON_GetObjectItem(js, "aui_kws_result");

    int32_t event = atoi(state->valuestring);
    switch (event) {
        case AUI_KWS_REJECT:
            LOGD(TAG, "wwv rejected");
            req_ack_info.data_type = AUI_AUDIO_REQ_TYPE_WWV;
            req_ack_info.data     = (char *)&event;
            req_ack_info.len      = sizeof(int32_t);
            break;

        case AUI_KWS_CONFIRM:
            LOGD(TAG, "wwv confirmed");
            req_ack_info.data_type = AUI_AUDIO_REQ_TYPE_WWV;
            req_ack_info.data     = (char *)&event;
            req_ack_info.len      = sizeof(int32_t);
            break;

        default:
            LOGD(TAG, "%s asr\n", __func__);
            req_ack_info.data_type = AUI_AUDIO_REQ_TYPE_NLP;
            req_ack_info.data     = data;
            req_ack_info.len      = len;
            break;
    }

    if (aui->cb.nlp_cb) {
        aui->cb.nlp_cb((void *)&req_ack_info, sizeof(aui_audio_req_ack_info_t), aui->cb.nlp_priv);
    }
#endif

}

static void get_session_id(char *buff, const int number)
{
    char rand_string[] = "0123456789abcdeffdecba9876543210";
    char ss[3]         = {0};

    /* use random number as last 12 bytes */
    struct timeval time_now; //= {0};
    gettimeofday(&time_now, NULL);
    long time_mil = 0;
    time_mil      = time_now.tv_sec * 1000 + time_now.tv_usec / 1000;
    srand(((unsigned int)time_mil));

    for (int i = 1; i <= number; i++) {
        memset(ss, 0x0, 3);
        sprintf(ss, "%c", rand_string[(int)(32.0 * rand() / (RAND_MAX + 1.0))]);
        strcat(buff, ss);
    }
}

static void get_hex_mac(char *hex_mac)
{
    static uint8_t s_mac[6] = {0};
    int ret, try = 0;

    if (s_mac[0] == 0 && s_mac[1] == 0 && s_mac[2] == 0 && 
        s_mac[3] == 0 && s_mac[4] == 0 && s_mac[5] == 0) {
        rvm_dev_t *wifi_dev = NULL;
        wifi_dev = rvm_hal_device_open("wifi0");
        if(NULL == wifi_dev) {
            LOGE(TAG, "open wifi device error!");
            return;
        }

        do {
            ret = rvm_hal_wifi_get_mac_addr(wifi_dev, s_mac);
            if (ret == 0) {
                break;
            }

            aos_msleep(500);
        } while (++try < 5);
    }

    if (try == 5) {
        LOGW(TAG, "get mac failed, use default mac");

        s_mac[0] = 0x00;
        s_mac[1] = 0xE0;
        s_mac[2] = 0x90;
        s_mac[3] = 0x60;
        s_mac[4] = 0x30;
        s_mac[5] = 0x12;
    }

    for (int i = 0; i < 6; i++) {
        sprintf(hex_mac + i * 2, "%02x", s_mac[i]);
    }
}

static void get_uuid(char *uuid)
{
    char product_key[32 + 1] = {0};
    char device_name[32 + 1] = {0};
    int pk_len = sizeof(product_key), dn_len = sizeof(device_name);

    int ret1 = aos_kv_get("hal_devinfo_pk", product_key, &pk_len);
    int ret2 = aos_kv_get("hal_devinfo_dn", device_name, &dn_len);
    if (ret1 == 0 && ret2 == 0) {
        sprintf(uuid, "%s&%s", product_key, device_name);
    } else {
        get_hex_mac(uuid);
    }

    LOGD(TAG, "device uuid %s", uuid);
}

static char *mit_asr_get_account(void)
{
    return NULL;
}

static int mit_asr_event_cb(void *user_data, NuiThingsEvent event, int dialog_finish)
{
    int   ret = 0;
    char  nui_things_info[1024];
    int   size;
    char *data;

    aui_t *        aui     = (aui_t *)user_data;
    mit_context_t *context = (mit_context_t *)aui->ops.nlp->priv;

    LOGD(TAG, "call %s(>>event: %d(%s)<<)", __FUNCTION__, event, nui_things_event_get_str(event));

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
            context->status =
                context->status == MIT_STATE_FINISH ? MIT_STATE_RESULT : MIT_STATE_CLOUD_RESULT;
            LOGD(TAG, "asr result %s", context->status == MIT_STATE_RESULT ? "local" : "cloud");

            size = nui_things_info_get_length(kNuiThingsInfoTypeAsr);
            if (size < 0) {
                LOGE(TAG, "nui things len1 error %d", size);
                context->err = MIT_ASR_PARSE_FAIL;
                return ret;
            }

            data = (char *)aos_malloc(size + 1);
            aos_check_mem(data);

            data[size] = 0;
            ret        = nui_things_info_get(kNuiThingsInfoTypeAsr, data, size);
            if (0 == ret) {
                LOGD(TAG, "get asr info %s\n", data);
                mit_nlp_handle((void *)data, strlen(data), aui->cb.nlp_priv);
            }

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
                    ;//LOGD(TAG, "get dialog info %s\n", data);
                } else {
                    LOGE(TAG, "get dialog info error:%d\n", ret);
                    context->err = MIT_ASR_PARSE_FAIL;
                    goto END_CB;
                }

                mit_nlp_handle((void *)data, strlen(data), aui->cb.nlp_priv);

                free(data);
            } else {
                context->err = MIT_ASR_PARSE_FAIL;
            }

        END_CB:
            context->status = MIT_STATE_END;
            break;

        case kNuiThingsEventWwvConfirm:
            {
                LOGD(TAG, "kws voice confirmed");
                char text[60] = {0};
                snprintf(text, 60, "{\"aui_kws_result\":%d}", 1);
                mit_nlp_handle((void *)text, strlen(text), aui->cb.nlp_priv);
            }
            break;

        case kNuiThingsEventWwvReject:
            {
                LOGD(TAG, "mit %s event error", "wwvreject");
                char text[60] = {0};
                snprintf(text, 60, "{\"aui_kws_result\":%d}", 0);
                mit_nlp_handle((void *)text, strlen(text), aui->cb.nlp_priv);
            }
            break;

        case kNuiThingsEventAsrError:
            aui_cloud_stop_tts(aui);

            if (context->status == MIT_STATE_CLOUD_RESULT || context->status == MIT_STATE_ONGOING) {
                const char *text = "{\"aui_result\":-1,\"msg\":\"asr parse error\"}";
                mit_nlp_handle((void *)text, strlen(text), aui->cb.nlp_priv);
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
        for (int i = 0; i < 30; i++) {
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
            g_mit_kws.do_wwv     = 0;
            LOGD(TAG, "wwv data end.");
        }
    } else {
        g_mit_kws.data_valid = 0;
        g_mit_kws.do_wwv     = 0;
        ret                  = -1;
        LOGW(TAG, "wwv provide data cb err");
    }

    return ret;
}

static int mit_asr_provide_data_cb(void *user_data, char *buffer, int len)
{
    int            ret     = 0;
    aui_t *        aui     = (aui_t *)user_data;
    mit_context_t *context = (mit_context_t *)aui->ops.nlp->priv;
    // LOGD(TAG, "call %s(%p, %d) [start]",__FUNCTION__, buffer, len);

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
    mit_context_t *context     = (mit_context_t *)aui->ops.nlp->priv;
    int            timeout_cnt = 0;

    // wait for data streaming finish, timeout in 5 s
    LOGD(TAG, "Wait consumig thread");
    while (context->status != MIT_STATE_END && context->err == MIT_ASR_SUCCESS &&
           ringbuffer_available_read_space(&context->rbuf) > 0) {
        aos_msleep(50);

        if (++timeout_cnt > 200)
            break;
    }

    if (context->status != MIT_STATE_END && context->err == MIT_ASR_SUCCESS) {
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
                if (aui->cb.nlp_cb) {
                    const char *data = "{\"aui_result\":-101,\"msg\":\"mit cloud connect fail(ENETUNREACH)\"}";
                    // aui->cb.nlp_cb((void *)data, strlen(data), aui->cb.nlp_priv);
                    mit_nlp_handle((void *)data, strlen(data), aui->cb.nlp_priv);
                }

                break;
            case MIT_ASR_PARSE_FAIL:
                if (aui->cb.nlp_cb) {
                    const char *data = "{\"aui_result\":-1,\"msg\":\"asr parse error\"}";
                    // aui->cb.nlp_cb((void *)data, strlen(data), aui->cb.nlp_priv);
                    mit_nlp_handle((void *)data, strlen(data), aui->cb.nlp_priv);
                }
                break;
            default:
                break;
        }
    }

    LOGD(TAG, "consumig thread end");
    aos_event_set(&g_asr_task_quit, MIT_ASR_TASK_QUIT_EVT, AOS_EVENT_OR);
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

static int mit_asr_set_account(aui_t *aui)
{
    return -1;
}

/** init only once with multiple talks */
static int mit_nlp_init(aui_t *aui)
{
    aos_check_return_einval(aui);

    if (g_asr_task_quit) {
        return 0;
    }

    mit_context_t *context = aos_malloc(sizeof(mit_context_t));
    aos_check_mem(context);

    asr_buf = aos_zalloc(MIT_ASR_BUFFER_SIZE);
    aos_check_mem(asr_buf);

    ringbuffer_create(&context->rbuf, asr_buf, MIT_ASR_BUFFER_SIZE);
    context->err         = MIT_ASR_SUCCESS;
    aui->ops.nlp->priv   = context;
    context->status      = MIT_STATE_END;

    g_mit_kws.do_wwv     = 0;
    g_mit_kws.data_valid = 0;
    g_mit_kws.pos        = 0;

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
    mit_init_config.log_link_enable     = 0;

    mit_asr_set_account(aui);

    if (!mit_dialog_config.session_id) {
        mit_dialog_config.session_id = (char *)aos_malloc_check(32 + 1);
    }

    /* get_session_id need clean buff to generate id */
    memset(mit_dialog_config.session_id, 0, 33);

    get_session_id(mit_dialog_config.session_id, 32);
    // log_setLevel(1);//Warning

extern int g_silence_log_level;
    g_silence_log_level=5;

    int ret;
    ret = aos_event_new(&g_asr_task_quit, 0);
    CHECK_RET_TAG_WITH_RET(ret == 0, -1);

    return nui_things_init(&mit_init_config);
}

static int mit_start_pcm(aui_t *aui)
{
    aos_check_return_einval(aui);

    if (aui->audio_req_type == AUI_AUDIO_REQ_TYPE_WWV) {
        LOGI(TAG, "enable wwv in cloud!");
        mit_dialog_config.enable_wwv = 1;
        mit_dialog_config.kws_format = "pcm";
        mit_dialog_config.kws_model  = "gushiji-baola";
        mit_dialog_config.kws_word   = "宝拉宝拉";
        g_mit_kws.do_wwv             = 1;
    } else {
        mit_context_t *context = (mit_context_t *)aui->ops.nlp->priv;

        mit_dialog_config.enable_wwv = 0;
        g_mit_kws.do_wwv             = 0;
        g_mit_kws.data_valid         = 0;
        aos_event_set(&g_asr_task_quit, 0, AOS_EVENT_AND);

        mit_status_t stat = context->status;
        if (stat != MIT_STATE_END) {
            unsigned int flag;

            context->status = MIT_STATE_END;
            nui_things_stop(1);

            if (stat == MIT_STATE_FINISH) {
                aos_event_get(&g_asr_task_quit, MIT_ASR_TASK_QUIT_EVT, AOS_EVENT_OR_CLEAR, &flag,
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

        context->status = MIT_STATE_ONGOING;
        context->err    = MIT_ASR_SUCCESS;
        g_buffer_locked    = 0;

        LOGD(TAG, "nui_things_start success");        
    }

    return 0;
}

static int mit_push_pcm(aui_t *aui, void *data, size_t size)
{
    aos_check_return_einval(aui && data && size);

    if (aui->audio_req_type == AUI_AUDIO_REQ_TYPE_WWV) {
        g_mit_kws.data_valid = 1;
        g_mit_kws.data       = data;
        g_mit_kws.len        = size;
        g_mit_kws.pos        = 0;
        aui->audio_req_type  = AUI_AUDIO_REQ_TYPE_NLP;
    } else if (aui->audio_req_type == AUI_AUDIO_REQ_TYPE_NLP) {
        mit_context_t *context = (mit_context_t *)aui->ops.nlp->priv;

        if (context->status != MIT_STATE_ONGOING) {
            return 0;
        }

        if (context->err != MIT_ASR_SUCCESS) {
            LOGE(TAG, "Source PCM Error");
            return -1;
        }

        if (g_buffer_locked || ringbuffer_full(&context->rbuf)) {
            if (!g_buffer_locked) {
                LOGD(TAG, "buffer locked");
            }
            g_buffer_locked = 1;
            return 0;
        }

        ringbuffer_write(&context->rbuf, (uint8_t *)data, size);
        // LOGD(TAG, "mit asr buf left %d", mit_rbuf_available_space(&context->rbuf));
        return 0;       
    } else {
        LOGE(TAG, "unsupport audio req type");
    }

    return -1;
}

static int mit_stop_pcm(aui_t *aui)
{
    aos_check_return_einval(aui);

    if (aui->audio_req_type == AUI_AUDIO_REQ_TYPE_WWV) {
        return 0;
    } else if (aui->audio_req_type == AUI_AUDIO_REQ_TYPE_NLP) {
        mit_context_t *context = (mit_context_t *)aui->ops.nlp->priv;

        if (context->status == MIT_STATE_RESULT) {
            return 0;
        } else if (context->status != MIT_STATE_ONGOING) {
            return -1;
        }

        context->status = MIT_STATE_FINISH;

        // create a task to wait for streaming the rest of the buffer
        aos_task_t task_handle;
        if (0 != aos_task_new_ext(&task_handle, "wait_mit", tsk_wait_for_mit_consuming, aui, 8 * 1024,
                                AOS_DEFAULT_APP_PRI)) {
            LOGE(TAG, "Create tsk_wait_for_mit_consuming failed.");
            return -1;
        }

        LOGD(TAG, "MIT source pcm finish");        
    } else {
         LOGE(TAG, "unsupport audio req type");
         return -1;
    }

    return 0;
}

static int mit_force_stop(aui_t *aui)
{
    aos_check_return_einval(aui);
    if (aui->audio_req_type == AUI_AUDIO_REQ_TYPE_WWV) {
        return 0;
    } else if (aui->audio_req_type == AUI_AUDIO_REQ_TYPE_NLP) {
        mit_context_t *context = (mit_context_t *)aui->ops.nlp->priv;
        mit_status_t   stat    = context->status;
        unsigned int   flags;

        g_mit_kws.do_wwv     = 0;
        g_mit_kws.data_valid = 0;
        context->status      = MIT_STATE_END;
        nui_things_stop(1);

        if (stat == MIT_STATE_FINISH) {
            aos_event_get(&g_asr_task_quit, MIT_ASR_TASK_QUIT_EVT, AOS_EVENT_OR_CLEAR, &flags,
                        AOS_WAIT_FOREVER);
        }
    } else {
         LOGE(TAG, "unsupport audio req type");
         return -1;
    }

    return 0;
}

#if 1
// FIXME: just for compile
// 因为mit库里面需要用到这个函数
#include <mbedtls/ssl.h>
void mbedtls_ssl_init_ext(mbedtls_ssl_context *ssl, int len)
{
    mbedtls_ssl_init(ssl);
}
#endif

static aui_nlp_cls_t mit_nlp_cls = {
    .init           = mit_nlp_init,
    .start          = mit_start_pcm,
    .push_data      = mit_push_pcm,
    .stop_push_data = mit_stop_pcm,
    .push_text      = NULL,
    .stop           = mit_force_stop
};

void aui_nlp_register(aui_t *aui, aui_nlp_cb_t cb, void *priv)
{
    aos_check_param(aui);
    aui_cloud_nlp_register(aui, &mit_nlp_cls, cb, priv);
}
