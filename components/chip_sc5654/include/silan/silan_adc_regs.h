#ifndef __SILAN_ADC_REGS_H__
#define __SILAN_ADC_REGS_H__
#include "silan_types.h"

typedef volatile struct adc_regs_struct
{
	uint32_t adc_cfg ;
	uint32_t adc_sts;
	uint32_t adc_data;
	uint32_t adc_int_rsts;
	uint32_t adc_int_msts;
	uint32_t adc_int_msk;
	uint32_t adc_int_clr; //0x18
	uint32_t reseve[0x39];
	uint32_t adc_seq_cfg; //0x100
	uint32_t reseve1;
	uint32_t adc_seq_period;
	uint32_t adc_seq_thh;
	uint32_t adc_seq_data;
}adc_regs_struct_t;

#define adc_regs  ((adc_regs_struct_t *)0x410C0000)

#define DISABLE   	        	1
#define NO_AUTO               	(1<<1)
#define CSEL(x)              	((x&0x7)<<4)
#define BYPASS                	(1<<7)
#define START			        (1<<8)
#define ADC_START   		    (1<<9)

#endif
