/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     ck_adc.h
 * @brief    header file for adc driver
 * @version  V1.0
 * @date     15. October 2017
 ******************************************************************************/
#ifndef _CK_ADC_H_
#define _CK_ADC_H_
#include <stdio.h>
#include <soc.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ADC_BUF_CAPABILITY  16

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
    CK_ADC_CH0    = 0,      /* PA8_WKUP_ADC0_ACMP0P */
    CK_ADC_CH1    = 1,      /* PA9_BOOT_ADC1_PWMFAULT */
    CK_ADC_CH2    = 2,      /* PA10_ADC2_TXD0 */
    CK_ADC_CH3    = 3,      /* PA11_ACMP0N_ADC3_RXD0 */
    CK_ADC_CH4    = 4,      /* PA12_PWM8_TCK_ADC4 */
    CK_ADC_CH5    = 5,      /* PA13_PWM9_TMS_ADC5 */
    CK_ADC_CH6    = 6,      /* PA14_PWM10_ADC6 */
    CK_ADC_CH7    = 7,      /* PA15_PWM11_ADC7 */
    CK_ADC_CH8    = 8,      /* PA16_RXD1_ADC8 */
    CK_ADC_CH9    = 9,      /* PA17_TXD1_ADC9 */
    CK_ADC_CH10   = 10,     /* PA22_SPI1MISO_PWM0_ADC10 */
    CK_ADC_CH11   = 11,     /* PA23_SPI1MOSI_PWM1_ADC11 */
    CK_ADC_CH12   = 12,     /* PA26_CTS2_I2SWS_ADC12 */
    CK_ADC_CH13   = 13,     /* PA27_RTS2_I2SSD_ADC13 */
    CK_ADC_CH14   = 14,     /* PC0_SCL1_CTS1_PWM10_ADC14 */
    CK_ADC_CH15   = 15      /* PC1_SDA1_RTS1_PWM11_ADC15 */
} CKENUM_ADC_CHANNEL;

#define CK_ADC_CR     CSKY_ADC_CTL_BASE              /* Offset: 0x000 (R/W)  A/D control */
#define CK_ADC_DR     (CSKY_ADC_CTL_BASE + 0x4UL)    /* Offset: 0x004 (R/W)  A/D data */
#define CK_ADC_SR     (CSKY_ADC_CTL_BASE + 0x8UL)    /* Offset: 0x008 (R/W)  A/D status */
#define CK_ADC_CMPR   (CSKY_ADC_CTL_BASE + 0xcUL)    /* Offset: 0x00c (R/W)  A/D comparator */
#define CK_ADC_IE     (CSKY_ADC_CTL_BASE + 0x10UL)   /* Offset: 0x010 (R/W)  A/D interrupt enable */
#define CK_ADC_IEFG   (CSKY_ADC_CTL_BASE + 0x14UL)   /* Offset: 0x014 (R/W)  A/D interrupt falg */
#define CK_ADC_STC    (CSKY_ADC_CTL_BASE + 0x18UL)   /* Offset: 0x018 (W)    A/D start conversion */
#define CK_ADC_SAMSEL (CSKY_ADC_CTL_BASE + 0x1cUL)   /* Offset: 0x01c (R/W)  A/D sample mode select */
#define CK_ADC_DMACR  (CSKY_ADC_CTL_BASE + 0x20UL)   /* Offset: 0x020 (R/W)  A/D FIFO DMA enable */
#define CK_ADC_DMADL  (CSKY_ADC_CTL_BASE + 0x24UL)   /* Offset: 0x024 (R/W)  A/D DMA receive data level */

#define CK_ADC_CR_ADEN_BIT                0
#define CK_ADC_CR_CMS_1_BIT               1
#define CK_ADC_CR_CMS_2_BIT               2
#define CK_ADC_CR_AOIC_BASE_BIT           3
#define CK_ADC_CR_SEQC_BASE_BIT           8
#define CK_ADC_DR_DATA_BASE_BIT           0
#define CK_ADC_SR_ADEF_BIT                0
#define CK_ADC_SR_CMPF_BIT                1
#define CK_ADC_SR_BUSY_BIT                2
#define CK_ADC_SR_CCC_BASE_BIT            3
#define CK_ADC_SR_VALID_BIT               7
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
    __IOM uint32_t ADC_CR;       /* offset: 0x00 (R/W) A/D Control Register        */
    __IM uint32_t ADC_DR;        /* offset: 0x04 (R/W) A/D Data Register        */
    __IOM uint32_t ADC_SR;       /* offset: 0x08 (R/W) A/D Status Register        */
    __IOM uint32_t ADC_CMPR;     /* offset: 0x0c (R/W) A/D Comparator Control Register        */
    __IOM uint32_t ADC_IE;       /* offset: 0x10 (R/W) A/D Interrupt Enable Register        */
    __IOM uint32_t ADC_IEFG;     /* offset: 0x14 (R/W) A/D Interrupt Flag Register        */
    __IOM uint32_t ADC_STC;      /* offset: 0x18 (R/W) A/D Interrupt Flag Register        */
    __IOM uint32_t ADC_SAMSEL;   /* offset: 0x1c (R/W) A/D Conversion sample mode select        */
    __IOM uint32_t ADC_DMACR;    /* offset: 0x20 (R/W) A/D FIFO DMA Enable Register        */
    __IOM uint32_t ADC_DMADL;    /* offset: 0x2c (R/W) A/D DMA Receive Data Level Register        */
} ck_adc_reg_t;


#ifdef __cplusplus
}
#endif

#endif/* _CK_ADC_H_ */
