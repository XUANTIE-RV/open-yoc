/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#ifndef _YOC_smart_audio_H_
#define _YOC_smart_audio_H_

#include <media.h>

// SYSTEM状态变化不会改变SMTAUDIO_STATE，只有MUSIC状态会改变
typedef enum {
    SMTAUDIO_STATE_PLAYING,      // 正在播放状态
    SMTAUDIO_STATE_PAUSE,        // 暂停状态
    SMTAUDIO_STATE_ZERO_VOLUME,  // 零音量状态
    SMTAUDIO_STATE_MUTE,         // 静音禁麦状态，除了禁麦键外，其他按键和语音指令都不响应
    SMTAUDIO_STATE_STOP,         // 停止状态
    SMTAUDIO_STATE_NUM
} smtaudio_state_t;

/**
 * @brief  init smart audio
 * @param  [in] stack_size     : stack size of media task
 * @param  [in] media_evt      : callback function to handle media event
 * @param  [in] min_vol        : minimum volume, if current system vol is 0, TTS will be set to min_vol
 * @param  [in] aef_conf       : audio effect config data buffer point, disable aef set NULL
 * @param  [in] aef_conf_size  : audio effect config data buffer size
 * @param  [in] speed          : audio play init speed, >=0.5 && <=2
 * @param  [in] resample       : resample rate, 16000, 44100, 48000
 * @return 0 on success, negative value on failed
 */
int8_t smtaudio_init(size_t stack_size, media_evt_t media_evt, uint8_t min_vol, uint8_t *aef_conf, size_t aef_conf_size, float speed, int resample);

/**
 * @brief  increase volume
 * @param  [in] vol             : volume step
 * @return 0 on success, negative value on failed
 */
int8_t smtaudio_vol_up(int16_t vol);

/**
 * @brief  decrease volume
 * @param  [in] vol             : volume step
 * @return 0 on success, negative value on failed
 */
int8_t smtaudio_vol_down(int16_t vol);

/**
 * @brief  set volume
 * @param  [in] vol             : target volume
 * @return 0 on success, negative value on failed
 */
int8_t smtaudio_vol_set(int16_t vol);

/**
 * @brief  get current volume
 * @return current volume, should be in 0~100
 */
int8_t smtaudio_vol_get(void);

/**
 * @brief  pause music playback
 * @return 0 on success, negative value on failed
 */
int8_t smtaudio_pause(void);

/**
 * @brief  mute music
 * @return 0 on success, negative value on failed
 */
int8_t smtaudio_mute(void);

/**
 * @brief  resume music playback
 * @return 0 on success, negative value on failed
 */
int8_t smtaudio_resume(void);

/**
 * @brief  start playing new stream
 * @param  [in] type            : can be system or music
 * @param  [in] url             : stream url
 * @param  [in] seek_time       : 0 - play from beginning
 *                                >0 - play from seek position
 * @param  [in] resume          : resume music automatically
 * @return 0 on success, negative value on failed
 */
int8_t smtaudio_start(int type, char *url, uint64_t seek_time, uint8_t resume);

/**
 * @brief  stop playing current stream
 * @param  [in] type            : can be system or music
 * @return 0 on success, negative value on failed
 */
int8_t smtaudio_stop(int type);

/**
 * @brief  get player configuration
 * @param  [in] config            : player configuration
 * @return 0 on success, negative value on failed
 */
int8_t smtaudio_get_config(aui_player_config_t *config);

/**
 * @brief  set player configuration
 * @param  [in] config            : player configuration
 * @return 0 on success, negative value on failed
 */
int8_t smtaudio_set_config(aui_player_config_t *config);

#define smtaudio_set_speed(speed) aui_player_set_speed(MEDIA_MUSIC, speed)


/**
 * @brief  set smart audio to lpm mode
 * @param  [in] state          : 1 - enter lpm, 0 - leave lpm
 * @return 0 on success, negative value on failed
 */
int8_t smtaudio_lpm(uint8_t state);

/**
 * @brief  get current smart audio state
 * @return smart audio state
 */
smtaudio_state_t smtaudio_get_state(void);

/**
 * @brief  deinit smart audio
 * @return 0 on success, negative value on failed
 */
int8_t smtaudio_deinit(void);

#endif //_YOC_smart_audio_H_