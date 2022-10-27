/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */
 /******************************************************************************
 * @file     startup.S
 * @brief    startup file for CH2201. Should use with
 *           GCC for CSKY Embedded Processors
 * @version  V1.0
 * @date     26. Apr 2019
 ******************************************************************************/

#include <csi_config.h>

#undef  EFLASH_CONTROL_BASE
#define EFLASH_CONTROL_BASE  0x4003f000

#undef  PMU_LP_CONTROL
#define PMU_LP_CONTROL  0x40002000

.section .vectors, "aw", @progbits
    .align 10
    .globl   __Vectors
    .type    __Vectors, @object
__Vectors:
    .long   Reset_Handler
    .long   Default_Handler
    .long   Default_Handler
    .long   Default_Handler
    .long   Default_Handler
    .long   Default_Handler
    .long   Default_Handler
    .long   Default_Handler
    .long   Default_Handler
    .long   Default_Handler
    .long   Default_Handler
    .long   Default_Handler
    .long   Default_Handler
    .long   Default_Handler
    .long   Default_Handler
    .long   Default_Handler
    .long   trap0_handler
    .long   Default_Handler
    .long   Default_Handler
    .long   Default_Handler
    .long   Default_Handler
    .long   Default_Handler
    .long   tspend_handler
    .long   Default_Handler
    .long   Default_Handler
    .long   Default_Handler
    .long   Default_Handler
    .long   Default_Handler
    .long   Default_Handler
    .long   Default_Handler
    .long   Default_Handler
    .long   Default_Handler

    /* External interrupts */
    .long   Default_IRQHandler
    .long   Default_IRQHandler
    .long   Default_IRQHandler
    .long   Default_IRQHandler
    .long   Default_IRQHandler
    .long   Default_IRQHandler
    .long   Default_IRQHandler
    .long   Default_IRQHandler
    .long   Default_IRQHandler
    .long   Default_IRQHandler
    .long   Default_IRQHandler
    .long   Default_IRQHandler
    .long   Default_IRQHandler
    .long   Default_IRQHandler
    .long   Default_IRQHandler
    .long   Default_IRQHandler
    .long   Default_IRQHandler
    .long   Default_IRQHandler
    .long   Default_IRQHandler
    .long   Default_IRQHandler
    .long   Default_IRQHandler
    .long   Default_IRQHandler
    .long   Default_IRQHandler
    .long   Default_IRQHandler
    .long   Default_IRQHandler
    .long   Default_IRQHandler
    .long   Default_IRQHandler
    .long   Default_IRQHandler
    .long   Default_IRQHandler
    .long   Default_IRQHandler
    .long   Default_IRQHandler
    .long   Default_IRQHandler
    .size    __Vectors, . - __Vectors

    .text
    br       Reset_Handler
    .align   2
    .long    0x594B5343 /* CSKY ASCII */
    .long    0x594B5343 /* CSKY ASCII */
    .align   2
_start:
    .text
    .align   2
    .globl   Reset_Handler
    .type    Reset_Handler, %function
Reset_Handler:
    psrclr  ie
#ifndef CONFIG_SYSTEM_SECURE
    lrw     r0, 0x80000200
#else
    lrw     r0, 0xe0000200
#endif
    mtcr    r0, psr

    lrw     r0, __Vectors
    mtcr    r0, vbr

/* Initialize the normal stack pointer from the linker definition. */
    lrw     r0, g_top_irqstack
    mov     sp, r0

#ifndef __NO_SYSTEM_INIT
    jbsr    SystemInit
#endif

    jbsr    pre_main

    .size   Reset_Handler, . - Reset_Handler
__exit:
    br __exit

.section .stack

    .align  2
    .global g_base_irqstack
    .global g_top_irqstack
g_base_irqstack:
    .space  CONFIG_ARCH_INTERRUPTSTACK
g_top_irqstack:
