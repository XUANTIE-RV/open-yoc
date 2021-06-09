/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     ck_adc_v3.h
 * @brief    header file for adc driver
 * @version  V1.0
 * @date     8. May 2019
 ******************************************************************************/
#ifndef _CK_ADC_V3_H_
#define _CK_ADC_V3_H_
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

/* ADC_CR, Offset: 0x0 */
#define ADC_CR_ADC_PWD_Pos               0U
#define ADC_CR_ADC_PWD_Msk               (0x1U << ADC_CR_ADC_PWD_Pos)
#define ADC_CR_ADC_PWD_ON                ADC_CR_ADC_PWD_Msk
#define ADC_CR_ADCCLKDIV_Pos             1U
#define ADC_CR_ADCCLKDIV_Msk             (0x1FU << ADC_CR_ADCCLKDIV_Pos)
#define ADC_CR_ADCCLKDIV_DIV2            (0x1U << ADC_CR_ADCCLKDIV_Pos)
#define ADC_CR_ADCCLKDIV_DIV4            (0x2U << ADC_CR_ADCCLKDIV_Pos)
#define ADC_CR_ADCCLKDIV_DIV8            (0x3U << ADC_CR_ADCCLKDIV_Pos)
#define ADC_CR_ADCCLKDIV_DIV16           (0x4U << ADC_CR_ADCCLKDIV_Pos)
#define ADC_CR_ADCCLKDIV_DIV32           (0x5U << ADC_CR_ADCCLKDIV_Pos)
#define ADC_CR_ADCCLKDIV_DIV64           (0x6U << ADC_CR_ADCCLKDIV_Pos)
#define ADC_CR_ADCCLKDIV_DIV128          (0x7U << ADC_CR_ADCCLKDIV_Pos)
#define ADC_CR_DRBC_Pos                  13U
#define ADC_CR_DRBC_Msk                  (0x3U << ADC_CR_DRBC_Pos)
#define ADC_CR_DRBC_12BITS               (0x0U << ADC_CR_DRBC_Pos)
#define ADC_CR_DRBC_11BITS               (0x1U << ADC_CR_DRBC_Pos)
#define ADC_CR_DRBC_10BITS               (0x2U << ADC_CR_DRBC_Pos)
#define ADC_CR_DRBC_9BITS                (0x3U << ADC_CR_DRBC_Pos)
#define ADC_CR_AVGC_Pos                  16U
#define ADC_CR_AVGC_Msk                  (0x7U << ADC_CR_AVGC_Pos)
#define ADC_CR_AVGC_NO_OVERSAMPLING      (0x0U << ADC_CR_AVGC_Pos)
#define ADC_CR_AVGC_2x_OVERSAMPLING      (0x1U << ADC_CR_AVGC_Pos)
#define ADC_CR_AVGC_4x_OVERSAMPLING      (0x2U << ADC_CR_AVGC_Pos)
#define ADC_CR_AVGC_8x_OVERSAMPLING      (0x3U << ADC_CR_AVGC_Pos)
#define ADC_CR_AVGC_16x_OVERSAMPLING     (0x4U << ADC_CR_AVGC_Pos)
#define ADC_CR_AVGC_32x_OVERSAMPLING     (0x5U << ADC_CR_AVGC_Pos)
#define ADC_CR_OFSTMC_Pos                20U
#define ADC_CR_OFSTMC_Msk                (0x1U << ADC_CR_OFSTMC_Pos)
#define ADC_CR_DLRC_Pos                  23U
#define ADC_CR_DLRC_Msk                  (0x1U << ADC_CR_DLRC_Pos)
#define ADC_CR_DLRC_LEFT_ALIGN           ADC_CR_DLRC_Msk
#define ADC_CR_NUSMPCLK_Pos              25U
#define ADC_CR_NUSMPCLK_Msk              (0x1FU << ADC_CR_NUSMPCLK_Pos)
#define ADC_CR_NUSMPCLK(n)               (((n) - 1) << ADC_CR_NUSMPCLK_Pos)

/* ADC_SCFG, Offset 0x0 */
#define ADC_NUOOSDCM_Pos                 0U
#define ADC_NUOOSDCM_Msk                 (0xFU << ADC_NUOOSDCM_Pos)
#define ADC_NUOOSDCM_NUM(n)              (((n) - 1) << ADC_NUOOSDCM_Msk)
#define ADC_NUOHPSDCM_Pos                8U
#define ADC_NUOHPSDCM_Msk                (0xFU << ADC_NUOHPSDCM_Pos)
#define ADC_NUOHPSDCM_NUM(n)             (((n) - 1) << ADC_NUOOSDCM_Msk)
#define ADC_HPSMC_Pos                    12U
#define ADC_HPSMC_Msk                    (0x3U << ADC_HPSMC_Pos)
#define ADC_HPSMC_SINGLE_MODE            (0x0U << ADC_HPSMC_Pos)
#define ADC_HPSMC_DIS_CONTINUE_MODE      (0x1U << ADC_HPSMC_Pos)
#define ADC_HPSMC_CONTINUE_MODE          (0x2U << ADC_HPSMC_Pos)
#define ADC_HPSMC_SCAN_MODE              (0x3U << ADC_HPSMC_Pos)
#define ADC_OSMC_Pos                     16U
#define ADC_OSMC_Msk                     (0x3U << ADC_OSMC_Pos)
#define ADC_HPSINTOSMS_Pos               20U
#define ADC_HPSINTOSMS_Msk               (0xFU << ADC_HPSINTOSMS_Pos)
#define ADC_HPSINTOSMS_AUTO_CONVERSION   ADC_HPSINTOSMS_Msk
#define ADC_HPSAUTO_Pos                  21U
#define ADC_HPSAUTO_Msk                  (0xFU << ADC_HPSAUTO_Pos)

