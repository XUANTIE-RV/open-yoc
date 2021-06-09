/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef __CODEC_TEST__
#define __CODEC_TEST__

#include <stdint.h>
#include <drv/codec.h>
#include <drv/dma.h>
#include <soc.h>
#include <string.h>
#include <stdlib.h>
#include <autotest.h>
#include <test_log.h>
#include <test_config.h>
#include <test_common.h>

typedef struct
{
    uint32_t codec_idx;
    uint32_t ch_idx;
    uint32_t sample_rate;
    uint32_t bit_width;
    uint32_t period;
    uint32_t sound_channel_num;
    uint32_t data_size;
    uint32_t buffer_size;
    uint32_t digital_gain;
    uint32_t analog_gain;
    uint32_t mix_gain;
    uint32_t mode;
} test_codec_args_t;

extern int test_codec_syncOutput(char *args);
extern int test_codec_syncInput(char *args);
extern int test_codec_asyncOutput(char *args);
extern int test_codec_asyncInput(char *args);
extern int test_codec_outputChannelState(char *args);
extern int test_codec_inputChannelState(char *args);
extern int test_codec_pauseResume(char *args);
extern int test_codec_outputMute(char *args);
extern int test_codec_inputMute(char *args);
extern int test_codec_outputBuffer(char *args);
extern int test_codec_main(char *args);
#endif
