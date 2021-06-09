/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
/******************************************************************************
 * @file     ck_pmu.c
 * @brief    CSI Source File for PMU Driver
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <drv/pmu.h>
#include "drv/irq.h"
#include <soc.h>
#include "wm_pmu.h"

#define ERR_PMU(errno) (CSI_DRV_ERRNO_PMU_BASE | errno)
#define PMU_NULL_PARAM_CHK(para) HANDLE_PARAM_CHK(para, ERR_PMU(DRV_ERROR_PARAMETER))


typedef struct {
    void *handler;
    uint32_t irq;
    pmu_event_cb_t cb;
} ck_pmu_priv_t;

extern int32_t target_get_pmu(int32_t idx, uint32_t *base, uint32_t *irq, void **handler);

static csi_power_stat_e pmu_power_status = DRV_POWER_FULL;
static ck_pmu_priv_t pmu_handle[CONFIG_PMU_NUM];

/* Driver Capabilities */
//
// Functions
//
void pmu_wake_irqhandler(int idx)
{
    ck_pmu_priv_t *pmu_priv = &pmu_handle[0];

    if (DRV_POWER_LOW == pmu_power_status)
    {
        if (pmu_priv && pmu_priv->cb) {
            pmu_priv->cb(0, PMU_EVENT_SLEEP_DONE, PMU_MODE_SLEEP);
        }
    }
}

/**
  \brief       Initialize PMU Interface. 1. Initializes the resources needed for the PMU interface 2.registers event callback function
  \param[in]   idx device id
  \param[in]   cb_event  Pointer to \ref pmu_event_cb_t
  \return      pointer to pmu handle
*/
pmu_handle_t csi_pmu_initialize(int32_t idx, pmu_event_cb_t cb_event)
{
    if (idx < 0 || idx >= CONFIG_PMU_NUM) {
        return NULL;
    }

    /* obtain the pmu information */
    uint32_t base = 0u;
    uint32_t irq = 0u;
    void *handler;
    int32_t real_idx = target_get_pmu(idx, &base, &irq, &handler);

    if (real_idx != idx) {
        return NULL;
    }

    ck_pmu_priv_t *pmu_priv = &pmu_handle[idx];

    /* initialize the pmu context */
    pmu_priv->cb = cb_event;
    pmu_priv->irq = irq;
    pmu_priv->handler = handler;

    drv_irq_register(irq, handler);
    drv_irq_enable(irq);

    return (pmu_handle_t)pmu_priv;
}

/**
  \brief       De-initialize PMU Interface. stops operation and releases the software resources used by the interface
  \param[in]   handle  pmu handle to operate.
  \return      error code
*/
int32_t csi_pmu_uninitialize(pmu_handle_t handle)
{
    PMU_NULL_PARAM_CHK(handle);

    ck_pmu_priv_t *pmu_priv = handle;

    pmu_priv->cb = NULL;
    drv_irq_disable(pmu_priv->irq);
    drv_irq_unregister(pmu_priv->irq);

    return 0;
}

int32_t csi_pmu_power_control(pmu_handle_t handle, csi_power_stat_e state)
{
    PMU_NULL_PARAM_CHK(handle);

    //ck_pmu_priv_t *pmu_priv = handle;

    switch (state) {
        case DRV_POWER_OFF:
            return ERR_PMU(DRV_ERROR_UNSUPPORTED);

        case DRV_POWER_LOW:
            //tls_pmu_sleep_start();
            break;

        case DRV_POWER_FULL:
            break;

        case DRV_POWER_SUSPEND:
            //tls_pmu_standby_start();
            break;

        default:
            return ERR_PMU(DRV_ERROR_PARAMETER);
    }

    pmu_power_status = state;

    return 0;
}

