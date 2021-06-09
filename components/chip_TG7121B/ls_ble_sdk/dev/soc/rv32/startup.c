#include "common.h"
#include "csi_core.h"
#include "exception_isr.h"
void SystemInit(void);
void _start(void) __attribute__ ((noreturn));
extern uint32_t __data_lma__;
extern uint32_t __data_start__;
extern uint32_t __data_end__;
extern uint32_t __StackTop;
extern uint32_t exception_entry;

__attribute__ ((naked)) void Reset_Handler()
{
    __set_MTVEC((uint32_t)&exception_entry);
    __set_MTVT((uint32_t)interrupt_vector);
    __set_SP((uint32_t)&__StackTop);
    SystemInit();
    uint32_t *src = &__data_lma__;
    uint32_t *start = &__data_start__;
    uint32_t *end = &__data_end__;
    memcpy32(start,src,end-start);
    _start();
}
