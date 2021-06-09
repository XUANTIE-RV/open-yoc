/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     silan_timer_csi.c
 * @brief    CSI Source File for timer Driver
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/
#include <drv/timer.h>
#include <soc.h>
#include <silan_syscfg.h>
#include <silan_irq.h>
#include <silan_timer.h>
#include <silan_printf.h>
#include <silan_pmu.h>


#define ERR_TIMER(errno) (CSI_DRV_ERRNO_TIMER_BASE | errno)
#define TIMER_NULL_PARAM_CHK(para)  HANDLE_PARAM_CHK(para, ERR_TIMER(DRV_ERROR_PARAMETER))

typedef struct {
    int32_t idx;
    uint32_t base;
    uint32_t irq;
    timer_event_cb_t cb_event;
    uint32_t timeout;                  ///< the set time (us)
    uint32_t load;
    uint32_t timeout_flag;
    timer_mode_e mode;
} silan_timer_priv_t;

static silan_timer_priv_t timer_instance[CONFIG_TIMER_NUM];

extern int32_t target_get_timer(int32_t idx, uint32_t *base, uint32_t *irq);

void silan_timer_irqhandler(int idx)
{
    silan_timer_priv_t *timer_priv = &timer_instance[idx];
    timer_priv->timeout_flag = 1;

    silan_timer_to_clear(TIMER_ADDR(idx));

    if (timer_priv->mode == TIMER_MODE_RELOAD) {
        silan_timer_count(TIMER_ADDR(idx), 0);
        silan_timer_compare(TIMER_ADDR(idx), timer_priv->load);
        silan_timer_irq_reset(TIMER_ADDR(idx));
    }

    if (timer_priv->cb_event) {
        return timer_priv->cb_event(idx, TIMER_EVENT_TIMEOUT);
    }
}

/**
  \brief       Initialize TIMER Interface. 1. Initializes the resources needed for the TIMER interface 2.registers event callback function
  \param[in]   idx  instance timer index
  \param[in]   cb_event  Pointer to \ref timer_event_cb_t
  \return      pointer to timer instance
*/
timer_handle_t csi_timer_initialize(int32_t idx, timer_event_cb_t cb_event)
{
    if (idx < 0 || idx >= CONFIG_TIMER_NUM) {
        return NULL;
    }

    uint32_t base = 0u;
    uint32_t irq = 0u;

    int32_t real_idx = target_get_timer(idx, &base, &irq);

    if (real_idx != idx) {
        return NULL;
    }

    silan_timer_priv_t *timer_priv = &timer_instance[idx];
    timer_priv->idx  = idx;
    timer_priv->base = base;
    timer_priv->irq  = irq;

    silan_timer_disable(TIMER_ADDR(idx));
    timer_priv->timeout = SILAN_TIMER_INIT_DEFAULT_VALUE;
    timer_priv->cb_event = cb_event;

    silan_timer_cclk_config(ADC_CCLK_PLLREF_DIV2, CLK_ON);

    silan_timer_count(TIMER_ADDR(idx), 0);

    silan_pic_request(PIC_IRQID_TIMER, idx, (hdl_t)silan_timer_irqhandler);

    silan_timer_irq_enable(TIMER_ADDR(idx));

    return (timer_handle_t)timer_priv;
}

/**
  \brief       De-initialize TIMER Interface. stops operation and releases the software resources used by the interface
  \param[in]   handle timer handle to operate.
  \return      error code
*/
int32_t csi_timer_uninitialize(timer_handle_t handle)
{
    TIMER_NULL_PARAM_CHK(handle);

    silan_timer_priv_t *timer_priv = (silan_timer_priv_t *)handle;

    silan_timer_disable(TIMER_ADDR(timer_priv->idx));
    timer_priv->cb_event = NULL;

    /* silan_pic_free(PIC_IRQID_TIMER, timer_priv->irq); */
    return 0;
}

