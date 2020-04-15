#ifndef __SILAN_MCU2RISC_H__
#define __SILAN_MCU2RISC_H__

#include "silan_types.h"

#define MBOX_SIZE  16
uint8_t SendCmd(void);
void mcu2risc_message(uint8_t cmd,uint8_t *para);

extern uint8_t Flag_CMDACK_FAIL;

#endif
