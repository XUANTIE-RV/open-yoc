#ifndef _SILAN_CXC_CMD_H_
#define _SILAN_CXC_CMD_H_

#include "silan_cxc_regs.h"

#define CMD_PARA_SIZE    16

#define LOCK             CXC_MUTEX(0)

#define MCU2DSP_REQ_BIT  1
#define DSP2MCU_ACK_BIT  0

#define MCU2RISC_REQ_BIT 3
#define RISC2MCU_ACK_BIT 2

#define	CMD		   		__REG32(CXC_MBOX_BASEADDR+0x040) 
#define	CMD_RET		    __REG32(CXC_MBOX_BASEADDR+0x044) 	
#define	CMD_STATUS		__REG32(CXC_MBOX_BASEADDR+0x048) 	// 命令空与忙 0==空
#define	DSP_STATUS		__REG32(CXC_MBOX_BASEADDR+0x054) 	// DSP_Status ==2
#define	RISC_STATUS		__REG32(CXC_MBOX_BASEADDR+0x058) 	// RISC_Dtatus ==1
#define	DEV_STATUS		__REG32(CXC_MBOX_BASEADDR+0x05c) 	// Risc_status 用于判断设备忙或空

int silan_mcu2dsp_send(uint8_t cmd, uint8_t *para);
int silan_mcu2risc_send(uint8_t cmd, uint8_t *para);

#endif
