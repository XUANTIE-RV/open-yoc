#ifndef __IPC_COMMON__
#define __IPC_COMMON__

#define YV_SERVICE_ID 0x20

#ifdef CONFIG_CHIP_PANGU
#define  CP_DSP_IDX 2
#define  AP_CPU_IDX 0
#define  CP_CPU_IDX 1
#else
#define  CP_DSP_IDX 0
#define  AP_CPU_IDX 1
#define  CP_CPU_IDX 2
#endif

typedef enum {
//ap
    YV_POWER_ENABLE_CMD,
    YV_KWS_ENABLE_CMD,
    YV_VAD_ENABLE_CMD,
    YV_PCM_ENABLE_CMD,
    YV_AEC_ENABLE_CMD,
    YV_ASR_WAKE_CMD,
    YV_VAD_TIMEOUT_CMD,
    YV_NOACK_TIME_CMD,
    YV_PCM_SET_CMD,
    YV_PCM_SET_DATA_CMD,
    YV_PCM_GET_CMD,
    YV_PCM_FREE_CMD,

    YV_CP_CMD_START = 0x80, //CP -> AP
    YV_PCM_DATA_CMD,
    YV_VOICE_DATA_CMD,
    YV_ASR_CMD,
    YV_SILENCE_CMD,
    YV_VAD_CMD,

    YV_DEBUG = 0x90,
    YV_DEBUG_PCM,

    YV_AD_REQ_DEBUG = 0xa0,
    YV_AD_RESP_COMMON,

} YV_IPC_CMD;

#define BUFFER_NUM (8)
#define BUFFER_SIZE (512)
#define SENTENCE_TIME (1500)
#define SAMPLE_BITS (16)
#define CHANNELS (2)
#define RATE (16000)
#define ENCODE (0)

#endif
