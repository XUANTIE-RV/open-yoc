#ifndef _CVI_SENSOR_I2C_H_
#define _CVI_SENSOR_I2C_H_

#include <drv/iic.h>
#include "cvi_type.h"

#define IIC_MAX 5

int sensor_i2c_init(CVI_U8 i2c_id);

int sensor_i2c_exit(CVI_U8 i2c_id);

int sensor_i2c_read(CVI_U8 i2c_id, CVI_U8 snsr_i2c_addr,
			CVI_U32 addr,CVI_U32 snsr_addr_byte,
			CVI_U32 snsr_data_byte);

int sensor_i2c_write(CVI_U8 i2c_id, CVI_U8 snsr_i2c_addr,
			CVI_U32 addr, CVI_U32 snsr_addr_byte,
			CVI_U32 data, CVI_U32 snsr_data_byte);

#endif
