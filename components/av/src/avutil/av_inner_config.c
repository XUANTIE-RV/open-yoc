/*
 * Copyright (C) 2018-2022 Alibaba Group Holding Limited
 */

#include "av/avutil/av_inner_config.h"
#include "av/avutil/av_config.h"

static struct {
    int ao_diff_out;
    int ao_channel_num;
} g_av_configs = {
    CONFIG_AO_DIFF_SUPPORT,
    CONFIG_AV_AO_CHANNEL_NUM
};

/**
 * @brief  get number of audio output
 * @return
 */
int av_get_ao_channel_num()
{
    return g_av_configs.ao_channel_num;
}

/**
 * @brief  set number of audio output
 * @param  [in] num
 * @return
 */
void av_set_ao_channel_num(int num)
{
    g_av_configs.ao_channel_num = num;
}

/**
 * @brief  get whether audio diff-output
 * @return
 */
int av_ao_diff_is_enable()
{
    return g_av_configs.ao_diff_out;
}

/**
 * @brief  enable/disable audio diff-output
 * @param  [in] enable
 * @return
 */
void av_ao_diff_enable(int enable)
{
    g_av_configs.ao_diff_out = enable;
}


