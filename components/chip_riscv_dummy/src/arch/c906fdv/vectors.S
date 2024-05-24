/*
 * Copyright (C) 2017-2024 Alibaba Group Holding Limited
 */

#include "riscv_asm_macro.h"

.section .stack
    .align  4
    .global g_trapstackbase
    .global g_top_trapstack
g_trapstackbase:
    .space CONFIG_ARCH_INTERRUPTSTACK
g_top_trapstack:

.text
.global _interrupt_return_address

    .align  3
    .global Scoret_Handler
    .weak   Scoret_Handler
    .type   Scoret_Handler, %function
Scoret_Handler:
    csrw    sscratch, sp
    la      sp, g_top_irqstack

    addi    sp, sp, -(76+76)
    sd      t0, (4+4)(sp)
    sd      t1, (8+8)(sp)
    sd      t2, (12+12)(sp)

    csrr    t0, sepc
    sd      t0, (68+68)(sp)
    csrr    t0, sstatus
    sd      t0, (72+72)(sp)

    sd      ra, (0 +0 )(sp)
    sd      a0, (16+16)(sp)
    sd      a1, (20+20)(sp)
    sd      a2, (24+24)(sp)
    sd      a3, (28+28)(sp)
    sd      a4, (32+32)(sp)
    sd      a5, (36+36)(sp)
    sd      a6, (40+40)(sp)
    sd      a7, (44+44)(sp)
    sd      t3, (48+48)(sp)
    sd      t4, (52+52)(sp)
    sd      t5, (56+56)(sp)
    sd      t6, (60+60)(sp)

    SAVE_FLOAT_REGISTERS
    SAVE_VECTOR_REGISTERS

    la      t2, CORET_IRQHandler
    jalr    t2

    RESTORE_VECTOR_REGISTERS
    RESTORE_FLOAT_REGISTERS

    ld      t0, (72+72)(sp)
    csrw    sstatus, t0
    ld      t0, (68+68)(sp)
    csrw    sepc, t0

    ld      ra, (0 +0 )(sp)
    ld      t0, (4 +4 )(sp)
    ld      t1, (8 +8 )(sp)
    ld      t2, (12+12)(sp)
    ld      a0, (16+16)(sp)
    ld      a1, (20+20)(sp)
    ld      a2, (24+24)(sp)
    ld      a3, (28+28)(sp)
    ld      a4, (32+32)(sp)
    ld      a5, (36+36)(sp)
    ld      a6, (40+40)(sp)
    ld      a7, (44+44)(sp)
    ld      t3, (48+48)(sp)
    ld      t4, (52+52)(sp)
    ld      t5, (56+56)(sp)
    ld      t6, (60+60)(sp)

    addi    sp, sp, (76+76)
    csrr    sp, sscratch
    sret


    .align  3
    .global Mcoret_Handler
    .weak   Mcoret_Handler
    .type   Mcoret_Handler, %function
Mcoret_Handler:
    csrw    mscratch, sp
    la      sp, g_top_irqstack

    addi    sp, sp, -(76+76)
    sd      t0, (4+4)(sp)
    sd      t1, (8+8)(sp)
    sd      t2, (12+12)(sp)

    csrr    t0, mepc
    sd      t0, (68+68)(sp)
    csrr    t0, mstatus
    sd      t0, (72+72)(sp)

    sd      ra, (0 +0 )(sp)
    sd      a0, (16+16)(sp)
    sd      a1, (20+20)(sp)
    sd      a2, (24+24)(sp)
    sd      a3, (28+28)(sp)
    sd      a4, (32+32)(sp)
    sd      a5, (36+36)(sp)
    sd      a6, (40+40)(sp)
    sd      a7, (44+44)(sp)
    sd      t3, (48+48)(sp)
    sd      t4, (52+52)(sp)
    sd      t5, (56+56)(sp)
    sd      t6, (60+60)(sp)

    SAVE_FLOAT_REGISTERS
    SAVE_VECTOR_REGISTERS

    la      t2, CORET_IRQHandler
    jalr    t2

    RESTORE_VECTOR_REGISTERS
    RESTORE_FLOAT_REGISTERS

    ld      t0, (72+72)(sp)
    csrw    mstatus, t0
    ld      t0, (68+68)(sp)
    csrw    mepc, t0

    ld      ra, (0 +0 )(sp)
    ld      t0, (4 +4 )(sp)
    ld      t1, (8 +8 )(sp)
    ld      t2, (12+12)(sp)
    ld      a0, (16+16)(sp)
    ld      a1, (20+20)(sp)
    ld      a2, (24+24)(sp)
    ld      a3, (28+28)(sp)
    ld      a4, (32+32)(sp)
    ld      a5, (36+36)(sp)
    ld      a6, (40+40)(sp)
    ld      a7, (44+44)(sp)
    ld      t3, (48+48)(sp)
    ld      t4, (52+52)(sp)
    ld      t5, (56+56)(sp)
    ld      t6, (60+60)(sp)

    addi    sp, sp, (76+76)
    csrr    sp, mscratch
    mret


    .align  3
    .global Sirq_Handler
    .weak   Sirq_Handler
    .type   Sirq_Handler, %function
Sirq_Handler:
    csrw    sscratch, sp
    la      sp, g_top_irqstack
    addi    sp, sp, -(76+76)
    sd      t0, (4+4)(sp)
    sd      t1, (8+8)(sp)
    sd      t2, (12+12)(sp)

    csrr    t0, sepc
    sd      t0, (68+68)(sp)
    csrr    t0, sstatus
    sd      t0, (72+72)(sp)

    sd      ra, (0 +0 )(sp)
    sd      a0, (16+16)(sp)
    sd      a1, (20+20)(sp)
    sd      a2, (24+24)(sp)
    sd      a3, (28+28)(sp)
    sd      a4, (32+32)(sp)
    sd      a5, (36+36)(sp)
    sd      a6, (40+40)(sp)
    sd      a7, (44+44)(sp)
    sd      t3, (48+48)(sp)
    sd      t4, (52+52)(sp)
    sd      t5, (56+56)(sp)
    sd      t6, (60+60)(sp)

    SAVE_FLOAT_REGISTERS
    SAVE_VECTOR_REGISTERS

    la      t2, do_irq
    jalr    t2
    RESTORE_VECTOR_REGISTERS
    RESTORE_FLOAT_REGISTERS

    ld      t0, (72+72)(sp)
    csrw    sstatus, t0
    ld      t0, (68+68)(sp)
    csrw    sepc, t0

    ld      ra, (0 +0 )(sp)
    ld      t0, (4 +4 )(sp)
    ld      t1, (8 +8 )(sp)
    ld      t2, (12+12)(sp)
    ld      a0, (16+16)(sp)
    ld      a1, (20+20)(sp)
    ld      a2, (24+24)(sp)
    ld      a3, (28+28)(sp)
    ld      a4, (32+32)(sp)
    ld      a5, (36+36)(sp)
    ld      a6, (40+40)(sp)
    ld      a7, (44+44)(sp)
    ld      t3, (48+48)(sp)
    ld      t4, (52+52)(sp)
    ld      t5, (56+56)(sp)
    ld      t6, (60+60)(sp)

    addi    sp, sp, (76+76)
    csrr    sp, sscratch
    sret


    .align  3
    .global Mirq_Handler
    .weak   Mirq_Handler
    .type   Mirq_Handler, %function
Mirq_Handler:
    csrw    mscratch, sp
    la      sp, g_top_irqstack
    addi    sp, sp, -(76+76)
    sd      t0, (4+4)(sp)
    sd      t1, (8+8)(sp)
    sd      t2, (12+12)(sp)

    csrr    t0, mepc
    sd      t0, (68+68)(sp)
    csrr    t0, mstatus
    sd      t0, (72+72)(sp)

    sd      ra, (0 +0 )(sp)
    sd      a0, (16+16)(sp)
    sd      a1, (20+20)(sp)
    sd      a2, (24+24)(sp)
    sd      a3, (28+28)(sp)
    sd      a4, (32+32)(sp)
    sd      a5, (36+36)(sp)
    sd      a6, (40+40)(sp)
    sd      a7, (44+44)(sp)
    sd      t3, (48+48)(sp)
    sd      t4, (52+52)(sp)
    sd      t5, (56+56)(sp)
    sd      t6, (60+60)(sp)

    SAVE_FLOAT_REGISTERS
    SAVE_VECTOR_REGISTERS

    la      t2, do_irq
    jalr    t2
