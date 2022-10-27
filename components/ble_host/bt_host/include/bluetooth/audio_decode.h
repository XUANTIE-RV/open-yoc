/**
 * Copyright (c) 2022  Alibaba Group Holding Limited
 */

#ifndef __AUDIO_DECODE_H__
#define __AUDIO_DECODE_H__
#include <ble_types/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

/** @brief audio output structure */
typedef struct bt_a2dp_audio_decode {
    /** codec_type ref to enum \ref bt_a2dp_codec_type */
    u8_t codec_type;
    /**  audio output init */
    int (*init)(void);
    /**  audio output media copfigure.*/
    int (*decode)(void **in_data, u32_t *in_size, void *out_data, u32_t *out_size);
    /** point to next decode */
    struct bt_a2dp_audio_decode *next;
} bt_a2dp_audio_decode_t;

/**
 * @brief  get sbc decoder interface
 * @return bt_a2dp_audio_decode_t* 
 */
bt_a2dp_audio_decode_t *bt_a2dp_audio_decode_sbc_get_interface(void);

/**
 * @brief  get aac decoder interface
 * @return bt_a2dp_audio_decode_t* 
 */
bt_a2dp_audio_decode_t *bt_a2dp_audio_decode_aac_get_interface(void);

#ifdef __cplusplus
}
#endif

#endif /* __AUDIO_DECODE_H__ */