/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <yoc/mic.h>
#include <aos/aos.h>
#include <aos/ringbuffer.h>

#define TAG "mic"

#define FRAME_SIZE ((16000 / 1000) * (16 / 8) * 20) /* 640 */
#define RINGBUFFER_SIZE (FRAME_SIZE * 20 + 1) /* 20ms * 20 */

struct __mic {
    uservice_t      *srv;
    utask_t         *task;
    mic_state_t      state;
    int              source;
    mic_hw_param_t   hw;
    mic_sw_param_t   sw;
    aui_mic_evt_t    cb;
    int              evt_cnt;
    uint8_t          rbuf[FRAME_SIZE];
    char            *recv_buf;
    dev_ringbuf_t     ring_buffer;

    mic_ops_t       *ops;
    void            *priv;
};

typedef enum {
    _SET_ACTIVE_CMD,
    _GET_STATE_CMD,
    _CONTROL_CMD,
    _SET_PARAM_CMD,
    _SET_ASR_CMD,
    _EVENT_CMD,
    _REC_CMD,

    MIC_END_CMD
} MIC_CMD;

typedef struct {
    int evt_id;
    union
    {
        void *ptr;
        int  ival;
    };
    int size;
} evt_param_t;

typedef struct {
    int cmd;
    const char *url;
    const char *save_name;
} rec_param_t;

typedef struct {
    int     valid;
    void    *data;
    size_t  size;
} wwv_param_t;

mic_t g_mic;

#define MIC_OPS(mic, fn, param) \
        if (mic->ops->fn) \
            mic->ops->fn(mic, param)
static int _get_state(mic_t *mic, rpc_t *rpc)
{
    rpc_put_reset(rpc);
    rpc_put_int(rpc, mic->state);
    return 0;
}
//0:进入离线识别  1：接受PCM数据 2：主动退出接收PCM数据并进入离线模式
static int _control(mic_t *mic, rpc_t *rpc)
{
    int type = rpc_get_int(rpc);

    if (type == MIC_CTRL_START_PCM) {
        MIC_OPS(mic, pcm_data_control, 1);
        if (mic->state == MIC_STATE_IDLE) {
            mic->state = MIC_STATE_SESSION;
        }
    } else if (type == MIC_CTRL_STOP_PCM) {
        MIC_OPS(mic, pcm_data_control, 0);
        mic->state = MIC_STATE_IDLE;
    } else if (type == MIC_CTRL_START_VAD) {
        MIC_OPS(mic, vad_control, 1);
    } else if (type == MIC_CTRL_STOP_VAD) {
        MIC_OPS(mic, vad_control, 0);
    } else if (type == MIC_CTRL_START_SESSION) {
        MIC_OPS(mic, kws_wake, 0);
    } else if (type == MIC_CTRL_ENABLE_WWV) {
        MIC_OPS(mic, wwv_enable, 1);
    } else if (type == MIC_CTRL_DISABLE_WWV) {
        MIC_OPS(mic, wwv_enable, 0);
    } else if (type == MIC_CTRL_DEBUG_LEVEL_1) {
        MIC_OPS(mic, debug_control, 1);
    } else if (type == MIC_CTRL_DEBUG_LEVEL_2) {
        MIC_OPS(mic, debug_control, 2);
    } else if (type == MIC_CTRL_DEBUG_CLOSE) {
        MIC_OPS(mic, debug_control, 0);
    } else {
        ;
    }

    return 0;
}

static void mic_event_hdl(void *priv, mic_event_id_t event_id, void *data, int size)
{
    mic_t *mic = (mic_t *)priv;
    evt_param_t param;

    aos_check_return(mic);

    memset(&param, 0, sizeof(param));
    param.evt_id = event_id;

    if (event_id == MIC_EVENT_SESSION_START || event_id == MIC_EVENT_VAD) {
        /* 唤醒的类型和VAD值通过 data指针传递，是一个局部变量，异步抛出需要复制该值 */
        param.ival   =  *((int *)data);
    } else if (event_id == MIC_EVENT_KWS_DATA) {
        param.ptr = data;
        param.size = size;
    } else if (event_id == MIC_EVENT_SESSION_STOP) {
    }

    if (event_id == MIC_EVENT_PCM_DATA) {
        int len = ringbuffer_write(&mic->ring_buffer, data, size);
        if (mic->evt_cnt < 2) {
            mic->evt_cnt ++;
        }

        if (len < size && mic->state == MIC_STATE_SESSION) {
        //    LOGW(TAG, "pcm full");
        }
    }

    if (mic->evt_cnt < 2 || (event_id != MIC_EVENT_PCM_DATA && event_id != MIC_EVENT_VAD_DATA)) {
        uservice_call_async(mic->srv, _EVENT_CMD, &param, sizeof(param));
    }

}

