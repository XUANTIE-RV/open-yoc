/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef __DRV_SOUND_ADD2010_H__
#define __DRV_SOUND_ADD2010_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void sound_drv_add2010_register(int vol_range, int fix_l_gain, int fix_r_gain);
extern int add2010_init(aos_dev_t *i2c_dev);

#ifdef __cplusplus
}
#endif

#endif