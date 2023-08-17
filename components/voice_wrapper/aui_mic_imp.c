/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */

#include <yoc/mic.h>
#include <yoc/mic_port.h>
#include <aos/aos.h>
#include <aos/ringbuffer.h>
#include <stdarg.h>
#include <ulog/ulog.h>

#define TAG "VoiceWrapper"

#define FRAME_SIZE      ((16000 / 1000) * (16 / 8) * 20)           /* 640 */
#define RINGBUFFER_SIZE (FRAME_SIZE * CONFIG_MIC_RINGBUF_RAME + 1) /* 20ms * 20 */

typedef enum
{
    MIC_AEC_DATA = 0,
    MIC_KWS_DATA,
    MIC_KWS_MAX
} mic_audio_type_t;

typedef struct {
    dev_ringbuf_t ring_buffer;
    char         *recv_buf;
} audio_t;

struct __mic {
    uservice_t   *srv;
    utask_t      *task;
    int           source;
    void         *param;
    aui_mic_evt_t cb;
    int           evt_cnt;
    uint8_t       rbuf[FRAME_SIZE];
    audio_t       audio[MIC_KWS_MAX];
    mic_ops_t    *ops;
    int           mute_state;
    int           start_state;
};

typedef enum
{
    _START_CMD,
    _STOP_CMD,
    _EVENT_CMD,
    _DEINIT_CMD,

    MIC_END_CMD
} MIC_CMD;

typedef struct {
    int evt_id;
    union
    {
        void *ptr;
        int   ival;
    };
    int size;
} evt_param_t;

static mic_t g_mic;

static int audio_new(int audio_type)
{
    if (audio_type >= MIC_KWS_MAX || audio_type < 0) {
        return -1;
    }

    g_mic.audio[audio_type].recv_buf = aos_malloc_check(RINGBUFFER_SIZE);

    ringbuffer_create(&g_mic.audio[audio_type].ring_buffer, g_mic.audio[audio_type].recv_buf, RINGBUFFER_SIZE);

    return 0;
}

static int audio_free(int audio_type)
{
    if (audio_type >= MIC_KWS_MAX) {
        return -1;
    }

    if (g_mic.audio[audio_type].recv_buf) {
        aos_free(g_mic.audio[audio_type].recv_buf);
        g_mic.audio[audio_type].recv_buf = NULL;
    }

    return 0;
}

static int audio_write(int audio_type, void *data, int size)
{
    if (audio_type >= MIC_KWS_MAX || audio_type < 0) {
        return -1;
    }

    if (g_mic.audio[audio_type].recv_buf == NULL) {
        audio_new(audio_type);
    }

    return ringbuffer_write(&g_mic.audio[audio_type].ring_buffer, data, size);
}

static int audio_read(int audio_type, void *data, int size)
{
    if (audio_type >= MIC_KWS_MAX) {
        return -1;
    }

    return ringbuffer_read(&g_mic.audio[audio_type].ring_buffer, data, size);
}

static int event_id_to_audio_id(int event_id)
{
    if (event_id == MIC_EVENT_KWS_DATA) {
        return MIC_KWS_DATA;
    } else if (event_id == MIC_EVENT_PCM_DATA) {
        return MIC_AEC_DATA;
    }

    return -1;
}

void aui_mic_send_wakeup_check(int checked)
{
    evt_param_t param;
    param.ival = checked;
    param.evt_id = MIC_EVENT_SESSION_WWV;

    uservice_call_async(g_mic.srv, _EVENT_CMD, &param, sizeof(param));
}

/* 适配层的语音事件回调处理，中转到MIC微服务 */
static void mic_event_hdl(mic_t *mic, mic_event_id_t event_id, void *data, int size)
{
    evt_param_t param;
    int         len;
    int         audio_recv_flag = 0;

    aos_check_return(mic);

    memset(&param, 0, sizeof(param));
    param.evt_id = event_id;

    if (event_id == MIC_EVENT_SESSION_START) {
        if (mic->mute_state) {
            return;
        }

        if (data != NULL) {
            mic_kws_t *k = aos_zalloc_check(sizeof(mic_kws_t));
            memcpy(k, data, sizeof(mic_kws_t));
            param.ptr = k;
        }
    } else if (event_id == MIC_EVENT_SESSION_STOP) {
        // stop
    } else if (event_id == MIC_EVENT_PCM_DATA) {

        int audio_type = event_id_to_audio_id(event_id);
        if (audio_type >= 0) {
            audio_recv_flag = 1;

            len = audio_write(audio_type, data, size);
            if (len < size) {
                LOGE(TAG, "audio(%d) is full, len(%d), size(%d)", audio_type, len, size);
            }

            /* 减少uservice数据事件通知次数 */
            if (mic->evt_cnt < 2) {
                mic->evt_cnt++;
            }
            // else {
            //     LOGD(TAG, "mic->evt_cnt %d >= 2, audio_recv_flag %d", mic->evt_cnt, audio_recv_flag);
            // }
            
        } else {
            LOGE(TAG, "event_id error");
        }
    } else if (event_id == MIC_EVENT_KWS_DATA) {
        /* MIC_EVENT_KWS_DATA  一次回调所有数据，数据保存部署模块中 */
        param.ptr =  data;
        param.size = size;
    } else if (event_id == MIC_EVENT_SESSION_DOA) {
        param.ptr =  data;
        param.size = size;
    } else if (event_id == MIC_EVENT_LOCAL_ASR) {
        param.ptr =  strdup(data);
        param.size = size;
    } else if (event_id == MIC_EVENT_VAD_BEGIN) {
        // stop
    } else if (event_id == MIC_EVENT_VAD_END) {
        // stop
    } else {
        ;
    }

    if (mic->evt_cnt < 2 || audio_recv_flag == 0) {
        uservice_call_async(mic->srv, _EVENT_CMD, &param, sizeof(param));
    }
}

/* MIC微服务中的事件处理函数，回调到应用 */
static int _event_hdl(mic_t *mic, rpc_t *rpc)
{
    evt_param_t *param = (evt_param_t *)rpc_get_buffer(rpc, NULL);

    switch (param->evt_id) {
        case MIC_EVENT_PCM_DATA:
            while (1) {
                int audio_type = event_id_to_audio_id(param->evt_id);

                int size = audio_read(audio_type, mic->rbuf, FRAME_SIZE);
                if (size > 0) {
                    if (mic->cb) {
                        mic->cb(mic->source, param->evt_id, mic->rbuf, size);
                    }
                } else {
                    break;
                }
            }
            break;
        case MIC_EVENT_KWS_DATA:
            if (mic->cb) {
                mic->cb(mic->source, param->evt_id, param->ptr, param->size);
            }
            break;
        case MIC_EVENT_SESSION_WWV:
            if (mic->cb) {
                mic->cb(mic->source, MIC_EVENT_SESSION_WWV, param->ptr, 0);
            } 
            break;
        case MIC_EVENT_SESSION_START:
            if (mic->cb) {
                mic->cb(mic->source, MIC_EVENT_SESSION_START, param->ptr, sizeof(mic_kws_t));
            }

            if (param->ptr) {
                aos_free(param->ptr);
            }
            break;

        case MIC_EVENT_SESSION_STOP:
            if (mic->cb) {
                mic->cb(mic->source, MIC_EVENT_SESSION_STOP, NULL, 0);
            }
            break;
        case MIC_EVENT_SESSION_DOA:
            if (mic->cb) {
                mic->cb(mic->source, MIC_EVENT_SESSION_DOA, param->ptr, 0);
            }
            break;
        case MIC_EVENT_LOCAL_ASR:
            if (mic->cb) {
                mic->cb(mic->source, MIC_EVENT_LOCAL_ASR, param->ptr, 0);
            }

            if (param->ptr) {
                aos_free(param->ptr);
            }
            break;
        case MIC_EVENT_VAD_BEGIN:
            if (mic->cb) {
                mic->cb(mic->source, MIC_EVENT_VAD_BEGIN, NULL, 0);
            }
            break;
        case MIC_EVENT_VAD_END:
            if (mic->cb) {
                mic->cb(mic->source, MIC_EVENT_VAD_END, NULL, 0);
            }
            break;
        default:
            break;
    }

    mic->evt_cnt = 0;
    return 0;
}

static int _start(mic_t *mic, rpc_t *rpc)
{
    if (g_mic.ops->start) {
        g_mic.ops->start(&g_mic);
        g_mic.start_state = 1;
    }

    return 0;
}

static int _stop(mic_t *mic, rpc_t *rpc)
{
    if (g_mic.ops->stop) {
        g_mic.ops->stop(&g_mic);
        g_mic.start_state = 0;
    }

    return 0;
}

static int _deinit_hdl(mic_t *mic, rpc_t *rpc)
{
    if (g_mic.ops->deinit) {
        g_mic.ops->deinit(mic);
    }

    for (int i = MIC_AEC_DATA; i < MIC_KWS_MAX; i++) {
        audio_free(i);
    }

    return 0;
}

static const rpc_process_t c_mic_cmd_cb_table[] = {
    { _START_CMD, (process_t)_start },       { _STOP_CMD, (process_t)_stop },  { _EVENT_CMD, (process_t)_event_hdl },
    { _DEINIT_CMD, (process_t)_deinit_hdl }, { MIC_END_CMD, (process_t)NULL },
};

