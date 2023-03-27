/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include "app_voice.h"

void app_voice_play(uint8_t index, uint8_t voice)
{

#if defined(CONFIG_BOARD_NSP_AUDIO) && CONFIG_BOARD_NSP_AUDIO
    extern void board_nsp_audio_play(uint8_t index, uint8_t vol);
    board_nsp_audio_play(index, voice);
#endif
}
