/**************************************************************************************************

  Phyplus Microelectronics Limited confidential and proprietary.
  All rights reserved.

  IMPORTANT: All rights of this software belong to Phyplus Microelectronics
  Limited ("Phyplus"). Your use of this Software is limited to those
  specific rights granted under  the terms of the business contract, the
  confidential agreement, the non-disclosure agreement and any other forms
  of agreements as a customer or a partner of Phyplus. You may not use this
  Software unless you agree to abide by the terms of these agreements.
  You acknowledge that the Software may not be modified, copied,
  distributed or disclosed unless embedded on a Phyplus Bluetooth Low Energy
  (BLE) integrated circuit, either as a product or is integrated into your
  products.  Other than for the aforementioned purposes, you may not use,
  reproduce, copy, prepare derivative works of, modify, distribute, perform,
  display or sell this Software and/or its documentation for any purposes.

  YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
  PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
  INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE,
  NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL
  PHYPLUS OR ITS SUBSIDIARIES BE LIABLE OR OBLIGATED UNDER CONTRACT,
  NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER
  LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
  INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE
  OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT
  OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
  (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

**************************************************************************************************/

/*******************************************************************************
* @file   adc.c
* @brief  Contains all functions support for adc driver
* @version  0.0
* @date   18. Oct. 2017
* @author qing.han
*
* Copyright(C) 2016, PhyPlus Semiconductor
* All rights reserved.
*
*******************************************************************************/
#include <string.h>
#include "error.h"
#include "gpio.h"
//#include "pwrmgr.h"
#include "clock.h"
#include "phy_adc.h"
#include "drv/irq.h"

#include "jump_function.h"

#define BM_SET(addr,bit)                ( *(addr) |= (bit) )     //bit set
#define BM_CLR(addr,bit)                ( *(addr) &= ~(bit) )    //bit clear
#define BM_IS_SET(addr,bit)             ( *(addr) & (bit) )      //judge bit is set


static adc_Cfg_t s_adc_cfg = {
	.channel = 0,	
	.is_continue_mode = TRUE,
	.is_differential_mode = 0x00,
	.is_high_resolution = 0xFF,
};

static adc_Ctx_t mAdc_Ctx;
static uint8_t  adc_cal_read_flag = 0;
static uint16_t adc_cal_postive = 0x0fff;
static uint16_t adc_cal_negtive = 0x0fff;

gpio_pin_e s_pinmap[ADC_CH_NUM] = {
    GPIO_DUMMY, //ADC_CH0 =0,
    GPIO_DUMMY, //ADC_CH1 =1,
    P11, //ADC_CH1N =2,
    P23, //ADC_CH1P =3,  ADC_CH1DIFF = 3,
    P24, //ADC_CH2N =4,
    P14, //ADC_CH2P =5,  ADC_CH2DIFF = 5,
    P15, //ADC_CH3N =6,
    P20, //ADC_CH3P =7,  ADC_CH3DIFF = 7,
    GPIO_DUMMY,  //ADC_CH_VOICE =8,
};

