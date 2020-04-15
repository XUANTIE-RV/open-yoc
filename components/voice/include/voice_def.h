/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef _VOICE_DEF_H_
#define _VOICE_DEF_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <alsa/pcm.h>

#define VOICE_IPC_ID (0x01)
#define VOICE_DCACHE_OFFSET 16

#define voice_dcache_writeback csi_dcache_clean_range
#define voice_dcache_invalidate csi_dcache_invalid_range

typedef struct __voice voice_t;
typedef struct __voice_pcm voice_pcm_t;
typedef void (*voice_pcm_send)(void *priv, void *data, int len);

typedef enum {
    VOICE_PCM_CMD, //AP --> CP
    VOICE_PCM_PARAM_SET_CMD,
    VOICE_BACKFLOW_CONTROL_CMD,
    VOICE_CP_START_CMD,

    VOICE_KWS_CMD = 0x80, //CP --> AP
    VOICE_DATA_CMD,
    VOICE_VAD_CMD,
    VOICE_SILENCE_CMD,
} ipc_msg_id;

typedef enum {
    VOICE_MIC_DATA,
    VOICE_REF_DATA,
    VOICE_VAD_DATA,
    VOICE_AEC_DATA,

    VOCIE_BACKFLOW_DATA
} voice_backflow_id_t;

typedef struct {
    int type;
    int seq;
    int flag;
    int len;
    char data[0];
} voice_data_t;

typedef struct {
    char            *pcm_name;
    unsigned int     rate;
    int              sample_bits;
    int              access;
    int              channles;
    int              channles_sum;
    int              period_bytes;
} voice_pcm_param_t;

typedef struct {
    int cts_ms;
    int ipc_mode;
} voice_param_t;

typedef struct {
    aos_pcm_t               *hdl;
    char                    *name;
    voice_pcm_param_t       *param;
    void                    *data;
    int                      len;
} voice_capture_t;

void *voice_malloc(unsigned int size);
void voice_free(void *data);

voice_pcm_t *pcm_init(voice_pcm_send send, void *priv);
void pcm_deinit(voice_pcm_t *p);
void pcm_mic_config(voice_pcm_t *p, voice_pcm_param_t *param);
void pcm_ref_config(voice_pcm_t *p, voice_pcm_param_t *param);
int pcm_start(voice_pcm_t *p);

#ifdef __cplusplus
}
#endif

#endif