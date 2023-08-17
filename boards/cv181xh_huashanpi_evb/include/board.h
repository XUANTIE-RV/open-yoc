/*
 * Copyright (C) 2018-2022 Alibaba Group Holding Limited
 */

#ifndef __BOARD_H__
#define __BOARD_H__

#include <stdint.h>
#include <soc.h>
#include <board_config.h>

#ifdef __cplusplus
extern "C" {
#endif

// 板级默认配置, 若需全局应用可重新定义到board组件的package.yaml中
/****************************************************************************/
#ifndef CONFIG_BOARD_UART
#define CONFIG_BOARD_UART 1
#endif

#ifndef CONFIG_BOARD_GPIO
#define CONFIG_BOARD_GPIO 1
#endif

#ifndef CONFIG_BOARD_DISPLAY
#define CONFIG_BOARD_DISPLAY 1
#endif

#ifndef CONFIG_BOARD_VENDOR
#define CONFIG_BOARD_VENDOR 1
#endif

// 板级可重定义项,解决应用包含功能但部分板子不支持的特殊情况
// 应用的package.yaml可以根据应用的需求配置CONFIG_BOARD_XXX宏
// 但是否生效由CONFIG_BOARD宏来确认板子实际支持情况
// 应用中使用BOARD_XXX宏来开启和关闭功能代码,避免不支持导致的编译错误
// 外设相关的引脚定义见board_config.h
#if defined(CONFIG_BOARD_BT) && CONFIG_BOARD_BT > 0
// BT
/* the board support bluetooth */
#ifndef BOARD_BT_SUPPORT
#define BOARD_BT_SUPPORT 1
#endif
#endif // defined(CONFIG_BOARD_BT) && CONFIG_BOARD_BT > 0
/****************************************************************************/
#if defined(CONFIG_BOARD_WIFI) && CONFIG_BOARD_WIFI > 0
// WIFI
/* the board support wifi */
#ifndef BOARD_WIFI_SUPPORT
#define BOARD_WIFI_SUPPORT 1
#endif
#endif // defined(CONFIG_BOARD_WIFI) && CONFIG_BOARD_WIFI > 0
/****************************************************************************/
#if defined(CONFIG_BOARD_ETH) && CONFIG_BOARD_ETH > 0
// eth
/* the board support eth */
#ifndef BOARD_ETH_SUPPORT
#define BOARD_ETH_SUPPORT 1
#endif
#endif // defined(CONFIG_BOARD_ETH) && CONFIG_BOARD_ETH > 0
/****************************************************************************/
#if defined(CONFIG_BOARD_AUDIO) && CONFIG_BOARD_AUDIO > 0
// Audio
/* the board support audio */
#ifndef BOARD_AUDIO_SUPPORT
#define BOARD_AUDIO_SUPPORT 1
#endif
#endif // defined(CONFIG_BOARD_AUDIO) && CONFIG_BOARD_AUDIO > 0
/****************************************************************************/
#if defined(CONFIG_BOARD_PWM) && CONFIG_BOARD_PWM
#ifndef BOARD_PWM_NUM
#define BOARD_PWM_NUM 0
#endif
#endif
/****************************************************************************/
#if defined(CONFIG_BOARD_ADC) && CONFIG_BOARD_ADC
#ifndef BOARD_ADC_NUM
#define BOARD_ADC_NUM 0
#endif
#endif
/****************************************************************************/
#if defined(CONFIG_BOARD_BUTTON) && CONFIG_BOARD_BUTTON
#ifndef BOARD_BUTTON_NUM
#define BOARD_BUTTON_NUM 0
#endif
#endif
/****************************************************************************/
#if defined(CONFIG_BOARD_LED) && CONFIG_BOARD_LED
#ifndef BOARD_LED_NUM
#define BOARD_LED_NUM 0
#endif
#endif

// 板级应用接口
/**
 * @brief  init the board for default: pin mux, etc.
 * re-implement if need.
 * @return
 */
void board_init(void);

/**
 * @brief  init the board uart for default: pin mux, etc.
 * re-implement if need.
 * @return
 */
void board_uart_init(void);

/**
 * @brief  init the board bt for default: pin mux, etc.
 * re-implement if need.
 * @return
 */
void board_bt_init(void);

/**
 * @brief  init the board audio for default: pin mux, etc.
 * re-implement if need.
 * @return
 */
void board_audio_init(void);

/**
 * @brief  init the board flash for default: pin mux, etc.
 * re-implement if need.
 * @return
 */
void board_flash_init(void);

/**
 * @brief  init the board wifi for default: pin mux, etc.
 * re-implement if need.
 * @return
 */
void board_wifi_init(void);

/**
 * @brief  init the board wifi for default: pin mux, etc.
 * re-implement if need.
 * @return
 */
void board_eth_init(void);


/**
 * @brief init board cli cmd
 * re-implement if need.
 * @return
 */
void board_vendor_init(void);

/****************************************************************************/
//board extend api

#ifdef CONFIG_BOARD_AUDIO
#include "audio/board_audio.h"
#endif

#ifdef CONFIG_BOARD_DISPLAY
#include "display/board_display.h"
#endif

#ifdef __cplusplus
}
#endif

#endif /* __BOARD_H__ */