static void set_sampling_resolution(adc_CH_t channel, bool is_high_resolution, bool is_differential_mode)
{
    uint8_t aio = 0;
    uint8_t diff_aio = 0;

    switch (channel) {
        case ADC_CH1N_P11:
            aio = 0;
            diff_aio = 1;
            break;

        case ADC_CH1P_P23:
            aio = 1;
            diff_aio = 0;
            break;

        case ADC_CH2N_P24:
            aio = 2;
            diff_aio = 3;
            break;

        case ADC_CH2P_P14:
            aio = 3;
            diff_aio = 2;
            break;

        case ADC_CH3N_P15:
            aio = 4;
            diff_aio = 7;
            break;

        case ADC_CH3P_P20:
            aio = 7;
            diff_aio = 4;
            break;

        default:
            return;
    }

    if (is_high_resolution) {
        if (is_differential_mode) {
            subWriteReg(&(AP_AON->PMCTL2_1), (diff_aio + 8), (diff_aio + 8), 0);
            subWriteReg(&(AP_AON->PMCTL2_1), diff_aio, diff_aio, 1);
        }

        subWriteReg(&(AP_AON->PMCTL2_1), (aio + 8), (aio + 8), 0);
        subWriteReg(&(AP_AON->PMCTL2_1), aio, aio, 1);
    } else {
        if (is_differential_mode) {
            subWriteReg(&(AP_AON->PMCTL2_1), (diff_aio + 8), (diff_aio + 8), 1);
            subWriteReg(&(AP_AON->PMCTL2_1), diff_aio, diff_aio, 0);
        }

        subWriteReg(&(AP_AON->PMCTL2_1), (aio + 8), (aio + 8), 1);
        subWriteReg(&(AP_AON->PMCTL2_1), aio, aio, 0);
    }
}

static void set_sampling_resolution_auto(uint8_t channel, uint8_t is_high_resolution, uint8_t is_differential_mode)
{
    uint8_t i_channel;
    adc_CH_t a_channel;

    AP_AON->PMCTL2_1 = 0x00;

    for (i_channel = 2; i_channel < (ADC_CH_NUM - 1); i_channel++) {
        if (channel & BIT(i_channel)) {
            a_channel = (adc_CH_t)i_channel;
            set_sampling_resolution(a_channel,
                                    (is_high_resolution & BIT(i_channel)),
                                    (is_differential_mode & BIT(i_channel)));
        }
    }
}

static void set_differential_mode(void)
{
    subWriteReg(&(AP_PCRM->ANA_CTL), 8, 8, 0);
    subWriteReg(&(AP_PCRM->ANA_CTL), 11, 11, 0);
}


static void phy_disable_analog_pin(adc_CH_t channel)
{
    int index = (int)channel;
    gpio_pin_e pin = s_pinmap[index];

    if (pin == GPIO_DUMMY) {
        return;
    }

    phy_gpio_cfg_analog_io(pin, Bit_DISABLE);
    phy_gpio_pin_init(pin, GPIO_INPUT_1);      //ie=0,oen=1 set to imput
    phy_gpio_pull_set(pin, GPIO_FLOATING);   //
}

static void phy_clear_adcc_cfg(void)
{
    memset(&mAdc_Ctx, 0, sizeof(mAdc_Ctx));
}

/////////////// adc ////////////////////////////
/**************************************************************************************
 * @fn          hal_ADC_IRQHandler
 *
 * @brief       This function process for adc interrupt
 *
 * input parameters
 *
 * @param       None.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 **************************************************************************************/
void __attribute__((used)) phy_ADC_IRQHandler(void)
{

}

static void adc_wakeup_hdl(void)
{
    NVIC_SetPriority((IRQn_Type)ADCC_IRQn, IRQ_PRIO_HAL);
}

/**************************************************************************************
 * @fn          hal_adc_init
 *
 * @brief       This function process for adc initial
 *
 * input parameters
 *
 * @param       ADC_MODE_e mode: adc sample mode select;1:SAM_MANNUAL(mannual mode),0:SAM_AUTO(auto mode)
 *              ADC_CH_e adc_pin: adc pin select;ADC_CH0~ADC_CH7 and ADC_CH_VOICE
 *              ADC_SEMODE_e semode: signle-ended mode negative side enable; 1:SINGLE_END(single-ended mode) 0:DIFF(Differentail mode)
 *              IO_CONTROL_e amplitude: input signal amplitude, 0:BELOW_1V,1:UP_1V
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 **************************************************************************************/
void phy_adc_init(void)
{
    //mAdc_init_flg = TRUE;
    //hal_pwrmgr_register(MOD_ADCC, NULL, adc_wakeup_hdl);
    phy_clear_adcc_cfg();
}

