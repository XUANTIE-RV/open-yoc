/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     wj_adc_ll.c
 * @brief
 * @version
 * @date     2020-03-05
 ******************************************************************************/
#include "wj_adc_ll.h"

static int wj_adc_get_os_channel(wj_adc_regs_t *adc_base, uint32_t rank, uint8_t *channel)
{
    uint32_t pos;
    int ret = 0;

    if (rank <= 16U) {
        pos = (rank % 8U) * 4U;

        if (rank > 8U) {
            *channel = (uint8_t)((adc_base->ADC_OSCHCDCFG1 >> pos) & 0xFU);
        } else {
            *channel = (uint8_t)((adc_base->ADC_OSCHCDCFG0 >> pos) & 0xFU);
        }

        ret = 0;
    }else{
        ret= -1; 
    }  

    return ret;
}

int wj_adc_get_os_channels(wj_adc_regs_t *adc_base, uint8_t *channels, uint8_t *num)
{
    uint8_t i;
    int   ret1;
    int   ret = 0;

    for (i = 0U; i < *num; i++) {
        ret1 = wj_adc_get_os_channel(adc_base, i, channels++);

        if (ret1 < 0) {
            ret = -1;
            break;
        }
    }
    return ret;
}

void wj_adc_config_conv_clock_num(wj_adc_regs_t *adc_base, uint32_t num)
{
    num = num / 2U;

    if (num < 0x1FU) {
        adc_base->AADCCFGEX &= ~WJ_ADC_AADCCFGEX_NUCLKCNV_Msk;
        adc_base->AADCCFGEX |= (num << WJ_ADC_AADCCFGEX_NUCLKCNV_Pos);
    }
}

void wj_adc_config_os_channel(wj_adc_regs_t *adc_base, uint32_t rank, uint32_t channel)
{
    uint32_t pos;

    if (rank <= 15U) {
        pos = (rank % 8U) * 4U;

        if (rank > 8U) {
            adc_base->ADC_OSCHCDCFG1 &= ~((uint32_t)0xFU << pos);
            adc_base->ADC_OSCHCDCFG1 |= (channel << pos);
        } else {
            adc_base->ADC_OSCHCDCFG0 &= ~((uint32_t)0xFU << pos);
            adc_base->ADC_OSCHCDCFG0 |= (channel << pos);
        }
    }
}

void wj_adc_config_hps_channel(wj_adc_regs_t *adc_base, uint32_t rank, uint32_t channel)
{
    uint32_t pos;

    if (rank <= 16U) {
        pos = (rank % 8U) * 4U;

        if (rank > 8U) {
            adc_base->ADC_HPSCHCDCFG1 &= ~((uint32_t)0xFU << pos);
            adc_base->ADC_HPSCHCDCFG1 |= (channel << pos);
        } else {
            adc_base->ADC_HPSCHCDCFG0 &= ~((uint32_t)0xFU << pos);
            adc_base->ADC_HPSCHCDCFG0 |= (channel << pos);
        }
    }
}

void wj_adc_config_hardware_average(wj_adc_regs_t *adc_base, uint32_t average)
{
    adc_base->ADC_CR &= ~WJ_ADC_CR_AVGC_Msk;

    switch (average) {
        case 0:
            adc_base->ADC_CR |= WJ_ADC_CR_AVGC_0;
            break;

        case 2:
            adc_base->ADC_CR |= WJ_ADC_CR_AVGC_2;
            break;

        case 4:
            adc_base->ADC_CR |= WJ_ADC_CR_AVGC_4;
            break;

        case 8:
            adc_base->ADC_CR |= WJ_ADC_CR_AVGC_8;
            break;

        case 16:
            adc_base->ADC_CR |= WJ_ADC_CR_AVGC_16;
            break;

        case 32:
            adc_base->ADC_CR |= WJ_ADC_CR_AVGC_32;
            break;

        default:
            adc_base->ADC_CR |= WJ_ADC_CR_AVGC_0;
            break;
    }
}

void wj_adc_config_data_bit(wj_adc_regs_t *adc_base, uint32_t data_bit)
{
    adc_base->ADC_CR &= ~WJ_ADC_CR_DRBC_Msk;

    switch (data_bit) {
        case 12:
            adc_base->ADC_CR |= WJ_ADC_CR_DRBC_12_BIT;
            break;

        case 11:
            adc_base->ADC_CR |= WJ_ADC_CR_DRBC_11_BIT;
            break;

        case 10:
            adc_base->ADC_CR |= WJ_ADC_CR_DRBC_10_BIT;
            break;

        case 9:
            adc_base->ADC_CR |= WJ_ADC_CR_DRBC_9_BIT;
            break;

        default:
            break;
    }
}

