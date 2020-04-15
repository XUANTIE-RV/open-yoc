#ifndef __YV_CP__
#define __YV_CP__

typedef enum {
    YV_PCM_EVT,
    YV_PARM_SET_EVT,
    YV_PARM_GET_EVT,
    YV_VAD_ENABLE_EVT,
    YV_PCM_ENABLE_EVT,
    YV_AEC_ENABLE_EVT,
    YV_ASR_ENABLE_EVT,
    YV_ASR_WAKE_EVT,
    YV_SET_VAD_TIMEOUT_EVT,

    YV_EVT_ID_END
} yv_evt_id_t;

// VAD状态定义
typedef enum {
    E_VAD_SILENCE = 0,
    E_VAD_VOICE = -2,
    E_VAD_STOP = -3
} VadStatus_E;

typedef struct voice_data {
    int   type; // PCM / OPUS
    void *data;
    int   len;
    int   vad;
} voice_t;

typedef struct _yv_cp_ yv_t;
typedef void (*yv_event_t)(void *priv, yv_evt_id_t evt_id, void *data, int len);
typedef struct yv_pcm_param yv_pcm_param_t;

struct yv_pcm_param {
    int sample_bits;
    int channels;
    int rate;
    int encode;
    int buffer_size;
    int buffer_num;
    int sentence_time_ms;
};

yv_t *yv_init(yv_event_t cb, void *priv);
int yv_deinit(yv_t *yv);

//Event to AP
int yv_pcm_send(yv_t *yv, void *pcm, int len);
int yv_voice_data_send(yv_t *yv, void *data);
int yv_asr_send(yv_t *yv, int type);
int yv_vad_send(yv_t *yv, int type);
int yv_sentence_timout(yv_t *yv);


#endif