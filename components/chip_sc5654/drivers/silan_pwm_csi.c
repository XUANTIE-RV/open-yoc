/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     silan_pwm_csi.c
 * @brief    CSI Source File for PWM Driver
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/

#include <drv/pwm.h>
#include <soc.h>
#include <silan_pwm.h>
#include <silan_printf.h>
#include <silan_syscfg.h>
#include <silan_iomux.h>
#include <silan_iomux_regs.h>

#define ERR_PWM(errno) (CSI_DRV_ERRNO_PWM_BASE | errno)
#define PWM_NULL_PARAM_CHK(para)  HANDLE_PARAM_CHK(para, ERR_PWM(DRV_ERROR_PARAMETER))

typedef struct {
    uint32_t base;
    uint32_t irq;
    uint32_t idx;
} silan_pwm_priv_t;

extern int32_t target_pwm_init(uint32_t idx, uint32_t *base, uint32_t *irq);
extern int32_t csi_pwm_power_control(pwm_handle_t handle, csi_power_stat_e state);

static silan_pwm_priv_t pwm_instance[CONFIG_PWM_NUM];

/**
  \brief       Initialize PWM Interface. 1. Initializes the resources needed for the PWM interface 2.registers event callback function
  \param[in]   idx pwm idx
  \return      handle pwm handle to operate.
*/
pwm_handle_t csi_pwm_initialize(uint32_t idx)
{
    uint32_t base = 0u;
    uint32_t irq = 0u;

    int32_t ret = target_pwm_init(idx, &base, &irq);

    if (ret != idx || ret < 0) {
        return NULL;
    }

    silan_pwm_priv_t *pwm_priv = &pwm_instance[idx];

    pwm_priv->base      = base;
    pwm_priv->irq       = irq;

    silan_pwm_cclk_config(PWM_CCLK_PLLREF_DIV2, CLK_ON);
    silan_pwm_prediv_set(6);

    silan_io_driver_config(IO_CONFIG_PA5, IO_ATTR_DS_8mA);

    return pwm_priv;
}

/**
  \brief       De-initialize PWM Interface. stops operation and releases the software resources used by the interface
  \param[in]   handle pwm handle to operate.
*/
void csi_pwm_uninitialize(pwm_handle_t handle)
{
    silan_pwm_cclk_config(PWM_CCLK_PLLREF_DIV2, CLK_OFF);
}

int32_t csi_pwm_power_control(pwm_handle_t handle, csi_power_stat_e state)
{
    PWM_NULL_PARAM_CHK(handle);

    return ERR_PWM(DRV_ERROR_UNSUPPORTED);
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
    PWM_NULL_PARAM_CHK(handle);

    if (channel > 7) {
        return ERR_PWM(DRV_ERROR_PARAMETER);
    }

    if (pulse_width_us > period_us) {
        return ERR_PWM(DRV_ERROR_PARAMETER);
    }

    silan_pwm_config(channel, pulse_width_us, period_us);

    return 0;
}

/**
  \brief       start generate pwm signal.
  \param[in]   handle pwm handle to operate.
  \param[in]   channel  chnnel num.
  \return      \ref execution_status
*/
void csi_pwm_start(pwm_handle_t handle, uint8_t channel)
{
    silan_pwm_channel_open(channel);
}

/**
  \brief       Stop generate pwm signal.
  \param[in]   handle pwm handle to operate.
  \param[in]   channel  chnnel num.
  \return      \ref execution_status
*/
void  csi_pwm_stop(pwm_handle_t handle, uint8_t channel)
{
    silan_pwm_channel_close(channel);
}