int32_t csi_timer_power_control(timer_handle_t handle, csi_power_stat_e state)
{
    TIMER_NULL_PARAM_CHK(handle);
    return ERR_TIMER(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief       config timer mode.
  \param[in]   handle timer handle to operate.
  \param[in]   mode      \ref timer_mode_e
  \return      error code
*/
int32_t csi_timer_config(timer_handle_t handle, timer_mode_e mode)
{
    TIMER_NULL_PARAM_CHK(handle);

    silan_timer_priv_t *timer_priv = handle;

    switch (mode) {
        case TIMER_MODE_FREE_RUNNING:
            timer_priv->mode = TIMER_MODE_FREE_RUNNING;
            break;

        case TIMER_MODE_RELOAD:
            timer_priv->mode = TIMER_MODE_RELOAD;
            break;

        default:
            return ERR_TIMER(DRV_ERROR_PARAMETER);
    }

    return 0;
}

/**
  \brief       Set timer.
  \param[in]   instance  timer instance to operate.
  \param[in]   timeout the timeout value in microseconds(us).
  \return      error code
*/
int32_t csi_timer_set_timeout(timer_handle_t handle, uint32_t timeout)
{
    TIMER_NULL_PARAM_CHK(handle);

    if (timeout == 0) {
        return ERR_TIMER(DRV_ERROR_PARAMETER);
    }

    silan_timer_priv_t *timer_priv = handle;
    timer_priv->timeout = timeout;
    return 0;
}

/**
  \brief       Start timer.
  \param[in]   handle timer handle to operate.
  \return      error code
*/
int32_t csi_timer_start(timer_handle_t handle)
{
    TIMER_NULL_PARAM_CHK(handle);
    silan_timer_priv_t *timer_priv = handle;

    timer_priv->timeout_flag = 0;

    uint32_t min_us = silan_get_timer_cclk() / 1000000;
    uint32_t load;

    if (timer_priv->timeout > 0xffffffff / min_us) {
        return ERR_TIMER(DRV_ERROR_PARAMETER);
    }

    if (min_us) {
        load = (uint32_t)(timer_priv->timeout * min_us);
    } else {
        return ERR_TIMER(DRV_ERROR_PARAMETER);
    }

    timer_priv->load = load;
    silan_timer_compare(TIMER_ADDR(timer_priv->idx), load);
    silan_timer_count(TIMER_ADDR(timer_priv->idx), 0);                           /* load time(us) */
    silan_timer_irq_enable(TIMER_ADDR(timer_priv->idx));

    return 0;
}

/**
  \brief       Stop timer.
  \param[in]   handle timer handle to operate.
  \return      error code
*/
int32_t csi_timer_stop(timer_handle_t handle)
{
    TIMER_NULL_PARAM_CHK(handle);

    silan_timer_priv_t *timer_priv = handle;

    silan_timer_disable(TIMER_ADDR(timer_priv->idx));
    return 0;
}

/**
  \brief       suspend timer.
  \param[in]   instance  timer instance to operate.
  \return      error code
*/
int32_t csi_timer_suspend(timer_handle_t handle)
{
    TIMER_NULL_PARAM_CHK(handle);

    return ERR_TIMER(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief       resume timer.
  \param[in]   handle timer handle to operate.
  \return      error code
*/
int32_t csi_timer_resume(timer_handle_t handle)
{
    TIMER_NULL_PARAM_CHK(handle);

    silan_timer_priv_t *timer_priv = handle;

    silan_timer_enable(TIMER_ADDR(timer_priv->idx));
    return 0;
}

/**
  \brief       get timer current value
  \param[in]   handle timer handle to operate.
  \param[out]   value     timer current value
  \return      error code
*/
int32_t csi_timer_get_current_value(timer_handle_t handle, uint32_t *value)
{
    TIMER_NULL_PARAM_CHK(handle);
    TIMER_NULL_PARAM_CHK(value);

    silan_timer_priv_t *timer_priv = handle;

    *value = silan_get_timer_count(TIMER_ADDR(timer_priv->idx));
    return 0;
}

/**
  \brief       Get TIMER status.
  \param[in]   handle timer handle to operate.
  \return      TIMER status \ref timer_status_t
*/
timer_status_t csi_timer_get_status(timer_handle_t handle)
{
    timer_status_t timer_status = {0};

    if (handle == NULL) {
        return timer_status;
    }

    silan_timer_priv_t *timer_priv = handle;

    if (__REG32(TIMER_ADDR(timer_priv->idx) + TIMER_CTRL) & TIMER_EN) {
        timer_status.active = 1;
    }

    if (timer_priv->timeout_flag == 1) {
        timer_status.timeout = 1;
    }

    return timer_status;
}

/**
  \brief       get timer reload value
  \param[in]   handle timer handle to operate.
  \param[out]   value    timer reload value
  \return      error code
*/
int32_t csi_timer_get_load_value(timer_handle_t handle, uint32_t *value)
{
    TIMER_NULL_PARAM_CHK(handle);
    TIMER_NULL_PARAM_CHK(value);

    silan_timer_priv_t *timer_priv = handle;

    *value = __sREG32(TIMER_ADDR(timer_priv->idx), TIMER_COMPARE);
    return 0;
}
