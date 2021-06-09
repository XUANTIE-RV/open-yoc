/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

 /******************************************************************************
 * @file     startup.S
 * @brief    startup file.
 * @version  V1.0
 * @date     15. May 2020
 ******************************************************************************/

.section .vectors
    .align  2
    .globl  __Vectors
__Vectors:
    .long   g_top_irqstack
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
    .long   PendSV_Handler
    .long   SysTick_Handler
    .long   do_irq
    .long   do_irq
    .long   do_irq
    .long   do_irq
    .long   do_irq
    .long   do_irq
    .long   do_irq
    .long   do_irq
    .long   do_irq
    .long   do_irq
    .long   do_irq
    .long   do_irq
    .long   do_irq
    .long   do_irq
    .long   do_irq
    .long   do_irq


    .text
    .align   2
_start:
    .align   2
    .globl   Reset_Handler
    .type    Reset_Handler, %function
Reset_Handler:
    movs    r0, #0x0
    msr     control, r0

    ldr     r0, =g_top_irqstack
    mov     sp, r0

#ifndef __NO_SYSTEM_INIT
    ldr     r0, =SystemInit
    blx     r0
#endif

    ldr     r0, =pre_main
    blx     r0

    .size   Reset_Handler, . - Reset_Handler
__exit:
    b       __exit

.section .stack

    .align  2
    .global g_top_irqstack
    .global g_base_irqstack
g_base_irqstack:
    .space  CONFIG_ARCH_INTERRUPTSTACK
g_top_irqstack:
