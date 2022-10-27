/*
 * Copyright (c) 2020 Allwinner Technology Co., Ltd. ALL rights reserved.
 */

#ifndef __SUNXI_HAL_REGULATOR_PRI_H__
#define __SUNXI_HAL_REGULATOR_PRI_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int16_t			s16;
typedef int64_t		    s64;

typedef uint8_t		    u8;
typedef uint16_t		u16;
typedef uint64_t	    u64;
int hal_axp_byte_read(struct regulator_dev *rdev, u8 reg, u8 *reg_val);
int hal_axp_byte_write(struct regulator_dev *rdev, u8 reg, u8 reg_val);
int hal_axp_byte_update(struct regulator_dev *rdev, u8 reg, u8 val, u8 mask);

#ifdef __cplusplus
}
#endif

#endif