uint32_t wj_adc_config_conversion_clock_div(wj_adc_regs_t *adc_base, uint32_t div)
{
    adc_base->ADC_CR &= ~WJ_ADC_CR_ADCCLKDIV_Msk;
    uint32_t ret = 0U;

    switch (div) {
        case 0:
            adc_base->ADC_CR |= WJ_ADC_CR_ADCCLKDIV_NO;
            break;

        case 2:
            adc_base->ADC_CR |= WJ_ADC_CR_ADCCLKDIV_2;
            break;

        case 4:
            adc_base->ADC_CR |= WJ_ADC_CR_ADCCLKDIV_4;
            break;

        case 8:
            adc_base->ADC_CR |= WJ_ADC_CR_ADCCLKDIV_8;
            break;

        case 16:
            adc_base->ADC_CR |= WJ_ADC_CR_ADCCLKDIV_16;
            break;

        case 32:
            adc_base->ADC_CR |= WJ_ADC_CR_ADCCLKDIV_32;
            break;

        case 64:
            adc_base->ADC_CR |= WJ_ADC_CR_ADCCLKDIV_64;
            break;

        case 128:
            adc_base->ADC_CR |= WJ_ADC_CR_ADCCLKDIV_128;
            break;

        default:
            ret = 1U;
            break;

    }
    return ret;
}

uint32_t wj_adc_get_conversion_clock_div(wj_adc_regs_t *adc_base)
{
    uint32_t div = 0U;

    switch (adc_base->ADC_CR & WJ_ADC_CR_ADCCLKDIV_Msk) {
        case 0:
            adc_base->ADC_CR |= WJ_ADC_CR_ADCCLKDIV_NO;
            break;

        case WJ_ADC_CR_ADCCLKDIV_2:
            div = 2U;
            break;

        case WJ_ADC_CR_ADCCLKDIV_4:
            div = 4U;
            break;

        case WJ_ADC_CR_ADCCLKDIV_8:
            div = 8U;
            break;

        case WJ_ADC_CR_ADCCLKDIV_16:
            div = 16U;
            break;

        case WJ_ADC_CR_ADCCLKDIV_32:
            div = 32U;
            break;

        case WJ_ADC_CR_ADCCLKDIV_64:
            div = 64U;
            break;

        case WJ_ADC_CR_ADCCLKDIV_128:
            div = 128U;
            break;

        default:
            div = 1U;
            break;

    }

    return div;
}

void wj_adc_config_hpsdc_num_in_dis_continue_mode(wj_adc_regs_t *adc_base, uint32_t nums)
{
    if (nums <= 16U) {
        adc_base->ADC_SCFG &= ~WJ_ADC_SCFG_NUOHPSDCM_Msk;
        adc_base->ADC_SCFG |= ((nums - 1U) << WJ_ADC_SCFG_NUOHPSDCM_Pos);
    }
}

void wj_adc_config_osdc_num_in_dis_continue_mode(wj_adc_regs_t *adc_base, uint32_t nums)
{
    if (nums <= 16U) {
        adc_base->ADC_SCFG &= ~WJ_ADC_SCFG_NUOOSDCM_Msk;
        adc_base->ADC_SCFG |= ((nums - 1U) << WJ_ADC_SCFG_NUOOSDCM_Pos);
    }
}

void wj_adc_config_os_channel_nums(wj_adc_regs_t *adc_base, uint32_t nums)
{
    if ((nums <= 16U) & (nums > 0U)) {
        adc_base->ADC_OSVCNU &= ~WJ_ADC_OSVCNU_Msk;
        adc_base->ADC_OSVCNU |= (nums << WJ_ADC_OSVCNU_Pos);
    } else {
        adc_base->ADC_OSVCNU &= ~WJ_ADC_OSVCNU_Msk;
        adc_base->ADC_OSVCNU |= (0U << WJ_ADC_OSVCNU_Pos);
    }


}

void wj_adc_config_hps_channel_nums(wj_adc_regs_t *adc_base, uint32_t nums)
{
    if ((nums <= 16U) & (nums > 0U)) {
        adc_base->ADC_SCFG &= ~WJ_ADC_HPSVCNU_Msk;
        adc_base->ADC_SCFG |= (nums << WJ_ADC_HPSVCNU_Pos);
    } else {
        adc_base->ADC_SCFG &= ~WJ_ADC_HPSVCNU_Msk;
        adc_base->ADC_SCFG |= (0U << WJ_ADC_HPSVCNU_Pos);
    }
}

void wj_adc_config_comp_channel(wj_adc_regs_t *adc_base, uint32_t channel)
{
    if (channel <= 7U) {
        adc_base->ADC_CMPCR &= ~WJ_ADC_CMPCR_CMPCHS_Msk;
        adc_base->ADC_CMPCR |= (channel << WJ_ADC_CMPCR_CMPCHS_Pos);
    }
}

void wj_adc_reset_regs(wj_adc_regs_t *adc_base)
{
    adc_base->ADC_CR      = 0U;
    adc_base->ADC_SCFG    = 0U;
    adc_base->ADC_OSVCNU  = 0U;
    adc_base->ADC_HPSVCNU = 0U;
    adc_base->ADC_IF |= WJ_ADC_IF_OSSCEDIF;
    adc_base->ADC_IF |= WJ_ADC_IF_HPSSCEDIF;
    adc_base->ADC_IF |= WJ_ADC_IF_CMPIF;
    adc_base->ADC_IF |= WJ_ADC_IF_OVWTIF;
    adc_base->ADC_IF |= WJ_ADC_IF_HSSS_EOCIF;
    adc_base->ADC_IF |= WJ_ADC_IF_OSS_EOCIF;
    adc_base->ADC_IE  = 0U;
    adc_base->ADC_OSCHCDCFG0  = 0U;
    adc_base->ADC_OSCHCDCFG1  = 0U;
    adc_base->ADC_HPSCHCDCFG0 = 0U;
    adc_base->ADC_HPSCHCDCFG1 = 0U;
}

