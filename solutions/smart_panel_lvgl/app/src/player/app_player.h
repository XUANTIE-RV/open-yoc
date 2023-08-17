/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */

#ifndef _APP_PLAYER_H_
#define _APP_PLAYER_H_

#include <stdint.h>
#include <smart_audio.h>
/************
 * 本地音播放
 ************/

int  local_audio_play(const char *name);
int  local_wakeup_audio_play(const char *name);

/*************
 * 播放器
 ************/
int app_player_init(void);

/*************
 * PA控制
 ************/
/**
 * set speaker mute
 *
 * @param[in] mute 1: set mute 0:cancel mute
 */
void app_speaker_mute(int mute);

/**
 * init speaker
 */
void app_speaker_init(void);

#endif