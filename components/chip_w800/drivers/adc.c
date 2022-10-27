/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     ck_adc.c
 * @brief    CSI Source File for ADC Driver
 * @version  V1.0
 * @date     16. October 2017
 ******************************************************************************/
#include <soc.h>
#include <csi_core.h>
#include <drv/adc.h>
#include <dw_adc.h>
#include <drv/gpio.h>
#include <drv/irq.h>
#include <io.h>
#include <string.h>
#include <drv/dmac.h>
#include "wm_regs.h"
#include "wm_adc.h"

#include "pinmux.h"
#include "pin_name.h"

#define ADC_START_TRY_TIMES    10
#define ADC_WAIT_DATA_VALID_TIMES  1000
#define ADC_STAB_DELAY_US    3

#define ERR_ADC(errno) (CSI_DRV_ERRNO_ADC_BASE | errno)
#define ADC_NULL_PARAM_CHK(para) HANDLE_PARAM_CHK(para, ERR_ADC(DRV_ERROR_PARAMETER))

typedef struct {
    uint32_t base;
    adc_event_cb_t cb_event;
    uint32_t data_num;
} ck_adc_priv_t;

extern int32_t target_adc_init(uint32_t channel);
extern int32_t target_get_adc_count(void);
extern void ADC_IRQHandler(void);

static ck_adc_priv_t adc_instance[CONFIG_ADC_NUM];

static const adc_capabilities_t adc_capabilities = {
    .single = 1,
    .continuous = 0,
    .scan = 0,
    .calibration = 0,
    .comparator = 1
};

static uint32_t adc_data_offset = 0;
void adc_irqhandler(int idx)
{
    ck_adc_priv_t *adc_priv = &adc_instance[idx];
    ck_adc_reg_t *addr = (ck_adc_reg_t *)(adc_priv->base);	

	/*with compare function*/
	if((addr->ADC_INT_SR & (CMP_INT_MASK|ADC_INT_MASK)) == (CMP_INT_MASK|ADC_INT_MASK))
	{
		addr->ADC_INT_SR |= CMP_INT_MASK|ADC_INT_MASK;
	    if(adc_priv->cb_event)
		{
			adc_priv->cb_event(idx, ADC_EVENT_DATA_COMPARE_VALID);
	    }
	}
	/*only transfer done isr*/
	if(addr->ADC_INT_SR & ADC_INT_MASK)
	{
		addr->ADC_INT_SR |= ADC_INT_MASK;
		if(adc_priv->cb_event)
		{
			adc_priv->cb_event(idx, ADC_EVENT_CONVERSION_COMPLETE);
		}
	}

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

    ck_adc_priv_t *adc_priv = &adc_instance[idx];
    adc_priv->cb_event  = cb_event;
    adc_priv->base = CSKY_ADC_CTL_BASE;

	/*config adc clock as 1MHz*/
	tls_adc_set_clk(0x28);
	drv_irq_register(ADC_IRQn, ADC_IRQHandler);

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

    ck_adc_priv_t *adc_priv = (ck_adc_priv_t *)handle;
    adc_priv->cb_event = NULL;

	tls_adc_stop(0);
    drv_irq_disable(ADC_IRQn);
    drv_irq_unregister(ADC_IRQn);

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

    if (config->channel_nbr > (CK_ADC_CH1 + 1)) {
        return -ADC_PARAM_INVALID;
    }

    uint32_t *ch_arr = config->channel_array;

    ck_adc_priv_t *adc_priv = (ck_adc_priv_t *)handle;
    ck_adc_reg_t *addr = (ck_adc_reg_t *)(adc_priv->base);

    if (config->channel_nbr != 1) {
        return -ADC_CHANNEL_ERROR;
    }

    ch_arr = config->channel_array;

    if (*ch_arr > CK_ADC_CH1) {
        return -ADC_CHANNEL_ERROR;
    }

    adc_priv->data_num = 1;

	adc_data_offset = adc_get_offset();

	addr->ADC_CR = CK_ADC_ANALOG_SWITCH_TIME_VAL(0x50)|CK_ADC_ANALOG_INIT_TIME_VAL(0x50)|CK_ADC_ADC_IRQ_EN_VAL(0x1);	
	/*set reference as internal*/	
	tls_adc_reference_sel(ADC_REFERENCE_INTERNAL);

	/*configure pga*/
	addr->ADC_PGA_CR = CK_ADC_PGA_GAIN_VAL(0)| CK_ADC_PGA_CHOP_VAL(0)| CK_ADC_PGA_BYPASS_VAL(0)|CK_ADC_PGA_CHOP_ENP_VAL(1)|CK_ADC_PGA_EN_VAL(1);

	/* configure channel */
	addr->ADC_ANA_CR &= ~(CK_ADC_CHL_SEL_MASK);
	addr->ADC_ANA_CR |= CK_ADC_CHL_SEL(*ch_arr);

    return 0;
}

