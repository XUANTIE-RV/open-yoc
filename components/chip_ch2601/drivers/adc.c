/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     wj_adc.c
 * @brief    CSI Source File for ADC Driver
 * @version  V1.0
 * @date     2020-03-05
 ******************************************************************************/

#include <string.h>

#include <drv/adc.h>
#include <drv/irq.h>
#include <drv/tick.h>

#include "wj_adc_ll.h"

#define DEFUALT_WJ_ADC_OPERATION_TIMEOUT    3000U

#define VOID_P_ADD(p, val) do{ uint8_t *temp = (uint8_t *)p; temp += val; p = (void *)temp; }while(0);
#define VOID_P_DEC(p, val) do{ uint8_t *temp = (uint8_t *)p; temp -= val; p = (void *)temp; }while(0);
static void wj_adc_channel_conv_end_irq(csi_adc_t *adc)
{
    wj_adc_regs_t  *adc_base;

    adc_base = (wj_adc_regs_t *)HANDLE_REG_BASE(adc);

    if (adc->num > 0U) {
        // TODO: support other bit width
        *adc->data++ = wj_adc_get_data(adc_base);
        adc->num--;
    }

    if (adc->num == 0U) {
        if (adc->callback) {
            wj_adc_disable_osc_cmplt_irq(adc_base);
            wj_adc_disable_os_eoc_irq(adc_base);

            adc->callback(adc, ADC_EVENT_CONVERT_COMPLETE, adc->arg);
            adc->state.readable = 1U;
        }
    }
}

static void wj_adc_irqhandler(void *args)
{
    csi_adc_t *adc;
    wj_adc_regs_t *adc_base;
    volatile uint32_t intr_status;

    adc         = (csi_adc_t *)args;
    adc_base    = (wj_adc_regs_t *)HANDLE_REG_BASE(adc);
    intr_status = wj_adc_get_interrupt_status(adc_base);
    wj_adc_clr_interrupt_status(adc_base, intr_status);

    /* handle end of single channel conversion completed */
    if (intr_status & WJ_ADC_IF_OSS_EOCIF) {
        wj_adc_channel_conv_end_irq(adc);
    }

    /* handle overwrite irq */
    if (intr_status & WJ_ADC_IF_OVWTIF) {
        if (adc->callback) {
            adc->callback(adc, ADC_EVENT_ERROR, adc->arg);
        }
    }
}

void wj_adc_dma_event_cb(csi_dma_ch_t *dma, csi_dma_event_t event, void *arg)
{
    csi_adc_t      *adc;

    adc = (csi_adc_t *)dma->parent;

    if (event == DMA_EVENT_TRANSFER_DONE) {
        csi_dma_ch_stop(dma);
        adc->num = 0U;
        adc->state.readable = 1U;

        if (adc->callback) {
            adc->callback(adc, ADC_EVENT_CONVERT_COMPLETE, adc->arg);
        }
    } else if (event == DMA_EVENT_TRANSFER_HALF_DONE) {
        adc->num /= 2U;
        if (adc->callback) {
            adc->callback(adc, ADC_EVENT_CONVERT_HALF_DONE, adc->arg);
        }
    }
}

static csi_error_t wj_adc_start_intr(csi_adc_t *adc)
{
    wj_adc_regs_t *adc_base;

    adc_base = (wj_adc_regs_t *)HANDLE_REG_BASE(adc);
    wj_adc_config_os_channel_nums(adc_base, (uint32_t)adc->priv);

    wj_adc_enable_osc_cmplt_irq(adc_base);
    wj_adc_enable_os_eoc_irq(adc_base);
    wj_adc_os_trigger(adc_base);
    return CSI_OK;
}

