#include "taurus.h"
#include "compile_flag.h"
__attribute__((aligned(64))) void (*interrupt_vector[IRQn_MAX])();

void rv_set_int_isr(uint8_t type,void (*isr)())
{
    interrupt_vector[type] = isr;
}
