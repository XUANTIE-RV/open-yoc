/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */
#include <aos/kernel.h>
#include <aos/list.h>
#include <ulog/ulog.h>
#include <aos/debug.h>
#include "yoc/pcm_input_port.h"

#ifndef CONFIG_PCM_INPUT_TASK_SIZE
#define CONFIG_PCM_INPUT_TASK_SIZE (1024 * 8)
#endif

typedef struct {
    slist_t     _next;
    mic_data_cb cb;
    void *      arg;
} _pcm_cb;

typedef struct {
    uint8_t          register_flag   : 1;
    uint8_t          input_init_flag : 1;
    pcm_input_ops_t *pcm_input_ops_ptr;
    slist_t          mic_cb_head;
    aos_mutex_t      cb_mutex;
    aos_task_t       pcm_task;
    int              cb_enable;
} _pcm_input_ctx;

_pcm_input_ctx g_pcm_ctx = { 0x0 };

#define TAG "PCM_INPUT"

static void _pcm_data_input(void *arg)
{
    int      capture_byte   = (ssize_t)arg;
    int      rlen           = 0;
    uint8_t *capture_buffer = NULL;
    _pcm_cb *pcm_iter_temp  = NULL;
    slist_t *tmp;

    LOGD(TAG, "Capture len %d", capture_byte);

    capture_buffer = malloc(capture_byte);
    if (!capture_buffer) {
        LOGE(TAG, "capture buffer malloc failed");
        return;
    }

    /* capture main loop */
    while (1) {
        if (g_pcm_ctx.pcm_input_ops_ptr && g_pcm_ctx.pcm_input_ops_ptr->pcm_acquire) {
            rlen = g_pcm_ctx.pcm_input_ops_ptr->pcm_acquire(capture_buffer, capture_byte);
            if (rlen <= 0) {
                continue;
            }
        } else {
            LOGE(TAG, "capture ops not register");
            goto exit;
        }

        if (g_pcm_ctx.cb_enable) {
            aos_mutex_lock(&g_pcm_ctx.cb_mutex, AOS_WAIT_FOREVER);
            slist_for_each_entry_safe(&g_pcm_ctx.mic_cb_head, tmp, pcm_iter_temp, _pcm_cb, _next)
            {
                pcm_iter_temp->cb(capture_buffer, rlen, pcm_iter_temp->arg);
            }
            aos_mutex_unlock(&g_pcm_ctx.cb_mutex);
        }
    }

exit:
    free(capture_buffer);
    aos_task_exit(0);
}

static _pcm_cb *_cb_list_search(slist_t *list_head, mic_data_cb cb)
{
    if (!list_head) {
        return NULL;
    }

    _pcm_cb *pcm_iter_temp = NULL;
    slist_t *tmp;
    aos_mutex_lock(&g_pcm_ctx.cb_mutex, AOS_WAIT_FOREVER);
    slist_for_each_entry_safe(list_head, tmp, pcm_iter_temp, _pcm_cb, _next)
    {
        if (!pcm_iter_temp) {
            aos_mutex_unlock(&g_pcm_ctx.cb_mutex);
            return NULL;
        } else {
            if (pcm_iter_temp->cb == cb) {
                aos_mutex_unlock(&g_pcm_ctx.cb_mutex);
                return pcm_iter_temp;
            }
        }
    }
    aos_mutex_unlock(&g_pcm_ctx.cb_mutex);
    return NULL;
}

static int _cb_list_add_cb(slist_t *list_head, mic_data_cb cb, void *arg)
{

    if (!list_head) {
        return -1;
    }

    _pcm_cb *cb_node_temp = _cb_list_search(list_head, cb);
    if (cb_node_temp) {
        if (cb_node_temp->arg == arg) {
            LOGE(TAG, "device already exist");
            return -EALREADY;
        } else {
            aos_mutex_lock(&g_pcm_ctx.cb_mutex, AOS_WAIT_FOREVER);
            cb_node_temp->arg = arg;
            aos_mutex_unlock(&g_pcm_ctx.cb_mutex);
            return 0;
        }
    }

    cb_node_temp = (_pcm_cb *)malloc(sizeof(_pcm_cb));
    if (!cb_node_temp) {
        LOGE(TAG, "No buffer for pcm cb");
        return -ENOMEM;
    }

    cb_node_temp->arg = NULL;
    cb_node_temp->cb  = cb;
    cb_node_temp->arg = arg;

    aos_mutex_lock(&g_pcm_ctx.cb_mutex, AOS_WAIT_FOREVER);
    slist_add_tail(&cb_node_temp->_next, list_head);
    aos_mutex_unlock(&g_pcm_ctx.cb_mutex);
    return 0;
}

