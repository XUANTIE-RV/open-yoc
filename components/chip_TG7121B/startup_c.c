#include <string.h>
#include "common.h"

void SystemInit(void);
void pre_main(void);

__attribute__((naked)) void Reset_Handler()
{
    extern uint32_t __vector_lma__;
    extern uint32_t __vector__start__;
    extern uint32_t __vector__end__;
    memcpy32(&__vector__start__,&__vector_lma__,&__vector__end__-&__vector__start__);
    extern uint32_t __data_lma__;
    extern uint32_t __data_start__;
    extern uint32_t __data_end__;
    memcpy32(&__data_start__,&__data_lma__,&__data_end__-&__data_start__);
    extern uint32_t __bss_start__;
    extern uint32_t __bss_end__;
    memset(&__bss_start__,0,(uint32_t)&__bss_end__-(uint32_t)&__bss_start__);
    extern uint32_t __ls_env_bss_start__;
    extern uint32_t __ls_env_bss_end__;
    memset(&__ls_env_bss_start__,0,(uint32_t)&__ls_env_bss_end__-(uint32_t)&__ls_env_bss_start__);
    SystemInit();
    pre_main();
}