int mic_process_rpc(void *context, rpc_t *rpc)
{
    return uservice_process(context, rpc, c_mic_cmd_cb_table);
}

int aui_mic_control(mic_ctrl_cmd_t cmd, ...)
{
    if (g_mic.start_state == 0 && cmd != MIC_CTRL_VOICE_MUTE) {
        LOGW(TAG, "service is starting up, wait");
        return -1;
    }

    int ret = 0;
    va_list ap;
    mic_t  *mic = &g_mic;

    va_start(ap, cmd);

    if (cmd == MIC_CTRL_START_PCM) {
        if (mic->ops->pcm_data_control) {
            mic->ops->pcm_data_control(mic, 1);
        }
    } else if (cmd == MIC_CTRL_STOP_PCM) {
        if (mic->ops->pcm_data_control) {
            mic->ops->pcm_data_control(mic, 0);
        }
    } else if (cmd == MIC_CTRL_START_SESSION) {
        int enable = va_arg(ap, int);
        if (mic->ops->set_push2talk) {
            mic->ops->set_push2talk(mic, enable);
        }
    } else if (cmd == MIC_CTRL_VOICE_MUTE) {
        mic->mute_state = va_arg(ap, int);
    } else if (cmd == MIC_CTRL_NOTIFY_PLAYER_STATUS) {
        int status = va_arg(ap, int);
        int delay  = va_arg(ap, int);
        if (mic->ops->notify_play_status) {
            mic->ops->notify_play_status(mic, status, delay);
        }
    } else if (cmd == MIC_CTRL_WAKEUP_LEVEL) {
        char *word = va_arg(ap, char*);
        int level = va_arg(ap, int);
        if (mic->ops->set_wakeup_level) {
            mic->ops->set_wakeup_level(mic, word, level);
        }
    } else if (cmd == MIC_CTRL_START_DOA) {
        if (mic->ops->start_doa) {
            mic->ops->start_doa(mic);
        }
    } else if (cmd == MIC_CTRL_ENABLE_LINEAR_AEC_DATA) {
        int enable = va_arg(ap, int);
        if (mic->ops->enable_linear_aec_data) {
            mic->ops->enable_linear_aec_data(mic, enable);
        }
    } else if (cmd == MIC_CTRL_ENABLE_ASR) {
        int enable = va_arg(ap, int);
        if (mic->ops->enable_asr) {
            mic->ops->enable_asr(mic, enable);
        }
    }

    va_end(ap);

    return ret;
}

int aui_mic_init(utask_t *task, aui_mic_evt_t evt_cb)
{
    aos_check_return_einval(task);

    /* The voice service runs throughout its lifecycle and does not need to be released. */
    if (g_mic.srv != NULL || g_mic.ops == NULL) {
        return -1;
    }

    if (g_mic.ops->init && g_mic.ops->init(&g_mic, mic_event_hdl) < 0) {
        LOGE(TAG, "mic srv ops init failed");
        return -1;
    }

    if (audio_new(MIC_AEC_DATA) < 0) {
        if (g_mic.ops->deinit) {
            g_mic.ops->deinit(&g_mic);
        }
        return -1;
    }

    g_mic.task = task;

    g_mic.srv = uservice_new("mic", mic_process_rpc, &g_mic);
    aos_check_return_enomem(g_mic.srv);

    utask_add(task, g_mic.srv);

    g_mic.cb = evt_cb;

    return 0;
}

int aui_mic_deinit(void)
{
    /* The voice service runs throughout its lifecycle and does not need to be released. */
#if 0
    aos_check_return_einval(g_mic.srv && g_mic.ops);

    uservice_call_sync(g_mic.srv, _DEINIT_CMD, NULL, NULL, 0);

    mic_t *mic = &g_mic;
    utask_remove(mic->task, mic->srv);
    uservice_destroy(mic->srv);
    memset(mic, 0x00, sizeof(mic_t));
#endif
    return 0;
}

int aui_mic_start(void)
{
    aos_check_return_einval(g_mic.srv && g_mic.ops);

    int ret = uservice_call_async(g_mic.srv, _START_CMD, NULL, 0);

    return ret;
}

int aui_mic_stop(void)
{
    aos_check_return_einval(g_mic.srv && g_mic.ops);

    int ret = uservice_call_async(g_mic.srv, _STOP_CMD, NULL, 0);

    return ret;
}

int mic_ops_register(mic_ops_t *ops)
{
    g_mic.ops = ops;

    return 0;
}
