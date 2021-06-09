#include <stdint.h>

void SWINT_Handler_C(uint32_t *args)
{
    uint32_t (*func)(uint32_t,uint32_t,uint32_t) = (uint32_t (*)(uint32_t,uint32_t,uint32_t))args[3];
    args[0] = func(args[0],args[1],args[2]);
}