/**
  \brief       choose the pmu mode to enter
  \param[in]   handle  pmu handle to operate.
  \param[in]   mode    \ref pmu_mode_e
  \return      error code
*/
int32_t csi_pmu_enter_sleep(pmu_handle_t handle, pmu_mode_e mode)
{
    PMU_NULL_PARAM_CHK(handle);

    ck_pmu_priv_t *pmu_priv = handle;

    switch (mode) {
        case PMU_MODE_RUN:
            break;

        case PMU_MODE_SHUTDOWN:
        case PMU_MODE_DOZE:
        case PMU_MODE_DORMANT:
            return ERR_PMU(DRV_ERROR_UNSUPPORTED);

        case PMU_MODE_SLEEP:
            if (pmu_priv->cb) {
                pmu_priv->cb(0, PMU_EVENT_PREPARE_SLEEP, mode);
            }

            tls_pmu_sleep_start();
            break;

        case PMU_MODE_STANDBY:
            if (pmu_priv->cb) {
                pmu_priv->cb(0, PMU_EVENT_PREPARE_SLEEP, mode);
            }

            tls_pmu_standby_start();
            break;

        default:
            return ERR_PMU(DRV_ERROR_PARAMETER);
    }

    return 0;
}

/**
  \brief       Config the wakeup source.
  \param[in]   handle  pmu handle to operate
  \param[in]   type    \ref pmu_wakeup_type
  \param[in]   pol     \ref pmu_wakeup_pol
  \param[in]   enable  flag control the wakeup source is enable or not
  \return      error code
*/
int32_t csi_pmu_config_wakeup_source(pmu_handle_t handle, uint32_t wakeup_num, pmu_wakeup_type_e type, pmu_wakeup_pol_e pol, uint8_t enable)
{
    PMU_NULL_PARAM_CHK(handle);

    if (wakeup_num >= 32 || (type != PMU_WAKEUP_TYPE_LEVEL) || (pol != PMU_WAKEUP_POL_HIGH)) {
        return ERR_PMU(DRV_ERROR_PARAMETER);
    }

    if (enable) {
        csi_vic_set_wakeup_irq(wakeup_num);
#if 0
    	/*configure wake up source begin*/
    	csi_vic_set_wakeup_irq(MAC_IRQn);
    	csi_vic_set_wakeup_irq(SEC_IRQn);
    	csi_vic_set_wakeup_irq(DMA_Channel0_IRQn);
    	csi_vic_set_wakeup_irq(DMA_Channel1_IRQn);
    	csi_vic_set_wakeup_irq(DMA_Channel2_IRQn);
    	csi_vic_set_wakeup_irq(DMA_Channel3_IRQn);
    	csi_vic_set_wakeup_irq(DMA_Channel4_7_IRQn);
    	csi_vic_set_wakeup_irq(DMA_BRUST_IRQn);
    	csi_vic_set_wakeup_irq(I2C_IRQn);
    	csi_vic_set_wakeup_irq(ADC_IRQn);
    	csi_vic_set_wakeup_irq(SPI_LS_IRQn);
    	csi_vic_set_wakeup_irq(GPIOA_IRQn);
    	csi_vic_set_wakeup_irq(GPIOB_IRQn);
    	csi_vic_set_wakeup_irq(UART0_IRQn);
    	csi_vic_set_wakeup_irq(UART1_IRQn);
    	csi_vic_set_wakeup_irq(UART24_IRQn);
    	csi_vic_set_wakeup_irq(BLE_IRQn);
    	csi_vic_set_wakeup_irq(BT_IRQn);
    	csi_vic_set_wakeup_irq(PWM_IRQn);
    	csi_vic_set_wakeup_irq(I2S_IRQn);
    	csi_vic_set_wakeup_irq(SYS_TICK_IRQn);
    	csi_vic_set_wakeup_irq(RSA_IRQn);
    	csi_vic_set_wakeup_irq(CRYPTION_IRQn);
    	csi_vic_set_wakeup_irq(PMU_IRQn);
    	csi_vic_set_wakeup_irq(TIMER_IRQn);
    	csi_vic_set_wakeup_irq(WDG_IRQn);
    	/*configure wake up source end*/
#endif
    } else {
        csi_vic_clear_wakeup_irq(wakeup_num);
    }

    return 0;
}
