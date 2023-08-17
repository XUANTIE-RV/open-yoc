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

/* UART */
#define CONSOLE_UART_IDX                (0)
#define CONFIG_CLI_USART_BAUD           (115200)
#define CONFIG_CONSOLE_UART_BUFSIZE     (512)

/* WIFI */
//PINMUX_CONFIG(AUX0, XGPIOA_30)
#define RTL8723DS_DEV_WIFI_EN_GRP 0
#define RTL8723DS_DEV_WIFI_EN_CHN 30
#ifndef CONFIG_SDIO_SDIF
#define CONFIG_SDIO_SDIF 0
#endif
//Disabled ctrl in WiFi driver
#define WLAN_ENABLE_PIN 0xFFFFFFFF
#define WLAN_POWER_PIN  0xFFFFFFFF

/* BT */
#define RTL8723DS_DEV_BT_UART_ID 2
#define BT_DIS_PIN_GRP 1
#define BT_DIS_PIN_ID  22

//Disabled ctrl in BT driver
#define BT_DISABLE_PIN -1

/* LED */
#define LED_PIN   -1

/* SPK AMP */
//PINMUX_CONFIG(SPK_EN, XGPIOA_15)
#define GPIO_SPKEN_GRP 0
#define GPIO_SPKEN_NUM 15

/* PANEL */
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

/* PANEL LCD */
//PINMUX_CONFIG(PWR_GPIO2, PWR_GPIO_2)
//custom_voparam.c 传递 CVI_GPIOE_02 引脚号
//#define LCD_RST_PIN_GRP xx
//#define LCD_RST_PIN_CHN xx

//PINMUX_CONFIG(PWR_GPIO0, PWM_8)
//custom_voparam.c 传递 CVI_GPIOE_00 引脚号
#define LCD_PWM_PIN_GRP 2 //4chn per grp
#define LCD_PWM_PIN_CHN 0

/* PANEL Touch */
#define TOUCH_I2C_ID 3

//PINMUX_CONFIG(JTAG_CPU_TCK, XGPIOA_18)
#define TOUCH_RST_PIN_GRP 0
#define TOUCH_RST_PIN_CHN 18

//PINMUX_CONFIG(JTAG_CPU_TMS, XGPIOA_19)
#define TOUCH_INTR_PIN_GRP 0
#define TOUCH_INTR_PIN_CHN 19

/* SENSOR */
#define SENSOR_RGB0_WIDTH 1920
#define SENSOR_RGB0_HEIGHT 1080

#ifdef __cplusplus
}
#endif

#endif /* _BOARD_CONFIG_H_ */

