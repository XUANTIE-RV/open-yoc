#ifndef TAURUS_H_
#define TAURUS_H_



typedef enum IRQn
{
  RV_SOFT_IRQn     = 3,
  RV_TIME_IRQn     = 7,
  RV_EXT_IRQn      = 11,
  RTC1_IRQn	       = 16,
  IWDT_IRQn	       = 17,
  RTC2_IRQn	       = 18,
  EXT_IRQn		   = 19,
  LVD33_IRQn	   = 20,
  MAC_LP_IRQn      = 21,
  MAC1_IRQn	       = 22,
  MAC2_IRQn        = 23,
  SWINT1_IRQn      = 24,
  USB_IRQn         = 25,
  QSPI_IRQn        = 26,
  CACHE_IRQn       = 27,
  GPTIMA1_IRQn	   = 28,
  GPTIMB1_IRQn	   = 29,
  GPTIMC1_IRQn	   = 30,
  ADTIM1_IRQn	   = 31,
  I2C1_IRQn	       = 32,
  I2C2_IRQn	       = 33,
  I2C3_IRQn	       = 34,
  UART1_IRQn	   = 35,
  UART2_IRQn	   = 36,
  UART3_IRQn       = 37,
  SPI2_IRQn        = 38,
  GPIO_IRQn  	   = 39,
  WWDT_IRQn        = 40,
  ADC_IRQn         = 41,
  TK_IRQn          = 42,
  SWINT2_IRQn      = 43,
  SWINT3_IRQn      = 44,
  IRQn_MAX,
}IRQn_Type;

#endif