static csi_error_t wj_adc_start_dma(csi_adc_t *adc)
{
    csi_dma_ch_config_t config;
    wj_adc_regs_t       *adc_base;

    adc_base = (wj_adc_regs_t *)HANDLE_REG_BASE(adc);
    memset(&config, 0, sizeof(csi_dma_ch_config_t));

    /* configure dma channel */
    config.src_inc = DMA_ADDR_CONSTANT;
    config.dst_inc = DMA_ADDR_INC;
    config.src_tw  = DMA_DATA_WIDTH_32_BITS;
    config.dst_tw  = DMA_DATA_WIDTH_32_BITS;

    config.group_len = 4U;
    config.trans_dir = DMA_PERH2MEM;
    config.handshake = 123U;
    csi_dma_ch_config(adc->dma, &config);

    wj_adc_config_os_channel_nums(adc_base, (uint32_t)adc->priv);

    /* set rx mode*/
    soc_dcache_clean_invalid_range((unsigned long)adc->data, adc->num * 4U);
    csi_dma_ch_start(adc->dma, (void *) & (adc_base->ADC_DR), adc->data, adc->num * 4U);

    wj_adc_os_trigger(adc_base);
    return CSI_OK;
}

static csi_error_t wj_adc_stop_intr(csi_adc_t *adc)
{
    wj_adc_regs_t *adc_base;
    csi_error_t   ret = CSI_OK;
    uint32_t      timestart;

    adc_base = (wj_adc_regs_t *)HANDLE_REG_BASE(adc);

    wj_adc_disable_osc_cmplt_irq(adc_base);
    wj_adc_disable_os_eoc_irq(adc_base);

    timestart = csi_tick_get_ms();

    if ((wj_adc_get_os_mode(adc_base) & WJ_ADC_SCFG_OSMC_Msk) == WJ_ADC_SCFG_OSMC_SCAN) {
        wj_adc_turn_off_conv(adc_base);

        while ((wj_adc_get_status(adc_base) & WJ_ADC_SR_BUSY_Msk) == WJ_ADC_SR_BUSY) {
            if ((csi_tick_get_ms() - timestart) > DEFUALT_WJ_ADC_OPERATION_TIMEOUT) {
                ret = CSI_TIMEOUT;
                break;
            }
        }

        while ((wj_adc_get_status(adc_base) & WJ_ADC_SR_VALID_Msk) == WJ_ADC_SR_VALID) {
            ( int32_t  )wj_adc_get_data(adc_base);
            if ((csi_tick_get_ms() - timestart) > DEFUALT_WJ_ADC_OPERATION_TIMEOUT) {
                ret = CSI_TIMEOUT;
                break;
            }
        }
    }

    return ret;
}

static csi_error_t wj_adc_stop_dma(csi_adc_t *adc)
{
    wj_adc_regs_t *adc_base;
    csi_error_t   ret = CSI_OK;
    uint32_t      timestart;

    adc_base = (wj_adc_regs_t *)HANDLE_REG_BASE(adc);

    timestart = csi_tick_get_ms();

    if ((wj_adc_get_os_mode(adc_base) & WJ_ADC_SCFG_OSMC_Msk) == WJ_ADC_SCFG_OSMC_SCAN) {
        wj_adc_turn_off_conv(adc_base);

        while ((wj_adc_get_status(adc_base) & WJ_ADC_SR_BUSY_Msk) == WJ_ADC_SR_BUSY) {
            if ((csi_tick_get_ms() - timestart) > DEFUALT_WJ_ADC_OPERATION_TIMEOUT) {
                ret = CSI_TIMEOUT;
                break;
            }
        }

        while ((wj_adc_get_status(adc_base) & WJ_ADC_SR_VALID_Msk) == WJ_ADC_SR_VALID) {
            ( int32_t  )wj_adc_get_data(adc_base);
            if ((csi_tick_get_ms() - timestart) > DEFUALT_WJ_ADC_OPERATION_TIMEOUT) {
                ret = CSI_TIMEOUT;
                break;
            }
        }
    }

    return ret;
}

