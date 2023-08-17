/* maxiao.maxiao
   define the interface and structure for MIT_RTOS
*/


#ifndef __MIND_MIT_RTOS_H__
#define __MIND_MIT_RTOS_H__

#if defined(CONFIG_CHIP_D1)
#include "chip_d1/c906fdv/mit_rtos.h"
#elif defined(CVI_SOC_CV181XC)
#include "chip_cv181x/c906fdv/mit_rtos.h"
#elif defined(CONFIG_CHIP_BL606P)
#include "chip_bl606p/c906fdv/mit_rtos.h"
#elif defined(CONFIG_CHIP_LIGHT)
#include "chip_light_c906/c906fdv/mit_rtos.h"
#else
#error Unknown Platform
#endif

#endif
