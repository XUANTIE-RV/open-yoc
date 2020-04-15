/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     ck_adc.c
 * @brief    CSI Source File for ADC Driver
 * @version  V1.0
 * @date     16. October 2017
 ******************************************************************************/
#include <csi_config.h>
#include <soc.h>
#include <csi_core.h>
#include <drv/adc.h>
#include <ck_adc.h>
#include <drv/gpio.h>
#include <drv/irq.h>
#include <io.h>
#include <string.h>
#include <drv/dmac.h>
#ifdef CONFIG_DW_DMAC
#include <dw_dmac.h>
#endif

#define ADC_START_TRY_TIMES    10
#define ADC_WAIT_DATA_VALID_TIMES  1000
#define ADC_STAB_DELAY_US    3

#define ERR_ADC(errno) (CSI_DRV_ERRNO_ADC_BASE | errno)
#define ADC_NULL_PARAM_CHK(para) HANDLE_PARAM_CHK(para, ERR_ADC(DRV_ERROR_PARAMETER))

typedef struct {
#ifdef CONFIG_LPM
    uint8_t adc_power_status;
    uint32_t adc_regs_saved[8];
#endif
    uint32_t base;
    adc_event_cb_t cb_event;
    uint32_t data_num;
#ifdef CONFIG_DW_DMAC
    int32_t dma_rx_ch;
    uint8_t dma_use;
#endif
} ck_adc_priv_t;

extern int32_t target_adc_init(uint32_t channel);
extern int32_t target_get_adc_count(void);

static ck_adc_priv_t adc_instance[CONFIG_ADC_NUM];

static const adc_capabilities_t adc_capabilities = {
    .single = 1,
    .continuous = 1,
    .scan = 1,
    .calibration = 0,
    .comparator = 1
};

#ifdef CONFIG_DW_DMAC
static void ck_adc_dma_event_cb(int32_t ch, dma_event_e event, void *cb_arg)
{
    ck_adc_priv_t *adc_priv = NULL;
    int i = 0;

    for (i = 0; i < CONFIG_ADC_NUM; i++) {
        adc_priv = &adc_instance[i];

        if ((adc_priv->dma_rx_ch == ch) && (adc_priv->dma_use == 1)) {
            break;
        }
    }

    if (event == DMA_EVENT_TRANSFER_DONE) {
        if (adc_priv->cb_event) {
            adc_priv->cb_event(i, ADC_EVENT_CONVERSION_COMPLETE);
        }
    } else if (event == DMA_EVENT_TRANSFER_ERROR) {
        if (adc_priv->cb_event) {
            adc_priv->cb_event(i, ADC_EVENT_DATA_LOST);
        }
    }

    csi_dma_stop(adc_priv->dma_rx_ch);
    csi_dma_release_channel(adc_priv->dma_rx_ch);
    adc_priv->dma_use = 0;

    return;
}
#endif

void ck_adc_irqhandler(int idx)
{
    ck_adc_priv_t *adc_priv = &adc_instance[idx];
    ck_adc_reg_t *addr = (ck_adc_reg_t *)(adc_priv->base);

    if (HAL_IS_BIT_SET(getreg32((volatile uint32_t *)CK_ADC_IEFG), ADC_CONVERSION_COMLETED_INTERRUPT_BIT)) {
        addr->ADC_IEFG &= ~(1 << ADC_CONVERSION_COMLETED_INTERRUPT_BIT);
        adc_priv->cb_event(0, ADC_EVENT_CONVERSION_COMPLETE);
    }

    if (HAL_IS_BIT_SET(getreg32((volatile uint32_t *)CK_ADC_IEFG), ADC_DATA_OVERWRITE_INTERRUPT_BIT)) {
        addr->ADC_IEFG &= ~(1 << ADC_DATA_OVERWRITE_INTERRUPT_BIT);
        adc_priv->cb_event(0, ADC_EVENT_DATA_OVERWRITE);
    }

    if (HAL_IS_BIT_SET(getreg32((volatile uint32_t *)CK_ADC_IEFG), ADC_DATA_COMPARE_RIGHT_INTERRUPT_BIT)) {
        addr->ADC_IEFG &= ~(1 << ADC_DATA_COMPARE_RIGHT_INTERRUPT_BIT);
        adc_priv->cb_event(0, ADC_EVENT_DATA_COMPARE_VALID);
    }

}

