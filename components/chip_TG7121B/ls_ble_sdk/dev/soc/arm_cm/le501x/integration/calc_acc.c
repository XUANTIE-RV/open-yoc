#include "field_manipulate.h"
#include "reg_rcc.h"

void calc_acc_init()
{
    REG_FIELD_WR(RCC->APB1RST, RCC_CALC, 1);
    REG_FIELD_WR(RCC->APB1RST, RCC_CALC, 0);
    REG_FIELD_WR(RCC->APB1EN, RCC_CALC, 1);
}
