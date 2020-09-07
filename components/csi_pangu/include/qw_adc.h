/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */
/******************************************************************************
 * @file     qw_adc.h
 * @brief    header file for adc driver
 * @version  V1.0
 * @date     29. April 2019
 ******************************************************************************/
#ifndef _QW_ADC_H_
#define _QW_ADC_H_

#define ADC_PARAM_INVALID           1
#define ADC_USER_BUFFER_LACK        2
#define ADC_MODE_ERROR              3
#define ADC_CHANNEL_ERROR           4
#define ADC_CONVERSION_INFO_ERROR   5
#define ADC_DATA_LOST               6
#define ADC_DATA_OVERFLOW           7

#define ADC_BUF_CAPABILITY  16

#define QW_ADC_CR_MODE_SEL_BIT            1
#define QW_ADC_CR_RESET_BIT               3
#define QW_ADC_CR_P2S_EN_BIT              4

/* channel enable */
#define QW_ADC_CHA_VLD_BIT                0
#define QW_ADC_CHB_VLD_BIT                1
#define QW_ADC_CHC_VLD_BIT                2
#define QW_ADC_CHD_VLD_BIT                3
#define QW_ADC_CHE_VLD_BIT                4
#define QW_ADC_CHF_VLD_BIT                5
#define QW_ADC_CHG_VLD_BIT                6
#define QW_ADC_CHH_VLD_BIT                7

/* interrupt enable */
#define QW_ADC_IC_EN_CHA_BIT              0
#define QW_ADC_IC_EN_CHB_BIT              1
#define QW_ADC_IC_EN_CHC_BIT              2
#define QW_ADC_IC_EN_CHD_BIT              3
#define QW_ADC_IC_EN_CHE_BIT              4
#define QW_ADC_IC_EN_CHF_BIT              5
#define QW_ADC_IC_EN_CHG_BIT              6
#define QW_ADC_IC_EN_CHH_BIT              7

/* ADC state */
#define QW_ADC_SR_BUSY_BIT                0

typedef enum {
    QW_ADC_CH_A    = 0,
    QW_ADC_CH_B    = 1,
    QW_ADC_CH_C    = 2,
    QW_ADC_CH_D    = 3,
    QW_ADC_CH_E    = 4,
    QW_ADC_CH_F    = 5,
    QW_ADC_CH_G    = 6,
    QW_ADC_CH_H    = 7,
} QWENUM_ADC_CHANNEL;

typedef struct {
    __IOM uint32_t ADC_CTRL;         /* offset: 0x00 (R/W) A/D Control Register */
    __IOM uint32_t ADC_BAUDR;        /* offset: 0x04 (R/W) A/D Clock baudr */
    __IOM uint32_t ADC_PD_SEOC_TIME; /* offset: 0x08 (R/W) A/D Power down clock set */
    __IOM uint32_t ADC_SOC_WIDTH;    /* offset: 0x0c (R/W) A/D Soc width set */
    __IOM uint32_t ADC_SCAN_TIME;    /* offset: 0x10 (R/W) A/D Scan time interval set (only PD use) */
    __IOM uint32_t ADC_CH_VLD;       /* offset: 0x14 (R/W) A/D Channel valid register */
    __IOM uint32_t ADC_INTR_EN;      /* offset: 0x18 (R/W) A/D Interrupt enable */
    __IOM uint32_t ADC_INTR_CONFIG;  /* offset: 0x1c (R/W) A/D Interrupt config */
    __IOM uint32_t ADC_INTR_STA;     /* offset: 0x20 (R/W) A/D Interrupt state */
    __OM  uint32_t ADC_CTRL_START;   /* offset: 0x24 ( /W) A/D Start register */
    __OM  uint32_t ADC_CTRL_STOP;    /* offset: 0x28 ( /W) A/D Stop register */
    __IM  uint32_t ADC_DATA_AB;      /* offset: 0x2c (R/ ) A/D A/B channel data */
    __IM  uint32_t ADC_DATA_CD;      /* offset: 0x30 (R/ ) A/D C/D channel data */
    __IM  uint32_t ADC_DATA_EF;      /* offset: 0x34 (R/ ) A/D E/F channel data */
    __IM  uint32_t ADC_DATA_GH;      /* offset: 0x38 (R/ ) A/D G/H channel data */
    __IOM uint32_t ADC_CTRL_STATE;   /* offset: 0x3c (R/W) A/D Control state */
    __IM  uint32_t ADC_CTRL_VERID;   /* offset: 0x40 (R/ ) A/D Control verid */
    __IM  uint32_t ADC_DIR_DAT_OP;   /* offset: 0x44 (R/ ) A/D Driect data */
} qw_adc_reg_t;

#endif/* _QW_ADC_H_ */
