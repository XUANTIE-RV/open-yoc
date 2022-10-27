
#include <string.h>
#include "wm_regs.h"
#include "wm_psram.h"
#include "wm_dma.h"

void psram_init(psram_mode_t mode)
{
	volatile unsigned int value = 0x600;

	value |= 2<<4;

	if(mode == PSRAM_QPI)
	{
		value |= 0x03;
	}

	/*reset psram*/
	value |= 0x01;
	tls_reg_write32(HR_PSRAM_CTRL_ADDR, value);
	do{
		value = tls_reg_read32(HR_PSRAM_CTRL_ADDR);
	}while(value&0x01);
}