int phy_adc_clock_config(adc_CLOCK_SEL_t clk)
{
    subWriteReg(0x4000F000 + 0x7c, 2, 1, clk);
    return PPlus_SUCCESS;
}

int phy_adc_start(void)
{
    mAdc_Ctx.enable = TRUE;
    //hal_pwrmgr_lock(MOD_ADCC);
    //JUMP_FUNCTION(V29_IRQ_HANDLER)                  = (uint32_t)&phy_ADC_IRQHandler;

    AP_PCRM->ANA_CTL |= BIT(3); //ENABLE_ADC;
    AP_PCRM->ANA_CTL |= BIT(0);
    
    //NVIC_EnableIRQ((IRQn_Type)ADCC_IRQn); 
    drv_irq_register(ADCC_IRQn, phy_ADC_IRQHandler);
    drv_irq_enable(ADCC_IRQn);
   
    AP_ADCC->intr_mask = 0x1ff;

    //disableSleep();
    return PPlus_SUCCESS;
}

int phy_adc_start_int_dis(void)
{
    mAdc_Ctx.enable = TRUE;
    //hal_pwrmgr_lock(MOD_ADCC);
    //JUMP_FUNCTION(V29_IRQ_HANDLER)                  =   (uint32_t)&hal_ADC_IRQHandler;

	MASK_ADC_INT;	
	drv_irq_disable(ADCC_IRQn);
	drv_irq_unregister(ADCC_IRQn);

	AP_PCRM->ANA_CTL |= BIT(3);
	AP_PCRM->ANA_CTL |= BIT(0);

    //disableSleep();
    return PPlus_SUCCESS;
}

//set_sampling_resolution_auto
static void phy_set_sampling_resolution_auto(uint8_t channel, uint8_t is_high_resolution, uint8_t is_differential_mode)
{
    uint8_t i_channel;
    adc_CH_t a_channel;

    AP_AON->PMCTL2_1 = 0x00;

    for (i_channel = 2; i_channel < (ADC_CH_NUM - 1); i_channel++) {
        if (channel & BIT(i_channel)) {
            a_channel = (adc_CH_t)i_channel;
            set_sampling_resolution(a_channel,
                                    (is_high_resolution & BIT(i_channel)),
                                    (is_differential_mode & BIT(i_channel)));
        }
    }
}

