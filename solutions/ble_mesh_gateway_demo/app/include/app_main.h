/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */
#ifndef _APP_MAIN_H_
#define _APP_MAIN_H_

#include <app_config.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <aos/aos.h>
#include <aos/cli.h>
#include <aos/kv.h>

#include <drv/gpio.h>

#include <yoc/yoc.h>
#include <uservice/eventid.h>
#include <uservice/uservice.h>

/* wifi provisioning method */
#define WIFI_PROVISION_MIN            0
#define WIFI_PROVISION_SL_SMARTCONFIG 0
#define WIFI_PROVISION_SOFTAP         1
#define WIFI_PROVISION_SL_BLE         2
#define WIFI_PROVISION_SL_DEV_AP      3
#define WIFI_PROVISION_MAX            4

/* event id define */
#define EVENT_NTP_RETRY_TIMER   (EVENT_USER + 1)
#define EVENT_NET_CHECK_TIMER   (EVENT_USER + 2)
#define EVENT_NET_NTP_SUCCESS   (EVENT_USER + 3)
#define EVENT_NET_LPM_RECONNECT (EVENT_USER + 4)

#define EVENT_APP_INIT_STATE (EVENT_USER + 5)

/* app at cmd */
#define EVENT_APP_AT_CMD (EVENT_USER + 26)

#define PWM_LED_LIGHT_MIN 10
#define PWM_LED_LIGHT_MAX 100

#define SUBDEV_GW_SELF 0

/* YMODEM */
#define YMODEM_UART_IDX 4

/* wifi & net */
typedef enum
{
    MODE_WIFI_TEST    = -2,
    MODE_WIFI_CLOSE   = -1,
    MODE_WIFI_NORMAL  = 0,
    MODE_WIFI_PAIRING = 1
} wifi_mode_e;

typedef enum
{
    APP_INIT_RESOURCE = 0,
    APP_INIT_GATEWAY,
    APP_INIT_MAX
} app_init_state_e;

typedef enum
{
    COMBO_BLE_SET = 0,
    COMBO_BLE_GET,
    COMBO_BLE_MAX
} combo_ble_operation_e;

wifi_mode_e app_network_init(void);

void        app_wifi_pair_start(void);
wifi_mode_e app_network_reinit();

/**
 * report property of the key number pressed for demo
 *
 * @param[in]  key_num       Key Number
 */
void user_post_key_press_event(int key_num);

void app_button_init(void);

void app_at_server_init(utask_t *task, const char *device_name);
int  app_at_cmd_init(void);

/* led */
void app_set_led_state(int state);
void app_set_led_enabled(int en);
void app_status_init(void);
void app_status_update(void);

/********************* PWM LED ****************************/
/**
 * PWM LED Initialize
 * @return 0 on success
 */
int8_t app_pwm_led_init();

/**
 * PWM LED Control
 * @param [in] flag      : on(1)/off(0)
 * @return 0 on success
 */
int8_t app_pwm_led_control(const uint8_t flag);

/**
 * PWM LED Set Brightness
 * @param [in] brightness : 0 ~ 100
 * @return 0 on success
 */
int8_t app_pwm_led_set_brightness(int32_t brightness);

/**
 * PWM LED Get Brightness
 * @return 0 ~ 100 for current brightness
 */
int8_t app_pwm_led_get_brightness();

/**
 * PWM LED Get Status
 * @return on(1)/off(0)
 */
int8_t app_pwm_led_get_status();

void cli_reg_cmd_w800(void);

void cli_reg_cmd_kp(void);

void combo_net_post_ext(void);

void app_rpt_fwver(char *version);
void app_wifi_network_deinit();
int  app_wifi_network_inited();
void app_rpt_fwver(char *version);

#endif
