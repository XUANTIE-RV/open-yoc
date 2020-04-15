/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     silan_wdt.c
 * @brief    CSI Source File for WDT Driver
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/

#include <csi_config.h>
#include <stdio.h>
#include <silan_pic.h>
#include <silan_syscfg.h>
#include <drv/wdt.h>
#include "silan_wdt.h"
#include "soc.h"
#include "csi_core.h"

#define ERR_WDT(errno) (CSI_DRV_ERRNO_WDT_BASE | errno)
#define WDT_NULL_PARAM_CHK(para)  HANDLE_PARAM_CHK(para, ERR_WDT(DRV_ERROR_PARAMETER))
#define SYSTEM_CLOCK_MS (drv_get_sys_freq() / 1000)

uint32_t timeout_ms[16];
#define SYS_RHC   11200000
typedef struct {
    uint32_t base;
    uint32_t irq;
    wdt_event_cb_t cb_event;
} silan_wdt_priv_t;

extern int32_t target_get_wdt_count(void);
extern int32_t target_get_wdt(int32_t idx, uint32_t *base, uint32_t *irq);

static silan_wdt_priv_t wdt_instance[CONFIG_WDT_NUM];

static inline void silan_wdt_enable(silan_wdt_reg_t *addr)
{
    uint32_t value = addr->WDT_CONTROL;
    value |= 1 << 0;
    addr->WDT_CONTROL = value;
}

static inline void silan_wdt_disable(silan_wdt_reg_t *addr)
{
    uint32_t value = addr->WDT_CONTROL;
    value &= ~(1 << 0);
    addr->WDT_CONTROL = value;
}

void silan_wdt_irqhandler(int32_t idx)
{
    silan_wdt_priv_t *wdt_priv = &wdt_instance[0];

    if (wdt_priv->cb_event) {
        wdt_priv->cb_event(0, WDT_EVENT_TIMEOUT);
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

    int32_t real_idx = target_get_wdt(idx, &base, &irq);

    if (real_idx != idx) {
        return NULL;
    }

    silan_wdt_priv_t *wdt_priv = &wdt_instance[idx];
    wdt_priv->base = base;
    wdt_priv->irq  = irq;

    wdt_priv->cb_event = cb_event;

    silan_risc_wdog_cclk_config(1);
    silan_pic_request(irq, 0, (hdl_t)silan_wdt_irqhandler);

    return (wdt_handle_t)wdt_priv;
}

/**
  \brief       De-initialize WDT Interface. stops operation and releases the software resources used by the interface
  \param[in]   instance  wdt instance to operate.
  \return      \ref execution_status
*/
int32_t csi_wdt_uninitialize(wdt_handle_t handle)
{
    WDT_NULL_PARAM_CHK(handle);

    silan_wdt_priv_t *wdt_priv = handle;

    wdt_priv->cb_event = NULL;
    silan_risc_wdog_cclk_config(0);
    silan_pic_free(PIC_IRQID_PMU, wdt_priv->irq);

    return 0;
}

int32_t csi_wdt_power_control(wdt_handle_t handle, csi_power_stat_e state)
{
    WDT_NULL_PARAM_CHK(handle);

    return ERR_WDT(DRV_ERROR_UNSUPPORTED);

}

/**
  \brief       Set the WDT value.
  \param[in]   handle wdt handle to operate.
  \param[in]   value     the timeout value(ms) \ref:timeout_ms[]
  \return      \ref execution_status
*/
int32_t csi_wdt_set_timeout(wdt_handle_t handle, uint32_t value)
{
    WDT_NULL_PARAM_CHK(handle);

    if ((value == 0) || (value > (0xffffffff / SYS_RHC / 2 *1000))) {
        return ERR_WDT(DRV_ERROR_PARAMETER);
    }

    silan_wdt_priv_t *wdt_priv = handle;
    silan_wdt_reg_t *addr = (silan_wdt_reg_t *)(wdt_priv->base);

    uint32_t config = addr->WDT_CONTROL;
    uint32_t en_stat = 0;   /*origin wdt enable status*/

    if ((config & 0x1) != 0) {
        en_stat = 1;
    }

    /*before configuration, must disable wdt first*/
    silan_wdt_disable(addr);

    addr->WDT_LOAD = SYS_RHC / 2  / 1000 * value;

    if (en_stat == 1) {
        silan_wdt_enable(addr);
        csi_wdt_restart(handle);
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
    WDT_NULL_PARAM_CHK(handle);

    silan_wdt_priv_t *wdt_priv = handle;
    silan_wdt_reg_t *addr = (silan_wdt_reg_t *)(wdt_priv->base);

    addr->WDT_CONTROL |= WDT_CONTROL_RESERN;

    silan_wdt_enable(addr);
    csi_wdt_restart(handle);
    return 0;
}

/**
  \brief       Stop the WDT.
  \param[in]   handle wdt handle to operate.
  \return      \ref execution_status
*/
int32_t csi_wdt_stop(wdt_handle_t handle)
{
    WDT_NULL_PARAM_CHK(handle);
    silan_wdt_priv_t *wdt_priv = handle;
    silan_wdt_reg_t *addr = (silan_wdt_reg_t *)(wdt_priv->base);

    silan_wdt_disable(addr);

    return 0;
}

/**
  \brief       Restart the WDT.
  \param[in]   handle wdt handle to operate.
  \return      \ref execution_status
*/
int32_t csi_wdt_restart(wdt_handle_t handle)
{
    WDT_NULL_PARAM_CHK(handle);

    silan_wdt_priv_t *wdt_priv = handle;
    silan_wdt_reg_t *addr = (silan_wdt_reg_t *)(wdt_priv->base);
    addr->WDT_INTCLR = WDT_INTCLR_EN;

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
    WDT_NULL_PARAM_CHK(handle);
    WDT_NULL_PARAM_CHK(value);

    silan_wdt_priv_t *wdt_priv = handle;
    silan_wdt_reg_t *addr = (silan_wdt_reg_t *)(wdt_priv->base);

    *value = addr->WDT_VALUE;
    return 0;
}
