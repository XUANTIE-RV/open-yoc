/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef __DRV_SOUND_SC5654_H__
#define __DRV_SOUND_SC5654_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void sound_drv_sc5654_register(int vol_range, int fix_l_gain, int fix_r_gain);

#ifdef __cplusplus
}
#endif

#endif