static int _set_param(mic_t *mic, rpc_t *rpc)
{
    int count;
    void *p = rpc_get_buffer(rpc, &count);

    MIC_OPS(mic, pcm_set_param, p);

    return 0;
}

static int _set_asr(mic_t *mic, rpc_t *rpc)
{
    int en = rpc_get_int(rpc);

    MIC_OPS(mic, kws_control, en);

    return 0;
}

// 设置方向焦点源，0-7， -1 为所有方向都一样，指定方向增强，抑制其他方向的声音
static int _set_active(mic_t *mic, rpc_t *rpc)
{
    int source = rpc_get_int(rpc);

    mic->source = source;
    return 0;
}

static void pcm_evt_hdl(mic_t *mic)
{
    int size, cap;
    uint8_t *data = mic->rbuf;

    cap = sizeof(mic->rbuf);
    while (1) {
        // ringbuffer_peek(&mic->ring_buffer,(uint8_t **)&data, &size);
        size = ringbuffer_read(&mic->ring_buffer, data, cap);
        if(size > 0) {
            if (mic->cb) {
                mic->cb(mic->source, MIC_EVENT_PCM_DATA, data, size);
            }
            // ringbuffer_skip(&mic->ring_buffer, size);
        } else {
            return;
        }
    }
}

long long g_mic_tick;

static int _event_hdl(mic_t *mic, rpc_t *rpc)
{
    evt_param_t *param = (evt_param_t *)rpc_get_buffer(rpc, NULL);

    switch (param->evt_id) {
    case MIC_EVENT_PCM_DATA:
    case MIC_EVENT_VAD_DATA:
        if (g_mic_tick == 0) {
            g_mic_tick = aos_now_ms();
        }
        pcm_evt_hdl(mic);
        break;
    case MIC_EVENT_VAD:
        if(mic->cb) {
            mic->cb(mic->source, MIC_EVENT_VAD, &param->ival, sizeof(int));
        }
        break;
    case MIC_EVENT_SESSION_START:
        mic->state = MIC_STATE_SESSION;
        if(mic->cb) {
            mic->cb(mic->source, MIC_EVENT_SESSION_START, &param->ival, sizeof(int));
        }
        break;

    case MIC_EVENT_SESSION_STOP:
        if(mic->cb) {
            mic->cb(mic->source, MIC_EVENT_SESSION_STOP, NULL, 0);
        }
        mic->state = MIC_STATE_IDLE;
        break;

    case MIC_EVENT_KWS_DATA:
        if(mic->cb) {
            mic->cb(mic->source, MIC_EVENT_KWS_DATA, param->ptr, param->size);
        }
        break;

    default:
        break; 
    }

    mic->evt_cnt = 0;
    return 0;
}

static int _rec_ctrl(mic_t *mic, rpc_t *rpc)
{
    rec_param_t *param = (rec_param_t *)rpc_get_buffer(rpc, NULL);

    if (param->cmd == MIC_REC_START) {
        g_mic.ops->mic_rec_start(mic, param->url, param->save_name);
    } else if (param->cmd == MIC_REC_STOP) {
        g_mic.ops->mic_rec_stop(mic);
    }
    return 0;
} 

static const rpc_process_t c_mic_cmd_cb_table[] = {
    {_CONTROL_CMD,           (process_t)_control},
    {_SET_PARAM_CMD,         (process_t)_set_param},
    {_SET_ASR_CMD,           (process_t)_set_asr},
    {_SET_ACTIVE_CMD,        (process_t)_set_active},
    {_GET_STATE_CMD,         (process_t)_get_state},
    {_EVENT_CMD,             (process_t)_event_hdl},
    {_REC_CMD,               (process_t)_rec_ctrl},
    {MIC_END_CMD,            (process_t)NULL},
};

int mic_process_rpc(void *context, rpc_t *rpc)
{
    return uservice_process(context, rpc, c_mic_cmd_cb_table);
}

int aui_mic_set_param(mic_pcm_param_t *param)
{
    int ret = -1;

    aos_check_return_einval(g_mic.srv);

    ret = uservice_call_async(g_mic.srv, _SET_PARAM_CMD, (void *)param, sizeof(mic_pcm_param_t));
    return ret;
}

