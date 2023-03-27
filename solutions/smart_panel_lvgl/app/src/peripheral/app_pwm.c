/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <aos/hal/pwm.h>

#include "app_main.h"

#define TAG "app"

#if defined(CONFIG_BOARD_PWM) && CONFIG_BOARD_PWM > 0

#define DEF_PWM_FREQ 10
#define DEF_PWM_DUTY 0

enum {
    PWM_CH_UNUSED   = 0,
    PWM_CH_NO_SPARE = 1,
    PWM_CH_USED     = 2,
};

typedef struct {
    pwm_dev_t dev;
    int8_t    flag;
} app_pwm_info_t;

static app_pwm_info_t app_pwm_info[BOARD_PWM_NUM] = {0x00};

static int8_t get_free_info_id(uint8_t ch)
{

    int8_t idx = PWM_CH_NO_SPARE;

    for (uint8_t i = 0; i < BOARD_PWM_NUM; i++) {
        if (app_pwm_info[i].flag == PWM_CH_USED && ch == app_pwm_info[i].dev.port) {
            LOGD(TAG, "ch %d, i %d", ch, i);
            return PWM_CH_USED;
        }

        if (app_pwm_info[i].flag == PWM_CH_UNUSED) {
            idx = i;
        }
    }

    return idx;
}

static pwm_dev_t *ch_to_handle(uint8_t ch)
{
    for (uint8_t i = 0; i < BOARD_PWM_NUM; i++) {
        if (app_pwm_info[i].flag == PWM_CH_USED && ch == app_pwm_info[i].dev.port) {
            return &app_pwm_info[i].dev;
        }
    }

    return NULL;
}

int8_t app_pwm_init(uint8_t ch)
{
    int    err    = 0;
    int8_t pwm_id = get_free_info_id(ch);

    if (PWM_CH_NO_SPARE == pwm_id) {
        LOGE(TAG, "No free pwm handle! Only %d handles suppoerted", BOARD_PWM_NUM);
        return -1;
    } else if (PWM_CH_USED == pwm_id) {
        /* channel has already been initialized, return directly */
        return 0;
    }

    app_pwm_info[pwm_id].dev.port              = ch;
    app_pwm_info[pwm_id].dev.config.freq       = DEF_PWM_FREQ;
    app_pwm_info[pwm_id].dev.config.duty_cycle = DEF_PWM_DUTY;

    err = hal_pwm_init(&app_pwm_info[pwm_id].dev);
    if (err) {
        LOGE(TAG, "Pwm ch init failed %d", err);
        return -1;
    }
    app_pwm_info[pwm_id].flag = PWM_CH_USED;
    return 0;
}

int8_t app_pwm_config(uint8_t ch, uint32_t freq, uint32_t duty)
{
    if (duty > 100) {
        LOGE(TAG, "duty [%d] should be in range 0 ~ 100", duty);
        return -1;
    }
    /* find a valid channel */
    pwm_dev_t *handle = ch_to_handle(ch);
    if (NULL == handle) {
        LOGE(TAG, "Cannot find a matched handle! ch %d", ch);
        return -1;
    }

    pwm_config_t para = {
        .duty_cycle = duty,
        .freq       = freq,
    };

    return hal_pwm_para_chg(handle, para);
}

int8_t app_pwm_start(uint8_t ch)
{
    pwm_dev_t *handle = ch_to_handle(ch);
    if (NULL == handle) {
        LOGE(TAG, "Cannot find a matched handle! ch %d", ch);
        return -1;
    }

    return hal_pwm_start(handle);
}

int8_t app_pwm_stop(uint8_t ch)
{
    pwm_dev_t *handle = ch_to_handle(ch);
    if (NULL == handle) {
        LOGE(TAG, "Cannot find a matched handle! ch %d", ch);
        return -1;
    }
    return hal_pwm_stop(handle);
}

#endif