csi_error_t csi_adc_init(csi_adc_t *adc, uint32_t idx)
{
    CSI_PARAM_CHK(adc, CSI_ERROR);
    csi_error_t ret = CSI_OK;
    wj_adc_regs_t *adc_base;

    // This variable used to fix hardware bug
    // It save ADC_OSVCNU register value, avoid multiple modification
    // This value will only set once at csi_adc_start()
    adc->priv = 0U;

    if (target_get(DEV_WJ_ADC_TAG, idx, &adc->dev) != CSI_OK) {
        ret = CSI_ERROR;
    }else{
        adc_base = (wj_adc_regs_t *)HANDLE_REG_BASE(adc);

        adc->state.writeable = 1U;
        adc->state.readable  = 1U;
        adc->state.error     = 0U;
        adc->callback        = NULL;
        adc->arg             = NULL;
        adc->data            = NULL;
        adc->dma             = NULL;
        adc->start           = NULL;
        adc->stop            = NULL;

        /* Reset all digital and analog register */
        wj_adc_rst_all_digital_register(adc_base);
        mdelay(1U);        
        wj_adc_set_all_digital_register_normal(adc_base);
        wj_adc_rst_all_digital_analog_register(adc_base);
        mdelay(1U);
        wj_adc_set_all_digital_analog_register_normal(adc_base);

        // Recommand use div equal 128, because sample speed faster then polling speed.
        wj_adc_config_conversion_clock_div(adc_base, 128U);

        /* Power on */
        wj_adc_power_on(adc_base);
    }

    return ret;
}

void csi_adc_uninit(csi_adc_t *adc)
{
    CSI_PARAM_CHK_NORETVAL(adc);

    wj_adc_regs_t *adc_base;

    adc_base = (wj_adc_regs_t *)HANDLE_REG_BASE(adc);

    /* unregister irq */
    csi_irq_detach((uint32_t)adc->dev.irq_num);

    /* reset all registers */
    wj_adc_reset_regs(adc_base);
    wj_adc_power_off(adc_base);
}

csi_error_t csi_adc_set_buffer(csi_adc_t *adc, uint32_t *data, uint32_t num)
{
    csi_error_t ret = CSI_OK;
    CSI_PARAM_CHK(adc, CSI_ERROR);
    CSI_PARAM_CHK(data, CSI_ERROR);
    CSI_PARAM_CHK(num, CSI_ERROR);

    if (num == 0U) {
        ret = CSI_ERROR;
    }else{
        adc->data = data;
        adc->num = num;
        ret = CSI_OK;
    }
    return ret;
}

csi_error_t csi_adc_start(csi_adc_t *adc)
{
    CSI_PARAM_CHK(adc, CSI_ERROR);

    wj_adc_regs_t *adc_base;
    csi_error_t   ret = CSI_OK;

    adc_base = (wj_adc_regs_t *)HANDLE_REG_BASE(adc);

    do {
        /* rx buffer not full */
        if (adc->state.readable == 0U) {
            ret = CSI_BUSY;
            break;
        }

        /* last conversion existed */
        if (adc->state.writeable == 0U) {
            ret = CSI_BUSY;
            break;
        }

        wj_adc_config_os_channel_nums(adc_base, (uint32_t)adc->priv);
        mdelay(10U);
        wj_adc_os_trigger(adc_base);

        if ((wj_adc_get_os_scan_mode(adc_base) & WJ_ADC_SCFG_OSMC_Msk) == WJ_ADC_SCFG_OSMC_SCAN) {
            adc->state.writeable = 0U;
        }
    } while(0);
    
    return ret;
}