int aui_mic_set_wake_enable(int en)
{
    int ret = -1;
    int param = en;

    aos_check_return_einval(g_mic.srv);

    ret = uservice_call_async(g_mic.srv, _SET_ASR_CMD, (void *)&param, sizeof(int));
    return ret;
}

int aui_mic_set_active(int source)
{
    int ret = -1;
    int param = source;

    aos_check_return_einval(g_mic.srv);

    ret = uservice_call_async(g_mic.srv, _SET_ACTIVE_CMD, (void *)&param, sizeof(int));
    return ret;
}

int aui_mic_control(mic_ctrl_cmd_t cmd)
{
    int ret = -1;
    int param = cmd;

    aos_check_return_einval(cmd >= MIC_CTRL_START_PCM && cmd < MIC_END);
    aos_check_return_einval(g_mic.srv);

    ret = uservice_call_async(g_mic.srv, _CONTROL_CMD, (void *)&param, sizeof(int));
    return ret;
}

void *aui_mic_get_wwv_data(size_t *len)
{
    void *data;
    int ret;

    if (g_mic.srv == NULL || len == 0) {
        return NULL;
    }

    ret = g_mic.ops->wwv_get_data(&g_mic, &data, len);
    if (ret == 0) {
        return data;
    }

    *len = 0;
    return NULL;
}

int aui_mic_get_state(mic_state_t *state)
{
    int ret = -1;

    aos_check_return_einval(state);

    if (g_mic.srv == NULL) {
        return MIC_STATE_IDLE;
    }

    ret = uservice_call_sync(g_mic.srv, _GET_STATE_CMD, NULL, state, sizeof(mic_state_t));
    return ret;
}

int aui_mic_stop(void)
{
    aos_check_return_einval(g_mic.srv && g_mic.ops);

    if (g_mic.ops->deinit)
        g_mic.ops->deinit(&g_mic);

    if (g_mic.task) {
        utask_destroy(g_mic.task);
    }

    uservice_destroy(g_mic.srv);
    memset(&g_mic, 0x00, sizeof(mic_t));

    return 0;
}

int aui_mic_start(utask_t *task, aui_mic_evt_t evt_cb)
{
    aos_check_return_einval(task);

    if (g_mic.srv != NULL || g_mic.ops == NULL) {
        return -1;
    }

    if (g_mic.ops->init && g_mic.ops->init(&g_mic, mic_event_hdl) < 0) {
        LOGE(TAG, "mic srv ops init failed");
        return -1;
    }

    g_mic.recv_buf = (char *)aos_malloc_check(RINGBUFFER_SIZE);

    if (g_mic.recv_buf == NULL) {
        return -1;
    }

    g_mic.task = task;

    g_mic.srv = uservice_new("mic", mic_process_rpc, &g_mic);
    aos_check_return_enomem(g_mic.srv);

    utask_add(task, g_mic.srv);
    g_mic.cb = evt_cb;

    ringbuffer_create(&g_mic.ring_buffer, g_mic.recv_buf, RINGBUFFER_SIZE);

    //asr enabled defualt at dsp
    //g_mic.ops->kws_control(&g_mic, 1);

    return 0;
}

int mic_set_privdata(void *priv)
{
    aos_check_return_einval(priv);

    g_mic.priv = priv;
    return 0;
}

void *mic_get_privdata(void)
{
    return (g_mic.priv);
}

int aui_mic_rec_start(const char *url, const char *save_name)
{
    int ret = -1;
    rec_param_t rec_param;

    if (!(url && save_name)) {
        LOGE(TAG, "no url or save_name");
        return -1;
    }

    rec_param.cmd = MIC_REC_START;
    rec_param.url = url;
    rec_param.save_name = save_name;

    aos_check_return_einval(g_mic.srv);

    ret = uservice_call_async(g_mic.srv, _REC_CMD, (void *)&rec_param, sizeof(rec_param_t));
    return ret;
}

int aui_mic_rec_stop(void)
{
    int ret = -1;
    rec_param_t rec_param;

    rec_param.cmd = MIC_REC_STOP;

    aos_check_return_einval(g_mic.srv);

    ret = uservice_call_async(g_mic.srv, _REC_CMD, (void *)&rec_param, sizeof(rec_param_t));
    return ret;
}

int mic_ops_register(mic_ops_t *ops)
{
    g_mic.ops = ops;

    return 0;
}
