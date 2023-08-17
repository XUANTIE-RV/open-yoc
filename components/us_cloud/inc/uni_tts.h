/**************************************************************************
 * Copyright (C) 2017-2017  Unisound
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 **************************************************************************
 *
 * Description : uni_tts_player.h
 * Author      : shangjinlong@unisound.com
 * Date        : 2018.06.19
 *
 **************************************************************************/
#ifndef SDK_VUI_BASIC_INC_UNI_TTS_H_
#define SDK_VUI_BASIC_INC_UNI_TTS_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  US_TTS_IDLE,
  US_TTS_RUNNING,
  US_TTS_FAIL,
  US_TTS_FINISH,
} UniTtsState;

typedef int (*UniTtsCb)(void *usr_ptr, UniTtsState state, const uint8_t *pcm, int len);

int TtsCreate(const char *config_file);
int TtsDestroy(void);
int TtsPlayString(char *play_content, UniTtsCb cb, void *cb_usr_ptr);
int TtsStop(void);
int TtsCheckIsPlaying();

#ifdef __cplusplus
}
#endif
#endif
