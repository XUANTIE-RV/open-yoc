/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef _VOICE_AP_H_
#define _VOICE_AP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <voice_def.h>

typedef enum {
    VOICE_ASR_EVT,
    VOICE_SILENCE_EVT,
    VOICE_DATA_EVT
} voice_evt_id_t;

typedef enum {
    VOICE_PCM_DATA,
    VOICE_OPUS_DATA,
    VOICE_USER_DATA
} voice_encode_t;

typedef void (*voice_evt_t)(void *priv, voice_evt_id_t evt_id, void *data, int len);
typedef void (*voice_load_hook)(void);

/**
 * @brief  voice init
 * @param  [in] cb          : voice event callback
 * @param  [in] priv        : argv back to cb
 * @return !NULL on success, NULL on failed
 */
voice_t *voice_init(voice_evt_t cb, void *priv);

/**
 * @brief  voice deinit
 * @param  [in] v          : voice handle
 */
void voice_deinit(voice_t *v);

/**
 * @brief  voice config
 * @param  [in] v          : voice handle
 * @param  [in] p          : voice param
 * @return 0 on success, -1 on failed
 */
int voice_config(voice_t *v, voice_param_t *p);

/**
 * @brief  voice add and config mic(ref)
 * @param  [in] v          : voice handle
 * @param  [in] p          : voice pcm param
 * @return 0 on success, -1 on failed
 */
int voice_add_mic(voice_t *v, voice_pcm_param_t *p);
int voice_add_ref(voice_t *v, voice_pcm_param_t *p);

/**
 * @brief  voice start
 * @param  [in] v          : voice handle
 * @return 0 on success, -1 on failed
 */
int voice_start(voice_t *v);

/**
 * @brief  voice stop
 * @param  [in] v          : voice handle
 * @return 0 on success, -1 on failed
 */
int voice_stop(voice_t *v);

/**
 * @brief  voice pcm backflow control
 * @param  [in] v          : voice handle
 * @param  [in] id         : voice backflow id
 * @param  [in] flag       : 1:open
 * @return 0 on success, -1 on failed
 */
int voice_backflow_control(voice_t *v, voice_backflow_id_t id, int flag);

// TODO
// event control
// int voice_kws_control(voice_t *v, int flag);
// int voice_svad_control(voice_t *v, int flag);
// int voice_hvad_control(voice_t *v, int flag);
// encode control
// int voice_encode_control(voice_t *v, voice_encode_t type, int flag);

#ifdef __cplusplus
}
#endif

#endif