/* ADC_IE, Offset 0x24 */
#define ADC_IE_OSS_EOCIE_Pos             0U
#define ADC_IE_OSS_EOCIE_Msk             (0x1U << ADC_IE_OSS_EOCIE_Pos)
#define ADC_IE_OSS_EOCIE_EN              ADC_IE_OSS_EOCIE_Msk
#define ADC_IE_HSSS_EOCIE_Pos            1U
#define ADC_IE_HSSS_EOCIE_Msk            (0x1U << ADC_IE_HSSS_EOCIE_Pos)
#define ADC_IE_HSSS_EOCIE_EN             ADC_IE_HSSS_EOCIE_Msk
#define ADC_IE_OVWTIE_Pos                4U
#define ADC_IE_OVWTIE_Msk                (0x1U << ADC_IE_OVWTIE_Pos)
#define ADC_IE_OVWTIE_EN                 ADC_IE_OVWTIE_Msk
#define ADC_IE_CMPIE_Pos                 5U
#define ADC_IE_CMPIE_Msk                 (0x1U << ADC_IE_CMPIE_Pos)
#define ADC_IE_CMPIE_EN                  ADC_IE_CMPIE_Msk
#define ADC_IE_HPSSCEDIE_Pos             7U
#define ADC_IE_HPSSCEDIE_Msk             (0x1U << ADC_IE_HPSSCEDIE_Pos)
#define ADC_IE_HPSSCEDIE_EN              ADC_IE_HPSSCEDIE_Msk
#define ADC_IE_OSSCEDIE_Pos              8U
#define ADC_IE_OSSCEDIE_Msk              (0x1U << ADC_IE_OSSCEDIE_Pos)
#define ADC_IE_OSSCEDIE_EN               ADC_IE_OSSCEDIE_Msk

/* ADC_IF, Offset 0x28 */
#define ADC_OSS_EOCIF_Pos                0U
#define ADC_OSS_EOCIF_Msk                (0x1U << ADC_OSS_EOCIF_Pos)
#define ADC_OSS_HSSS_EOCIF_Pos           1U
#define ADC_OSS_HSSS_EOCIF_Msk           (0x1U << ADC_OSS_HSSS_EOCIF_Pos)
#define ADC_IE_OVWTIF_Pos                4U
#define ADC_IE_OVWTIF_Msk                (0x1U << ADC_IE_OVWTIF_Pos)
#define ADC_IE_CMPIF_Pos                 5U
#define ADC_IE_CMPIF_Msk                 (0x1U << ADC_IE_CMPIF_Pos)
#define ADC_IE_HPSSCEDIF_Pos             7U
#define ADC_IE_HPSSCEDIF_Msk             (0x1U << ADC_IE_HPSSCEDIF_Pos)
#define ADC_IE_OSSCEDIF_Pos              8U
#define ADC_IE_OSSCEDIF_Msk              (0x1U << ADC_IE_OSSCEDIF_Pos)

/* ADC_SWTC, Offset 0x2C */
#define ADC_SWTC_SWOSTR_Pos              0U
#define ADC_SWTC_SWOSTR_Msk              (0x1U << ADC_SWTC_SWOSTR_Pos)
#define ADC_SWTC_SWOSTR_EN               ADC_SWTC_SWOSTR_Msk
#define ADC_SWTC_SWHPSTR_Pos             2U
#define ADC_SWTC_SWHPSTR_Msk             (0x1U << ADC_SWTC_SWHPSTR_Pos)
#define ADC_SWTC_SWHPSTR_EN              ADC_SWTC_SWHPSTR_Msk
#define ADC_SWTC_ADCCOFF_Pos             3U
#define ADC_SWTC_ADCCOFF_Msk             (0x1U << ADC_SWTC_ADCCOFF_Pos)
#define ADC_SWTC_ADCCOFF_EN              ADC_SWTC_ADCCOFF_Msk

/* ADC_ANA_CONFIG, Offset 0x50 */
#define ADC_ANA_CONFIG_SELDO_HS_LU_Pos   0U
#define ADC_ANA_CONFIG_SELDO_HS_LU_Msk   (0x1U << ADC_ANA_CONFIG_SELDO_HS_LU_Pos)
#define ADC_ANA_CONFIG_SELVI_HD_LS_Pos   1U
#define ADC_ANA_CONFIG_SELVI_HD_LS_Msk   (0x1U << ADC_ANA_CONFIG_SELVI_HD_LS_Pos)
#define ADC_ANA_CONFIG_ADC_LATCH_M_Pos   2U
#define ADC_ANA_CONFIG_ADC_LATCH_M_Msk   (0x1U << ADC_ANA_CONFIG_ADC_LATCH_M_Pos)
#define ADC_ANA_CONFIG_ADC_BIAS_C_Pos    3U
#define ADC_ANA_CONFIG_ADC_BIAS_C_Msk    (0x7U << ADC_ANA_CONFIG_ADC_BIAS_C_Pos)
#define ADC_ANA_CONFIG_ADC_BIAS_C_VAL(n) ((n) << ADC_ANA_CONFIG_ADC_BIAS_C_Pos)




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
    __IOM uint32_t ADC_ANA_CFG;      /* offset: 0x50 (R/W) Analog configure of extend figures Register */
    __IOM uint32_t ADC_ACFG1;        /* offset: 0x54 (R/W) Analog configure of extend figures Register */
    __IM uint32_t ADC_ASTATUS;       /* offset: 0x58 (R/W) Analog configure of extend figures Register */
    uint32_t RESERVED1[0];
    __IOM uint32_t CONVERR;          /* offset: 0x60 (R/W) ADC conversion error interrupt Register */

} ck_adc_reg_t;


#endif/* _CK_ADC_V3_H_ */
