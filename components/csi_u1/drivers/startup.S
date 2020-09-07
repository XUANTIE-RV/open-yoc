/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */
 /******************************************************************************
 * @file     startup.S
 * @brief    startup file for CH2201. Should use with
 *           GCC for CSKY Embedded Processors
 * @version  V1.0
 * @date     24. August 2017
 ******************************************************************************/


.section .vectors
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
#ifdef CONFIG_KERNEL_NONE
    /* External interrupts */
    .long   Default_Handler
    .long   CORET_IRQHandler
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
    .long   Default_Handler
    .long   Default_Handler
    .long   Default_Handler
    .long   Default_Handler
    .long   Default_Handler
    .long   Default_Handler
    .long   Default_Handler
    .long   Default_Handler
    .long   Default_Handler
#else
    .long   NOVIC_IRQ_Default_Handler    /*  default interrupt entry  */
    .long   NOVIC_IRQ_Default_Handler    /*  default interrupt entry  */
    .long   NOVIC_IRQ_Default_Handler    /*  default interrupt entry  */
    .long   NOVIC_IRQ_Default_Handler    /*  default interrupt entry  */
    .long   NOVIC_IRQ_Default_Handler    /*  default interrupt entry  */
    .long   NOVIC_IRQ_Default_Handler    /*  default interrupt entry  */
    .long   NOVIC_IRQ_Default_Handler    /*  default interrupt entry  */
    .long   NOVIC_IRQ_Default_Handler    /*  default interrupt entry  */
    .long   NOVIC_IRQ_Default_Handler    /*  default interrupt entry  */
    .long   NOVIC_IRQ_Default_Handler    /*  default interrupt entry  */
    .long   NOVIC_IRQ_Default_Handler    /*  default interrupt entry  */
    .long   NOVIC_IRQ_Default_Handler    /*  default interrupt entry  */
    .long   NOVIC_IRQ_Default_Handler    /*  default interrupt entry  */
    .long   NOVIC_IRQ_Default_Handler    /*  default interrupt entry  */
    .long   NOVIC_IRQ_Default_Handler    /*  default interrupt entry  */
    .long   NOVIC_IRQ_Default_Handler    /*  default interrupt entry  */
    .long   NOVIC_IRQ_Default_Handler    /*  default interrupt entry  */
    .long   NOVIC_IRQ_Default_Handler    /*  default interrupt entry  */
    .long   NOVIC_IRQ_Default_Handler    /*  default interrupt entry  */
    .long   NOVIC_IRQ_Default_Handler    /*  default interrupt entry  */
    .long   NOVIC_IRQ_Default_Handler    /*  default interrupt entry  */
    .long   NOVIC_IRQ_Default_Handler    /*  default interrupt entry  */
    .long   NOVIC_IRQ_Default_Handler    /*  default interrupt entry  */
    .long   NOVIC_IRQ_Default_Handler    /*  default interrupt entry  */
    .long   NOVIC_IRQ_Default_Handler    /*  default interrupt entry  */
    .long   NOVIC_IRQ_Default_Handler    /*  default interrupt entry  */
    .long   NOVIC_IRQ_Default_Handler    /*  default interrupt entry  */
    .long   NOVIC_IRQ_Default_Handler    /*  default interrupt entry  */
    .long   NOVIC_IRQ_Default_Handler    /*  default interrupt entry  */
    .long   NOVIC_IRQ_Default_Handler    /*  default interrupt entry  */
    .long   NOVIC_IRQ_Default_Handler    /*  default interrupt entry  */
    .long   NOVIC_IRQ_Default_Handler    /*  default interrupt entry  */
#endif
    .size    __Vectors, . - __Vectors

    .text
    .long _start
    .align   2
_start:
    .text
    .align   2
    .globl   Reset_Handler
    .type    Reset_Handler, %function
Reset_Handler:
.LCPUINIT:
#ifndef CONFIG_SYSTEM_SECURE
    lrw     r0, 0x80000000
#else
    lrw     r0, 0xe0000000
#endif
    mtcr    r0, psr

/* Initialize the normal stack pointer from the linker definition. */
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

    .size   Reset_Handler, . - Reset_Handler
__exit:
    br __exit

.section .bss

    .align  2
    .global g_intstackalloc
    .global g_intstackbase
    .global g_top_irqstack
g_intstackalloc:
g_intstackbase:
    .space  CONFIG_ARCH_INTERRUPTSTACK
g_top_irqstack:
