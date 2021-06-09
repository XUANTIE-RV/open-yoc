#include "aos/hal/adc.h"
#include "sdk_default_config.h"
#include "field_manipulate.h"
#include "reg_rcc.h"
#include "reg_lsadc.h"
#include <stddef.h>
#include "sdk_default_config.h"
#include "le501x.h"
#include "io_config.h"
#include "reg_syscfg.h"

enum adc_reg
{
    ADC_VBAT_CFG,
    ADC_IO_CFG,
};

uint32_t g_ref_vbat = 0;

static void set_adc_channl_pin(uint32_t adc_channl)
{
    switch (adc_channl)
    {
    case 0:
        adc12b_in0_io_init();
        break;
    case 1:
        adc12b_in1_io_init();
        break;
    case 2:
        adc12b_in2_io_init();
        break;
    case 3:
        adc12b_in3_io_init();
        break;
    case 4:
        adc12b_in4_io_init();
        break;
    case 5:
        adc12b_in5_io_init();
        break;
    case 6:
        adc12b_in6_io_init();
        break;
    case 7:
        adc12b_in7_io_init();
        break;
    case 8:
        adc12b_in8_io_init();
        break;
    default:
        break;
    }

}

void adc_reg_init(enum adc_reg adc_cfg)
{
    uint32_t tmp_cr2,tmp_ccr;
    REG_FIELD_WR(RCC->APB2EN,  RCC_ADC, 0);
    REG_FIELD_WR(RCC->APB2EN,  RCC_ADC, 1);

    tmp_cr2 = FIELD_BUILD(ADC_CONT, 1) | FIELD_BUILD(ADC_BATADJ, 0)| FIELD_BUILD(ADC_BINRES, 1) |
              FIELD_BUILD(ADC_BINBUF, 1) |FIELD_BUILD(ADC_EINBUF, 0) |FIELD_BUILD(ADC_TEST, 0) |FIELD_BUILD(ADC_DIFF, 0);
    
    MODIFY_REG(LSADC->CR2, ADC_CONT_MASK |ADC_BATADJ_MASK |ADC_BINRES_MASK |
               ADC_BINBUF_MASK |ADC_EINBUF_MASK |ADC_TEST_MASK |ADC_DIFF_MASK,
               tmp_cr2);

    if(adc_cfg == ADC_VBAT_CFG){
        tmp_ccr = FIELD_BUILD(ADC_MSBCAL, 2)  | FIELD_BUILD(ADC_VRPS, 4) | FIELD_BUILD(ADC_VRBUFEN, 1) |
                  FIELD_BUILD(ADC_BP, 0)      | FIELD_BUILD(ADC_VCMEN, 1)| FIELD_BUILD(ADC_VREFEN, 1)  |
                  FIELD_BUILD(ADC_LPCTL, 1)   | FIELD_BUILD(ADC_GCALV, 0)| FIELD_BUILD(ADC_OCALV, 0)   |
                  FIELD_BUILD(ADC_CKDIV, 4);
    }
    else{
        tmp_ccr = FIELD_BUILD(ADC_MSBCAL, 2)  | FIELD_BUILD(ADC_VRPS, 1) | FIELD_BUILD(ADC_VRBUFEN, 0) |
                  FIELD_BUILD(ADC_BP, 1)      | FIELD_BUILD(ADC_VCMEN, 1)| FIELD_BUILD(ADC_VREFEN, 0)  |
                  FIELD_BUILD(ADC_LPCTL, 1)   | FIELD_BUILD(ADC_GCALV, 0)| FIELD_BUILD(ADC_OCALV, 0)   |
                  FIELD_BUILD(ADC_CKDIV, 4);
    }
    MODIFY_REG(LSADC->CCR, ADC_MSBCAL_MASK| ADC_VRPS_MASK | ADC_VRBUFEN_MASK |ADC_BP_MASK |
               ADC_VCMEN_MASK | ADC_LPCTL_MASK |ADC_GCALV_MASK | ADC_OCALV_MASK   |ADC_CKDIV_MASK,
               tmp_ccr);
}

uint32_t get_vbat_ref(void)
{
    uint32_t vbat_value;
    uint32_t g_adc_value;
    adc_reg_init(ADC_VBAT_CFG);

    REG_FIELD_WR(LSADC->SMPR1, ADC_SMP10, 3); //select adc sample
    REG_FIELD_WR(LSADC->RSQR1, ADC_RSQ1, 10);

    REG_FIELD_WR(LSADC->CR2,ADC_BATADJ, 0x02);  //3/8 VBAT
    REG_FIELD_WR(SYSCFG->PMU_TRIM,SYSCFG_EN_BAT_DET,1);  //enable VBAT test

    REG_FIELD_WR(LSADC->CR2,ADC_ADEN,1);
    for(uint8_t i=0;i<3;i++){
        REG_FIELD_WR(LSADC->SFCR, ADC_REOSC,0);
        REG_FIELD_WR(LSADC->CR2, ADC_RTRIG,1);
        while( REG_FIELD_RD(LSADC->SR,ADC_REOC) != 1);
        REG_FIELD_WR(LSADC->SFCR, ADC_REOCC,1);
        REG_FIELD_WR(LSADC->SFCR, ADC_RSTRTCC,1);
        g_adc_value = REG_FIELD_RD(LSADC->RDR,ADC_RDATA);
    }
    vbat_value = g_adc_value*175/192;//g_adc_value*(1400/4096)*(8/3)

    return vbat_value;
}

int hal_adc_init(adc_dev_t *adc)
{
    
    if (adc == NULL) {
        return -1;
    }
    if(adc->port > 8){
        return -1;
    }

    g_ref_vbat = get_vbat_ref();
    set_adc_channl_pin(adc->port);
    adc_reg_init(ADC_IO_CFG);
    if(adc->config.sampling_cycle == 1 )
        LSADC->SMPR1 = 0 << (adc->port * 2);
    else if(adc->config.sampling_cycle == 2)
        LSADC->SMPR1 = 1 << (adc->port * 2);
    else if(adc->config.sampling_cycle ==3 )
        LSADC->SMPR1 = 2 << (adc->port * 2);
    else
        LSADC->SMPR1 = 4 << (adc->port * 2);
    REG_FIELD_WR(LSADC->RSQR1, ADC_RSQ1, adc->port);


    REG_FIELD_WR(LSADC->CR2,ADC_ADEN,1);

    return 0;
}

int32_t hal_adc_value_get(adc_dev_t *adc,uint32_t *output,uint32_t timeout)
{
    uint32_t g_adc_value = 0;
    
    if (adc == NULL || output == NULL) {
        return -1;
    }

    if(REG_FIELD_RD(LSADC->CR2,ADC_ADEN)==0) {
        return -1;
    }
    
    for(uint8_t i=0;i<5;i++){
        REG_FIELD_WR(LSADC->CR2, ADC_RTRIG,1);
        while( REG_FIELD_RD(LSADC->SR,ADC_REOC) != 1);
        REG_FIELD_WR(LSADC->SFCR, ADC_REOCC,1);
        REG_FIELD_WR(LSADC->SFCR, ADC_RSTRTCC,1);
        g_adc_value = REG_FIELD_RD(LSADC->RDR,ADC_RDATA);
    }
    *output = g_ref_vbat*g_adc_value/4096;
    //*output = g_adc_value;
    return 0; 
}

int32_t hal_adc_finalize(adc_dev_t *adc)
{
    REG_FIELD_WR(LSADC->CR2,ADC_ADEN,0);
    REG_FIELD_WR(RCC->APB2EN, RCC_ADC, 0);
    return 0;
}
