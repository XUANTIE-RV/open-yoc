/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     add2010_drv.h
 * @brief    header file for add2010 driver
 * @version  V1.0
 * @date     31. Oct. 2019
 ******************************************************************************/

#ifndef __ADD2010_DRV_H__
#define __ADD2010_DRV_H__

#include <aos/aos.h>
#include "add2010_config.h"

int add2010_init(aos_dev_t *i2c_dev);
int add2010_shutdown(aos_dev_t *i2c_dev);
int add2010_set_eq_param(aos_dev_t *i2c_dev, uint8_t params[180]);
int add2010_write_reg_table(aos_dev_t *i2c_dev, add2010_reg_table_t *reg_address, int reg_num);
int add2010_write_ram_table(aos_dev_t *i2c_dev, int channel, add2010_ram_table_t *ram_address, int data_size);
int add2010_set_vol(aos_dev_t *i2c_dev, int l_gain, int r_gain);

#endif