//hal_adc_config_channel
int phy_adc_config_channel(adc_Cfg_t cfg, adc_event_cb_t evt_handler)
{
    uint8_t i;
    uint8_t chn_sel = 0, evt_index = 0;
    gpio_pin_e pin = 0, pin_neg = 0;

#if 0
    if (!mAdc_init_flg) {
        return PPlus_ERR_NOT_REGISTED;
    }

    if (mAdc_Ctx.enable) {
        return PPlus_ERR_BUSY;
    }

    if (evt_handler == NULL) {
        return PPlus_ERR_INVALID_PARAM;
    }

    if (cfg.channel & BIT(0)/*||channel == ADC_CH1*/) {
        return PPlus_ERR_NOT_SUPPORTED;
    }

    if ((!cfg.channel & BIT(1)) && (cfg.is_differential_mode && (cfg.channel & BIT(1)))) {
        return PPlus_ERR_INVALID_PARAM;
    }

    if (cfg.is_differential_mode != 0) {
        if ((cfg.is_differential_mode != 0x80) && (cfg.is_differential_mode != 0x20) && (cfg.is_differential_mode != 0x08)) {
            return PPlus_ERR_INVALID_PARAM;
        }
    }
#endif

    mAdc_Ctx.continue_mode = cfg.is_continue_mode;
    mAdc_Ctx.all_channel = cfg.channel & 0x03;

    for (i = 2; i < 8; i++) {
        if (cfg.channel & BIT(i)) {
            if (i % 2) {
                mAdc_Ctx.all_channel |= BIT(i - 1);
            } else {
                mAdc_Ctx.all_channel |= BIT(i + 1);
            }
        }
    }

    if ((AP_PCR->SW_CLK & BIT(MOD_ADCC)) == 0) {
        hal_clk_gate_enable(MOD_ADCC);
    }

    //CLK_1P28M_ENABLE;
    AP_PCRM->CLKSEL |= BIT(6);

    //ENABLE_XTAL_OUTPUT;         //enable xtal 16M output,generate the 32M dll clock
    AP_PCRM->CLKHF_CTL0 |= BIT(18);

    //ENABLE_DLL;                  //enable DLL
    AP_PCRM->CLKHF_CTL1 |= BIT(7);

    //ADC_DBLE_CLOCK_DISABLE;      //disable double 32M clock,we are now use 32M clock,should enable bit<13>, diable bit<21>
    AP_PCRM->CLKHF_CTL1 &= ~BIT(21);//check
    //subWriteReg(0x4000F044,21,20,3);

    //ADC_CLOCK_ENABLE;            //adc clock enbale,always use clk_32M
    AP_PCRM->CLKHF_CTL1 |= BIT(13);

    //subWriteReg(0x4000f07c,4,4,1);    //set adc mode,1:mannual,0:auto mode
    AP_PCRM->ADC_CTL4 |= BIT(4);
    AP_PCRM->ADC_CTL4 |= BIT(0);

    phy_set_sampling_resolution_auto(cfg.channel, cfg.is_high_resolution, cfg.is_differential_mode);

    AP_PCRM->ADC_CTL0 &= ~BIT(20);
    AP_PCRM->ADC_CTL0 &= ~BIT(4);
    AP_PCRM->ADC_CTL1 &= ~BIT(20);
    AP_PCRM->ADC_CTL1 &= ~BIT(4);
    AP_PCRM->ADC_CTL2 &= ~BIT(20);
    AP_PCRM->ADC_CTL2 &= ~BIT(4);
    AP_PCRM->ADC_CTL3 &= ~BIT(20);
    AP_PCRM->ADC_CTL3 &= ~BIT(4);

    AP_PCRM->ANA_CTL &= ~BIT(23);//disable micbias

    if (cfg.is_differential_mode == 0) {
        AP_PCRM->ADC_CTL4 &= ~BIT(4); //enable auto mode

        for (i = 2; i < 8; i++) {
            if (cfg.channel & BIT(i)) {
                gpio_pin_e pin = s_pinmap[i];
                phy_gpio_ds_control(pin, Bit_ENABLE);
                phy_gpio_cfg_analog_io(pin, Bit_ENABLE);

                switch (i) {
                    case 0:
                        AP_PCRM->ADC_CTL0 |= BIT(20);
                        break;

                    case 1:
                        AP_PCRM->ADC_CTL0 |= BIT(4);
                        break;

                    case 2:
                        AP_PCRM->ADC_CTL1 |= BIT(20);
                        break;

                    case 3:
                        AP_PCRM->ADC_CTL1 |= BIT(4);
                        break;

                    case 4:
                        AP_PCRM->ADC_CTL2 |= BIT(20);
                        break;

                    case 5:
                        AP_PCRM->ADC_CTL2 |= BIT(4);
                        break;

                    case 6:
                        AP_PCRM->ADC_CTL3 |= BIT(20);
                        break;

                    case 7:
                        AP_PCRM->ADC_CTL3 |= BIT(4);
                        break;

                    default:
                        break;
                }

                mAdc_Ctx.evt_handler[i] = evt_handler;
            }
        }
    } else {
        switch (cfg.is_differential_mode) {

            case 0x80:
                pin = P20;
                pin_neg = P15;
                chn_sel = 0x04;
                evt_index = 7;
                break;

            case 0x20:
                pin = P14;
                pin_neg = P24;
                chn_sel = 0x03;
                evt_index = 5;
                break;

            case 0x08:
                pin = P23;
                pin_neg = P11;
                chn_sel = 0x02;
                evt_index = 3;
                break;

            case 0x02:
                pin = P18;
                pin_neg = P25;
                chn_sel = 0x01;
                evt_index = 1;
                *(volatile int *)(0x4000F020) = 0x0060;
                break;

            default:
                break;
        }

        phy_gpio_ds_control(pin, Bit_ENABLE);
        subWriteReg(0x4000f048, 7, 5, chn_sel);
        set_differential_mode();

        //LOG("%d %d %x\n",pin,pin_neg,*(volatile int*)0x40003800);
        phy_gpio_cfg_analog_io(pin, Bit_ENABLE);
        phy_gpio_cfg_analog_io(pin_neg, Bit_ENABLE);
        //LOG("%d %d %x\n",pin,pin_neg,*(volatile int*)0x40003800);
        mAdc_Ctx.all_channel = (cfg.is_differential_mode >> 1);
        mAdc_Ctx.evt_handler[evt_index] = evt_handler;
    }

    return PPlus_SUCCESS;
}

