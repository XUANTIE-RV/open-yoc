/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#ifndef _APP_LPM_H_
#define  _APP_LPM_H_

#include <yoc/lpm.h>

typedef int (*pwr_idle_check_cb_t)(void);

#define LPM_PIN_SRC_NONE    0
#define LPM_PIN_SRC_GPIO    1
#define LPM_PIN_SRC_ADC     2

typedef struct {
    uint8_t type;  /* 1:GPIO 2:ADC 3:PWR */
    uint16_t pin;  /* 引脚号 */
    gpio_irq_mode_e mode;
} lpm_pin_src_t;

typedef struct _wake_src_cfg_ {
    lpm_pin_src_t pin_src;
    pm_policy_t lpm_policy; /* 哪个模式下有效，值同pm_policy_t */
} lpm_wake_src_cfg_t;

/* API */
void app_lpm_init(pwr_idle_check_cb_t idle_check_cb, lpm_event_cb_t lpm_enter_hook);
int app_lpm_add_wake_src(lpm_wake_src_cfg_t cfg);
int app_lpm_set_power_pin(uint16_t pwr_pin);
void app_lpm_sleep(pm_policy_t policy, int power_key_pressed);//进入睡眠模式
int app_lpm_check_pwr_key(int press_sec);
void app_lpm_enter_standby_mode(void);
int app_low_power_init(void);
void app_lpm_vad_check(void);

/**
 *  battery management API
 **/
int app_battery_init(int pin_adc, int pin_pwr, int check_interval_s);
uint32_t app_battery_read(void);
void battery_update_avg(void); /* 更新adc采集平均值 */
void app_charger_check(void);

#endif
