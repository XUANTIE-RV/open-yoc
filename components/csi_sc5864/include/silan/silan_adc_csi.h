/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
/******************************************************************************
 * @file     silan_adc.h
 * @brief    header file for adc driver
 * @version  V1.0
 * @date     15. October 2017
 ******************************************************************************/
#ifndef _SILAN_ADC_H_
#define _SILAN_ADC_H_
#include <stdio.h>
#include <soc.h>

typedef enum {
    SILAN_ADC_CH0    = 0,      /* PA8_WKUP_ADC0_ACMP0P */
    SILAN_ADC_CH1    = 1,      /* PA9_BOOT_ADC1_PWMFAULT */
    SILAN_ADC_CH2    = 2,      /* PA10_ADC2_TXD0 */
    SILAN_ADC_CH3    = 3,      /* PA11_ACMP0N_ADC3_RXD0 */
    SILAN_ADC_CH4    = 4,      /* PA12_PWM8_TCK_ADC4 */
    SILAN_ADC_CH5    = 5,      /* PA13_PWM9_TMS_ADC5 */
    SILAN_ADC_CH6    = 6,      /* PA14_PWM10_ADC6 */
    SILAN_ADC_CH7    = 7,      /* PA15_PWM11_ADC7 */
} SILANENUM_ADC_CHANNEL;


typedef struct {
    __IOM uint32_t adc_cfg ;
    __IM  uint32_t adc_sts;
    __IM  uint32_t adc_data;
    __IM  uint32_t adc_int_rsts;
    __IM  uint32_t adc_int_msts;
    __IOM uint32_t adc_int_msk;
    __IOM uint32_t adc_int_clr; //0x18
    uint32_t reseve[0x39];
    __IOM uint32_t adc_seq_cfg; //0x100
    uint32_t reseve1;
    __IOM uint32_t adc_seq_period;
    __IOM uint32_t adc_seq_thh;
    __IM uint32_t adc_seq_data;
} silan_adc_reg_t;

#define DISABLE                         1
#define NO_AUTO                         (1<<1)
#define CSEL(x)                         ((x&0x7)<<4)
#define BYPASS                          (1<<7)
#define START                           (1<<8)
#define ADC_START                       (1<<9)

#define INT_STAT_SEQ_CHG                (1 << 3)
#define INT_STAT_SEQ_FINISH             (1 << 2)
#define INT_STAT_SIN_FINISH             (1 << 1)
#define INT_STAT_KEYPRESSD              (1 << 0)
#endif/* _SILAN_ADC_H_ */
