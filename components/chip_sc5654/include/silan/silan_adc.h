#ifndef __SILAN_ADC_H__
#define __SILAN_ADC_H__

#include "silan_types.h"

typedef enum {
	ADC_VREF_EXT33 = 0,
	ADC_VREF_IN24
} silan_adc_vref_t;

/*
vref_sel :
1: inner 2.4V
0: ex    3.3V

channel:0~7
*/
void silan_adc_init(silan_adc_vref_t vref);

/*
Single get 
*/
uint16_t silan_get_adc_data(uint8_t channel);
void silan_adc_pwd_deal(void);

#endif
