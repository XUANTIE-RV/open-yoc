/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

 /******************************************************************************
 * @file     vectors.S
 * @brief    define default vector handlers. Should use with
 *           GCC for CSKY Embedded Processors
 * @version  V1.0
 * @date     28. Nove 2017
 ******************************************************************************/

#include <csi_config.h>

.import trap_c
.import do_irq

.section .bss
    .align  2
    .globl  g_trapstackalloc
    .global g_trapstackbase
    .global g_top_trapstack
g_trapstackalloc:
g_trapstackbase:
    .space 768
g_top_trapstack:

.text

/******************************************************************************
 * Functions:
 *     void trap(void);
 * default exception handler
 ******************************************************************************/
    .global trap
    .type   trap, %function
trap:
    psrset  ee
    subi    sp, 4
    stw     r0, (sp)

    lrw     r0, g_top_trapstack
    subi    r0, 140
    stm     r0-r15, (r0)
    stw     r28, (r0, 64)

    ldw     r1, (sp)
    addi    sp, 4
    stw     sp, (r0, 56) /* save r14 */
    mov     sp, r0
    stw     r1, (sp)     /* save r0 */

    addi    r0, 68
    fstms   vr0-vr15, (r0)
    mov     r0, sp

    mfcr    r1, epsr
    stw     r1, (sp, 132)
    mfcr    r1, epc
    stw     r1, (sp, 136)

    jbsr    trap_c

    .align  2
    .weak   Default_Handler
    .type   Default_Handler, %function
Default_Handler:
    br      trap
    .size   Default_Handler, . - Default_Handler

#if defined(CONFIG_SUPPORT_TSPEND) || defined(CONFIG_KERNEL_NONE)
    .align  2
    .global Default_IRQHandler
    .type   Default_IRQHandler, %function
Default_IRQHandler:
    nie
    ipush
    subi        r14, r14, 32
    fstms       fr0-fr7, (r14)
    push        r15
    lrw         r0, do_irq
    jsr         r0
    ld.w        r15, (r14, 0x0)
    addi        r14, r14, 4
    fldms       fr0-fr7, (r14)
    addi        r14, r14, 32
    ipop
    nir
    .size   Default_IRQHandler, . - Default_IRQHandler
#endif

/*    Macro to define default handlers. Default handler
 *    will be weak symbol and just dead loops. They can be
 *    overwritten by other handlers */
    .macro  def_irq_handler handler_name
    .weak   \handler_name
    .globl  \handler_name
    .set    \handler_name, Default_Handler
    .endm

    def_irq_handler trap0_handler
    def_irq_handler tspend_handler
    def_irq_handler CORET_IRQHandler
