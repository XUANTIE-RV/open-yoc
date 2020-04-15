/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <yoc/aui.h>
#include <devices/uart.h>
#include <alsa/pcm.h>

#define TAG "mic"

#define UART_NAME "uart2"
struct __mic{
    uservice_t      *srv;
    utask_t         *task;
    aos_dev_t           *uart_dev;
    int              have_uart_event;
    int              state;
    int              source;
    mic_config_t    *config;

    int              have_pcm_event;
    aos_task_t       alsa_task;
    aos_pcm_t       *pcm;
};

typedef enum {
    _SET_ACTIVE_CMD,
    _CONFIG_CMD,
    _GET_STATE_CMD,
    _UART_RECV_CMD,
    _PCM_RECV_CMD,
    _CONTROL_CMD,

    MIC_END_CMD
} MIC_CMD;

static mic_t g_mic;

static void mic_uart_event(aos_dev_t *dev, int event_id, void *priv)
{
    if (event_id == USART_EVENT_READ) {
        if (g_mic.have_uart_event < 2) {
            uservice_call_async(g_mic.srv, _UART_RECV_CMD, NULL, 0);
            g_mic.have_uart_event++;
        }
    }
}

static int _config(mic_t *mic, rpc_t *rpc)
{
    mic_config_t *config = (mic_config_t*)rpc_get_point(rpc);
    int rpc_ret = 0;
    if(mic->config == NULL) {
        mic->config = aos_malloc(sizeof(mic_config_t));
        if(mic->config == NULL) {
            rpc_ret = -ENOMEM;
            return 0;
        }
    }
    memcpy(mic->config, config, sizeof(mic_config_t));

    rpc_put_reset(rpc);
    rpc_put_int(rpc, rpc_ret);
    return 0;
}

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

    if(type == MIC_IDLE) {
        mic->state = MIC_STATE_IDLE;
    }else if(type == MIC_CTRL_START_PCM) {
        char cmd = 0x21;
        printf("uart send 0x21\n");
        uart_send(mic->uart_dev, &cmd, 1);
        mic->state = MIC_STATE_PCM;
    } else if (type == MIC_CTRL_STOP_PCM) {
        char cmd = 0x25;
        printf("uart send 0x25\n");
        uart_send(mic->uart_dev, &cmd, 1);
        mic->state = MIC_STATE_IDLE;
    } else if (type == MIC_WAKEUP) {
        mic->state = MIC_STATE_SESSION;
    }
    return 0;
}

// 设置方向焦点源，0-7， -1 为所有方向都一样，指定方向增强，抑制其他方向的声音
static int _set_active(mic_t *mic, rpc_t *rpc)
{
    int source = rpc_get_int(rpc);

    mic->source = source;
    return 0;
}

static int _uart_handle(mic_t *mic, rpc_t *rpc)
{
    char c= 0;
    while(1) {
        int ret = uart_recv(mic->uart_dev,&c,1,0);
        printf("\r\n cmd id:%02x\r\n",c);
        if(ret > 0) {
            if(mic->config && mic->config->command) {
                mic->config->command(mic->source,c,NULL);
            }
        } else {
            break;
        }
    }
    mic->have_uart_event --;
    return 0;
}

static int mic_uart_init(mic_t *mic)
{
    mic->uart_dev = uart_open(UART_NAME);
    if (mic->uart_dev == NULL) {
        return -1;
    }

    uart_config_t config;
    uart_config_default(&config);
    config.baud_rate = 9600;
    uart_config(mic->uart_dev, &config);
    uart_set_event(mic->uart_dev, mic_uart_event, NULL);

    return 0;
}

static void mic_uart_deinit(mic_t *mic)
{
    uart_close(mic->uart_dev);
    mic->uart_dev = NULL;
}

static const rpc_process_t c_mic_cmd_cb_table[] = {
    {_CONTROL_CMD,           (process_t)_control},
    {_SET_ACTIVE_CMD,        (process_t)_set_active},
    {_UART_RECV_CMD,         (process_t)_uart_handle},
    {_CONFIG_CMD,            (process_t)_config},
    {_GET_STATE_CMD,         (process_t)_get_state},
    {MIC_END_CMD,            (process_t)NULL},
};

