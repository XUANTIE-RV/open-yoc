#include "reg_rcc.h"
#include "field_manipulate.h"
#include "compile_flag.h"

XIP_BANNED void lscache_msp_init()
{
    REG_FIELD_WR(RCC->AHBRST, RCC_CACHE, 1);
    REG_FIELD_WR(RCC->AHBRST, RCC_CACHE, 0);
    REG_FIELD_WR(RCC->AHBEN,RCC_CACHE,1);
    REG_FIELD_WR(RCC->APB2EN,RCC_APB_CACHE, 1);
}


