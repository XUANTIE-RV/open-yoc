/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <board.h>

#if defined(BOARD_AUDIO_SUPPORT) && BOARD_AUDIO_SUPPORT > 0
#include <stdlib.h>
#include <stdio.h>
#include <drv/codec.h>
#include <drv/pin.h>

void board_audio_init(void)
{
    csi_pin_set_mux(PB11, PB11_DMIC_DATA0);
    csi_pin_set_mux(PE17, PE17_DMIC_CLK);

    csi_codec_t codec;
    csi_codec_init(&codec, 0);
}
#endif
