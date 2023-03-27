#ifndef _MIND_DEBUG_MODE_H_
#define _MIND_DEBUG_MODE_H_

#if defined(CONFIG_CHIP_D1)
#include "chip_d1/c906fdv/debug_mode.h"
#elif defined(CVI_SOC_CV181XC)
#include "chip_cv181x/c906fdv/debug_mode.h"
#elif defined(CONFIG_CHIP_BL606P)
#include "chip_bl606p/c906fdv/debug_mode.h"
#else
#error Unknown Platform
#endif

#endif /* _RTF_CAL_H_ */