#ifdef CONFIG_LPM
static void manage_clock(adc_handle_t handle, uint8_t enable)
{
    drv_clock_manager_config(CLOCK_MANAGER_ADC, enable);
}

static void do_prepare_sleep_action(adc_handle_t handle)
{

    ck_adc_priv_t *adc_priv = (ck_adc_priv_t *)handle;
    uint32_t *abase = (uint32_t *)CSKY_ADC_CTL_BASE;
    registers_save(adc_priv->adc_regs_saved, abase, 1);
    registers_save(&adc_priv->adc_regs_saved[1], abase + 1, 4);
    registers_save(&adc_priv->adc_regs_saved[5], abase + 7, 3);
}

static void do_wakeup_sleep_action(adc_handle_t handle)
{
    ck_adc_priv_t *adc_priv = (ck_adc_priv_t *)handle;
    uint32_t *abase = (uint32_t *)CSKY_ADC_CTL_BASE;
    *(volatile uint32_t *)abase &= ~(1);
    registers_restore(abase + 1, &adc_priv->adc_regs_saved[1], 4);
    registers_restore(abase + 7, &adc_priv->adc_regs_saved[5], 3);
    registers_restore(abase, adc_priv->adc_regs_saved, 1);
}
#endif

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
#ifdef CONFIG_DW_DMAC
    adc_priv->dma_rx_ch = -1;
    adc_priv->dma_use = 0;
#endif
    ck_adc_reg_t *addr = (ck_adc_reg_t *)(adc_priv->base);

#ifdef CONFIG_LPM
    drv_adc_power_control(adc_priv, DRV_POWER_FULL);
#endif

    addr->ADC_IE |= (1 << ADC_CONVERSION_COMLETED_INTERRUPT_BIT);

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
#ifdef CONFIG_DW_DMAC
    adc_priv->dma_rx_ch = -1;
    adc_priv->dma_use = 0;
#endif

    drv_irq_disable(ADC_IRQn);
    drv_irq_unregister(ADC_IRQn);

#ifdef CONFIG_LPM
    drv_adc_power_control(adc_priv, DRV_POWER_OFF);
#endif
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
#ifdef CONFIG_LPM
    power_cb_t callback = {
        .wakeup = do_wakeup_sleep_action,
        .sleep = do_prepare_sleep_action,
        .manage_clock = manage_clock
    };
    return drv_soc_power_control(handle, state, &callback);
#else
    return ERR_ADC(DRV_ERROR_UNSUPPORTED);
