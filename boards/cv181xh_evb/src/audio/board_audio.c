/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#include <cv181x_snd.h>

#include <board.h>

#include "board_audio.h"


void board_audio_init(void)
{
    snd_card_register(NULL);

    board_audio_out_set_gain(2, AUIDO_OUT_GAIN);
}
