/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     dw_wdt.c
 * @brief    CSI Source File for WDT Driver
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/

#include <csi_config.h>
#include <stdio.h>
#include <sys_freq.h>
#include "drv/wdt.h"
#include "drv/irq.h"
#include "soc.h"
#include "csi_core.h"

#define ERR_WDT(errno) (CSI_DRV_ERRNO_WDT_BASE | errno)

typedef struct {
    uint32_t WDG_LOAD_VALUE;
    uint32_t WDG_CUR_VALUE;
    uint32_t WDG_CTRL;
    uint32_t WDG_INT_CLR;
    uint32_t WDG_INT_SRC;
    uint32_t WDG_INT_MIS;
    uint32_t WDG_LOCK;
} dw_wdt_reg_t;

typedef struct {
    uint32_t base;
    uint32_t irq;
    wdt_event_cb_t cb_event;

    uint32_t usec;
} dw_wdt_priv_t;

extern int32_t target_get_wdt(int32_t idx, uint32_t *base, uint32_t *irq, void **handler);

static dw_wdt_priv_t wdt_instance[CONFIG_WDT_NUM];

void wdt_irqhandler(int32_t idx)
{
    dw_wdt_priv_t *wdt_priv = &wdt_instance[idx];

    if (wdt_priv->cb_event) {
        wdt_priv->cb_event(idx, WDT_EVENT_TIMEOUT);
    }
}

/**
  \brief       Initialize WDT Interface. 1. Initializes the resources needed for the WDT interface 2.registers event callback function
  \param[in]   idx   wdt index
  \param[in]   cb_event  Pointer to \ref wdt_event_cb_t
  \return      pointer to wdt instance
*/
wdt_handle_t csi_wdt_initialize(int32_t idx, wdt_event_cb_t cb_event)
{
    if (idx < 0 || idx >= CONFIG_WDT_NUM) {
        return NULL;
    }

    uint32_t base = 0u;
    uint32_t irq = 0u;
    void *handler;

    int32_t real_idx = target_get_wdt(idx, &base, &irq, &handler);

    if (real_idx != idx) {
        return NULL;
    }

    dw_wdt_priv_t *wdt_priv = &wdt_instance[idx];
    wdt_priv->base = base;
    wdt_priv->irq = irq;

    wdt_priv->cb_event = cb_event;

    drv_irq_register(wdt_priv->irq, handler);
    drv_irq_enable(wdt_priv->irq);

    return wdt_priv;
}

/**
  \brief       De-initialize WDT Interface. stops operation and releases the software resources used by the interface
  \param[in]   instance  wdt instance to operate.
  \return      \ref execution_status
*/
int32_t csi_wdt_uninitialize(wdt_handle_t handle)
{
    if (!handle)
        return ERR_WDT(DRV_ERROR_PARAMETER);

    dw_wdt_priv_t *wdt_priv = handle;

    wdt_priv->cb_event = NULL;
    drv_irq_disable(wdt_priv->irq);
    drv_irq_unregister(wdt_priv->irq);

    return 0;
}

int32_t csi_wdt_power_control(wdt_handle_t handle, csi_power_stat_e state)
{
    return ERR_WDT(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief       Set the WDT value. value = (2^t*0xffff * 10^6 /freq)/10^3(t: 0 ~ 15).
  \param[in]   handle wdt handle to operate.
  \param[in]   value     the timeout value(ms) \ref:timeout_ms[]
  \return      \ref execution_status
*/
int32_t csi_wdt_set_timeout(wdt_handle_t handle, uint32_t value)
{
    if (!handle)
        return ERR_WDT(DRV_ERROR_PARAMETER);

    dw_wdt_priv_t *wdt_priv = handle;
    dw_wdt_reg_t *addr = (dw_wdt_reg_t *)(wdt_priv->base);

    wdt_priv->usec = value * 1000;

    if (addr->WDG_CTRL & 0x1)
    {
        csi_wdt_stop(handle);
        csi_wdt_start(handle);
    }

    return 0;
}

/**
  \brief       Start the WDT.
  \param[in]   handle wdt handle to operate.
  \return      \ref execution_status
*/
int32_t csi_wdt_start(wdt_handle_t handle)
{
    if (!handle)
        return ERR_WDT(DRV_ERROR_PARAMETER);

    uint32_t value;
    dw_wdt_priv_t *wdt_priv = handle;
    dw_wdt_reg_t *addr = (dw_wdt_reg_t *)(wdt_priv->base);

    value = drv_get_apb_freq(0) / 1000000 * wdt_priv->usec / 2; /* Can't reset on time without dividing by 2, and can reset on time by dividing by 2, but it will affect performance */
    if (!value) value = 1;

    addr->WDG_LOAD_VALUE = value;
    addr->WDG_CTRL = 0x3; /* irq & reset */

    return 0;
}

/**
  \brief       Stop the WDT.
  \param[in]   handle wdt handle to operate.
  \return      \ref execution_status
*/
int32_t csi_wdt_stop(wdt_handle_t handle)
{
    if (!handle)
        return ERR_WDT(DRV_ERROR_PARAMETER);

    dw_wdt_priv_t *wdt_priv = handle;
    dw_wdt_reg_t *addr = (dw_wdt_reg_t *)(wdt_priv->base);

    addr->WDG_CTRL = 0x0;
    addr->WDG_INT_CLR = 0x1;

    return 0;
}

/**
  \brief       Restart the WDT.
  \param[in]   handle wdt handle to operate.
  \return      \ref execution_status
*/
int32_t csi_wdt_restart(wdt_handle_t handle)
{
    if (!handle)
        return ERR_WDT(DRV_ERROR_PARAMETER);

    dw_wdt_priv_t *wdt_priv = handle;
    dw_wdt_reg_t *addr = (dw_wdt_reg_t *)(wdt_priv->base);

    addr->WDG_INT_CLR = 0x1;

    return 0;
}

/**
  \brief       Read the WDT Current value.
  \param[in]   handle wdt handle to operate.
  \param[in]   value     Pointer to the Value.
  \return      \ref execution_status
*/
int32_t csi_wdt_read_current_value(wdt_handle_t handle, uint32_t *value)
{
    if (!handle || !value)
        return ERR_WDT(DRV_ERROR_PARAMETER);

    dw_wdt_priv_t *wdt_priv = handle;
    dw_wdt_reg_t *addr = (dw_wdt_reg_t *)(wdt_priv->base);

    *value = addr->WDG_CUR_VALUE / 40;

    return 0;
}

