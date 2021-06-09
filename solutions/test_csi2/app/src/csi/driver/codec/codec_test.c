/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <codec_test.h>

test_func_map_t codec_test_funcs_map[] = {
    {"CODEC_SYNC_OUTPUT", test_codec_syncOutput},
    {"CODEC_SYNC_INPUT", test_codec_syncInput},
    {"CODEC_ASYNC_OUTPUT", test_codec_asyncOutput},
    {"CODEC_ASYNC_INPUT", test_codec_asyncInput},
    {"CODEC_OUTPUT_CHANNEL_STATE", test_codec_outputChannelState},
    {"CODEC_INPUT_CHANNEL_STATE", test_codec_inputChannelState},
    {"CODEC_PAUSE_RESUME", test_codec_pauseResume},
    {"CODEC_OUTPUT_MUTE", test_codec_outputMute},
    {"CODEC_INPUT_MUTE", test_codec_inputMute},
    {"CODEC_OUTPUT_BUFFER", test_codec_outputBuffer},
};

int test_codec_main(char *args)
{
    uint8_t i;

    for (i = 0; i < sizeof(codec_test_funcs_map) / sizeof(test_func_map_t); i++) {
        if (!strcmp((void *)_mc_name, codec_test_funcs_map[i].test_func_name)) {
            (*(codec_test_funcs_map[i].test_func_p))(args);
            return 0;
        }
    }

    TEST_CASE_TIPS("CODEC module don't support this command.");
    return -1;
}