_interrupt_return_address:
    RESTORE_VECTOR_REGISTERS
    RESTORE_FLOAT_REGISTERS

    ld      t0, (72+72)(sp)
    csrw    mstatus, t0
    ld      t0, (68+68)(sp)
    csrw    mepc, t0

    ld      ra, (0 +0 )(sp)
    ld      t0, (4 +4 )(sp)
    ld      t1, (8 +8 )(sp)
    ld      t2, (12+12)(sp)
    ld      a0, (16+16)(sp)
    ld      a1, (20+20)(sp)
    ld      a2, (24+24)(sp)
    ld      a3, (28+28)(sp)
    ld      a4, (32+32)(sp)
    ld      a5, (36+36)(sp)
    ld      a6, (40+40)(sp)
    ld      a7, (44+44)(sp)
    ld      t3, (48+48)(sp)
    ld      t4, (52+52)(sp)
    ld      t5, (56+56)(sp)
    ld      t6, (60+60)(sp)

    addi    sp, sp, (76+76)
    csrr    sp, mscratch
    mret


/******************************************************************************
 * Functions:
 *     void trap(void);
 * default exception handler
 ******************************************************************************/
    .align  3
    .global trap
    .type   trap, %function
trap:
    csrw    mscratch, sp
    la      sp, g_top_trapstack
    addi    sp, sp, -(140+140)
    sd      x1, ( 0 + 0 )(sp)
    sd      x3, ( 8 + 8 )(sp)
    sd      x4, ( 12+ 12)(sp)
    sd      x5, ( 16+ 16)(sp)
    sd      x6, ( 20+ 20)(sp)
    sd      x7, ( 24+ 24)(sp)
    sd      x8, ( 28+ 28)(sp)
    sd      x9, ( 32+ 32)(sp)
    sd      x10,( 36+ 36)(sp)
    sd      x11,( 40+ 40)(sp)
    sd      x12,( 44+ 44)(sp)
    sd      x13,( 48+ 48)(sp)
    sd      x14,( 52+ 52)(sp)
    sd      x15,( 56+ 56)(sp)
    sd      x16,( 60+ 60)(sp)
    sd      x17,( 64+ 64)(sp)
    sd      x18,( 68+ 68)(sp)
    sd      x19,( 72+ 72)(sp)
    sd      x20,( 76+ 76)(sp)
    sd      x21,( 80+ 80)(sp)
    sd      x22,( 84+ 84)(sp)
    sd      x23,( 88+ 88)(sp)
    sd      x24,( 92+ 92)(sp)
    sd      x25,( 96+ 96)(sp)
    sd      x26,(100+100)(sp)
    sd      x27,(104+104)(sp)
    sd      x28,(108+108)(sp)
    sd      x29,(112+112)(sp)
    sd      x30,(116+116)(sp)
    sd      x31,(120+120)(sp)
    csrr    a0, mepc
    sd      a0, (124+124)(sp)
    csrr    a0, mstatus
    sd      a0, (128+128)(sp)
    csrr    a0, mcause
    sd      a0, (132+132)(sp)
    csrr    a0, mtval
    sd      a0, (136+136)(sp)
    csrr    a0, mscratch
    sd      a0, ( 4 + 4 )(sp)

    mv      a0, sp
    la      a1, exceptionHandler
    jalr    a1

    .align  3
    .weak   Default_Handler
    .global Default_Handler
    .type   Default_Handler, %function
Default_Handler:
    j       trap

    .size   Default_Handler, . - Default_Handler

/*    Macro to define default handlers. Default handler
 *    will be weak symbol and just dead loops. They can be
 *    overwritten by other handlers */
    .macro  def_irq_handler handler_name
    .weak   \handler_name
    .globl  \handler_name
    .set    \handler_name, Default_Handler
    .endm

    def_irq_handler Stspend_Handler
    def_irq_handler Mtspend_Handler
    def_irq_handler CORET_IRQHandler
