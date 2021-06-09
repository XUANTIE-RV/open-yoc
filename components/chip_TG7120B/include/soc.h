/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

/**************************************************************************//**
 * @file     soc.h
 * @brief    CSI Core Peripheral Access Layer Header File for
 *           CSKYSOC Device Series
 * @version  V1.0
 * @date     15. May 2019
 ******************************************************************************/

#ifndef _SOC_H_
#define _SOC_H_

#include "csi_core.h"                     /* Processor and core peripherals */
#include "bus_dev.h"


#ifndef IHS_VALUE
#define  IHS_VALUE    (8000000)
#endif

#ifndef EHS_VALUE
#define  EHS_VALUE    (8000000)
#endif

#ifndef ILS_VALUE
#define  ILS_VALUE    (5000)
#endif

#ifndef ELS_VALUE
#define  ELS_VALUE    (16384)
#endif

#define CSKY_UART0_BASE  AP_UART0_BASE

#define CORET_IRQn CORETIM_IRQn

#define CONFIG_USART_NUM 2
#define CONFIG_GPIO_NUM  1
#define CONFIG_SPI_NUM      2
#define CONFIG_IIC_NUM      2

#define CONFIG_GPIO_PIN_NUM 23
#define CONFIG_TIMER_NUM 2
#define CONFIG_RTC_NUM 1
#define CONFIG_WDT_NUM 1
#define CONFIG_DMA_CHANNEL_NUM 4
#define CONFIG_TRNG_NUM 1
#define CONFIG_AES_NUM 1
#define CONFIG_PMU_NUM  1
#define CONFIG_ADC_NUM 1

#endif  /* _SOC_H_ */
