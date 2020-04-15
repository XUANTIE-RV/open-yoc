/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     silan_adc_csi.c
 * @brief    CSI Source File for ADC Driver
 * @version  V1.0
 * @date     16. October 2017
 ******************************************************************************/
#include <csi_config.h>
#include <soc.h>
#include <csi_core.h>
#include <drv/adc.h>
#include <string.h>

#include "silan_types.h"
#include "ap1508.h"
#include "silan_adc_regs.h"
#include "silan_adc.h"
#include "silan_syscfg.h"
#include "silan_syscfg_regs.h"
#include "silan_pmu.h"
#include "silan_pmu.h"
#include "silan_pic.h"
#include "silan_iomux.h"

#include <silan_adc_csi.h>

#define ADC_BUSY_TIMEOUT  1000

#define silan_adc_regs  ((adc_regs_struct_t *)SILAN_ADC_BASE)

#define ERR_ADC(errno) (CSI_DRV_ERRNO_ADC_BASE | errno)
#define ADC_NULL_PARAM_CHK(para)  HANDLE_PARAM_CHK(para, ERR_ADC(DRV_ERROR_PARAMETER))

typedef struct {
    adc_event_cb_t cb_event;
    uint32_t    data_num;
    uint8_t     channel;
} silan_adc_priv_t;

extern int32_t target_adc_init(uint32_t channel);
extern int32_t target_get_adc_count(void);

static silan_adc_priv_t adc_instance[CONFIG_ADC_NUM];

static const adc_capabilities_t adc_capabilities = {
    .single = 1,
    .continuous = 0,
    .scan = 0,
    .calibration = 0,
    .comparator = 0
};

void silan_adc_irqhandler(int idx)
{
    silan_adc_priv_t *adc_priv = &adc_instance[idx];

    adc_priv->cb_event(0, ADC_EVENT_CONVERSION_COMPLETE);
}

/**
  \brief       get adc instance count.
  \return      adc instance count
*/
int32_t drv_adc_get_instance_count(void)
{
    return target_get_adc_count();
}

/**
  \brief       Initialize adc Interface. 1. Initializes the resources needed for the adc interface 2.registers event callback function.
  \param[in]   idx       adc index.
  \param[in]   cb_event  event call back function \ref adc_event_cb_t
  \return      return adc handle if success
*/
adc_handle_t drv_adc_initialize(int32_t idx, adc_event_cb_t cb_event)
{
    if ((idx < 0) || (idx > (CONFIG_ADC_NUM - 1))) {
        return NULL;
    }

    silan_adc_priv_t *adc_priv = &adc_instance[idx];
    adc_priv->cb_event  = cb_event;

    silan_adc_cclk_config(ADC_CCLK_PLLREF_DIV2, CLK_ON);   // sar adc fs   3:40kHz   2:6MHz
    silan_pmu_wakeup_disable(PMU_WAKEUP_KEY);
    silan_sar_adc_vref_select(ADC_VREF_EXT33);
    silan_soft_rst(SOFT_RST_ADC);

    silan_pic_request(PIC_IRQID_SAR, 0, (hdl_t)silan_adc_irqhandler);
    return (adc_handle_t)adc_priv;
}

/**
  \brief       De-initialize adc Interface. stops operation and releases the software resources used by the interface
  \param[in]   handle   adc handle to operate.
  \return      error code
*/
int32_t drv_adc_uninitialize(adc_handle_t handle)
{
    ADC_NULL_PARAM_CHK(handle);

    silan_adc_priv_t *adc_priv = (silan_adc_priv_t *)handle;
    adc_priv->cb_event = NULL;

    silan_adc_cclk_config(ADC_CCLK_PLLREF_DIV2, CLK_OFF);
    silan_pic_free(PIC_IRQID_SAR, 0);
    return 0;
}

