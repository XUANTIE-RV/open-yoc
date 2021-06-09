#include <stdio.h>
#include <stdlib.h>
#ifdef CONFIG_EXTERNAL_24M
#include <sys_freq.h>
#endif
#include <soc.h>

#ifndef CONFIG_SYSTICK_HZ
#define CONFIG_SYSTICK_HZ 100
#endif

extern void (*BootVectors[])(void);

__attribute__((weak)) void SystemInit(void)
{
    __set_VBR((uint32_t) & (BootVectors));

    __enable_excp_irq();
    __disable_irq();

#ifdef CONFIG_EXTERNAL_24M
    drv_set_sys_freq(EHS_CLK, OSR_8M_CLK_24M);
#endif
    csi_coret_config(drv_get_sys_freq() / CONFIG_SYSTICK_HZ, CORET_IRQn);    //10ms
}
