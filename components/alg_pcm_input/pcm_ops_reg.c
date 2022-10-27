/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */
#include "pcm_input_internal.h"

static pcm_acquire_ops_t *g_pcm_acquire_ops_ptr = NULL;

int voice_pcm_acquire_init(int bit_format, int sample_rate, int frame_ms, int chn_num)
{
    if (g_pcm_acquire_ops_ptr) {
        return g_pcm_acquire_ops_ptr->init(bit_format, sample_rate, frame_ms, chn_num);
    }
    return -1;
}

int voice_pcm_acquire(void *data, int len)
{
    if (g_pcm_acquire_ops_ptr) {
        return g_pcm_acquire_ops_ptr->acquire(data, len);
    }
    return -1;
}

void voice_pcm_acquire_register(pcm_acquire_ops_t* ops)
{
    g_pcm_acquire_ops_ptr = ops;
}