#endif
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
    ADC_NULL_PARAM_CHK(config->conv_cnt);

    if ((config->trigger != 0) && (config->trigger != 1)) {
        return -ADC_PARAM_INVALID;
    }

    if ((config->intrp_mode != 0) && (config->intrp_mode != 1)) {
        return -ADC_PARAM_INVALID;
    }

    if (config->channel_nbr > (CK_ADC_CH15 + 1)) {
        return -ADC_PARAM_INVALID;
    }

    uint32_t *ch_arr = config->channel_array;
    int i = 0;

    ck_adc_priv_t *adc_priv = (ck_adc_priv_t *)handle;
    ck_adc_reg_t *addr = (ck_adc_reg_t *)(adc_priv->base);

    if (config->mode == ADC_SINGLE) {
        if (config->channel_nbr != 1) {
            return -ADC_CHANNEL_ERROR;
        }

        if (config->conv_cnt != 1) {
            return -ADC_PARAM_INVALID;
        }

        ch_arr = config->channel_array;

        if (*ch_arr > CK_ADC_CH15) {
            return -ADC_CHANNEL_ERROR;
        }

        adc_priv->data_num = 1;
        addr->ADC_CR &= ((~(1 << CK_ADC_CR_CMS_2_BIT)) & (~(1 << CK_ADC_CR_CMS_1_BIT)));
        addr->ADC_CR = ((~(0xf << CK_ADC_CR_AOIC_BASE_BIT)) & (addr->ADC_CR)) | ((*config->channel_array) << CK_ADC_CR_AOIC_BASE_BIT);
    } else if (config->mode == ADC_CONTINUOUS) {
        if (config->conv_cnt > CK_ADC_CONTINOUS_MODE_MAX_CONVERT_TIMES || config->conv_cnt == 0) {
            return -ADC_CONVERSION_INFO_ERROR;
        }

        if (config->channel_nbr != 1) {
            return -ADC_CONVERSION_INFO_ERROR;
        }

        ch_arr = config->channel_array;

        if ((*ch_arr > CK_ADC_CH15)) {
            return -ADC_CHANNEL_ERROR;
        }

        adc_priv->data_num = config->conv_cnt;
        addr->ADC_CR |= (1 << CK_ADC_CR_CMS_2_BIT);
        addr->ADC_CR &= ~(1 << CK_ADC_CR_CMS_1_BIT);
        addr->ADC_CR = ((~(0xf << CK_ADC_CR_SEQC_BASE_BIT)) & (addr->ADC_CR)) | (config->conv_cnt - 1) << CK_ADC_CR_SEQC_BASE_BIT;
        addr->ADC_CR = ((~(0xf << CK_ADC_CR_AOIC_BASE_BIT)) & (addr->ADC_CR)) | (*ch_arr) << CK_ADC_CR_AOIC_BASE_BIT;
    } else if (config->mode == ADC_SCAN) {
        if (config->conv_cnt != CK_ADC_SCAN_MODE_MAX_CONVERT_TIMES) {
            return -ADC_CONVERSION_INFO_ERROR;
        }

        if (config->channel_nbr > (CK_ADC_CH15 + 1)) {
            return -ADC_CHANNEL_ERROR;
        }

        for (i = 0; i < config->channel_nbr; i++) {
            if ((*ch_arr > CK_ADC_CH15)) {
                return -ADC_CHANNEL_ERROR;
            }

            if (i > 0) {
                if ((*ch_arr) != ((*(ch_arr - 1)) + 1)) {
                    return -ADC_CHANNEL_ERROR;
                }
            }

            ch_arr ++;
        }

        adc_priv->data_num = config->channel_nbr;
        addr->ADC_CR &= ~(1 << CK_ADC_CR_CMS_2_BIT);
        addr->ADC_CR |= (1 << CK_ADC_CR_CMS_1_BIT);
        addr->ADC_CR = ((~(0xf << CK_ADC_CR_AOIC_BASE_BIT)) & (addr->ADC_CR)) | i << CK_ADC_CR_AOIC_BASE_BIT;
    } else {
        return -ADC_MODE_ERROR;
    }

    return 0;
}

int32_t drv_adc_comparator_config(adc_handle_t handle, adc_cmp_conf_t *config)
{
    ADC_NULL_PARAM_CHK(handle);
    ADC_NULL_PARAM_CHK(config);

    ck_adc_priv_t *adc_priv = (ck_adc_priv_t *)handle;
    ck_adc_reg_t *addr = (ck_adc_reg_t *)(adc_priv->base);

    if (config->cmp_channel > CK_ADC_CH15) {
        return -ADC_CHANNEL_ERROR;
    }

    if (config->cmp_condition != 0 && config->cmp_condition != 1) {
        return ERR_ADC(DRV_ERROR_PARAMETER);
    }

    addr->ADC_CMPR = 0;
    addr->ADC_CMPR = 1 << CK_ADC_CMPR_CMPEN_BIT | config->cmp_condition << CK_ADC_CMPR_CMPCOND_BIT | config->cmp_channel << CK_ADC_CMPR_CMPCH_BASE_BIT | config->cmp_data << CK_ADC_CMPR_CMPD_BASE_BIT | config->cmp_match_cnt << CK_ADC_CMPR_CMPMATCNT_BASE_BIT;
    addr->ADC_IE |= (1 << ADC_DATA_COMPARE_RIGHT_INTERRUPT_BIT);

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

    addr->ADC_CR &= ~(1 << CK_ADC_CR_ADEN_BIT);

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

    addr->ADC_CR |= (1 << CK_ADC_CR_ADEN_BIT);
    return 0;
}

/**
  \brief       receiving data from ADC receiver.
  \param[in]   handle ADC handle to operate.
  \param[out]  data  Pointer to buffer for data to receive from ADC receiver.
  \param[in]   num   Number of data items to receive.
  \return      error code
*/
#ifdef CONFIG_DW_DMAC
int32_t drv_adc_read(adc_handle_t handle, uint32_t *data, uint32_t num)
{
    int i = 0;
    int32_t timeout = 0;
    int32_t ret = 0;

    ADC_NULL_PARAM_CHK(handle);
    ADC_NULL_PARAM_CHK(data);

    ck_adc_priv_t *adc_priv = (ck_adc_priv_t *)handle;
    ck_adc_reg_t *addr = (ck_adc_reg_t *)(adc_priv->base);

    dma_config_t config;
    memset(&config, 0, sizeof(dma_config_t));

    config.src_inc = DMA_ADDR_CONSTANT;
    config.dst_inc = DMA_ADDR_INC;
    config.src_endian = DMA_ADDR_LITTLE;
    config.dst_endian = DMA_ADDR_LITTLE;
    config.src_tw = DMA_DATAWIDTH_SIZE32;
    config.dst_tw = DMA_DATAWIDTH_SIZE32;
    config.hs_if  = CKENUM_DMA_ADC_RX;
    config.type   = DMA_PERH2MEM;

    adc_priv->dma_rx_ch = csi_dma_alloc_channel();

    if (adc_priv->dma_rx_ch == -1) {
        adc_priv->dma_use = 0;
        return ERR_ADC(DRV_ERROR_BUSY);
    }

    adc_priv->dma_use = 1;
    ret = csi_dma_config_channel(adc_priv->dma_rx_ch, &config, ck_adc_dma_event_cb, NULL);

    if (ret < 0) {
        csi_dma_release_channel(adc_priv->dma_rx_ch);
        return ERR_ADC(DRV_ERROR_BUSY);;
    }

    addr->ADC_DMACR |= 1 << CK_ADC_DMACR_BIT;

    for (i = 2; i > 0; i--) {
        if (!((num) % (2 << i))) {
            break;
        }
    }

    if (i == 0) {
        addr->ADC_DMADL = 0;
    } else {
        addr->ADC_DMADL = (2 << i) - 1;
    }

    addr->ADC_STC |= (1 << CK_ADC_STC_ADSTC_BIT);

    while (addr->ADC_SR & (1 << CK_ADC_SR_BUSY_BIT)) {
        if (timeout++ > ADC_WAIT_DATA_VALID_TIMES) {
            return ERR_ADC(DRV_ERROR_TIMEOUT);
        }
    }

    csi_dma_start(adc_priv->dma_rx_ch, (void *)&addr->ADC_DR, data, num);

    return 0;
}
#else
int32_t drv_adc_read(adc_handle_t handle, uint32_t *data, uint32_t num)
{
    ADC_NULL_PARAM_CHK(handle);
    ADC_NULL_PARAM_CHK(data);

    ck_adc_priv_t *adc_priv = (ck_adc_priv_t *)handle;
    ck_adc_reg_t *addr = (ck_adc_reg_t *)(adc_priv->base);

    uint32_t read_num = 0;
    uint32_t *pbuf = data;
    uint32_t get_mode = 0;
    uint32_t real_mode = 0;
    uint32_t timeout = 0;
    uint32_t counter = 0;

    real_mode = 3 & addr->ADC_CR >> CK_ADC_CR_CMS_1_BIT;

    if (real_mode == CK_ADC_SINGLE_MODE) {
        get_mode = ADC_SINGLE;
    } else if (real_mode == CK_ADC_SCAN_MODE) {
        get_mode = ADC_SCAN;
    } else if (real_mode == CK_ADC_CONTINUOUS) {
        get_mode = ADC_CONTINUOUS;
    } else {
        return -ADC_MODE_ERROR;
    }

    addr->ADC_STC |= (1 << CK_ADC_STC_ADSTC_BIT);

    counter = (ADC_STAB_DELAY_US * (drv_get_sys_freq() / 1000000U));

    while(counter != 0U) {
        counter--;
    }

    while (addr->ADC_SR & (1 << CK_ADC_SR_BUSY_BIT)) {
        if (timeout++ > ADC_WAIT_DATA_VALID_TIMES) {
            return ERR_ADC(DRV_ERROR_TIMEOUT);
        }
    }

    if (num != adc_priv->data_num) {
        return -ADC_PARAM_INVALID;
    }

    if (get_mode == ADC_SINGLE) {
        uint32_t i = 0;

        while (i < ADC_WAIT_DATA_VALID_TIMES) {
            if (((1 & (addr->ADC_SR >> CK_ADC_SR_VALID_BIT))) == 1) {
                break;
            }
            i ++;
        }

        *pbuf = ((1 << CK_ADC_DATA_WIDTH) - 1) & (addr->ADC_DR >> CK_ADC_DR_DATA_BASE_BIT);
        return SUCCESS;
    } else if (get_mode == ADC_CONTINUOUS) { // FIXME: same code asd ADC_SCAN
        while (read_num < adc_priv->data_num && (1 & (addr->ADC_SR >> CK_ADC_SR_VALID_BIT))) {
            if (read_num < adc_priv->data_num) {
                *pbuf ++ = ((1 << CK_ADC_DATA_WIDTH) - 1) & (addr->ADC_DR >> CK_ADC_DR_DATA_BASE_BIT);
                read_num ++;
            }
        }

        if (read_num == adc_priv->data_num) {
            return SUCCESS;
        } else if (read_num < adc_priv->data_num) {
            return -ADC_DATA_LOST;
        } else {
            return -ADC_DATA_OVERFLOW;
        }
    } else if (get_mode == ADC_SCAN) {
        while (read_num < adc_priv->data_num && (1 & (addr->ADC_SR >> CK_ADC_SR_VALID_BIT))) {
            if (read_num < adc_priv->data_num) {
                *pbuf ++ = ((1 << CK_ADC_DATA_WIDTH) - 1) & (addr->ADC_DR >> CK_ADC_DR_DATA_BASE_BIT);
                read_num ++;
            }
        }

        if (read_num == adc_priv->data_num) {
            return SUCCESS;
        } else if (read_num < adc_priv->data_num) {
            return -ADC_DATA_LOST;
        } else {
            return -ADC_DATA_OVERFLOW;
        }
    } else {
        return -ADC_MODE_ERROR;
    }

    return -ADC_MODE_ERROR;
}
#endif
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

    uint32_t get_stat = 1 & (addr->ADC_SR >> CK_ADC_SR_BUSY_BIT);

    if (get_stat == 0) {
        adc_status.complete = 1;
    } else if (get_stat == 1) {
        adc_status.busy = 1;
    }

    return adc_status;
}