int32_t drv_adc_comparator_config(adc_handle_t handle, adc_cmp_conf_t *config)
{
    ADC_NULL_PARAM_CHK(handle);
    ADC_NULL_PARAM_CHK(config);

    ck_adc_priv_t *adc_priv = (ck_adc_priv_t *)handle;
    ck_adc_reg_t *addr = (ck_adc_reg_t *)(adc_priv->base);

    if (config->cmp_channel > CK_ADC_CH1) 
	{
        return -ADC_CHANNEL_ERROR;
    }

    if (config->cmp_condition != 0 && config->cmp_condition != 1) 
	{
        return ERR_ADC(DRV_ERROR_PARAMETER);
    }

	tls_adc_init(0,0);

	addr->ADC_CR = CK_ADC_ANALOG_SWITCH_TIME_VAL(0x50)|CK_ADC_ANALOG_INIT_TIME_VAL(0x50)|CK_ADC_ADC_IRQ_EN_VAL(0x1);	
	tls_adc_reference_sel(ADC_REFERENCE_INTERNAL);

	/*configure pga*/
	addr->ADC_PGA_CR = CK_ADC_PGA_GAIN_VAL(0)| CK_ADC_PGA_CHOP_VAL(0)| CK_ADC_PGA_BYPASS_VAL(0)|CK_ADC_PGA_CHOP_ENP_VAL(1)|CK_ADC_PGA_EN_VAL(1);

	/* configure channel */
	addr->ADC_ANA_CR &= ~(CK_ADC_CHL_SEL_MASK);
	addr->ADC_ANA_CR |= CK_ADC_CHL_SEL(config->cmp_channel);
	if (config->cmp_condition)
	{
		tls_adc_config_cmp_reg(config->cmp_data, 0);
	}
	else
	{
		tls_adc_config_cmp_reg(config->cmp_data, 1);	
	}

	/*Enable compare function and compare irq*/
	addr->ADC_CR |= CK_ADC_CMP_IRQ_EN_VAL(1)| CK_ADC_CMP_EN_VAL(1);

    return 0;
}

/**
  \brief       start adc.
  \param[in]   handle adc handle to operate.
  \return      error code
*/
int32_t drv_adc_start(adc_handle_t handle)
{
    ADC_NULL_PARAM_CHK(handle);

    ck_adc_priv_t *adc_priv = (ck_adc_priv_t *)handle;
    ck_adc_reg_t *addr = (ck_adc_reg_t *)(adc_priv->base);

	addr->ADC_ANA_CR |= CK_ADC_PD_ADC_VAL(1);
	addr->ADC_ANA_CR &= ~(CK_ADC_RSTN_ADC_VAL(1)|CK_ADC_EN_LDO_ADC_VAL(1));	

	addr->ADC_ANA_CR &= ~(CK_ADC_PD_ADC_VAL(1));
	addr->ADC_ANA_CR |= (CK_ADC_RSTN_ADC_VAL(1)|CK_ADC_EN_LDO_ADC_VAL(1));	

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

    ck_adc_priv_t *adc_priv = (ck_adc_priv_t *)handle;
    ck_adc_reg_t *addr = (ck_adc_reg_t *)(adc_priv->base);

	addr->ADC_PGA_CR = 0;
	addr->ADC_ANA_CR |= CK_ADC_PD_ADC_VAL(1);
	addr->ADC_ANA_CR &= ~(CK_ADC_RSTN_ADC_VAL(1)|CK_ADC_EN_LDO_ADC_VAL(1));	

	/*Disable dma*/
	addr->ADC_CR &= ~(1<<0);

	/*Disable compare function and compare irq*/
	addr->ADC_CR &= ~(3<<4);

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
    ADC_NULL_PARAM_CHK(handle);
    ADC_NULL_PARAM_CHK(data);

    ck_adc_priv_t *adc_priv = (ck_adc_priv_t *)handle;
    ck_adc_reg_t *addr = (ck_adc_reg_t *)(adc_priv->base);

    uint32_t read_num = 0;
    uint32_t *pbuf = data;
    uint32_t value = 0;
	uint32_t timeout = 10000;

	/*wait for transfer success*/
	drv_irq_disable(ADC_IRQn);
	while(timeout--)
	{
		if (addr->ADC_INT_SR & ADC_INT_MASK)
		{
			value++;
			addr->ADC_INT_SR |= ADC_INT_MASK;

			if (value == 4)
			{
				break;
			}
		}
		else if(addr->ADC_INT_SR & CMP_INT_MASK)
        {
        	value++;
            addr->ADC_INT_SR |= CMP_INT_MASK;
			if (value == 4)
			{
				break;
			}
        }
	}

	/*read adc result*/
	for (read_num = 0; read_num < num; read_num++)
    {
    	value = CK_ADC_DR_VAL(addr->ADC_DR);
		signedToUnsignedData(&value);
       *pbuf++ = value;
    }

	drv_irq_enable(ADC_IRQn);

	tls_adc_stop(0);

    return SUCCESS;
}

/**
  \brief       Get ADC status.
  \param[in]   handle   adc handle to operate.
  \return      ADC status \ref adc_status_t
*/
adc_status_t drv_adc_get_status(adc_handle_t handle)
{
    adc_status_t adc_status = {0};

    if (handle == NULL) {
        return adc_status;
    }

    ck_adc_priv_t *adc_priv = (ck_adc_priv_t *)handle;
    ck_adc_reg_t *addr = (ck_adc_reg_t *)(adc_priv->base);

    uint32_t get_stat = 1 & (addr->ADC_INT_SR);

    if (get_stat == 0) {
        adc_status.complete = 1;
    } else if (get_stat == 1) {
        adc_status.busy = 1;
    }

    return adc_status;
}

