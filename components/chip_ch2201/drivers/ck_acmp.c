/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     ck_acmp.c
 * @brief    CSI Source File for ACMP Driver
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/

#include <stdio.h>
#include <string.h>
#include "drv/acmp.h"
#include <soc.h>
#include <ck_acmp.h>

#define ERR_ACMP(errno) (CSI_DRV_ERRNO_ACMP_BASE | errno)
#define ACMP_NULL_PARAM_CHK(para)  HANDLE_PARAM_CHK(para, ERR_ACMP(DRV_ERROR_PARAMETER))

typedef struct {
#ifdef CONFIG_LPM
    uint8_t acmp_power_status;
    uint32_t acmp_regs_saved[4];
#endif
    uint32_t base;
    uint32_t irq;
} ck_acmp_priv_t;

extern int32_t target_get_acmp(int32_t idx, uint32_t *base, uint32_t *irq);
extern int32_t drv_acmp_power_control(acmp_handle_t handle, csi_power_stat_e state);
extern int32_t drv_soc_power_control(void *handle, csi_power_stat_e state, power_cb_t *cb);
static ck_acmp_priv_t acmp_instance[CONFIG_ACMP_NUM];
/* Driver Capabilities */
static const acmp_capabilities_t driver_capabilities = {
    .fast_mode = 1,
    .slow_mode = 1,
    .hysteresis_fun = 1,
};

#ifdef CONFIG_LPM
static void manage_clock(acmp_handle_t handle, uint8_t enable)
{
    if (handle == &acmp_instance[0]) {
        drv_clock_manager_config(CLOCK_MANAGER_CMPCTRL, enable);
    }
}

static void do_prepare_sleep_action(acmp_handle_t handle)
{
    ck_acmp_priv_t *acmp_priv = handle;
    uint32_t *ibase = (uint32_t *)(acmp_priv->base);
    registers_save(acmp_priv->acmp_regs_saved, ibase, 4);
}

static void do_wakeup_sleep_action(acmp_handle_t handle)
{
    ck_acmp_priv_t *acmp_priv = handle;
    uint32_t *ibase = (uint32_t *)(acmp_priv->base);
    registers_restore(ibase, acmp_priv->acmp_regs_saved, 4);
}
#endif
/**
  \brief       Initialize acmp Interface. 1. Initializes the resources needed for the acmp interface 2.registers event callback function
  \param[in]   idx  device id
  \return      return acmp handle if success
*/
acmp_handle_t drv_acmp_initialize(int32_t idx)
{
    if (idx < 0 || idx >= CONFIG_ACMP_NUM) {
        return NULL;
    }

    /* obtain the crc information */
    uint32_t base = 0u;
    uint32_t irq = 0u;

    int32_t real_idx = target_get_acmp(idx, &base, &irq);

    if (real_idx != idx) {
        return NULL;
    }

    ck_acmp_priv_t *acmp_priv = &acmp_instance[idx];

    acmp_priv->base = base;
    acmp_priv->irq = irq;
#ifdef CONFIG_LPM
    drv_acmp_power_control(acmp_priv, DRV_POWER_FULL);
#endif
    ck_acmp_reg_t *addr = (ck_acmp_reg_t *)(acmp_priv->base);
    addr->ACMP_CR = ACMP_CR_HYSTERESIS_ENBALE;
    return (acmp_handle_t)acmp_priv;
}

/**
  \brief       De-initialize acmp Interface. stops operation and releases the software resources used by the interface
  \param[in]   handle  acmp handle to operate.
*/
void drv_acmp_uninitialize(acmp_handle_t handle)
{
#ifdef CONFIG_LPM
    ck_acmp_priv_t *acmp_priv = handle;
    drv_acmp_power_control(acmp_priv, DRV_POWER_OFF);
#endif
}

