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
    .space 512
g_top_trapstack:

.section .bss
    .align 2
    .globl g_trap_sp
    .type  g_trap_sp, object
g_trap_sp:
    .long 0
    .size g_trap_sp, .-g_trap_sp

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
    stw     r13, (sp)
    addi    sp, 4

    lrw     r13, g_trap_sp
    stw     sp, (r13)

    lrw     sp, g_top_trapstack

    subi    sp, 72
    stm     r0-r12, (sp)

    lrw     r0, g_trap_sp
    ldw     r0, (r0)

    stw     r0, (sp, 56) /* save r14 */

    subi    r0, 4
    ldw     r13, (r0)
    stw     r13, (sp, 52)

    stw     r15, (sp, 60)
    mfcr    r0, epsr
    stw     r0, (sp, 64)
    mfcr    r0, epc
    stw     r0, (sp, 68)

    mov     r0, sp

    jbsr    trap_c


.text

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
    subi    sp, 4
    stw     r15, (sp)
    jbsr    do_irq
    ldw     r15, (sp)
    addi    sp, 4
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

