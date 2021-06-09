/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     dw_timer.c
 * @brief    CSI Source File for TIMER Driver
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/

#include <csi_config.h>
#include <stdio.h>
#include <string.h>
#include <sys_freq.h>
#include "drv/timer.h"
#include "drv/irq.h"
#include "soc.h"
#include "csi_core.h"

#define ERR_TIMER(errno) (CSI_DRV_ERRNO_TIMER_BASE | errno)

typedef struct {
    uint32_t TIMER_CFG;
    uint32_t TIMER_CSR;
    uint32_t TIMER0_PRD;
    uint32_t TIMER1_PRD;
    uint32_t TIMER2_PRD;
    uint32_t TIMER3_PRD;
    uint32_t TIMER4_PRD;
    uint32_t TIMER5_PRD;
    uint32_t TIMER0_CNT;
    uint32_t TIMER1_CNT;
    uint32_t TIMER2_CNT;
    uint32_t TIMER3_CNT;
    uint32_t TIMER4_CNT;
    uint32_t TIMER5_CNT;
} dw_timer_reg_t;

typedef struct {
    uint32_t base;
    uint32_t irq;
    timer_event_cb_t cb_event;
    uint32_t idx;
} dw_timer_priv_t;

extern int32_t target_get_timer(int32_t idx, uint32_t *base, uint32_t *irq, void **handler);

static dw_timer_priv_t timer_instance[CONFIG_TIMER_NUM];

static void timer_clear_irq(int32_t idx, dw_timer_reg_t *addr)
{
    volatile uint8_t i;
    volatile uint32_t value;

    value = addr->TIMER_CSR;

    for (i = 0; i < 6; i++)
    {
        value &= ~(TLS_TIMER_INT_CLR(i));
    }

    addr->TIMER_CSR = value | TLS_TIMER_INT_CLR(idx);
}

void timer_irqhandler(int32_t idx)
{
    dw_timer_priv_t *timer_priv;
    dw_timer_reg_t *addr;
    uint32_t timer_csr;

    addr = (dw_timer_reg_t *)CSKY_TIMER_BASE;
    timer_csr = addr->TIMER_CSR;
    addr->TIMER_CSR = timer_csr;

    if(timer_csr & TLS_TIMER_INT_CLR(0))
	{
        timer_clear_irq(0, addr);
	
		timer_priv = &timer_instance[0];

        if (timer_priv->cb_event) {
            timer_priv->cb_event(0, TIMER_EVENT_TIMEOUT);
        }
	}

	if(timer_csr & TLS_TIMER_INT_CLR(1))
	{
        timer_clear_irq(1, addr);
	
		timer_priv = &timer_instance[1];

        if (timer_priv->cb_event) {
            timer_priv->cb_event(1, TIMER_EVENT_TIMEOUT);
        }
	}

	if(timer_csr & TLS_TIMER_INT_CLR(2))
	{
        timer_clear_irq(2, addr);
	
		timer_priv = &timer_instance[2];

        if (timer_priv->cb_event) {
            timer_priv->cb_event(2, TIMER_EVENT_TIMEOUT);
        }
	}

	if(timer_csr & TLS_TIMER_INT_CLR(3))
	{
        timer_clear_irq(3, addr);
	
		timer_priv = &timer_instance[3];

        if (timer_priv->cb_event) {
            timer_priv->cb_event(3, TIMER_EVENT_TIMEOUT);
        }
	}

	if(timer_csr & TLS_TIMER_INT_CLR(4))
	{
        timer_clear_irq(4, addr);
	
		timer_priv = &timer_instance[4];

        if (timer_priv->cb_event) {
            timer_priv->cb_event(4, TIMER_EVENT_TIMEOUT);
        }
	}

	if(timer_csr & TLS_TIMER_INT_CLR(5))
	{
        timer_clear_irq(5, addr);
	
		timer_priv = &timer_instance[5];

        if (timer_priv->cb_event) {
            timer_priv->cb_event(5, TIMER_EVENT_TIMEOUT);
        }
	}
}

