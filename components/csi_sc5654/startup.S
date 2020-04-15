/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
 /******************************************************************************
 * @file     startup.S
 * @brief    startup file for smartl-ck803. Should use with
 *           GCC for CSKY Embedded Processors
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/

#include <csi_config.h>

.import tspend_handler
.import CORET_IRQHandler
.import Default_Handler

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

#ifdef CONFIG_KERNEL_NONE
    /* External interrupts */
    .long    Default_Handler           /*  0:   */
    .long    Default_Handler           /*  1:   */
    .long    SARADC_IRQHandler         /*  2:   */
    .long    SPIFLASH_IRQHandler       /*  3:   */
    .long    USART_IRQHandler          /*  4:   */
    .long    PDM_IRQHandler            /*  5:   */
    .long    GPIO1_IRQHandler          /*  6:   */
    .long    GPIO2_IRQHandler          /*  7:   */
    .long    TIMER_IRQHandler          /*  8:   */
    .long    SDMAC_IRQHandler          /*  9:   */
    .long    SDMMC_IRQHandler          /*  10:  */
    .long    IIR_IRQHandler            /*  11:  */
    .long    SDIO_IRQHandler           /*  12:  */
    .long    USBFS_IRQHandler          /*  13:  */
    .long    USBHS_IRQHandler          /*  14:  */
    .long    CXC_IRQHandler            /*  15:  */
    .long    ADMAC_IRQHandler          /*  16:  */
    .long    SPDIF_IRQHandler          /*  17:  */
    .long    PCM_IRQHandler            /*  18:  */
    .long    IIS_IRQHandler            /*  19:  */
    .long    RTC_IRQHandler            /*  20:  */
    .long    PMU_IRQHandler            /*  21:  */
    .long    DMAC_IRQHandler           /*  22:  */
    .long    Default_Handler           /*  23:  */
    .long    Default_Handler           /*  24:  */
    .long    Default_Handler           /*  25:  */
    .long    Default_Handler           /*  26:  */
    .long    Default_Handler           /*  27:  */
    .long    Default_Handler           /*  28:  */
    .long    Default_Handler           /*  29:  */
    .long    WDT_IRQHandler         /*  30:  */
    .long    CORET_IRQHandler          /*  31:  */
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
#ifdef CONFIG_STARTUP_MODE_LONG
    .long    Reset_Handler
#else
    br    Reset_Handler
#endif
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
    lrw      r0, 0x80000000
    mtcr     r0, psr

    lrw      r0, g_top_irqstack
    mov      sp, r0

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
    .globl  g_intstackalloc
    .global g_intstackbase
    .global g_top_irqstack
g_intstackalloc:
g_intstackbase:
    .space  CONFIG_ARCH_INTERRUPTSTACK
g_top_irqstack:
