/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
/******************************************************************************
 * @file     ck_adc.h
 * @brief    header file for adc driver
 * @version  V1.0
 * @date     10. December 2020
 ******************************************************************************/
#ifndef _CK_ADC_H_
#define _CK_ADC_H_
#include <stdio.h>
#include <soc.h>

#define ADC_BUF_CAPABILITY  16

#define SUCCESS    0
#define ADC_PARAM_INVALID           1
#define ADC_USER_BUFFER_LACK        2
#define ADC_MODE_ERROR              3
#define ADC_CHANNEL_ERROR           4
#define ADC_CONVERSION_INFO_ERROR   5


#define ADC_DATA_COMPARE_RIGHT_INTERRUPT_BIT     2
#define ADC_DATA_OVERWRITE_INTERRUPT_BIT         1
#define ADC_CONVERSION_COMLETED_INTERRUPT_BIT    0

#define CK_ADC_SINGLE_MODE  0
#define CK_ADC_SCAN_MODE    1
#define CK_ADC_CONTINUOUS   2

#define CK_ADC_DATA_WIDTH   18

#define CK_ADC_CONTINOUS_MODE_MAX_CONVERT_TIMES    16
#define CK_ADC_SCAN_MODE_MAX_CONVERT_TIMES    1

typedef enum {
    CK_ADC_CH0    = 0,      /* PA1_WKUP_ADC0_ACMP0P */
    CK_ADC_CH1    = 1,      /* PA4_BOOT_ADC1_PWMFAULT */
} CKENUM_ADC_CHANNEL;


#define CK_ADC_DR     	  CSKY_ADC_CTL_BASE              /* Offset: 0x000 (R/W)  A/D data */
#define CK_ADC_ANA_CR     (CSKY_ADC_CTL_BASE + 0x4UL)    /* Offset: 0x004 (R/W)  A/D channel control */
#define CK_ADC_PGA_CR     (CSKY_ADC_CTL_BASE + 0x8UL)    /* Offset: 0x008 (R/W)  A/D pga control */
#define CK_ADC_TEMP_CR    (CSKY_ADC_CTL_BASE + 0xcUL)    /* Offset: 0x00c (R/W)  A/D tempertaure control */
#define CK_ADC_CR         (CSKY_ADC_CTL_BASE + 0x10UL)   /* Offset: 0x010 (R/W)  A/D control */
#define CK_ADC_INT_SR     (CSKY_ADC_CTL_BASE + 0x14UL)   /* Offset: 0x014 (R/W)  A/D interrupt status */
#define CK_ADC_CMP_VAL    (CSKY_ADC_CTL_BASE + 0x18UL)   /* Offset: 0x018 (R/W)  A/D compare value */

/*ADC Result*/
#define CK_ADC_DR_MASK					(0x3FFFC)
#define CK_ADC_DR_VAL(n)				((n)&ADC_RESULT_MASK)

/*ADC_ANALOG_CTRL*/
#define CK_ADC_CHL_SEL_MASK 		(0xF<<8)
#define CK_ADC_CHL_SEL(n)	 		((n)<<8)

#define CK_ADC_PD_ADC_MASK             	(0x1<<2)
#define CK_ADC_PD_ADC_VAL(n)           	((n)<<2)	/*1:pd adc, 0: normal work*/

#define CK_ADC_RSTN_ADC_MASK           	(0x1<<1)
#define CK_ADC_RSTN_ADC_VAL(n)          ((n)<<1)   /*1:normal work, 0:adc reset*/

#define CK_ADC_EN_LDO_ADC_MASK         	(0x1<<0)
#define CK_ADC_EN_LDO_ADC_VAL(n)       	((n)<<0)	/*1:ldo work, 0: ldo shutdown*/

/*PGA_CTRL*/
#define CK_ADC_PGA_CHOP_MASK			(0x7<<4)
#define CK_ADC_PGA_CHOP_VAL(n)			((n)<<4)


#define CK_ADC_PGA_GAIN_MASK				(0x3<<7)
#define CK_ADC_PGA_GAIN_VAL(n)			((n)<<7)


#define CK_ADC_PGA_BYPASS_MASK					(0x1<<3)
#define CK_ADC_PGA_BYPASS_VAL(n)				((n)<<3)   /*1:bypass pga, 0:use pga*/