static aos_pcm_t *pcm_init(unsigned int *rate)
{
    aos_pcm_hw_params_t *params;
    aos_pcm_t *pcm;
    int err, period_frames, buffer_frames;

    aos_pcm_open(&pcm, "pcmC0", SND_PCM_STREAM_CAPTURE, 0);

    aos_pcm_hw_params_alloca(&params);
    err = aos_pcm_hw_params_any(pcm, params);

    if (err < 0) {
        LOGD(TAG, "Broken configuration for this PCM: no configurations available");
    }

    err = aos_pcm_hw_params_set_access(pcm, params, SND_PCM_ACCESS_RW_INTERLEAVED);

    if (err < 0) {
        LOGD(TAG, "Access type not available");
    }

    err = aos_pcm_hw_params_set_format(pcm, params, 16);

    if (err < 0) {
        LOGD(TAG, "Sample format non available");
    }

    err = aos_pcm_hw_params_set_channels(pcm, params, 1);

    if (err < 0) {
        LOGD(TAG, "Channels count non available");
    }

    aos_pcm_hw_params_set_rate_near(pcm, params, rate, 0);

    period_frames = 8;
    aos_pcm_hw_params_set_period_size_near(pcm, params, &period_frames, 0);

    buffer_frames = period_frames * 2;
    aos_pcm_hw_params_set_buffer_size_near(pcm, params, &buffer_frames);


    err = aos_pcm_hw_params(pcm, params);

    return pcm;
}

static void alsa_recv_thread(void *priv)
{
    mic_t *mic = (mic_t *)priv;
    aos_pcm_t *pcm ;
    int ret;
    unsigned int rate = 16000;
    int r_size = (rate / 1000) * (16 / 8 * 2) * 1;//1ms
    char *data = aos_malloc_check(r_size);

    mic->pcm = pcm_init(&rate);
    pcm = mic->pcm;
    LOGD(TAG, "alsa init succful");

    while (1) {
        ret = aos_pcm_readi(pcm, (void *)data, aos_pcm_bytes_to_frames(pcm, r_size));
        if(ret <= 0) {
            aos_pcm_wait(pcm, -1);
        } else {
            if (mic->config->pcm) {
                mic->config->pcm(mic->source,data,aos_pcm_frames_to_bytes(pcm, ret));
            }
        }
    }

    LOGD(TAG, "captrue over");
    aos_pcm_drain(pcm);
    aos_pcm_close(pcm);
}

int mic_process_rpc(void *context, rpc_t *rpc)
{
    return uservice_process(context, rpc, c_mic_cmd_cb_table);
}

int mic_config(mic_config_t *config)
{
    aos_check_param(config);
    int ret = -1;
    int rpc_ret;

    ret = uservice_call_sync(g_mic.srv, _CONFIG_CMD, (void*)config, &rpc_ret, sizeof(int));
    return ret < 0?ret : rpc_ret;
}

// 设置方向焦点源，0-7， -1 为所有方向都一样，指定方向增强，抑制其他方向的声音
int aui_mic_set_active(int source)
{
    int ret = -1;
    int param = source;

    ret = uservice_call_async(g_mic.srv, _SET_ACTIVE_CMD, (void*)&param, sizeof(int));
    return ret;
}

//0:进入离线识别  1：接受PCM数据 2：主动退出接收PCM数据并进入离线模式
int aui_mic_control(int type)
{
    aos_check_param(type >= MIC_IDLE && type < MIC_END);
    int ret = -1;
    int param = type;

    ret = uservice_call_async(g_mic.srv, _CONTROL_CMD, (void*)&param, sizeof(int));
    return ret;
}

int aui_mic_get_state(mic_state_t *state)
{
    aos_check_param(state);
    int ret = -1;

    if (g_mic.srv == NULL) {
        return ret;
    }

    ret = uservice_call_sync(g_mic.srv, _GET_STATE_CMD, NULL, state, sizeof(int));
    return ret;
}

int aui_mic_stop(void)
{
    if(g_mic.srv == NULL)
        return -1;

    mic_uart_deinit(&g_mic);
    if(g_mic.task)
        utask_destroy(g_mic.task);
    uservice_destroy(g_mic.srv);
    memset(&g_mic, 0x00, sizeof(mic_t));

    return 0;
}

int aui_mic_start(utask_t *task)
{
    if(g_mic.srv != NULL)
        return -1;

    if (task == NULL) {
        task = utask_new("mic", 2 * 1024, QUEUE_MSG_COUNT, AOS_DEFAULT_APP_PRI);
        g_mic.task = task;
    }

    g_mic.srv = uservice_new("mic", mic_process_rpc, &g_mic);
    aos_assert(g_mic.srv);
    utask_add(task, g_mic.srv);

    if(mic_uart_init(&g_mic) < 0) {
        aui_mic_stop();
        return -EIO;
    }

    if (0 != aos_task_new_ext(&g_mic.alsa_task, "alsa", alsa_recv_thread, &g_mic, 1024, AOS_DEFAULT_APP_PRI + 3)) {
            LOGE(TAG, "Create alsa task failed.");
    }

    return 0;
}
