/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     ck_adc_v2.h
 * @brief    header file for adc driver
 * @version  V1.0
 * @date     8. May 2019
 ******************************************************************************/
#ifndef _CK_ADC_V2_H_
#define _CK_ADC_V2_H_
#include <stdio.h>
#include <soc.h>

#define ADC_BUF_CAPABILITY    1

#define SUCCESS    0
#define ADC_PARAM_INVALID           1
#define ADC_USER_BUFFER_LACK        2
#define ADC_MODE_ERROR              3
#define ADC_CHANNEL_ERROR           4
#define ADC_CONVERSION_INFO_ERROR   5
#define ADC_DATA_LOST               6
#define ADC_DATA_OVERFLOW           7

#define ADC_DATA_COMPARE_RIGHT_INTERRUPT_BIT     2
#define ADC_DATA_OVERWRITE_INTERRUPT_BIT         1
#define ADC_CONVERSION_COMLETED_INTERRUPT_BIT    0

#define CK_ADC_SINGLE_MODE  0
#define CK_ADC_SCAN_MODE    1
#define CK_ADC_CONTINUOUS   2

#define CK_ADC_DATA_WIDTH   12

#define CK_ADC_CONTINOUS_MODE_MAX_CONVERT_TIMES    16
#define CK_ADC_SCAN_MODE_MAX_CONVERT_TIMES    1

typedef enum {
    CK_ADC_CH0    = 0,      /* */
    CK_ADC_CH1    = 1,      /* */
    CK_ADC_CH2    = 2,      /* */
    CK_ADC_CH3    = 3,      /* */
    CK_ADC_CH4    = 4,      /* */
    CK_ADC_CH5    = 5,      /* */
    CK_ADC_CH6    = 6,      /* */
    CK_ADC_CH7    = 7,      /* */
    CK_ADC_CH8    = 8,      /* */
    CK_ADC_CH9    = 9,      /* */
    CK_ADC_CH10   = 10,      /* */
    CK_ADC_CH11   = 11,      /* */
    CK_ADC_CH12   = 12,      /* */
    CK_ADC_CH13   = 13,      /* */
    CK_ADC_CH14   = 14,      /* */
    CK_ADC_CH15   = 15      /* */
} CKENUM_ADC_CHANNEL;

#define CK_ADC_CR_ADEN_BIT                0
#define CK_ADC_CR_CMS_1_BIT               1
#define CK_ADC_CR_CMS_2_BIT               2
#define CK_ADC_CR_AOIC_BASE_BIT           3
#define CK_ADC_CR_SEQC_BASE_BIT           8
#define CK_ADC_DR_DATA_BASE_BIT           0
#define CK_ADC_SR_ADEF_BIT                0
#define CK_ADC_SR_CMPF_BIT                1
#define CK_ADC_SR_BUSY_BIT                15
#define CK_ADC_SR_CCC_BASE_BIT            3
#define CK_ADC_SR_VALID_BIT               27
#define CK_ADC_SR_OVERW                   8
#define CK_ADC_CMPR_CMPEN_BIT             0
#define CK_ADC_CMPR_CMPCOND_BIT           1
#define CK_ADC_CMPR_CMPCH_BASE_BIT        2
#define CK_ADC_CMPR_CMPMATCNT_BASE_BIT    6
#define CK_ADC_CMPR_CMPD_BASE_BIT         10
#define CK_ADC_STC_ADSTC_BIT              0
#define CK_ADC_SAMSEL_BIT                 0
#define CK_ADC_DMACR_BIT                  0
#define CK_ADC_DMADL_BASE_BIT             0


#define CK_ADC_VREFP    3.3    /* Unit: volt*/


typedef struct {
    __IOM uint32_t ADC_CR;           /* offset: 0x00 (R/W) A/D Control Register        */
    __IOM uint32_t ADC_SCFG;         /* offset: 0x04 (R/W) A/D Sequence config Register        */
    __IOM uint32_t ADC_OSVCNU;       /* offset: 0x08 (R/W) A/D ADC order sequence0 valid channel number control Register        */
    __IOM uint32_t ADC_HPSVCNU;      /* offset: 0x0c (R/W) A/D ADC order sequence0 valid channel number control Register        */
    __IOM uint32_t ADC_SR;           /* offset: 0x10 (R/W) A/D Status Register        */
    __IM uint32_t ADC_DR;            /* offset: 0x14 (R) A/D Data Register        */
    __IOM uint32_t ADC_CMPCR;        /* offset: 0x18 (R/W) A/D Comparator Control Register        */
    __IOM uint32_t ADC_CMPR0;        /* offset: 0x1c (R/W) A/D Comparator Control Register        */
    __IOM uint32_t ADC_CMPR1;        /* offset: 0x20 (R/W) A/D Comparator Control Register        */
    __IOM uint32_t ADC_IE;           /* offset: 0x24 (R/W) A/D Interrupt Enable Register        */
    __IOM uint32_t ADC_IF;           /* offset: 0x28 (R/W) A/D Interrupt Flag Register        */
    __OM uint32_t ADC_SWTC;          /* offset: 0x2c (W) A/D software starting Register        */
    __IOM uint32_t OFFSETDR;         /* offset: 0x30 (R/W) A/D conversion result offset data Register */
    uint32_t RESERVED0[2];
    __IOM uint32_t AADCCFGEX;        /* offset: 0x3c (R/W) Analog configure of extend figures Register */
    __IOM uint32_t ADC_OSCHCDCFG0;   /* offset: 0x40 (R/W) Analog configure of extend figures Register */
    __IOM uint32_t ADC_OSCHCDCFG1;   /* offset: 0x44 (R/W) Analog configure of extend figures Register */
    __IOM uint32_t ADC_HPSCHCDCFG0;  /* offset: 0x48 (R/W) Analog configure of extend figures Register */
    __IOM uint32_t ADC_HPSCHCDCFG1;  /* offset: 0x4c (R/W) Analog configure of extend figures Register */
    __IOM uint32_t ADC_ACFG0;        /* offset: 0x50 (R/W) Analog configure of extend figures Register */
    __IOM uint32_t ADC_ACFG1;        /* offset: 0x54 (R/W) Analog configure of extend figures Register */
    __IM uint32_t ADC_ASTATUS;       /* offset: 0x58 (R/W) Analog configure of extend figures Register */
    uint32_t RESERVED1[0];
    __IOM uint32_t CONVERR;          /* offset: 0x60 (R/W) ADC conversion error interrupt Register */

} ck_adc_reg_t;


#endif/* _CK_ADC_V2_H_ */
