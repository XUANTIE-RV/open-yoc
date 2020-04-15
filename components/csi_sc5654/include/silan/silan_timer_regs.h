//
// Timer.h
// 

#ifndef _TIMER_H_
#define _TIMER_H_

#include "silan_types.h"
#include "ap1508.h"

#define TIMER_BASE_ADDR				SPER_APB2_TIMER_BASE   //0x41860000

#define TIMER0_COUNT        				__REG32(TIMER_BASE_ADDR + 0x00)
#define TIMER0_COMPARE      				__REG32(TIMER_BASE_ADDR + 0x04)
#define TIMER0_CTRL         				__REG32(TIMER_BASE_ADDR + 0x08)
#define TIMER0_SEL							__REG32(TIMER_BASE_ADDR + 0x0C)
#define TIMER1_COUNT        				__REG32(TIMER_BASE_ADDR + 0x10)
#define TIMER1_COMPARE      				__REG32(TIMER_BASE_ADDR + 0x14)
#define TIMER1_CTRL         				__REG32(TIMER_BASE_ADDR + 0x18)
#define TIMER1_SEL							__REG32(TIMER_BASE_ADDR + 0x1C)
#define TIMER2_COUNT        				__REG32(TIMER_BASE_ADDR + 0x20)
#define TIMER2_COMPARE      				__REG32(TIMER_BASE_ADDR + 0x24)
#define TIMER2_CTRL         				__REG32(TIMER_BASE_ADDR + 0x28)
#define TIMER2_SEL							__REG32(TIMER_BASE_ADDR + 0x2C)
#define TIMER3_COUNT        				__REG32(TIMER_BASE_ADDR + 0x30)
#define TIMER3_COMPARE      				__REG32(TIMER_BASE_ADDR + 0x34)
#define TIMER3_CTRL         				__REG32(TIMER_BASE_ADDR + 0x38)
#define TIMER3_SEL							__REG32(TIMER_BASE_ADDR + 0x3C)
#define TIMER4_COUNT        				__REG32(TIMER_BASE_ADDR + 0x40)
#define TIMER4_COMPARE      				__REG32(TIMER_BASE_ADDR + 0x44)
#define TIMER4_CTRL         				__REG32(TIMER_BASE_ADDR + 0x48)
#define TIMER4_SEL							__REG32(TIMER_BASE_ADDR + 0x4C)

#define TIMER_MS_COUNT						__REG32(TIMER_BASE_ADDR + 0x50)
#define TIMER_MS_COMPARE 					__REG32(TIMER_BASE_ADDR + 0x54)
#define TIMER_MS_CTRL						__REG32(TIMER_BASE_ADDR + 0x58)
#define TIMER_1MS_COMPARE					__REG32(TIMER_BASE_ADDR + 0x5C)
#define TIMER_S_COUNT						__REG32(TIMER_BASE_ADDR + 0x60)
#define TIMER_S_COMPARE 					__REG32(TIMER_BASE_ADDR + 0x64)
#define TIMER_S_CTRL						__REG32(TIMER_BASE_ADDR + 0x68)

#define TIMER0_BASE                         0x41860000
#define TIMER1_BASE                         0x41860010
#define TIMER2_BASE                         0x41860020
#define TIMER3_BASE                         0x41860030
#define TIMER4_BASE                         0x41860040
#define TIMER_MS_BASE                       0x41860050
#define TIMER_S_BASE                        0x41860060

#define TIMER_COUNT                         0x00
#define TIMER_COMPARE                       0x04
#define TIMER_CTRL                          0x08
#define TIMER_SEL                           0x0c

#define TIMER_EN    					    (1<<0)
#define TIMER_TO    					    (1<<1)
#define TIMER_INT  							(1<<2)

#define TIMER_ADDR(x)						(TIMER_BASE_ADDR+x*0x10)

#endif /* _TIMER_H_ */

