#ifndef PLATFORM_H_
#define PLATFORM_H_
#include <stdint.h>
#include "sdk_config.h"
void rv32_delay_asm(uint32_t,uint32_t);

#define DELAY_US(a) rv32_delay_asm((a)*SDK_HCLK_MHZ/3,1)



#endif
