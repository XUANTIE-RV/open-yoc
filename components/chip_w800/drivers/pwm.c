/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     ck_pwm.c
 * @brief    CSI Source File for PWM Driver
 * @version  V1.0
 * @date     19. Feb 2019
 ******************************************************************************/
#include <sys_freq.h>
#include <drv/irq.h>
#include "drv/pwm.h"
#include "soc.h"

#include "wm_cpu.h"
#include "wm_pwm.h"

#define ERR_PWM(errno) (CSI_DRV_ERRNO_PWM_BASE | errno)

#define PWM_NULL_PARAM_CHK(para)          HANDLE_PARAM_CHK(para, ERR_PWM(DRV_ERROR_PARAMETER))
#define PWM_NULL_PARAM_CHK_NORETVAL(para) HANDLE_PARAM_CHK_NORETVAL(para, ERR_PWM(DRV_ERROR_PARAMETER))

typedef struct {
    uint32_t base;
    uint32_t irq;
    uint32_t idx;
    void *handler;

    uint32_t run;

    uint8_t  duty;

    pwm_event_cb_t pwm_event_cb[CONFIG_PER_PWM_CHANNEL_NUM];
} ck_pwm_priv_t;

extern int32_t target_pwm_init(uint32_t idx, uint32_t *base, uint32_t *irq, void **handler);

static ck_pwm_priv_t pwm_instance[CONFIG_PWM_NUM];

/**
  \brief       Initialize PWM Interface. 1. Initializes the resources needed for the PWM interface 2.registers event callback function
  \param[in]   idx pwm idx
  \return      handle pwm handle to operate.
*/
pwm_handle_t csi_pwm_initialize(uint32_t idx)
{
    uint32_t base = 0u;
    uint32_t irq = 0u;
    void *handler;

    int32_t ret = target_pwm_init(idx, &base, &irq, &handler);

    if (ret != idx || ret < 0) {
        return NULL;
    }

    ck_pwm_priv_t *pwm_priv = &pwm_instance[idx];

    pwm_priv->base      = base;
    pwm_priv->irq       = irq;
    pwm_priv->idx       = idx;
    pwm_priv->handler   = handler;
    pwm_priv->run       = 0;

    pwm_priv->duty      = 0;

    return pwm_priv;
}

/**
  \brief       De-initialize PWM Interface. stops operation and releases the software resources used by the interface
  \param[in]   handle pwm handle to operate.
*/
void csi_pwm_uninitialize(pwm_handle_t handle)
{
    ck_pwm_priv_t *pwm_priv = handle;

    tls_pwm_stop(pwm_priv->idx);
}

