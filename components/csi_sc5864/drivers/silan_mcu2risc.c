#include "ap1508.h"
#include "silan_mcu2risc.h"
#include "silan_mcu2risc_regs.h"
#include "silan_types.h"
#include "silan_printf.h"

uint8_t Flag_CMDACK_FAIL=0;

void wait_xus(void)
{
	uint16_t i=0xFF;
	while(i--);
}

uint8_t SendCmd(void)
{
	uint32_t retry = 100000;
	CXC_RAW_INT_SET |= 0x2;  
	while(!(CXC_RAW_INT_STATUS&0x1))
	{
	  retry--;
	  if(!retry)
	  {
			if((Risc_Status & bit4) != 0)	//握手成功
			{
				Flag_CMDACK_FAIL = 0;
				return 1;
			}
			else		//握手失败，要异常恢复，需要重新boot
			{		
				Flag_CMDACK_FAIL = 1;
				return 0;
			}
		}
	}
	CXC_RAW_INT_CLR |= 0x1;
	return 1;
	
}
void mcu2risc_message(uint8_t cmd,uint8_t *para)
{
	int i;
	for(i=0;i<MBOX_SIZE;i++){
		if(i==0)
			__sREG8(CXC_MBOX_BASEADDR, 0x0)    = cmd;
		else
			__sREG8(CXC_MBOX_BASEADDR, 0x1*i)    = para[i-1];
			
	}	
	SendCmd();
}