/**
  \brief       Initialize TIMER Interface. 1. Initializes the resources needed for the TIMER interface 2.registers event callback function
  \param[in]   idx  timer index
  \param[in]   cb_event  event call back function \ref timer_event_cb_t
  \param[in]   cb_arg    arguments of cb_event
  \return      pointer to timer instance
*/
timer_handle_t csi_timer_initialize(int32_t idx, timer_event_cb_t cb_event)
{
    if (idx < 0 || idx >= CONFIG_TIMER_NUM) {
        return NULL;
    }

    uint32_t base = 0u;
    uint32_t irq = 0u;
    void *handler;
    dw_timer_reg_t *addr;

    int32_t real_idx = target_get_timer(idx, &base, &irq, &handler);

    if (real_idx != idx) {
        return NULL;
    }

    dw_timer_priv_t *timer_priv = &timer_instance[idx];
    timer_priv->base = base;
    timer_priv->irq = irq;
    timer_priv->idx = idx;

    timer_priv->cb_event = cb_event;

    addr = (dw_timer_reg_t *)base;
	addr->TIMER_CFG = drv_get_apb_freq(0) / 1000000 - 1;
	addr->TIMER_CSR |= TLS_TIMER_INT_CLR(idx);

    drv_irq_register(timer_priv->irq, handler);
    drv_irq_enable(timer_priv->irq);

    return timer_priv;
}

/**
  \brief       De-initialize TIMER Interface. stops operation and releases the software resources used by the interface
  \param[in]   handle timer handle to operate.
  \return      error code
*/
int32_t csi_timer_uninitialize(timer_handle_t handle)
{
    if (!handle)
        return ERR_TIMER(DRV_ERROR_PARAMETER);

    dw_timer_priv_t *timer_priv = handle;

    timer_priv->cb_event = NULL;
    drv_irq_disable(timer_priv->irq);
    drv_irq_unregister(timer_priv->irq);

    return 0;
}

/**
  \brief       control timer power.
  \param[in]   handle  timer handle to operate.
  \param[in]   state   power state.\ref csi_power_stat_e.
  \return      error code
*/
int32_t csi_timer_power_control(timer_handle_t handle, csi_power_stat_e state)
{
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
    if (!handle)
        return ERR_TIMER(DRV_ERROR_PARAMETER);

    dw_timer_priv_t *timer_priv = handle;
    dw_timer_reg_t *addr = (dw_timer_reg_t *)(timer_priv->base);

    if (TIMER_MODE_FREE_RUNNING == mode)
        return ERR_TIMER(DRV_ERROR_UNSUPPORTED);//addr->TIMER_CSR |= TLS_TIMER_ONE_TIME(timer_priv->idx);
    else if (TIMER_MODE_RELOAD == mode)
        addr->TIMER_CSR &= ~(TLS_TIMER_ONE_TIME(timer_priv->idx));
    else
        return ERR_TIMER(DRV_ERROR_PARAMETER);

    return 0;
}

/**
  \brief       Set timeout just for the reload mode.
  \param[in]   handle timer handle to operate.
  \param[in]   timeout the timeout value in microseconds(us).
  \return      error code
*/
int32_t csi_timer_set_timeout(timer_handle_t handle, uint32_t timeout)
{
    if (!handle)
        return ERR_TIMER(DRV_ERROR_PARAMETER);

    uint8_t is_run = 0;
    dw_timer_priv_t *timer_priv = handle;
    dw_timer_reg_t *addr = (dw_timer_reg_t *)(timer_priv->base);

    if (addr->TIMER_CSR & TLS_TIMER_EN(timer_priv->idx))
    {
        csi_timer_stop(handle);
        is_run = 1;
    }

    addr->TIMER_CSR &= ~(TLS_TIMER_MS_UNIT(timer_priv->idx));

    if (0 == timer_priv->idx)
        addr->TIMER0_PRD = timeout;
    else if (1 == timer_priv->idx)
        addr->TIMER1_PRD = timeout;
    else if (2 == timer_priv->idx)
        addr->TIMER2_PRD = timeout;
    else if (3 == timer_priv->idx)
        addr->TIMER3_PRD = timeout;
    else if (4 == timer_priv->idx)
        addr->TIMER4_PRD = timeout;
    else if (5 == timer_priv->idx)
        addr->TIMER5_PRD = timeout;

    if (is_run)
        csi_timer_start(handle);

    return 0;
}

