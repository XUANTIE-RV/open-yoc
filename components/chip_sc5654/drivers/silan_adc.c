#include "silan_types.h"
#include "ap1508.h"
#include "silan_printf.h"
#include "silan_adc_regs.h"
#include "silan_adc.h"
#include "silan_syscfg.h"
#include "silan_syscfg_regs.h"
#include "silan_pmu.h"
#include "silan_pmu_regs.h"

uint16_t silan_get_adc_data(uint8_t channel)
{
	uint32_t tmp;
	uint32_t time_out;
	volatile uint32_t wait_loop;
	//reset
	__REG32(SILAN_SYSCFG_REG9)&=~(0x3<<24);
  	__REG32(SILAN_SYSCFG_REG9)|=0x3<<24;
	//adc pwd
	adc_regs->adc_cfg  = 1;
	//clear adc pwd
	 adc_regs->adc_cfg  = 0|BYPASS;
	//config channel
	tmp = adc_regs->adc_cfg;
	tmp &= ~CSEL(0x7);
	tmp |= CSEL(channel);
	adc_regs->adc_cfg = tmp;
	//enter lowfreq mode ,wait_loop= 1024/current_freq*low_freq
	for(wait_loop=1024;wait_loop>0;wait_loop--);        //delay 1024 cycles

	adc_regs->adc_cfg |= START;
	adc_regs->adc_cfg |= ADC_START;
	//enter lowfreq mode ,wait_loop= 1024/current_freq*low_freq
	for(wait_loop=1024;wait_loop>0;wait_loop--);        //delay 1024 cycles
	adc_regs->adc_cfg &= ~ADC_START;                    //启动转换

	time_out=0x1000;
	while(1){
		time_out--;
		if(adc_regs->adc_int_rsts&0x02)                   //转换结束
			break;
		if(!time_out){
			SL_LOG("get adc data timer out");
			break;
		}
	};
	tmp = adc_regs->adc_data;
	//clear flag
	adc_regs->adc_int_clr = 0x2;
	//	SL_LOG("ADC SINGLE ,data = %x",tmp);
	return tmp;
}

void silan_adc_init(silan_adc_vref_t vref)
{
	silan_adc_cclk_config(ADC_CCLK_PLLREF_DIV2, CLK_ON);   // sar adc fs   3:40kHz   2:6MHz

	silan_pmu_wakeup_disable(PMU_WAKEUP_KEY);
	silan_sar_adc_vref_select(vref);

	silan_soft_rst(SOFT_RST_ADC);
}

void silan_adc_pwd_deal(void)
{
	adc_regs->adc_cfg  = 1;
}

