#ifndef __YV_AP__
#define __YV_AP__

typedef enum {
    YV_VAD_EVT,   // 探测到人声
    YV_VOICE_EVT, // 有 PCM 数据
    YV_ASR_EVT,   // 检测到唤醒词
    YV_SILENCE_EVT,
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

typedef struct _yv_ap_ yv_t;
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

//init
void ai_init(void);
yv_t *yv_init(yv_event_t cb, void *priv);
int   yv_deinit(yv_t *yv);

//control
int yv_kws_enable(yv_t *yv);
int yv_asr_disable(yv_t *yv);

int yv_vad_enable(yv_t *yv);
int yv_vad_disable(yv_t *yv);

int yv_pcm_enable(yv_t *yv);
int yv_pcm_disable(yv_t *yv);

int yv_aec_enable(yv_t *yv);
int yv_aec_disable(yv_t *yv);

int yv_set_vad_timeout(yv_t *yv, int ms);
// int yv_pcm_auto(yv_t *yv);

//pcm param
// no suport yet
int yv_pcm_param_set(yv_t *yv, yv_pcm_param_t *param);
int yv_pcm_param_get(yv_t *yv, yv_pcm_param_t *param);

#endif
