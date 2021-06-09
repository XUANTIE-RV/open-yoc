#ifndef _SILAN_SDRAM_H_
#define _SILAN_SDRAM_H_

#include "silan_types.h"

typedef enum{
	SDRAM16M = 0,
	SDRAM4M = 1,
	SDRAM1M = 2,
	SDRAM512K = 3,
}sdram_type_t;


#define SDRAM_REFCNT(m)    (((int)(m * 78 / 10)) & 0xFFFF)

//#define SDRAM_PARA  0x7060
#define SDRAM_PARA  0x2870

void silan_sdram_init(void);  //sdram init api

#endif
