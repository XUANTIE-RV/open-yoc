#include "core_rv32.h"
#include "cpu.h"
#include "compile_flag.h"

static uint32_t critical_nested_cnt;
XIP_BANNED void enter_critical()
{
    CLIC->MINTTHRESH = 254<<24;
    ++critical_nested_cnt;
}

XIP_BANNED void exit_critical()
{
    if(--critical_nested_cnt==0)
    {
        CLIC->MINTTHRESH = 0;
    }
}

bool in_interrupt()
{
    return false;
}

XIP_BANNED void disable_global_irq()
{
    __disable_irq();
}

void enable_global_irq()
{
    __enable_irq();
}
