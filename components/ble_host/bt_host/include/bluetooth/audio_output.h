/**
 * Copyright (c) 2022  Alibaba Group Holding Limited
 */

#ifndef __AUDIO_OUTPUT_H__
#define __AUDIO_OUTPUT_H__
#include <ble_types/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @brief audio output configure structure */
typedef struct audio_output_conf {
    /** frame len set from upper layer */
    uint16_t frame_len;
    /** frame number set from upper layer */
    uint8_t  frame_num;
    /** audio channel */
    uint8_t  channel;
    /** audio sample rate */
    uint32_t sample_rate;
    /** audio buffer, should alloced from upper layer */
    void     *buf;
} audio_output_conf_t ;

/** @brief audio output structure */
typedef struct bt_a2dp_audio_output {
    /**  audio output init */
    int (*init)(void);
    /**  audio output media copfigure */
    int (*conf)(audio_output_conf_t *conf);
    /**  audio output media open */
    int (*open)(void);
    /**  audio output start */
    int (*start)(void);
    /**  audio output get frame buf */
    int (*get_write_frame_buf)(void **buf, u32_t *size);
    /**  audio output write done */
    int (*write_done)(void *buf, u32_t size);
    /** audio output stop */
    int (*stop)(void);
    /**  audio output media close */
    int (*close)(void);
    /**  audio output vol control, range: 0 ~ 127 */
    int (*vol)(u8_t vol);
} bt_a2dp_audio_output_t;

/**
 * @brief  get audio output interface
 * @return bt_a2dp_audio_output_t* 
 */
const bt_a2dp_audio_output_t *bt_a2dp_audio_output_get_interface(void);

#ifdef __cplusplus
}
#endif

#endif /* __AUDIO_OUTPUT_H__ */