/**
  \brief       control acmp power.
  \param[in]   handle  acmp handle to operate.
  \param[in]   state   power state.\ref csi_power_stat_e.
  \return      error code
*/
int32_t drv_acmp_power_control(acmp_handle_t handle, csi_power_stat_e state)
{
    ACMP_NULL_PARAM_CHK(handle);
#ifdef CONFIG_LPM
    power_cb_t callback = {
        .wakeup = do_wakeup_sleep_action,
        .sleep = do_prepare_sleep_action,
        .manage_clock = manage_clock
    };
    return drv_soc_power_control(handle, state, &callback);
#else
    return ERR_ACMP(DRV_ERROR_UNSUPPORTED);
#endif
}

/**
  \brief       Get driver capabilities.
  \param[in]   idx  device id
  \return      \ref acmp_capabilities_t
*/
acmp_capabilities_t drv_acmp_get_capabilities(int32_t idx)
{
    if (idx < 0 || idx >= CONFIG_ACMP_NUM) {
        acmp_capabilities_t ret;
        memset(&ret, 0, sizeof(acmp_capabilities_t));
        return ret;
    }

    return driver_capabilities;
}

/**
  \brief       config acmp mode.
  \param[in]   handle  acmp handle to operate.
  \param[in]   mode      \ref acmp_mode_e
  \param[in]   standard  \ref acmp_standard_acmp_e
  \return      error code
*/
int32_t drv_acmp_config(acmp_handle_t handle, acmp_mode_e mode)
{
    ACMP_NULL_PARAM_CHK(handle);

    if ((mode != ACMP_MODE_FAST) && (mode != ACMP_MODE_SLOW)) {
        return ERR_ACMP(DRV_ERROR_PARAMETER);
    }

    ck_acmp_priv_t *acmp_priv = handle;
    ck_acmp_reg_t *addr = (ck_acmp_reg_t *)(acmp_priv->base);

    if (mode == ACMP_MODE_FAST) {
        addr->ACMP_CR |= ACMP_CR_FAST_MODE;
        addr->ACMP_RPTC = (50 * drv_get_sys_freq() / 1000000000);                            /* fast mode response time 50ns */
    } else {
        addr->ACMP_CR &= ~ACMP_CR_FAST_MODE;
        addr->ACMP_RPTC = (20 * drv_get_sys_freq() / 1000000);                           ////other mode response time 20us
    }

    return 0;
}

/**
  \brief       start comparison
  \param[in]   handle  acmp handle to operate.
*/
void drv_acmp_comp_start(acmp_handle_t handle)
{
    if (handle == NULL) {
        return ;
    }

    ck_acmp_priv_t *acmp_priv = handle;
    ck_acmp_reg_t *addr = (ck_acmp_reg_t *)(acmp_priv->base);
    addr->ACMP_CR |= ACMP_CR_ENBALE;
}

/**
  \brief       Stop comparison.
  \param[in]   handle  acmp handle to operate.
*/
void drv_acmp_comp_stop(acmp_handle_t handle)
{
    if (handle == NULL) {
        return ;
    }

    ck_acmp_priv_t *acmp_priv = handle;
    ck_acmp_reg_t *addr = (ck_acmp_reg_t *)(acmp_priv->base);
    addr->ACMP_CR &= ~(ACMP_CR_ENBALE);
}
/**
  \brief       Get acmp status.
  \param[in]   handle  acmp handle to operate.
  \return      acmp status \ref acmp_status_t
*/
acmp_status_t drv_acmp_get_status(acmp_handle_t handle)
{
    acmp_status_t ret;

    if (handle == NULL) {
        memset(&ret, 0, sizeof(acmp_status_t));
        return ret;
    }

    ck_acmp_priv_t *acmp_priv = handle;

    ck_acmp_reg_t *addr = (ck_acmp_reg_t *)(acmp_priv->base);
    volatile uint32_t reg = addr->ACMP_SR;
    ret.output_state   = reg & 0x1;

    return ret;
}