csi_error_t csi_adc_stop(csi_adc_t *adc)
{
    CSI_PARAM_CHK(adc, CSI_ERROR);

    wj_adc_regs_t *adc_base;
    uint32_t      timestart;
    csi_error_t   ret = CSI_OK;

    adc_base = (wj_adc_regs_t *)HANDLE_REG_BASE(adc);

    if ((wj_adc_get_os_mode(adc_base) & WJ_ADC_SCFG_OSMC_Msk) == WJ_ADC_SCFG_OSMC_SCAN) {
        wj_adc_turn_off_conv(adc_base);

        timestart = csi_tick_get_ms();

        while ((wj_adc_get_status(adc_base) & WJ_ADC_SR_BUSY_Msk) == WJ_ADC_SR_BUSY) {
            if ((csi_tick_get_ms() - timestart) > DEFUALT_WJ_ADC_OPERATION_TIMEOUT) {
                ret = CSI_TIMEOUT;
                break;
            }
        }

        while ((wj_adc_get_status(adc_base) & WJ_ADC_SR_VALID_Msk) == WJ_ADC_SR_VALID) {
            ( int32_t  )wj_adc_get_data(adc_base);
            if ((csi_tick_get_ms() - timestart) > DEFUALT_WJ_ADC_OPERATION_TIMEOUT) {
                ret = CSI_TIMEOUT;
                break;
            }
        }
    }

    wj_adc_get_data(adc_base);
    adc->state.readable  = 1U;
    adc->state.writeable = 1U;
    adc->data = NULL;
    adc->num = 0U;
#ifdef CHIP_CETUS
/*Cetus must be delayed*/
    if(ret == CSI_OK){
        mdelay(3000);
    }
#endif
    return ret;
}

static int adc_channel_delete(uint8_t *channels, uint8_t nums, uint8_t channel)
{
    uint8_t i, j ;
    int     ret = -1;

    for (i = 0U; i < nums; i++) {
        if (channel == *channels) {
            for (j = i; j < (nums - 1U); j++) {
                channels[j] = channels[j + 1U];
            }

            ret = 0;
            break;
        }

        channels++;
    }
    return ret;
}

csi_error_t csi_adc_channel_enable(csi_adc_t *adc, uint8_t ch_id, bool is_enable)
{
    CSI_PARAM_CHK(adc, CSI_ERROR);

    uint32_t    curr_nums;
    uint8_t     channels[16];
    csi_error_t ret = CSI_OK;
    uint8_t     i = 0;

    wj_adc_regs_t *adc_base = (wj_adc_regs_t *)HANDLE_REG_BASE(adc);
    curr_nums = (uint32_t)adc->priv;

    if (is_enable) {
        VOID_P_ADD(adc->priv, 1U);
        wj_adc_config_os_channel(adc_base, curr_nums, (uint32_t)ch_id);
    } else {
        wj_adc_get_os_channels(adc_base, channels, (uint8_t*)&curr_nums);

        if (adc_channel_delete(channels, (uint8_t)curr_nums, ch_id) == 0) {
            VOID_P_DEC(adc->priv, 1U);

            // Update new channels to register
            for(i = 0; i < curr_nums - 1; i++){
                wj_adc_config_os_channel(adc_base, i, channels[i]);
            }
            wj_adc_config_os_channel_nums(adc_base, (uint32_t)adc->priv);
            mdelay(1);

            // Clear remain data
            i = 0;
            while (((wj_adc_get_status(adc_base) & WJ_ADC_SR_VALID_Msk) == WJ_ADC_SR_VALID) && (i++ < 20U)) {
                ( int32_t  )wj_adc_get_data(adc_base);
            }
        } else {
            ret = CSI_ERROR;
        }
    }

    return ret;
}

csi_error_t csi_adc_channel_sampling_time(csi_adc_t *adc, uint8_t ch_id, uint16_t clock_num)
{
    CSI_PARAM_CHK(adc, CSI_ERROR);
    return CSI_UNSUPPORTED;
}

csi_error_t csi_adc_sampling_time(csi_adc_t *adc, uint16_t clock_num)
{
    CSI_PARAM_CHK(adc, CSI_ERROR);

    csi_error_t ret = CSI_OK;

    if (clock_num == 0U) {
        ret = CSI_ERROR;
    } else {
        wj_adc_regs_t *adc_base = (wj_adc_regs_t *)HANDLE_REG_BASE(adc);
        wj_adc_config_sample_clock(adc_base, (uint32_t)clock_num);
    }

    return ret;
}


