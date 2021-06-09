#include "ARMCM3.h"
#include "cpu.h"
#include "compile_flag.h"

static uint32_t critical_nested_cnt;
XIP_BANNED void enter_critical()
{
    __set_BASEPRI((1 << (8U - __NVIC_PRIO_BITS)));
    ++critical_nested_cnt;
}

XIP_BANNED void exit_critical()
{
    if(--critical_nested_cnt==0)
    {
        __set_BASEPRI(0);
    }
}

bool in_interrupt()
{
    return (SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk) != 0;
}

XIP_BANNED void disable_global_irq()
{
    __disable_irq();
}

void enable_global_irq()
{
    __enable_irq();
}
