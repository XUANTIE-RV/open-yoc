#include "cpu.h"
#include <stdarg.h>
#include "log.h"
#include "compile_flag.h"
//#include "platform.h"

//void iob_output_set(uint8_t i);

void ls_assert(const char *expr,const char *file,int line)
{
    disable_global_irq();
//    iob_output_set(2);
//    *(uint32_t *)0x50020020 = 0;
//    *(uint32_t *)0x5002003c = 0;
    LOG_RAW("Assertion Failed: file %s, line %d, %s\n",file,line,expr);
    while(1);
}

XIP_BANNED void ls_ram_assert()
{
    disable_global_irq();
    while(1);
}

void stack_assert_c(uint32_t lvl,uint32_t param0,uint32_t param1,uint32_t lr)
{
    LOG_RAW("lvl:%x,lr=0x%08x,param0=0x%x,param1=0x%x\n",lvl,lr,param0,param1);
    if(lvl==LVL_ERROR)
    {
        disable_global_irq();
//        platform_reset(LVL_ERROR);
        while(1);
    }
}
