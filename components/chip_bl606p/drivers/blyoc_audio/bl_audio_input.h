#ifndef _BL_AUDIO_INPUT_H_
#define _BL_AUDIO_INPUT_H_

#include "bl_audio.h"
#include <blyoc_ringbuffer/blyoc_ringbuffer.h>
#include <bl_os_hal.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DEF_AUDIO_ANALOG_GAIN_INPUT1      (33)//33db,   check 0 ? or 6~42(step 3 db)
#define DEF_AUDIO_ANALOG_GAIN_INPUT2      (33)//33db,   check 0 ? or 6~42(step 3 db)
#define DEF_AUDIO_ANALOG_GAIN_INPUT3      (0) // 0db,   check 0 ? or 6~42(step 3 db)

#define DEF_AUDIO_DIGITAL_VOLUME_INPUT1   (0)
#define DEF_AUDIO_DIGITAL_VOLUME_INPUT2   (0)
#define DEF_AUDIO_DIGITAL_VOLUME_INPUT3   (0)

typedef struct _aui_segment {
    uint32_t        dirty;    // 0-dirty; 1-clear; 2-valid data
    uint32_t        remain;   //
    _dmahw_cfg_t    dma_cfg;   // hw node cfg
} aui_segment_t;

typedef struct {
    uint32_t                sample_rate;                ///< Output data sample rate
    uint32_t                bit_width;                  ///< Onput data sample width
    uint8_t                 *buffer;                    ///< The buffer used to save audio data
    uint32_t                buffer_size;                ///< Output buffer size
    uint32_t                sound_channel_num;          ///< Number of soundchannel per channel
} aui_cfg_t;

/* dma*/
typedef struct _aui_dma {
    uint8_t          id;        /*dma id*/
    uint8_t          ch;        /*dma channel*/
    uint32_t         per_node_size;
    uint32_t         maxcount;   /* max node num */
    aui_segment_t    *node;
} aui_dma_t;

typedef struct _aui_ch aui_ch_t;

typedef void (*aui_cb_t)(aui_ch_t *context, audio_codec_event_t event, void *arg);

/* audio input ch */
struct _aui_ch {
    uint32_t                  ch_idx;            ///< Codec output channel idx
    aui_cb_t                  callback;
    void                      *arg;
    aos_task_t   task;                          /* for isr handle */
    uint8_t      task_exit;                     /* for stop task flag;0-runding,1-stoped */
    aos_event_t  event;                         /* for task event stop or isr notify */
    blyoc_ringbuf_t             *ringbuffer;
    uint8_t                   *buffer;         ///< The csi_ringbuf used to save audio data
    uint32_t                  buffer_size;                ///< Output buffer size
    uint32_t                  sound_channel_num; ///< Number of sound channel
    uint32_t                  bit_width;                  ///< Onput data sample width
    aui_dma_t                 *dma;              ///< Dma channel handle
    //csi_state_t               state;             ///< Codec output channel current state
    uint8_t                   st;
    uint32_t                  pre_indx;
    void                      *priv;
    uint32_t                    head;
};

int aui_init(aui_ch_t *context);
int aui_start(aui_ch_t *context);
int aui_stop(aui_ch_t *context);
int aui_pause(aui_ch_t *context);
int aui_resume(aui_ch_t *context);
uint32_t aui_write(aui_ch_t *context, const void *data, uint32_t size);
uint32_t aui_write_async(aui_ch_t *context, const void *data, uint32_t size);
int aui_mute(aui_ch_t *context, uint8_t en);

int aui_analog_gain(aui_ch_t *context, float val);
int aui_digtal_gain(aui_ch_t *context, float val);

int aui_mix_gain(aui_ch_t *context, uint32_t val);
int aui_attach_callback(aui_ch_t *context, aui_cb_t callback, void *arg);

uint32_t aui_buffer_avail(aui_ch_t *context);
uint32_t aui_buffer_remain(aui_ch_t *context);

int aui_channel_config(aui_ch_t *context, aui_cfg_t *config);
int aui_rx_dma_link(aui_ch_t *context, void *dma);
uint32_t aui_read(aui_ch_t *context, const void *data, uint32_t size);

 #ifdef __cplusplus
 }
 #endif

 #endif /* _BL_AUDIO_INPUT_H_ */

