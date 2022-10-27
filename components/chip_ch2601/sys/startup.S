/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

 /******************************************************************************
 * @file     startup.S
 * @brief    startup file. Should use with
 *           GCC for CSKY Embedded Processors
 * @version  V1.0
 * @date     29. July 2019
 ******************************************************************************/

#include <csi_config.h>

.section .vectors, "aw", @progbits
    .align  6
    .globl  __Vectors
    .type   __Vectors, @object
__Vectors:
    .long   Default_Handler
    .long   Default_Handler
    .long   Default_Handler
    .long   tspend_handler
    .long   Default_Handler
    .long   Default_Handler
    .long   Default_Handler
    .long   Default_IRQHandler
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

    .text
    .align   2
    j        Reset_Handler
    .align   2
    .long    0x594B5343 /* CSKY ASCII */
    .long    0x594B5343 /* CSKY ASCII */
    .align   2
    .rept    13
    .long    0
    .endr
    .long    Reset_Handler
_start:
    .text
    .align  2
    .globl  Reset_Handler
    .type   Reset_Handler, %function
    .globl  __driver_test_service__
    .weak   __driver_test_service__
    .type   __driver_test_service__, %function
    
Reset_Handler:
    csrc    mstatus, 8
.option push
.option norelax
    la      gp, __global_pointer$
.option pop
    la      a0, Default_Handler
    ori     a0, a0, 3
    csrw    mtvec, a0

    li      a0, 0x2000
    csrs    mstatus, a0

    la      a0, __Vectors
    csrw    mtvt, a0

    la      a0, 0x100000
    csrw    mhint, a0

    la      sp, g_top_irqstack

#ifndef __NO_SYSTEM_INIT
    jal     SystemInit
#endif

    la     a0, __driver_test_service__
	beq    zero, a0, 1f
	jalr   a0
    j      __exit
	
1:
    jal    pre_main

    .size   Reset_Handler, . - Reset_Handler

__exit:
    j      __exit

.section .stack

    .align  3
    .global g_base_irqstack
    .global g_top_irqstack
g_base_irqstack:
    .space CONFIG_ARCH_INTERRUPTSTACK
g_top_irqstack:
