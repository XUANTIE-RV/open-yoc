/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

 /******************************************************************************
 * @file     startup.S
 * @brief    startup file for DANICA. Should use with
 *           GCC for CSKY Embedded Processors
 * @version  V1.0
 * @date     05. July 2020
 ******************************************************************************/

#include <csi_config.h>

.section .vectors, "aw", @progbits
    .align  10
    .globl  __Vectors
    .type   __Vectors, @object
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
    .long   Default_Handler
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
    .align   2
    .rept    16
    .long    0
    .endr
    .long    Reset_Handler
_start:
    .text
    .align   2
    .globl   Reset_Handler
    .type    Reset_Handler, %function
Reset_Handler:
    psrclr  ie

    lrw     r0, 0x80000200
    mtcr    r0, psr

    lrw     r0, __Vectors
    mtcr    r0, vbr

    lrw     r0, g_top_irqstack
    mov     sp, r0

#ifndef __NO_SYSTEM_INIT
    jbsr    SystemInit
#endif

    jbsr    pre_main


    .size    Reset_Handler, . - Reset_Handler
__exit:
    br      __exit

.section .stack

    .align  2
    .global g_base_irqstack
    .global g_top_irqstack
g_base_irqstack:
    .space  CONFIG_ARCH_INTERRUPTSTACK
g_top_irqstack:
