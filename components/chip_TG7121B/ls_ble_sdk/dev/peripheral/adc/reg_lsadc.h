#ifndef REG_LSADC_H_
#define REG_LSADC_H_
#include "reg_lsadc_type.h"
#include "reg_rcc.h"

#define LSADC ((reg_adc_t *)(0x40012400))

/**
  * @brief  Configure adc  APB2 clock, sleep and deep sleep
  * @param  .
  * @retval None
  */

#define __HAL_RCC_ADC_CLK_ENABLE()   do { \
                                       REG_FIELD_WR(RCC->APB2EN,  RCC_ADC, 1);\
                                      } while(0U)

#define __HAL_RCC_ADC_SLP_ENABLE()   do { \
                                       REG_FIELD_WR(RCC->APB2SL, RCC_ADC, 1);\
                                      } while(0U)    

#define __HAL_RCC_ADC_SLP_DISABLE()   do { \
                                       REG_FIELD_WR(RCC->APB2SL, RCC_ADC, 0);\
                                      } while(0U)                                        
 

#define __HAL_RCC_ADC_DSLP_ENABLE()   do { \
                                       REG_FIELD_WR(RCC->APB2DSL, RCC_ADC, 1);\
                                      } while(0U)     

#define __HAL_RCC_ADC_DSLP_DISABLE()   do { \
                                       REG_FIELD_WR(RCC->APB2DSL, RCC_ADC, 0);\
                                      } while(0U)      
#endif //(REG_LSADC_H_)