uint32_t csi_adc_freq_div(csi_adc_t *adc, uint32_t div)
{
    CSI_PARAM_CHK(adc, CSI_ERROR);
    wj_adc_regs_t *adc_base = (wj_adc_regs_t *)HANDLE_REG_BASE(adc);
    wj_adc_config_conversion_clock_div(adc_base, div);
    div = wj_adc_get_conversion_clock_div(adc_base);
    return soc_get_adc_freq((uint32_t)adc->dev.idx) / div;
}

int32_t csi_adc_read(csi_adc_t *adc)
{
    CSI_PARAM_CHK(adc, CSI_ERROR);

    uint32_t timestart;
    int32_t  ret = CSI_OK;

    wj_adc_regs_t *adc_base = (wj_adc_regs_t *)HANDLE_REG_BASE(adc);

    timestart = csi_tick_get_ms();

    while ((wj_adc_get_status(adc_base) & WJ_ADC_SR_VALID_Msk) != WJ_ADC_SR_VALID) {
        if ((csi_tick_get_ms() - timestart) > DEFUALT_WJ_ADC_OPERATION_TIMEOUT) {
            ret = CSI_TIMEOUT;
            break;
        }
    }

    if (ret == CSI_OK) {
        ret = ( int32_t )wj_adc_get_data(adc_base);
    }

    return ret;
}

csi_error_t csi_adc_get_state(csi_adc_t *adc, csi_state_t *state)
{
    CSI_PARAM_CHK(adc, CSI_ERROR);
    *state = adc->state;
    return CSI_OK;
}

uint32_t csi_adc_get_freq(csi_adc_t *adc)
{
    CSI_PARAM_CHK(adc, 0U);

    uint32_t div;

    wj_adc_regs_t *adc_base = (wj_adc_regs_t *)HANDLE_REG_BASE(adc);

    div = wj_adc_get_conversion_clock_div(adc_base);
    return soc_get_adc_freq((uint32_t)adc->dev.idx) / div;
}

csi_error_t csi_adc_attach_callback(csi_adc_t *adc, void *callback, void *arg)
{
    CSI_PARAM_CHK(adc, CSI_ERROR);
    CSI_PARAM_CHK(callback, CSI_ERROR);

    adc->callback = callback;
    adc->arg      = arg;
    adc->start    = wj_adc_start_intr;
    adc->stop     = wj_adc_stop_intr;

    csi_irq_attach((uint32_t)adc->dev.irq_num, &wj_adc_irqhandler, &adc->dev);
    csi_irq_enable((uint32_t)adc->dev.irq_num);
    return CSI_OK;

}

void csi_adc_detach_callback(csi_adc_t *adc)
{
    CSI_PARAM_CHK_NORETVAL(adc);

    adc->callback  = NULL;
    adc->arg       = NULL;
    adc->start     = NULL;
    adc->stop      = NULL;
}

csi_error_t csi_adc_start_async(csi_adc_t *adc)
{
    CSI_PARAM_CHK(adc, CSI_ERROR);

    wj_adc_regs_t *adc_base;
    csi_error_t   ret = CSI_OK;

    adc_base = (wj_adc_regs_t *)HANDLE_REG_BASE(adc);

    do{
        if ((adc->data == NULL) || (adc->num == 0U)) {
            ret = CSI_ERROR;
            break;
        }

        /* rx buffer not full */
        if (adc->state.readable == 0U) {
            ret = CSI_BUSY;
            break;
        }

        /* last conversion existed */
        if (adc->state.writeable == 0U) {
            ret = CSI_ERROR;
            break;
        }

        if (adc->start) {
            adc->start(adc);

            if ((wj_adc_get_os_scan_mode(adc_base) & WJ_ADC_SCFG_OSMC_Msk) == WJ_ADC_SCFG_OSMC_SCAN) {
                adc->state.writeable = 0U;
                adc->state.readable  = 0U;
            }
        }
    }while(0);

    return ret;
}

