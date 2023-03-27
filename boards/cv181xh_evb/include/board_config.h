/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     board_config.h
 * @brief    header File for pin definition
 * @version  V1.0
 * @date     02. June 2020
 ******************************************************************************/

#ifndef _BOARD_CONFIG_H_
#define _BOARD_CONFIG_H_

#include <stdint.h>
#include <soc.h>

#ifdef __cplusplus
extern "C" {
#endif

#define WLAN_ENABLE_PIN     PG12
#define WLAN_POWER_PIN      0xFFFFFFFF

#define LED_PIN             PC1     // LED RGB

//PANEL
#if CONFIG_PANEL_ILI9488
#define PANEL_WIDTH 320
#define PANEL_HEIGHT 480
#elif CONFIG_PANEL_HX8394
#define PANEL_WIDTH 720
#define PANEL_HEIGHT 1280
#elif CONFIG_PANEL_ST7701S
#define PANEL_WIDTH 480
#define PANEL_HEIGHT 480
#else
#define PANEL_WIDTH 720
#define PANEL_HEIGHT 1280
#endif

#define SENSOR_RGB0_WIDTH 1600
#define SENSOR_RGB0_HEIGHT 1200
#define SENSOR_IR0_WIDTH 1600
#define SENSOR_IR0_HEIGHT 1200
#define SENSOR_IR1_WIDTH 1600
#define SENSOR_IR1_HEIGHT 1200

#define GPIO_SPKEN_GRP 0
#define GPIO_SPKEN_NUM 15

#ifdef __cplusplus
}
#endif

#endif /* _BOARD_CONFIG_H_ */

