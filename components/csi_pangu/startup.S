/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

 /******************************************************************************
 * @file     startup.S
 * @brief    startup file. Should use with
 *           GCC for CSKY Embedded Processors
 * @version  V1.0
 * @date     06. Mar 2019
 * @vendor   csky
 * @chip     pangu
 ******************************************************************************/

#include <csi_config.h>

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
#if defined (CONFIG_SYSTEM_SECURE)
    lrw     r0, 0xe000f000   /* Cache register base address */
    lrw     r2, 0x800006f    /* CRCR1 value means: Cached data from 0x8000000(QSPIFLASH), Size= 8MB, Enable*/
    lrw     r1, 0x1800006f   /* CRCR0 value means: Cached data from 0x18000000(SDRAM), Size= 8MB, Enable*/
    stw     r1, (r0, 0x8)    /* store 0x10000063 to CRCR0(0xe000f008) */
    stw     r2, (r0, 0xC)    /* store 0x10000063 to CRCR0(0xe000f008) */
    movi    r1, 0x1          /* store 0x1 to r1 */
    stw     r1, (r0, 0x4)    /* set CIR(0xe000f004) to 0x1, means invalid all caches */
    movi    r1, 0x15          /* store 0x15 to r1 */
    stw     r1, (r0, 0x0)    /* set CER(0xe000f000) to 0x1, means enable all caches */
#endif

    /* enable return speculative execution */
    mfcr    r0, cr<31,0>
    bseti   r0, 3
    mtcr    r0, cr<31,0>

#ifndef CONFIG_TEE_CA
#ifndef CONFIG_SYSTEM_SECURE
    lrw     r0, 0x80000200
#else
    lrw     r0, 0xe0000200
#endif
    mtcr    r0, psr
#endif

    lrw     r0, g_top_irqstack
    mov     sp, r0

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