static int _cb_list_rm_cb(slist_t *list_head, mic_data_cb cb)
{
    if (!list_head) {
        LOGE(TAG, "list head null");
        return -1;
    }

    _pcm_cb *cb_node_temp = _cb_list_search(list_head, cb);
    if (!cb_node_temp) {
        LOGE(TAG, "no cb node found");
        return -1;
    }

    aos_mutex_lock(&g_pcm_ctx.cb_mutex, AOS_WAIT_FOREVER);
    slist_del(&cb_node_temp->_next, list_head);
    aos_mutex_unlock(&g_pcm_ctx.cb_mutex);

    free(cb_node_temp);

    return 0;
}

static int _pcm_cb_register(mic_data_cb cb, void *arg)
{
    return _cb_list_add_cb(&g_pcm_ctx.mic_cb_head, cb, arg);
}

static int _pcm_cb_unregister(mic_data_cb cb)
{
    return _cb_list_rm_cb(&g_pcm_ctx.mic_cb_head, cb);
}

/*user api*/
void pcm_acquire_register(pcm_input_ops_t *ops)
{
    if (g_pcm_ctx.register_flag) {
        return;
    }

    g_pcm_ctx.pcm_input_ops_ptr = ops;

    g_pcm_ctx.register_flag = 1;
}

int pcm_input_init(int bit_format, int sample_rate, int frame_ms, int chn_num)
{
    int  err          = 0;
    long capture_byte = 0;
    if (!g_pcm_ctx.register_flag) {
        LOGE(TAG, "Call pcm_acquire_register first");
        return -1;
    }

    if (g_pcm_ctx.input_init_flag) {
        LOGE(TAG, "Pcm input init already");
        return -1;
    }

    if (g_pcm_ctx.pcm_input_ops_ptr) {
        capture_byte = g_pcm_ctx.pcm_input_ops_ptr->init(bit_format, sample_rate, frame_ms, chn_num);
        if (capture_byte <= 0) {
            LOGE(TAG, "Invalid capture size");
            return -1;
        }
    } else {
        return -1;
    }

    slist_init(&g_pcm_ctx.mic_cb_head);
    aos_mutex_new(&g_pcm_ctx.cb_mutex);

    err = aos_task_new_ext(&g_pcm_ctx.pcm_task, "PcmInput", &_pcm_data_input, (void *)capture_byte,
                           CONFIG_PCM_INPUT_TASK_SIZE, AOS_DEFAULT_APP_PRI - 4);
    if (err) {
        LOGE(TAG, "Capture task create failed %d", err);
        return -1;
    }

    g_pcm_ctx.input_init_flag = 1;
    g_pcm_ctx.cb_enable = 1;

    return capture_byte;
}

int pcm_input_cb_register(mic_data_cb cb, void *arg)
{
    if (!g_pcm_ctx.register_flag) {
        LOGE(TAG, "Call pcm_acquire_register first");
        return -1;
    }

    if (!g_pcm_ctx.input_init_flag) {
        LOGE(TAG, "Call pcm_input_init first");
        return -1;
    }

    if (g_pcm_ctx.pcm_input_ops_ptr) {
        return _pcm_cb_register(cb, arg);
    }
    return 0;
}

int pcm_input_cb_unregister(mic_data_cb cb)
{
    if (!g_pcm_ctx.register_flag) {
        LOGE(TAG, "Call pcm_acquire_register first");
        return -1;
    }

    if (!g_pcm_ctx.input_init_flag) {
        LOGE(TAG, "Call pcm_input_init first");
        return -1;
    }

    if (g_pcm_ctx.pcm_input_ops_ptr) {
        return _pcm_cb_unregister(cb);
    }
    return 0;
}

void pcm_acquire_set_enable(int en)
{
    g_pcm_ctx.cb_enable = en;
}
