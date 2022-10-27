#ifndef _BL_AUDIO_OUTPUT_H_
#define _BL_AUDIO_OUTPUT_H_

#include "bl_audio.h"
#include <blyoc_ringbuffer/blyoc_ringbuffer.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _auo_segment {
    uint32_t        dirty;    // 0-dirty; 1-clear; 2-valid data
    uint32_t        remain;   //
    _dmahw_cfg_t    dma_cfg;   // hw node cfg
} auo_segment_t;

typedef struct {
    uint32_t                sample_rate;                ///< Output data sample rate
    uint32_t                bit_width;                  ///< Onput data sample width
    uint8_t                 *buffer;                    ///< The buffer used to save audio data
    uint32_t                buffer_size;                ///< Output buffer size
    uint32_t                sound_channel_num;          ///< Number of soundchannel per channel
} auo_cfg_t;

/* dma*/
typedef struct _auo_dma {
    uint8_t          id;        /*dma id*/
    uint8_t          ch;        /*dma channel*/
    uint32_t         per_node_size;
    uint32_t         maxcount;   /* max node num */
    auo_segment_t    *node;
} auo_dma_t;

typedef struct _auo_ch auo_ch_t;

typedef void (*auo_cb_t)(auo_ch_t *context, audio_codec_event_t event, void *arg);

/* audio output ch */
struct _auo_ch {
    uint32_t                  ch_idx;            ///< Codec output channel idx
    auo_cb_t                  callback;
    void                      *arg;
    blyoc_ringbuf_t             *ringbuffer;
    uint8_t                   *buffer;         ///< The csi_ringbuf used to save audio data
    uint32_t                  buffer_size;                ///< Output buffer size
    uint32_t                ridx;
    uint32_t                widx;
    uint32_t                  sound_channel_num; ///< Number of sound channel
    uint32_t                  bit_width;                  ///< Onput data sample width
    auo_dma_t                 *dma;              ///< Dma channel handle
    //csi_state_t               state;             ///< Codec output channel current state
    uint8_t                   st;
    uint32_t                  pre_indx;
    void                      *priv;
    uint32_t                    head;
};

int auo_init(auo_ch_t *context);
int auo_start(auo_ch_t *context);
int auo_stop(auo_ch_t *context);
int auo_pause(auo_ch_t *context);
int auo_resume(auo_ch_t *context);
uint32_t auo_write(auo_ch_t *context, const void *data, uint32_t size);
uint32_t auo_write_async(auo_ch_t *context, const void *data, uint32_t size);
int auo_mute(auo_ch_t *context, uint8_t en);

int auo_analog_gain(auo_ch_t *context, float val);
int auo_digtal_gain(auo_ch_t *context, float val);

int auo_mix_gain(auo_ch_t *context, uint32_t val);
int auo_attach_callback(auo_ch_t *context, auo_cb_t callback, void *arg);

uint32_t auo_buffer_avail(auo_ch_t *context);
uint32_t auo_buffer_remain(auo_ch_t *context);

int auo_eq_set(uint32_t *eq_filt_coef, int bytes);
void auo_eq_set_example(void);
int auo_drc_set(uint32_t *eq_filt_coef, int bytes);

int auo_channel_config(auo_ch_t *context, auo_cfg_t *config);
int auo_tx_dma_link(auo_ch_t *context, void *dma);

void audio_poweron(void);

 #ifdef __cplusplus
 }
 #endif

 #endif /* _BL_AUDIO_OUT_H_ */

