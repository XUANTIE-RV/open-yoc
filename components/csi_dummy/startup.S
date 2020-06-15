/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

 /******************************************************************************
 * @file     startup.S
 * @brief    startup file. Should use with
 *           GCC for CSKY Embedded Processors
 * @version  V1.0
 * @date     06. Apr 2020
 ******************************************************************************/

.section .vectors
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
#if defined (CONFIG_KERNEL_NONE) || defined (CONFIG_SUPPORT_TSPEND)
    .rept   10
    .long   Default_Handler
    .endr
    .long   CORET_IRQHandler
    .rept   53
    .long   Default_Handler
    .endr
#else

    .rept   64
    .long   NOVIC_IRQ_Default_Handler    /*  default interrupt entry  */
    .endr
#endif

    .size    __Vectors, . - __Vectors

    .text
    br       Reset_Handler
    .align   2
    .long    0x594B5343 /* CSKY ASCII */
    .long    0x594B5343 /* CSKY ASCII */
    .align   2
_start:
    .text
    .long    Reset_Handler
    .long    __data_end__
    .align   2
    .globl   Reset_Handler
    .type    Reset_Handler, %function
Reset_Handler:
    psrclr  ie

    lrw     r0, 0x80000200
    mtcr    r0, psr

    lrw     r0, g_top_irqstack
    mov     sp, r0

/*
 *  The ranges of copy from/to are specified by following symbols
 *    __etext: LMA of start of the section to copy from. Usually end of text
 *    __data_start__: VMA of start of the section to copy to
 *    __data_end__: VMA of end of the section to copy to
 *
 *  All addresses must be aligned to 4 bytes boundary.
 */
    lrw     r1, __erodata
    lrw     r2, __data_start__
    lrw     r3, __data_end__

    subu    r3, r2
    cmpnei  r3, 0
    bf      .L_loop0_done

.L_loop0:
    ldw     r0, (r1, 0)
    stw     r0, (r2, 0)
    addi    r1, 4
    addi    r2, 4
    subi    r3, 4
    cmpnei  r3, 0
    bt      .L_loop0

.L_loop0_done:

/*
 *  The BSS section is specified by following symbols
 *    __bss_start__: start of the BSS section.
 *    __bss_end__: end of the BSS section.
 *
 *  Both addresses must be aligned to 4 bytes boundary.
 */
    lrw     r1, __bss_start__
    lrw     r2, __bss_end__

    movi    r0, 0

    subu    r2, r1
    cmpnei  r2, 0
    bf      .L_loop1_done

.L_loop1:
    stw     r0, (r1, 0)
    addi    r1, 4
    subi    r2, 4
    cmpnei  r2, 0
    bt      .L_loop1
.L_loop1_done:

#ifndef __NO_SYSTEM_INIT
    jbsr    SystemInit
#endif

#ifndef __NO_BOARD_INIT
    jbsr    board_init
#endif

    jbsr    entry_c

    .size    Reset_Handler, . - Reset_Handler
    .globl  __exit
__exit:
    br      __exit

.section .bss

    .align  2
    .global g_intstackalloc
    .global g_intstackbase
    .global g_top_irqstack
g_intstackalloc:
g_intstackbase:
    .space  CONFIG_ARCH_INTERRUPTSTACK
g_top_irqstack:
