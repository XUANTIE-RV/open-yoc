#include "silan_cxc_regs.h"
#include "silan_types.h"
#include "silan_printf.h"
#include "silan_errno.h"
#include "silan_cxc_cmd.h"

int silan_mcu2dsp_send(uint8_t cmd, uint8_t *para)
{
	int i;
	uint32_t retry = 100000;
	
	for(i=0;i<CMD_PARA_SIZE;i++) {
		if(i==0) {
			__sREG8(CXC_MBOX_BASEADDR, 0x0)	 = cmd;
			__sREG8(CXC_MBOX_BASEADDR, 0x40) = cmd;
		}
		else {
			__sREG8(CXC_MBOX_BASEADDR, 0x1*i) = para[i-1];
		}
	}

	if((LOCK&0x1))
		return 0;
	CXC_RAW_INT_SET |= (1 << MCU2DSP_REQ_BIT);  
	while(!(CXC_RAW_INT_STATUS & (1 << DSP2MCU_ACK_BIT)))
	{
		retry--;
		if(!retry)
			return 0;
	}
	CXC_RAW_INT_CLR |= (1 << DSP2MCU_ACK_BIT);
//	LOCK = 0;
	return 1;
}

int silan_mcu2risc_send(uint8_t cmd, uint8_t *para)
{
	int i;
	uint32_t retry = 100000;
	
	for(i=0;i<CMD_PARA_SIZE;i++) {
		if(i==0) {
			__sREG8(CXC_MBOX_BASEADDR, 0x0)  = cmd;
			__sREG8(CXC_MBOX_BASEADDR, 0x40) = cmd;
		}
		else {
			__sREG8(CXC_MBOX_BASEADDR, 0x1*i) = para[i-1];
		}	
	}
		
	if((LOCK&0x1))
		return 0;
	CXC_RAW_INT_SET |= (1 << MCU2RISC_REQ_BIT);  
	while(!(CXC_RAW_INT_STATUS & (1 << RISC2MCU_ACK_BIT)))
	{
		retry--;
		if(!retry)
			return 0;
	}
	CXC_RAW_INT_CLR |= (1 << RISC2MCU_ACK_BIT);
//	LOCK = 0;
	return 1;
}