int32_t csi_pwm_power_control(pwm_handle_t handle, csi_power_stat_e state)
{
    PWM_NULL_PARAM_CHK(handle);

    return ERR_PWM(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief       config pwm clock division.
  \param[in]   handle   pwm handle to operate.
  \param[in]   channel  channel num.
  \param[in]   div      clock div.
*/
void drv_pwm_config_clockdiv(pwm_handle_t handle, uint8_t channel, uint32_t div)
{
    PWM_NULL_PARAM_CHK_NORETVAL(handle);

    tls_pwm_freq_config(channel, div, 255);
}

/**
  \brief       get pwm clock division.
  \param[in]   handle   pwm handle to operate.
  \param[in]   channel  channel num.
  \return      div      clock div.
*/
uint32_t drv_pwm_get_clockdiv(pwm_handle_t handle, uint8_t channel)
{
    PWM_NULL_PARAM_CHK(handle);

    uint32_t clockdiv;

    clockdiv = tls_pwm_get_clkdiv(channel);

    return clockdiv;
}

/**
  \brief       config pwm mode.
  \param[in]   handle           pwm handle to operate.
  \param[in]   channel          channelnel num.
  \param[in]   period_us        the PWM period in us
  \param[in]   pulse_width_us   the PMW pulse width in us
  \return      error code
*/
int32_t csi_pwm_config(pwm_handle_t handle, uint8_t channel, uint32_t period_us, uint32_t pulse_width_us)
{
    uint8_t duty;
    uint32_t freq;
    uint16_t clkdiv;
    tls_sys_clk sysclk;
    ck_pwm_priv_t *pwm_priv = handle;

    PWM_NULL_PARAM_CHK(handle);

    if (!period_us)
        return ERR_PWM(DRV_ERROR_PARAMETER);

    if (pwm_priv->run & BIT(channel))
        tls_pwm_stop(channel);

    tls_pwm_out_mode_config(channel, WM_PWM_OUT_MODE_INDPT);
    tls_pwm_cnt_type_config(channel, WM_PWM_CNT_TYPE_EDGE_ALIGN_OUT);

    freq = 1000000 / period_us;
    tls_sys_clk_get(&sysclk);
    clkdiv = (sysclk.apbclk * UNIT_MHZ) / (256 * freq);

    tls_pwm_freq_config(channel, clkdiv, 255);

    duty = (pulse_width_us * 100 * 255) / (period_us * 100); /* duty: 0-255, 255/100 */
    if (duty)
    {
        tls_pwm_duty_config(channel, duty);
    }

    pwm_priv->duty = duty;

    tls_pwm_loop_mode_config(channel, WM_PWM_LOOP_TYPE_LOOP);
    tls_pwm_out_inverse_cmd(channel, FALSE);
    tls_pwm_stoptime_by_period_config(channel, 0);
    tls_pwm_stoptime_irq_cmd(channel, FALSE);

    if (channel == 0 || channel == 4)
        tls_pwm_output_en_cmd(channel, WM_PWM_OUT_EN_STATE_OUT);

    if ((pwm_priv->run & BIT(channel)) && duty)
        tls_pwm_start(channel);

    return 0;
}

/**
  \brief       start generate pwm signal.
  \param[in]   handle pwm handle to operate.
  \param[in]   channel  channel num.
  \return      \ref execution_status
*/
void csi_pwm_start(pwm_handle_t handle, uint8_t channel)
{
    ck_pwm_priv_t *pwm_priv = handle;

    PWM_NULL_PARAM_CHK_NORETVAL(handle);

    if (pwm_priv->duty)
        tls_pwm_start(channel);
    pwm_priv->run |= BIT(channel);
}

/**
  \brief       Stop generate pwm signal.
  \param[in]   handle pwm handle to operate.
  \param[in]   channel  channel num.
  \return      \ref execution_status
*/
void  csi_pwm_stop(pwm_handle_t handle, uint8_t channel)
{
    ck_pwm_priv_t *pwm_priv = handle;

    PWM_NULL_PARAM_CHK_NORETVAL(handle);

    tls_pwm_stop(channel);
    pwm_priv->run &= ~BIT(channel);
}

static void csi_pwm_isr_callback(void *arg)
{
    uint8_t channel = (uint8_t)(uint32_t)arg;
    ck_pwm_priv_t *pwm_priv = &pwm_instance[0];
    pwm_event_cb_t cb = pwm_priv->pwm_event_cb[channel];

    if (cb)
    {
        cb(channel, PWM_CAPTURE_EVENT_TIME, 0);
    }
}

/**
  \brief       config pwm clock division.
  \param[in]   handle   pwm handle to operate.
  \param[in]   channel  channel num.
  \param[in]   cb_event event callback.
*/
void drv_pwm_config_cb(pwm_handle_t handle, uint8_t channel, pwm_event_cb_t cb_event)
{
    PWM_NULL_PARAM_CHK_NORETVAL(handle);

    ck_pwm_priv_t *pwm_priv = handle;
    pwm_priv->pwm_event_cb[channel] = cb_event;

    tls_pwm_isr_register(channel, csi_pwm_isr_callback, (void *)(uint32_t)channel);

    drv_irq_register(pwm_priv->irq, pwm_priv->handler);
    drv_irq_enable(pwm_priv->irq);
}

/**
  \brief       config pwm capture mode.
  \param[in]   handle   pwm handle to operate.
  \param[in]   channel  channel num.
  \param[in]   config   capture config.
*/
void drv_pwm_capture_config(pwm_handle_t handle, uint8_t channel, pwm_input_config_t *config)
{
    enum tls_pwm_cap_int_type int_type;
    PWM_NULL_PARAM_CHK_NORETVAL(handle);
    PWM_NULL_PARAM_CHK_NORETVAL(config);

    if (PWM_INPUT_MODE_EDGE_TIME != config->input_mode)
        return;// ERR_PWM(DRV_ERROR_UNSUPPORTED);

    if (PWM_INPUT_EVENT_MODE_POSEDGE == config->event_mode)
        int_type = WM_PWM_CAP_RISING_EDGE_INT;
    else if (PWM_INPUT_EVENT_MODE_NEGEDGE == config->event_mode)
        int_type = WM_PWM_CAP_FALLING_EDGE_INT;
    else
        int_type = WM_PWM_CAP_RISING_FALLING_EDGE_INT;

    tls_pwm_cap_init(channel, drv_get_apb_freq(0) / 256 / 65535, DISABLE, int_type);
}

/**
  \brief       start pwm capture.
  \param[in]   handle   pwm handle to operate.
  \param[in]   channel  channel num.
*/
void drv_pwm_capture_start(pwm_handle_t handle, uint8_t channel)
{
    PWM_NULL_PARAM_CHK_NORETVAL(handle);

    tls_pwm_start(channel);
}

/**
  \brief       stop pwm capture.
  \param[in]   handle   pwm handle to operate.
  \param[in]   channel  channel num.
*/
void drv_pwm_capture_stop(pwm_handle_t handle, uint8_t channel)
{
    PWM_NULL_PARAM_CHK_NORETVAL(handle);

    tls_pwm_stop(channel);
}

/**
  \brief       set pwm timeout.
  \param[in]   handle pwm handle to operate.
  \param[in]   channel  channel num.
  \param[in]   timeout the timeout value in microseconds(us).
*/
void drv_pwm_timer_set_timeout(pwm_handle_t handle, uint8_t channel, uint32_t timeout)
{
    PWM_NULL_PARAM_CHK_NORETVAL(handle);
}

/**
  \brief       start pwm timer.
  \param[in]   handle pwm handle to operate.
  \param[in]   channel  chnnel num.
*/
void drv_pwm_timer_start(pwm_handle_t handle, uint8_t channel)
{
    PWM_NULL_PARAM_CHK_NORETVAL(handle);
}

/**
  \brief       stop pwm timer.
  \param[in]   handle pwm handle to operate.
  \param[in]   channel  chnnel num.
*/
void drv_pwm_timer_stop(pwm_handle_t handle, uint8_t channel)
{
    PWM_NULL_PARAM_CHK_NORETVAL(handle);
}

/**
  \brief       get pwm timer current value
  \param[in]   handle pwm handle to operate.
  \param[in]   channel   channel num.
  \param[out]  value     timer current value
*/
void drv_pwm_timer_get_current_value(pwm_handle_t handle, uint8_t channel, uint32_t *value)
{
    PWM_NULL_PARAM_CHK_NORETVAL(handle);
    PWM_NULL_PARAM_CHK_NORETVAL(value);
}

/**
  \brief       get pwm timer reload value
  \param[in]   handle pwm handle to operate.
  \param[in]   channel   channel num.
  \param[out]  value    timer reload value
*/
void drv_pwm_timer_get_load_value(pwm_handle_t handle, uint8_t channel, uint32_t *value)
{
    PWM_NULL_PARAM_CHK_NORETVAL(handle);
    PWM_NULL_PARAM_CHK_NORETVAL(value);
}
