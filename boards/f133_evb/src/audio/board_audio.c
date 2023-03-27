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
    csi_pin_set_mux(PD19, PD19_DMIC_DATA0);
    csi_pin_set_mux(PD20, PD20_DMIC_CLK);

    // Audio PA Enable
	csi_pin_set_mux(PB4, PIN_FUNC_GPIO);

    csi_codec_t codec;
    csi_codec_init(&codec, 0);
}
#endif