/**
  \brief       Start timer.
  \param[in]   handle timer handle to operate.
  \return      error code
*/
int32_t csi_timer_start(timer_handle_t handle)
{
    if (!handle)
        return ERR_TIMER(DRV_ERROR_PARAMETER);

    dw_timer_priv_t *timer_priv = handle;
    dw_timer_reg_t *addr = (dw_timer_reg_t *)(timer_priv->base);

	addr->TIMER_CSR |= TLS_TIMER_INT_EN(timer_priv->idx) | TLS_TIMER_EN(timer_priv->idx);

    return 0;
}

/**
  \brief       Stop timer.
  \param[in]   handle timer handle to operate.
  \return      error code
*/
int32_t csi_timer_stop(timer_handle_t handle)
{
    if (!handle)
        return ERR_TIMER(DRV_ERROR_PARAMETER);

    dw_timer_priv_t *timer_priv = handle;
    dw_timer_reg_t *addr = (dw_timer_reg_t *)(timer_priv->base);

    addr->TIMER_CSR |= TLS_TIMER_INT_CLR(timer_priv->idx);
    addr->TIMER_CSR &= ~(TLS_TIMER_EN(timer_priv->idx));

    return 0;
}

/**
  \brief       suspend timer.
  \param[in]   handle timer handle to operate.
  \return      error code
*/
int32_t csi_timer_suspend(timer_handle_t handle)
{
    return ERR_TIMER(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief       resume timer.
  \param[in]   handle timer handle to operate.
  \return      error code
*/
int32_t csi_timer_resume(timer_handle_t handle)
{
    return ERR_TIMER(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief       get timer current value
  \param[in]   handle timer handle to operate.
  \param[out]   value     timer current value
  \return      error code
*/
int32_t csi_timer_get_current_value(timer_handle_t handle, uint32_t *value)
{
    if (!handle || !value)
        return ERR_TIMER(DRV_ERROR_PARAMETER);

    dw_timer_priv_t *timer_priv = handle;
    dw_timer_reg_t *addr = (dw_timer_reg_t *)(timer_priv->base);

    if (0 == timer_priv->idx)
        *value = addr->TIMER0_CNT;
    else if (1 == timer_priv->idx)
        *value = addr->TIMER1_CNT;
    else if (2 == timer_priv->idx)
        *value = addr->TIMER2_CNT;
    else if (3 == timer_priv->idx)
        *value = addr->TIMER3_CNT;
    else if (4 == timer_priv->idx)
        *value = addr->TIMER4_CNT;
    else if (5 == timer_priv->idx)
        *value = addr->TIMER5_CNT;

    return 0;
}

/**
  \brief       Get TIMER status.
  \param[in]   handle timer handle to operate.
  \return      TIMER status \ref timer_status_t
*/
timer_status_t csi_timer_get_status(timer_handle_t handle)
{
    timer_status_t status = {0};

    if (!handle)
        return status;

    dw_timer_priv_t *timer_priv = handle;
    dw_timer_reg_t *addr = (dw_timer_reg_t *)(timer_priv->base);

    memset(&status, 0, sizeof(timer_status_t));

    if (addr->TIMER_CSR & (TLS_TIMER_INT_EN(timer_priv->idx) | TLS_TIMER_EN(timer_priv->idx)))
        status.active = 1;
    if (addr->TIMER_CSR & TLS_TIMER_INT_CLR(timer_priv->idx))
        status.timeout = 1;

    return status;
}

/**
  \brief       get timer reload value
  \param[in]   handle timer handle to operate.
  \param[out]   value    timer reload value
  \return      error code
*/
int32_t csi_timer_get_load_value(timer_handle_t handle, uint32_t *value)
{
    if (!handle || !value)
        return ERR_TIMER(DRV_ERROR_PARAMETER);

    dw_timer_priv_t *timer_priv = handle;
    dw_timer_reg_t *addr = (dw_timer_reg_t *)(timer_priv->base);

    if (0 == timer_priv->idx)
        *value = addr->TIMER0_PRD;
    else if (1 == timer_priv->idx)
        *value = addr->TIMER1_PRD;
    else if (2 == timer_priv->idx)
        *value = addr->TIMER2_PRD;
    else if (3 == timer_priv->idx)
        *value = addr->TIMER3_PRD;
    else if (4 == timer_priv->idx)
        *value = addr->TIMER4_PRD;
    else if (5 == timer_priv->idx)
        *value = addr->TIMER5_PRD;

    return 0;
}

