/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */
#ifndef _APP_MAIN_H_
#define _APP_MAIN_H_

#include <app_config.h>
#include <uservice/eventid.h>
#include <uservice/uservice.h>
#include <drv/gpio.h>

/*************
 * USER EVENT LIST
 ************/
/* wifi provisioning method */
#define WIFI_PROVISION_MIN            0
#define WIFI_PROVISION_SL_SMARTCONFIG 0
#define WIFI_PROVISION_SOFTAP         1
#define WIFI_PROVISION_SL_BLE         2
#define WIFI_PROVISION_SL_DEV_AP      3
#define WIFI_PROVISION_MAX            4
/* 网络处理 */
#define EVENT_NTP_RETRY_TIMER            (EVENT_USER + 1)
#define EVENT_NET_CHECK_TIMER            (EVENT_USER + 2)
#define EVENT_NET_LPM_RECONNECT          (EVENT_USER + 4)
#define EVENT_NET_RECONNECT              (EVENT_USER + 5)

#define EVENT_APP_INIT_STATE             (EVENT_USER + 5)

/* 其他事件 */
#define EVENT_PA_CHECK       (EVENT_USER + 14)

#define PWM_LED_LIGHT_MIN 10
#define PWM_LED_LIGHT_MAX 100

#define SUBDEV_GW_SELF 0

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

void board_yoc_init(void);
void app_cli_init(void);

#endif