/*
 * Copyright (C) 2018-2022 Alibaba Group Holding Limited
 */

#ifndef __AV_INNER_CONFIG_H__
#define __AV_INNER_CONFIG_H__

#include "av/avutil/common.h"

__BEGIN_DECLS__

/**
 * @brief  get number of audio output
 * @return
 */
int av_get_ao_channel_num();

/**
 * @brief  set number of audio output
 * @param  [in] num
 * @return
 */
void av_set_ao_channel_num(int num);

/**
 * @brief  get whether audio diff-output
 * @return
 */
int av_ao_diff_is_enable();

/**
 * @brief  enable/disable audio diff-output
 * @param  [in] enable
 * @return
 */
void av_ao_diff_enable(int enable);

__END_DECLS__

#endif /* __AV_INNER_CONFIG_H__ */

