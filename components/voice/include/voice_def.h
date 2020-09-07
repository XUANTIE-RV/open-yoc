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

#define  VOICE_MESSAGE_ASYNC 0x00
#define  VOICE_MESSAGE_SYNC  0x01
#define  VOICE_MESSAGE_ACK   0x02

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

typedef struct {
    uint8_t         flag;            /** flag for MESSAGE_SYNC and MESSAGE_ACK */
    uint16_t        command;         /** command id the service provide */
    void           *req_data;            /** message data */
    int             req_len;             /** message len */
    void           *resp_data;
    int             resp_len;
} voice_msg_t;

typedef void (*voice_msg_evt_t)(void *priv, voice_msg_t *msg);

typedef struct {
    int hdl;
    voice_msg_evt_t cb;
    void *priv;
} voice_ch_t;

typedef struct {
    voice_ch_t *(*init)(voice_msg_evt_t cb, void *priv);
    int (*msg_send)(voice_ch_t *ch, voice_msg_t *msg);
} voice_ch_io_t;

voice_ch_io_t *ipc_ch_get(void);

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