int phy_adc_stop(void)
{
    int i;

    //MASK_ADC_INT;
    //AP_ADCC->intr_mask = 0x1ff;

	MASK_ADC_INT;	
	drv_irq_disable(ADCC_IRQn);
	drv_irq_unregister(ADCC_IRQn);
	
    ADC_INIT_TOUT(to);
    AP_ADCC->intr_clear = 0x1FF;

    while (AP_ADCC->intr_status != 0) {
        ADC_CHECK_TOUT(to, ADC_OP_TIMEOUT, "hal_adc_clear_int_status timeout\n");
        AP_ADCC->intr_clear = 0x1FF;
    }

    //DISABLE_ADC;
    AP_PCRM->ANA_CTL &= ~BIT(3);

    //ADC_CLOCK_DISABLE;
    AP_PCRM->CLKHF_CTL1 &= ~BIT(13);

    for (i = 0; i < ADC_CH_NUM; i++) {
        if (mAdc_Ctx.evt_handler[i]) {
            phy_disable_analog_pin((adc_CH_t)i);
        }
    }

    AP_PCRM->ANA_CTL &= ~BIT(0);//Power down analog LDO
    hal_clk_gate_disable(MOD_ADCC);//disable I2C clk gated
    phy_clear_adcc_cfg();
    
//  hal_pwrmgr_unlock(MOD_ADCC);
    return PPlus_SUCCESS;
}

/**************************************************************************************
 * @fn          hal_adc_value
 *
 * @brief       This function process for get adc value
 *
 * input parameters
 *
 * @param       ADC_CH_e adc_pin: adc pin select;ADC_CH0~ADC_CH7 and ADC_CH_VOICE
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      ADC value
 **************************************************************************************/
static void phy_adc_load_calibration_value(void)
{
    if (adc_cal_read_flag == FALSE) {
        adc_cal_read_flag = TRUE;
        adc_cal_negtive = read_reg(0x11001000) & 0x0fff;
        adc_cal_postive = (read_reg(0x11001000) >> 16) & 0x0fff;
		//printf("->adc_cal_negtive:%x\n",adc_cal_negtive);
		//printf("->adc_cal_postive:%x\n",adc_cal_postive);
    }
}

int phy_adc_value_cal(adc_CH_t ch, uint16_t *buf, uint32_t size, bool high_resol, bool diff_mode)
{
    uint32_t i;
    unsigned int adc_sum = 0;
	float result = 0;
		
    for (i = 0; i < size; i++) {
        adc_sum += (buf[i] & 0xfff);
    }

    phy_adc_load_calibration_value();
	result = (800 * adc_sum) / size;//800=0.8*1000

	//printf("->[%d %d]",buf[0],adc_sum);	
    if ((adc_cal_postive != 0xfff) && (adc_cal_negtive != 0xfff)) {
        float delta = ((int)(adc_cal_postive - adc_cal_negtive)) / 2.0;

        if (ch & 0x01) {
            result = (diff_mode) ? ((result - 2048 - delta) * 2 / (adc_cal_postive + adc_cal_negtive))
                     : ((result + delta) / (adc_cal_postive + adc_cal_negtive));
        } else {
            result = (diff_mode) ? ((result - 2048 - delta) * 2 / (adc_cal_postive + adc_cal_negtive))
                     : ((result - delta) / (adc_cal_postive + adc_cal_negtive));
        }

    } else {
        result = (diff_mode) ? (result / 2048 - 1) : (result / 4096);
    }
	
    if (high_resol == FALSE) {
        result = result * 4;
    }

    return (int)result;
}


/*
*/
#include <soc.h>
#include <drv/adc.h>
#include <io.h>
#include <string.h>

#define ERR_ADC(errno) (CSI_DRV_ERRNO_ADC_BASE | errno)
#define ADC_NULL_PARAM_CHK(para) HANDLE_PARAM_CHK(para, ERR_ADC(DRV_ERROR_PARAMETER))

static ck_adc_priv_t adc_instance[CONFIG_ADC_NUM];	
static const adc_capabilities_t adc_capabilities = {
    .single = 0,
    .continuous = 1,
    .scan = 1,
    .calibration = 0,
    .comparator = 0
};

adc_handle_t drv_adc_initialize(int32_t idx, adc_event_cb_t cb_event)
{
    if (idx != 0) {
        return NULL;
    }

    ck_adc_priv_t *adc_priv = &adc_instance[idx];

    adc_priv->evt_handler = cb_event;

    hal_clk_gate_enable(MOD_ADCC);

    phy_adc_init();

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
    //hal_pwrmgr_register(MOD_ADCC,NULL,NULL);
    phy_clear_adcc_cfg();
    //hal_adc_init();

    hal_clk_gate_disable(MOD_ADCC);

    return 0;
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


static uint32_t set_channel_bit(uint32_t *channel_array, uint32_t channel_num)
{
    int i = 0;
    uint32_t channel_mask = 0;

    while (i < channel_num) {
        if (*channel_array > 7 || *channel_array < 2) {
            return ADC_CHANNEL_ERROR;
        }

        channel_mask |= BIT(*channel_array);
        channel_array++;
        i++;
    }

    return channel_mask;
}


int32_t drv_adc_config(adc_handle_t handle, adc_conf_t *config)
{
    int ret = -1;

    ADC_NULL_PARAM_CHK(handle);
    ADC_NULL_PARAM_CHK(config);
    ADC_NULL_PARAM_CHK(config->channel_array);
    ADC_NULL_PARAM_CHK(config->channel_nbr);
    ADC_NULL_PARAM_CHK(config->conv_cnt);

    ck_adc_priv_t *adc_priv = handle;
    uint32_t *ch_arry = config->channel_array;
    uint32_t ch_num = config->channel_nbr;

    adc_priv->sampling_frequency = config->sampling_time;
    adc_priv->mode = config->mode;
    adc_priv->intrp_mode = config->intrp_mode;


    adc_priv->cfg.is_continue_mode = TRUE;

    adc_CLOCK_SEL_t clk;

    if (adc_priv->sampling_frequency == 80000) {
        clk = HAL_ADC_CLOCK_80K;
    } else if (adc_priv->sampling_frequency == 160000) {
        clk = HAL_ADC_CLOCK_160K;
    } else if (adc_priv->sampling_frequency == 320000) {
        clk = HAL_ADC_CLOCK_320K;
    } else {
        adc_priv->sampling_frequency = 320000;
        clk = HAL_ADC_CLOCK_320K;
    }

    phy_adc_clock_config(clk);

    if (ch_num > 6 || ch_num == 0) {
        return -ADC_PARAM_INVALID;
    }

    adc_priv->channel = set_channel_bit(ch_arry, ch_num);

	//printf("1:%x\n",adc_priv->channel);

	adc_priv->cfg.channel = adc_priv->channel;
	adc_priv->cfg.is_continue_mode = s_adc_cfg.is_continue_mode;
	adc_priv->cfg.is_differential_mode = s_adc_cfg.is_differential_mode; 
	adc_priv->cfg.is_high_resolution = s_adc_cfg.is_high_resolution;
	
	ret = phy_adc_config_channel(adc_priv->cfg, adc_priv->evt_handler);

    if (ret != 0) {
        return -1;
    }

    return 0;
}


int32_t drv_adc_battery_config(adc_handle_t handle, adc_conf_t *config, uint32_t battery_channel_index)
{
	int ret = -1;

	ADC_NULL_PARAM_CHK(handle);
	ADC_NULL_PARAM_CHK(config);
	ADC_NULL_PARAM_CHK(config->channel_array);
	ADC_NULL_PARAM_CHK(config->channel_nbr);
	ADC_NULL_PARAM_CHK(config->conv_cnt);

	ck_adc_priv_t *adc_priv = handle;
	uint32_t *ch_arry = config->channel_array;
	uint32_t ch_num = config->channel_nbr;

	adc_priv->sampling_frequency = config->sampling_time;
	adc_priv->mode = config->mode;
	adc_priv->intrp_mode = config->intrp_mode;

	if (config->mode == ADC_SINGLE) {
		return ADC_PARAM_INVALID;
	} else if (config->mode == ADC_CONTINUOUS) {
		adc_priv->cfg.is_continue_mode = TRUE;
	} else if (config->mode == ADC_SCAN) {
		adc_priv->cfg.is_continue_mode = TRUE;
	}

	adc_CLOCK_SEL_t clk;

	if (adc_priv->sampling_frequency == 80000) {
		clk = HAL_ADC_CLOCK_80K;
	} else if (adc_priv->sampling_frequency == 160000) {
		clk = HAL_ADC_CLOCK_160K;
	} else if (adc_priv->sampling_frequency == 320000) {
		clk = HAL_ADC_CLOCK_320K;
	} else {
		adc_priv->sampling_frequency = 320000;
		clk = HAL_ADC_CLOCK_320K;
	}

	phy_adc_clock_config(clk);

	if (ch_num > 6 || ch_num == 0) {
		return -ADC_PARAM_INVALID;
	}

	adc_priv->channel = set_channel_bit(ch_arry, ch_num);

	//printf("1:%x\n",adc_priv->channel);

	adc_priv->cfg.channel = adc_priv->channel;
	adc_priv->cfg.is_continue_mode = s_adc_cfg.is_continue_mode;
	adc_priv->cfg.is_differential_mode = s_adc_cfg.is_differential_mode; 
	adc_priv->cfg.is_high_resolution = s_adc_cfg.is_high_resolution;

	if(adc_priv->channel & (1ul<<battery_channel_index))
	{
		if(adc_priv->cfg.is_differential_mode != 0x00)
			return -ADC_PARAM_INVALID;

		adc_priv->cfg.is_high_resolution &= ~(1ul<<battery_channel_index);
		phy_gpio_cfg_analog_io(s_pinmap[battery_channel_index],Bit_DISABLE);
		phy_gpio_write(s_pinmap[battery_channel_index], 1);		

		ret = phy_adc_config_channel(adc_priv->cfg, adc_priv->evt_handler);
		phy_gpio_cfg_analog_io(s_pinmap[battery_channel_index],Bit_DISABLE);

		s_adc_cfg.is_high_resolution = adc_priv->cfg.is_high_resolution;
		//printf("s_adc_cfg.is_high_resolution:%x\n",s_adc_cfg.is_high_resolution);
	}
	else
	{
		ret = phy_adc_config_channel(adc_priv->cfg, adc_priv->evt_handler);//no battery channel
	}

	if (ret != 0) {
		return -1;
	}

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
    ck_adc_priv_t *adc_priv = handle;

    if (adc_priv->intrp_mode == 1) {
        phy_adc_start();
    } else {
        phy_adc_start_int_dis();
    }

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
    phy_adc_stop();
    return 0;
}

static void wait_read_end()
{
    ;
}

#define S_US 1000000 //us
static uint32_t wait_data_ready(uint32_t sampling_frequency, int data_num, int ch_num)
{
    //uint32_t us = (S_US * data_num * ch_num + sampling_frequency - 1) / sampling_frequency;
    //udelay(us + 10);//dbg
    return 0;
}

static int read_multiple_channel_n(adc_handle_t handle, uint32_t *data, uint32_t read_len)
{
	ck_adc_priv_t *adc_priv = handle;
	uint8_t channel_mask = adc_priv->channel;;
	uint32_t adc_sum = 0;
	uint16_t adc_avg[1];
	int ch = 2,ch_cur = 0,n = 0,i = 7;
	bool high_resol;
	bool diff_mode;
	
	while (ch < 8){
		
		while (ch < 8) {
			if (channel_mask & BIT(ch)) {
				break;
			}
			ch++;
		}

		if(ch >= 8) 
			break;
		
		ch_cur=(ch%2)?(ch-1):(ch+1);

        int temp = (++i)%8;
        i = temp;
		adc_sum = 0;
		for (n = 0; n < 16; n++) {					
			adc_sum += (uint16_t)(read_reg(ADC_CH_BASE + (ch_cur * 0x80) + ((n+2) * 4))&0xfff);
			adc_sum +=  (uint16_t)((read_reg(ADC_CH_BASE + (ch_cur * 0x80) + ((n+2) * 4))>>16)&0xfff);
		}
		
		//data[i] = (uint16_t)(adc_sum>>5);	
			
		adc_avg[0] = (uint16_t)(adc_sum>>5);
		high_resol = (adc_priv->cfg.is_high_resolution   & (1ul<<ch))?TRUE:FALSE;
		diff_mode =  (adc_priv->cfg.is_differential_mode & (1ul<<ch))?TRUE:FALSE;
		//printf("[%x %x ]",s_adc_cfg.is_high_resolution,s_adc_cfg.is_differential_mode);
		//printf("[%d %d %d]",i,high_resol,diff_mode);
		data[i] = phy_adc_value_cal(ch_cur,adc_avg,1,high_resol,diff_mode);
		//printf("[%d %d %d ] ",i,data[i],ch_cur);
	
		ch++;
	}			
	return 0;
}

int32_t drv_adc_read(adc_handle_t handle, uint32_t *data, uint32_t num)
{
    ADC_NULL_PARAM_CHK(handle);
    ADC_NULL_PARAM_CHK(data);

    ck_adc_priv_t *adc_priv = handle;

    if (adc_priv->intrp_mode == TRUE) {
        wait_read_end();
        return 0;
    }

    if (adc_priv->mode == ADC_SINGLE) {
        read_multiple_channel_n(handle, data, num);
    } else if (adc_priv->mode == ADC_CONTINUOUS) {
        read_multiple_channel_n(handle, data, num);
    } else if (adc_priv->mode == ADC_SCAN) {
        read_multiple_channel_n(handle, data, num);
    }
     
    /*
    copy data from adc_data to data
    */
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
    return adc_status;
}

int32_t drv_adc_comparator_config(adc_handle_t handle, adc_cmp_conf_t *config)
{
    return ERR_ADC(DRV_ERROR_UNSUPPORTED);
}

int32_t drv_adc_power_control(adc_handle_t handle, csi_power_stat_e state)
{
    return ERR_ADC(DRV_ERROR_UNSUPPORTED);
}
