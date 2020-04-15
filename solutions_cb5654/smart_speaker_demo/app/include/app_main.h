/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#ifndef _APP_MAIN_H_
#define _APP_MAIN_H_

#include <hw_config.h>
#include <app_config.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <aos/aos.h>
#include <aos/cli.h>
#include <aos/cli_cmd.h>
#include <vfs.h>

#include <pin_name.h>
#include <drv/adc.h>
#include <drv/gpio.h>
#include <pinmux.h>
#include <aos/hal/adc.h>
#include <devices/devicelist.h>
#include <devices/led_rgb.h>

#include <yoc/yoc.h>
#include <yoc/eventid.h>
#include <yoc/uservice.h>

#include <clock_alarm.h>
#include <rtc_alarm.h>

#include "audio_res.h"
#include "app_player.h"
#include "app_aui_cloud.h"
#include "app_i2c_led.h"

extern i2c_dev_t g_i2c_dev;
extern int g_fct_mode;

extern void mdelay(uint32_t ms);

/* app init */
void yoc_base_init(void);
int app_softvad_init(void);

/* mic */
int app_mic_is_busy(void);

/* gpio & adc */
int app_adc_value_read(int adc_ch_id, int *vol); /*sample value 0~4095*/
int app_gpio_read(int pin, int *val);
int app_gpio_write(int pin, int val);

void app_button_init(void);


/* led & status */
#define LED_TURN_OFF    0
#define LED_LOADING     1
#define LED_NET_DOWN    2
#define LED_NET_READY   3
#define LED_PLAYING     4
#define LED_NET_CONFIG  5
#define LED_TALKING     6

void app_set_led_state(int state);
void app_set_led_enabled(int en);
void app_status_init(void);
void app_status_update(void);

/* sys event */
void sys_event_init(void);
void sys_event_handle(uint32_t event_id);

/* wifi & net */
typedef enum {
    MODE_WIFI_TEST = -2,
    MODE_WIFI_CLOSE = -1,
    MODE_WIFI_NORMAL = 0,
    MODE_WIFI_PAIRING = 1
}wifi_mode_e;
wifi_mode_e app_network_init(void);
int wifi_connecting();
void wifi_lpm_enable(int lpm_en);
void wifi_set_lpm(int lpm_on);
void wifi_pair_start(void);
int wifi_internet_is_connected();
int wifi_is_pairing();
int wifi_getmac(uint8_t mac[6]);

/* other */
void app_audio_pa_ctrl(int enabled);
void app_audio_pa_auto(void);
int  app_subboard_ldo_enable(int enabled);

/* fota */
void app_fota_init(void);
void app_fota_start(void);
int app_fota_is_downloading(void);
void app_fota_do_check(void);
void app_fota_set_auto_check(int enable);

/* factory test */
void fct_case_init(void);

/* wifi provisioning method */
#define WIFI_PROVISION_SOFTAP           1
#define WIFI_PROVISION_SL_SMARTCONFIG   2
#define WIFI_PROVISION_SL_DEV_AP        3

/* event id define */
#define EVENT_NTP_RETRY_TIMER       (EVENT_USER + 1)
#define EVENT_NET_CHECK_TIMER       (EVENT_USER + 2)
#define EVENT_NET_NTP_SUCCESS       (EVENT_USER + 3)
#define EVENT_NET_LPM_RECONNECT     (EVENT_USER + 4)

/*app_status_update 函数发送该消息，用于设备状态变化后更新LED灯 */
#define EVENT_UPDATE_STATUS (EVENT_USER + 10)

#define EVENT_LPM_CHECK     (EVENT_USER + 11)
#define EVENT_BATTERY_CHECK (EVENT_USER + 12)

/* media event */
#define EVENT_MEDIA_START           (EVENT_USER + 21)
#define EVENT_MEDIA_MUSIC_FINISH    (EVENT_USER + 22)
#define EVENT_MEDIA_SYSTEM_FINISH   (EVENT_USER + 23)
#define EVENT_MEDIA_MUSIC_ERROR     (EVENT_USER + 24)
#define EVENT_MEDIA_SYSTEM_ERROR    (EVENT_USER + 25)

/* 命令行测试 */
void cli_reg_cmd_aui(void);
void cli_reg_cmd_yvdbg(void);
void cli_reg_cmd_app(void);
void cli_reg_cmd_gpio(void);
void cli_reg_cmd_tftp(void);

#endif
