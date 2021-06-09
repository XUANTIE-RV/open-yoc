#include "reg_v33_rg.h"
#include "ARMCM3.h"

static uint32_t sleep_msp;

void cpu_recover_asm(void);
void cpu_sleep_recover_init()
{
    V33_RG->SFT_CTRL0F = (uint32_t)cpu_recover_asm >> 1;
}

void store_msp()
{
    sleep_msp = __get_MSP();
}

void restore_msp()
{
    __set_MSP(sleep_msp);
}

void before_wfi()
{
    
}

void after_wfi()
{
    
}