#define CK_ADC_PGA_BYPASS_INNER_REF_SEL			(0x1<<2)   /*Internal or external reference select*/

#define CK_ADC_PGA_CHOP_ENP_MASK				(0x1<<1)
#define CK_ADC_PGA_CHOP_ENP_VAL(n)				((n)<<1)   /*1: enable chop, 0: disable chop*/

#define CK_ADC_PGA_EN_MASK						(0x1<<0)
#define CK_ADC_PGA_EN_VAL(n)					((n)<<0)   /*1: enable pga, 0: disable pga*/


/*Temperature Control*/
#define CK_ADC_TEMP_GAIN_MASK					(0x3<<4)
#define CK_ADC_TEMP_GAIN_VAL(n)				((n)<<4)

#define CK_ADC_TEMP_CAL_OFFSET_MASK			(0x1<<1)

#define CK_ADC_TEMP_EN_MASK					(0x1<<0)
#define CK_ADC_TEMP_EN_VAL(n)					((n)<<0)  /*1: enable temperature, 0: disable temperature*/


/*ADC CTRL*/
#define CK_ADC_ANALOG_SWITCH_TIME_MASK			(0x3FF<<20)
#define CK_ADC_ANALOG_SWITCH_TIME_VAL(n)		(((n)&0x3FF)<<20)

#define CK_ADC_ANALOG_INIT_TIME_MASK			(0x3FF<<8)
#define CK_ADC_ANALOG_INIT_TIME_VAL(n)			(((n)&0x3FF)<<8)

#define CK_ADC_CMP_POLAR_MASK                  (0x1<<6)

#define CK_ADC_CMP_IRQ_EN_MASK                 (0x1<<5)
#define CK_ADC_CMP_IRQ_EN_VAL(n)				((n)<<5)  /*1: enable cmp irq, 0: disable cmp irq*/


#define CK_ADC_CMP_EN_MASK                 	(0x1<<4)
#define CK_ADC_CMP_EN_VAL(n)					((n)<<4) /*1: enable cmp function, 0: disable cmp function*/

#define CK_ADC_ADC_IRQ_EN_MASK                 (0x1<<1)
#define CK_ADC_ADC_IRQ_EN_VAL(n)				((n)<<1)   /*1:enable adc transfer irq, 0: disable*/

#define CK_ADC_ADC_DMA_EN_MASK                 (0x1<<0)
#define CK_ADC_ADC_DMA_EN_VAL(n)				((n)<<0)   /*1:enable adc dma, 0: disable*/


/*ADC IRQ Status*/
#define CK_ADC_CMP_INT_MASK					(0x1<<1)

#define CK_ADC_ADC_INT_MASK					(0x1<<0)

/*CMP Value*/
#define CK_ADC_INPUT_CMP_VAL(n)		((n)&0x3FFFF)


/*ADC Channel*/
#define CK_ADC_CHL_OFFSET			(0x0E)
#define CK_ADC_CHL_VOLT				(0x0D)
#define CK_ADC_CHL_TEMP				(0x0C)



#define ADC_INT_TYPE_ADC				0
#define ADC_INT_TYPE_DMA				1
#define ADC_INT_TYPE_ADC_COMP 			2

#define ADC_REFERENCE_EXTERNAL  		0       //外部参考
#define ADC_REFERENCE_INTERNAL  		1       //内部参考


typedef struct {
    __IOM uint32_t ADC_DR;       /* offset: 0x00 (R/W) A/D Data Register        */
    __IOM uint32_t  ADC_ANA_CR;   /* offset: 0x04 (R/W) A/D channel control Register        */
    __IOM uint32_t ADC_PGA_CR;   /* offset: 0x08 (R/W) A/D PGA control Register        */
    __IOM uint32_t ADC_TEMP_CR;  /* offset: 0x0c (R/W) A/D Temperature Control Register        */
    __IOM uint32_t ADC_CR;       /* offset: 0x10 (R/W) A/D Control Register        */
    __IOM uint32_t ADC_INT_SR;   /* offset: 0x14 (R/W) A/D Interrupt Status Register        */
    __IOM uint32_t ADC_CMP_VAL;  /* offset: 0x18 (R/W) A/D Compare Register        */
} ck_adc_reg_t;


#endif/* _CK_ADC_H_ */
