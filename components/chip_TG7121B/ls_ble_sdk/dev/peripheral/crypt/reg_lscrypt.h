#ifndef REG_LSCRYPT_H_
#define REG_LSCRYPT_H_
#include "reg_lscrypt_type.h"
#include "reg_rcc.h"

#define LSCRYPT ((reg_crypt_t *)(0x40002800))

/**
  * @brief  Configure Crypt  APB1 clock, sleep and deep sleep
  * @param  .
  * @retval None
  */

#define __HAL_RCC_CRYPT_CLK_ENABLE()   do { \
                                       REG_FIELD_WR(RCC->APB1RST, RCC_CRYPT, 1);\
                                       REG_FIELD_WR(RCC->APB1RST, RCC_CRYPT, 0);\
                                       REG_FIELD_WR(RCC->APB1EN,  RCC_CRYPT, 1);\
                                      } while(0U)

#define __HAL_RCC_CRYPT_SLP_ENABLE()   do { \
                                       REG_FIELD_WR(RCC->APB1SL, RCC_CRYPT, 1);\
                                      } while(0U)

#define __HAL_RCC_CRYPT_SLP_DISABLE()   do { \
                                       REG_FIELD_WR(RCC->APB1SL, RCC_CRYPT, 0);\
                                      } while(0U)


#define __HAL_RCC_CRYPT_DSLP_ENABLE()   do { \
                                       REG_FIELD_WR(RCC->APB1DSL, RCC_CRYPT, 1);\
                                      } while(0U)

#define __HAL_RCC_CRYPT_DSLP_DISABLE()   do { \
                                       REG_FIELD_WR(RCC->APB1DSL, RCC_CRYPT, 0);\
                                      } while(0U)

#endif //(REG_LSCRYPT_H_)