/**
  \brief       control adc power.
  \param[in]   handle  adc handle to operate.
  \param[in]   state   power state.\ref csi_power_stat_e.
  \return      error code
*/
int32_t drv_adc_power_control(adc_handle_t handle, csi_power_stat_e state)
{
    ADC_NULL_PARAM_CHK(handle);
    return ERR_ADC(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief       Get driver capabilities.
  \param[in]   idx    adc index.
  \return      \ref adc_capabilities_t
*/
adc_capabilities_t drv_adc_get_capabilities(int32_t idx)
{
    if (idx > (CONFIG_ADC_NUM - 1) || idx < 0) {
        adc_capabilities_t ret;
        memset(&ret, 0, sizeof(adc_capabilities_t));
        return ret;
    }

    return adc_capabilities;
}

int32_t drv_adc_pin2channel(int pin)
{
    return pin;
    //pin - PA1 + 1;// PA1 = 1;
}

/**
  \brief       config adc mode.
  \param[in]   handle     adc handle to operate.
  \param[in]   config   adc_conf_t\ref  . pointer to adc configuration structure.
  \return      error code
*/
int32_t drv_adc_config(adc_handle_t handle, adc_conf_t *config)
{
    ADC_NULL_PARAM_CHK(handle);
    ADC_NULL_PARAM_CHK(config);
    ADC_NULL_PARAM_CHK(config->channel_array);
    ADC_NULL_PARAM_CHK(config->channel_nbr);
    ADC_NULL_PARAM_CHK(config->conv_cnt);

    if ((config->trigger != 0) && (config->trigger != 1)) {
        return ERR_ADC(DRV_ERROR_PARAMETER);
    }

    if ((config->intrp_mode != 0) && (config->intrp_mode != 1)) {
        return ERR_ADC(DRV_ERROR_PARAMETER);
    }

    if (config->channel_nbr > (SILAN_ADC_CH7 + 1)) {
        return ERR_ADC(DRV_ERROR_PARAMETER);
    }

    if (config->mode == ADC_SCAN || config->mode == ADC_CONTINUOUS) {
        return ERR_ADC(DRV_ERROR_UNSUPPORTED);
    }

    uint32_t *ch_arr = config->channel_array;
    uint32_t temp = 0;

    silan_adc_priv_t *adc_priv = (silan_adc_priv_t *)handle;

    if (config->mode == ADC_SINGLE) {
        if (config->channel_nbr != 1) {
            return ERR_ADC(DRV_ERROR_PARAMETER);
        }

        if (config->conv_cnt != 1) {
            return ERR_ADC(DRV_ERROR_PARAMETER);
        }

        ch_arr = config->channel_array;

        if (*ch_arr > SILAN_ADC_CH7) {
            return ERR_ADC(DRV_ERROR_PARAMETER);
        }

        silan_adc_regs->adc_cfg |= BYPASS;

        temp = silan_adc_regs->adc_cfg;
        temp &= ~CSEL(0x7);
        temp |= CSEL(*ch_arr);
        silan_adc_regs->adc_cfg = temp;

        adc_priv->data_num = 1;
        adc_priv->channel = *ch_arr;
    } else {
        return ERR_ADC(DRV_ERROR_PARAMETER);
    }

    return 0;
}

int32_t drv_adc_comparator_config(adc_handle_t handle, adc_cmp_conf_t *config)
{
    return ERR_ADC(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief       start adc.
  \param[in]   handle adc handle to operate.
  \return      error code
*/
int32_t drv_adc_start(adc_handle_t handle)
{
    ADC_NULL_PARAM_CHK(handle);

    silan_adc_priv_t *adc_priv = (silan_adc_priv_t *)handle;
    silan_iomux_adc_open(adc_priv->channel);
    silan_adc_regs->adc_cfg &= ~DISABLE;

    return 0;
}

/**
  \brief       stop adc.
  \param[in]   handle adc handle to operate.
  \return      error code
*/
int32_t drv_adc_stop(adc_handle_t handle)
{
    ADC_NULL_PARAM_CHK(handle);

    silan_adc_priv_t *adc_priv = (silan_adc_priv_t *)handle;
    silan_iomux_adc_close(adc_priv->channel);
    silan_adc_regs->adc_cfg |= DISABLE;
    return 0;
}

/**
  \brief       receiving data from ADC receiver.
  \param[in]   handle ADC handle to operate.
  \param[out]  data  Pointer to buffer for data to receive from ADC receiver.
  \param[in]   num   Number of data items to receive.
  \return      error code
*/
int32_t drv_adc_read(adc_handle_t handle, uint32_t *data, uint32_t num)
{
    int32_t timecount = 0;
    uint32_t int_status = 0;

    ADC_NULL_PARAM_CHK(handle);
    ADC_NULL_PARAM_CHK(data);

    silan_adc_priv_t *adc_priv = (silan_adc_priv_t *)handle;

    if (num > adc_priv->data_num) {
        return ERR_ADC(DRV_ERROR_PARAMETER);
    }

    silan_adc_regs->adc_cfg |= START;
    silan_adc_regs->adc_cfg |= ADC_START;

    int_status = silan_adc_regs->adc_int_rsts;
    while (!(int_status & INT_STAT_SIN_FINISH)) {
        int_status = silan_adc_regs->adc_int_rsts;
        timecount++;

        if (timecount >= ADC_BUSY_TIMEOUT) {
            return ERR_ADC(DRV_ERROR_TIMEOUT);
        }
    }

    *data = silan_adc_regs->adc_data;

    silan_adc_regs->adc_int_clr |= INT_STAT_SIN_FINISH;
    silan_adc_regs->adc_cfg &= ~ADC_START;
    return 0;
}

/**
  \brief       Get ADC status.
  \param[in]   handle   adc handle to operate.
  \return      ADC status \ref adc_status_t
*/
adc_status_t drv_adc_get_status(adc_handle_t handle)
{
    adc_status_t adc_status = {0};
    uint32_t int_status = 0;

    if (handle == NULL) {
        return adc_status;
    }

    int_status = silan_adc_regs->adc_int_rsts;

    if (int_status & INT_STAT_SIN_FINISH) {
        adc_status.complete = 1;
    } else {
        adc_status.busy = 1;
    }

    return adc_status;
}
