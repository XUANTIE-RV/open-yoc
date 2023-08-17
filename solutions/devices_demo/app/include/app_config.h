/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef __APP_CONFIG_H__
#define __APP_CONFIG_H__


#ifdef CONFIG_CHIP_CH2601

#endif

#ifdef CONFIG_CHIP_D1

#endif

#ifdef CONFIG_CHIP_BL606P_E907
#include <bl606p_gpio.h>
#include <bl606p_glb.h>
#include <bl606p_spi.h>
#endif

#ifdef CVI_SOC_CV181XC
#define CONFIG_HAL_SPI_DISABLED
#endif




#endif