csi_error_t csi_adc_stop_async(csi_adc_t *adc)
{
    CSI_PARAM_CHK(adc, CSI_ERROR);

    csi_error_t ret = CSI_OK;

    if (adc->stop) {
        adc->stop(adc);
        adc->state.readable  = 1U;
        adc->state.writeable = 1U;
        adc->data = NULL;
        adc->num = 0U;
    } else {
        ret = CSI_ERROR;
    }

    return ret;
}

csi_error_t csi_adc_continue_mode(csi_adc_t *adc, bool is_enable)
{
    CSI_PARAM_CHK(adc, CSI_ERROR);

    wj_adc_regs_t *adc_base;

    adc_base = (wj_adc_regs_t *)HANDLE_REG_BASE(adc);

    if (is_enable) {
        wj_adc_set_os_scan_mode(adc_base);
    } else {
        wj_adc_set_os_single_mode(adc_base);
    }

    return CSI_OK;
}

csi_error_t csi_adc_link_dma(csi_adc_t *adc, csi_dma_ch_t *dma)
{
    CSI_PARAM_CHK(adc, CSI_ERROR);

    csi_error_t ret = CSI_OK;

    if (dma != NULL) {
        dma->parent = adc;
        ret = csi_dma_ch_alloc(dma, -1, -1);

        if (ret == CSI_OK) {
            csi_dma_ch_attach_callback(dma, wj_adc_dma_event_cb, NULL);
            adc->dma   = dma;
            adc->start = wj_adc_start_dma;
            adc->stop  = wj_adc_stop_dma;
        } else {
            dma->parent = NULL;
        }
    } else {
        if (adc->dma) {
            csi_dma_ch_free(adc->dma);
            csi_dma_ch_detach_callback(adc->dma);
            adc->dma = NULL;
        }

        if (adc->callback != NULL) {
            adc->start = wj_adc_start_intr;
            adc->stop  = wj_adc_stop_intr;
        } else {
            adc->start = NULL;
            adc->stop  = NULL;
        }
    }

    return ret;
}

#ifdef CONFIG_PM
csi_error_t dw_adc_pm_action(csi_dev_t *dev, csi_pm_dev_action_t action)
{
    CSI_PARAM_CHK(dev, CSI_ERROR);

    csi_error_t ret = CSI_OK;
    csi_pm_dev_t *pm_dev = &dev->pm_dev;
    wj_adc_regs_t *adc_base = (wj_adc_regs_t *)dev->reg_base;

    switch (action) {
        case PM_DEV_SUSPEND:
            csi_pm_dev_save_regs(pm_dev->reten_mem, (uint32_t *)dev->reg_base, 1U);
            csi_pm_dev_save_regs(pm_dev->reten_mem + 1U, (uint32_t *)(dev->reg_base + 4U), 4U);
            csi_pm_dev_save_regs(pm_dev->reten_mem + 5U, (uint32_t *)(dev->reg_base + 28U), 3U);
            break;

        case PM_DEV_RESUME:
            adc_base->ADC_CR &= ~(1U);
            csi_pm_dev_restore_regs(pm_dev->reten_mem + 1U, (uint32_t *)(dev->reg_base + 4U), 4U);
            csi_pm_dev_restore_regs(pm_dev->reten_mem + 5U, (uint32_t *)(dev->reg_base + 28U), 3U);
            csi_pm_dev_restore_regs(pm_dev->reten_mem, (uint32_t *)dev->reg_base, 1U);
            break;

        default:
            ret = CSI_ERROR;
            break;
    }

    return ret;
}

csi_error_t csi_adc_enable_pm(csi_adc_t *adc)
{
    return csi_pm_dev_register(&adc->dev, dw_adc_pm_action, 32U, 0U);
}

void csi_adc_disable_pm(csi_adc_t *adc)
{
    csi_pm_dev_unregister(&adc->dev);
}
#endif

