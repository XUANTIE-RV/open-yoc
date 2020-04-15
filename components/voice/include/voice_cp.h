/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef _VOICE_AP_H_
#define _VOICE_AP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <voice_def.h>

typedef struct __voice_cts_ops {
    int (*init)(void *priv);
    int (*deinit)(void *priv);
    int (*aec)(void *priv, void *mic, void *ref, int ms, void *out);
    int (*vad)(void *priv, void *mic, void *ref, int ms, void *out);
    int (*kws)(void *priv, void *mic, void *ref, int ms, void *out);
    int (*asr)(void *priv, void *vad_data, int ms);
} voice_cts_ops_t;

voice_t *voice_cp_init(void *priv, voice_cts_ops_t *ops);
void voice_cp_deinit(voice_t *v);

// int voice_start(voice_t *v);
// int voice_stop(voice_t *v);

#ifdef __cplusplus
}
#endif

#endif