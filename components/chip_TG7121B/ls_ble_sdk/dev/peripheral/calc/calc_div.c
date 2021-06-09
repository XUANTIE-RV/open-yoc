#include "calc_div.h"
#include "field_manipulate.h"
#include "reg_calc.h"

void calc_div(uint32_t dividend,uint32_t divisor,bool signed_div,uint32_t *quotient,uint32_t *remainder)
{
    CALC->DIVCSR = FIELD_BUILD(CALC_DIV_SIGN,signed_div?1:0) | FIELD_BUILD(CALC_DIV_TRM,0);
    CALC->DIVS = divisor;
    CALC->DIVD = dividend;
    while(REG_FIELD_RD(CALC->DIVCSR,CALC_DIV_BUSY));
    *quotient = CALC->DIVQ;
    *remainder = CALC->DIVR;
}

