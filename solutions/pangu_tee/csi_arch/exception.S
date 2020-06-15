/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include "tee_misc.h"
#include "tee_addr_map.h"


#define EXCEPTION_STACK_SIZE    (240)

.global sys_exception_handler
.global wsc_intr_handler
.import reboot
.import dispatcher_main
.extern unsigned int g_ntw_addr
/*
 * Common exception  handler. Standard exceptions come through here first
 */
    .text
    .balign 4
sys_exception_handler:
    bsr reboot

    .text
    .balign 4
    /*
     * wsc_intr_handler: this handler only handle NTW's new request!!!
     */
wsc_intr_handler:
    /*
     * now 1. all interrupts/exceptions are unmasked.
     *     2. non-secure NS_PSR, NS_PC are pushed to non-secure stack.
     *     2. sp is secure world svc sp.
     */

    /* 1. temporary store r4-r5 */
    subi sp, 8
    stm r4-r5, (sp)

    /* 2. disable secure IE for safe */
    mfcr r4, psr   /* secure PSR */
    bclri r4, 6
    mtcr r4, psr

    /* 3. check our status if really need to handle this new request */
        /* S  == 1 */
        /* SE == 1 */
        /* SP == 0 */
        /* HS == 0 */
        /* SC == 1 */
    lrw     r5, 0xf8000000
    and     r4, r4, r5
    lsri    r4, r4, 24
    cmpnei  r4, 0xc8
    bf      __valid_condition

    /* no valid status, we should direct rte */
    ldm r4-r5, (sp)
    addi sp, 8
    rte

__valid_condition:
    ldm r4-r5, (sp)
    addi sp, 8

    /* 1. save NTW GP registers to stack */
    subi sp, GP_CONTEXT_SIZE
    addi sp, 8
    stm r4-r13, (sp)
    subi sp, 8
    stw r15,    (sp, 0x30)
    addi sp, 0x34
    stm r16-r31, (sp)
    subi sp, 0x34

    /* 2. restore TW GP registers from stack */
    addi sp, GP_CONTEXT_SIZE
    addi sp, 0x30
    ldm r16-r31, (sp)
    subi sp, 0x30
    ldw r15,    (sp, 0x28)
    ldm r4-r13, (sp)

    /* 3. restore sp */
    subi sp, GP_CONTEXT_SIZE

    /* 4. enable ie */
    psrset ie
    /* now we are ready for TEE dispatcher! */
    bsr     dispatcher_main
    psrclr ie

    /* we have finished one command */

    /* 1. save TW GP registers */
    addi sp, GP_CONTEXT_SIZE
    addi sp, 8
    stm r4-r13, (sp)
    subi sp, 8
    stw r15,    (sp, 0x30)
    addi sp, 0x34
    stm r16-r31, (sp)
    subi sp, 0x34

    /* 2. restore NTW GP registers */
    subi sp, GP_CONTEXT_SIZE
    addi sp, 0x30
    ldm r16-r31, (sp)
    subi sp, 0x30
    ldw r15,    (sp, 0x28)
    ldm r4-r13, (sp)

    /* 3. restore sp */
    addi sp, GP_CONTEXT_SIZE

    /* now we are ready to return */
    rte

.globl rte_to_ntw
    .type rte_to_ntw, function
rte_to_ntw:

    /* 1. save TW GP registers to current stack */
    subi sp, GP_CONTEXT_SIZE

    movi r0, 0
    bseti r0, 31
    bseti r0, 30
    stw r0, (sp)
    lrw r0, tee_defs_entry
    stw r0, (sp, 4)

    addi sp, 8
    stm r4-r13, (sp)
    subi sp, 8
    stw r15,    (sp, 0x30)
    addi sp, 0x34
    stm r16-r31, (sp)
    subi sp, 0x34

    /* 2. clear all GP registers */
    movi r0, 0
    movi r1, 0
    movi r2, 0
    movi r3, 0
    movi r4, 0
    movi r5, 0
    movi r6, 0
    movi r7, 0
    movi r8, 0
    movi r9, 0
    movi r10, 0
    movi r11, 0
    movi r12, 0
    movi r13, 0
    movi r15, 0
    movi r16, 0
    movi r17, 0
    movi r18, 0
    movi r19, 0
    movi r20, 0
    movi r21, 0
    movi r22, 0
    movi r23, 0
    movi r24, 0
    movi r25, 0
    movi r26, 0
    movi r27, 0
    movi r28, 0
    movi r29, 0
    movi r30, 0
    movi r31, 0

    /* 3. save psr to epsr, because SC == 1 in PSR */
    mfcr    r0, psr
    mtcr    r0, epsr

    /* 4. disable EE/IE clear SP */
    bclri   r0, 29  /* clear SP */
    bclri   r0, 8   /* disable EE */
    bclri   r0, 6   /* disable IE */
    mtcr    r0, psr

    /* 5. set non-secure SP */
    lrw  r0, NTW_RW_ADDR + NTW_RW_SIZE
    subi r0, 8
    mtcr r0, cr<6, 3>

    /* 6. prepare non-secure stack */
    lrw     r1, g_ntw_addr  /* set pc */
    ldw     r1, (r1, 0)

    ldw     r3, (r1, 4)
    lrw     r2, 0x594B5343
    cmpne   r3, r2
    bf      __magic_path
    ldw     r1, (r1, 0)
__magic_path:
    movi    r2, 0
    bseti   r2, 31              /* set psr */
    stw     r2, (r0, 0)
    stw     r1, (r0, 4)

    movi r0, 0
    movi r1, 0
    movi r2, 0
    movi r3, 0

    /* Ready to NTW */
    rte
