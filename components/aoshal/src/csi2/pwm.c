/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef CONFIG_HAL_PWM_DISABLED
#include "soc.h"
#include "aos/hal/pwm.h"
#include "drv/pwm.h"
#include <drv/pin.h>
#include <ulog/ulog.h>

#define TAG "PWM"

static csi_pwm_t pwm_handlers;

int32_t hal_pwm_init(pwm_dev_t *pwm)
{
    int ret = 0;
    uint32_t period_us, pulse_width;

    if (pwm == NULL) {
        return -1;
    }

    ret = csi_pwm_init(&pwm_handlers, (pwm->port >> 6));

    if (ret != CSI_OK) {
        LOGE(TAG, "csi_pwm_init error");
        return -1;
    }

    period_us = 1000000 / pwm->config.freq;
    pulse_width = period_us * pwm->config.duty_cycle;
    ret = csi_pwm_out_config(&pwm_handlers, pwm->port & PWM_PORT_MASK, period_us, pulse_width, PWM_POLARITY_HIGH);

    if (ret != CSI_OK) {
        LOGE(TAG, "csi_pwm_out_config error");
        return -1;
    }

    return 0;
}

/**
 * Starts Pulse-Width Modulation signal output on a PWM pin
 *
 * @param[in]  pwm  the PWM device
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_pwm_start(pwm_dev_t *pwm)
{
    csi_error_t ret = CSI_OK;
    if (pwm == NULL) {
        return -1;
    }

    ret = csi_pwm_out_start(&pwm_handlers, pwm->port & PWM_PORT_MASK);

    if (ret != CSI_OK) {
        LOGE(TAG, "csi_pwm_out_start error");
        return -1;
    }

    return 0;
}

/**
 * Stops output on a PWM pin
 *
 * @param[in]  pwm  the PWM device
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_pwm_stop(pwm_dev_t *pwm)
{
    if (pwm == NULL) {
        return -1;
    }

    csi_pwm_out_stop(&pwm_handlers, pwm->port & PWM_PORT_MASK);

    return 0;
}

/**
 * change the para of pwm
 *
 * @param[in]  pwm  the PWM device
 * @param[in]  para the para of pwm
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_pwm_para_chg(pwm_dev_t *pwm, pwm_config_t para)
{
    uint32_t period_us, pulse_width;

    if (pwm == NULL) {
        return -1;
    }

    period_us = 1000000 / para.freq;
    pulse_width = period_us * para.duty_cycle;

    return csi_pwm_out_config(&pwm_handlers, pwm->port & PWM_PORT_MASK, period_us, pulse_width, PWM_POLARITY_HIGH);
}

/**
 * De-initialises an PWM interface, Turns off an PWM hardware interface
 *
 * @param[in]  pwm  the interface which should be de-initialised
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_pwm_finalize(pwm_dev_t *pwm)
{
    if (pwm == NULL) {
        return -1;
    }

    csi_pwm_uninit(&pwm_handlers);

    return 0;
}
#endif
