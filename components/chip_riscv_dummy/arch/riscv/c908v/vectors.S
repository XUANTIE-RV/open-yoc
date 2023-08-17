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

.macro SAVE_VECTOR_REGISTERS
#ifdef __riscv_vector
    addi    sp, sp, -(20+20)
    csrr    t0, vl
    sd      t0,  (0  +0  )(sp)
    csrr    t0, vtype
    sd      t0,  (4  +4  )(sp)
    csrr    t0, vstart
    sd      t0,  (8  +8  )(sp)
    csrr    t0, vxsat
    sd      t0,  (12 +12 )(sp)
    csrr    t0, vxrm
    sd      t0,  (16 +16 )(sp)

    csrr     t0, vlenb
    slli     t0, t0, 3
    slli     t1, t0, 2
    sub      sp, sp, t1
    vsetvli  zero, zero, e8, m8
#if (__riscv_v == 7000)
    vsb.v    v0, (sp)
    add      sp, sp, t0
    vsb.v    v8, (sp)
    add      sp, sp, t0
    vsb.v    v16, (sp)
    add      sp, sp, t0
    vsb.v    v24, (sp)
#elif (__riscv_v == 1000000)
    vs8r.v   v0, (sp)
    add      sp, sp, t0
    vs8r.v   v8, (sp)
    add      sp, sp, t0
    vs8r.v   v16, (sp)
    add      sp, sp, t0
    vs8r.v   v24, (sp)
#endif
    sub      t0, t1, t0
    sub      sp, sp, t0
#endif /*__riscv_vector*/
.endm

.macro RESTORE_VECTOR_REGISTERS
#ifdef __riscv_vector
    csrr     t0, vlenb
    slli     t0, t0, 3
    vsetvli  zero, zero, e8, m8
#if (__riscv_v == 7000)
    vlb.v    v0, (sp)
    add      sp, sp, t0
    vlb.v    v8, (sp)
    add      sp, sp, t0
    vlb.v    v16, (sp)
    add      sp, sp, t0
    vlb.v    v24, (sp)
    add      sp, sp, t0
#elif (__riscv_v == 1000000)
    vl8r.v   v0, (sp)
    add      sp, sp, t0
    vl8r.v   v8, (sp)
    add      sp, sp, t0
    vl8r.v   v16, (sp)
    add      sp, sp, t0
    vl8r.v   v24, (sp)
    add      sp, sp, t0
#endif
    lwu     t0, (0 +0)(sp)
    lwu     t1, (4 +4)(sp)
    lwu     t2, (8 +8)(sp)
    vsetvl  zero, t0, t1
    csrw    vstart, t2
    lwu     t2, (12 +12)(sp)
    csrw    vxsat, t2
    lwu     t2, (16 +16)(sp)
    csrw    vxrm, t2
    addi    sp, sp, (20+20)
#endif /*__riscv_vector*/
.endm

.section .bss
    .align  3
    .global g_trapstackalloc
    .global g_trapstackbase
    .global g_top_trapstack
g_trapstackalloc:
g_trapstackbase:
    .space CONFIG_ARCH_INTERRUPTSTACK
g_top_trapstack:

    .align 2
    .globl g_trap_sp
    .type  g_trap_sp, object
g_trap_sp:
    .long 0
    .size g_trap_sp, .-g_trap_sp

irq_nested_level:
    .long 0

.text
.global _interrupt_return_address

    .align  2
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

    addi    sp,  sp, -160
    fsd     ft0, (0 +0 )(sp)
    fsd     ft1, (4 +4 )(sp)
    fsd     ft2, (8 +8 )(sp)
    fsd     ft3, (12+12)(sp)
    fsd     ft4, (16+16)(sp)
    fsd     ft5, (20+20)(sp)
    fsd     ft6, (24+24)(sp)
    fsd     ft7, (28+28)(sp)
    fsd     fa0, (32+32)(sp)
    fsd     fa1, (36+36)(sp)
    fsd     fa2, (40+40)(sp)
    fsd     fa3, (44+44)(sp)
    fsd     fa4, (48+48)(sp)
    fsd     fa5, (52+52)(sp)
    fsd     fa6, (56+56)(sp)
    fsd     fa7, (60+60)(sp)
    fsd     ft8, (64+64)(sp)
    fsd     ft9, (68+68)(sp)
    fsd     ft10,(72+72)(sp)
    fsd     ft11,(76+76)(sp)

    SAVE_VECTOR_REGISTERS

    la      t2, do_irq
    jalr    t2

    RESTORE_VECTOR_REGISTERS

    fld     ft0, (0 +0 )(sp)
    fld     ft1, (4 +4 )(sp)
    fld     ft2, (8 +8 )(sp)
    fld     ft3, (12+12)(sp)
    fld     ft4, (16+16)(sp)
    fld     ft5, (20+20)(sp)
    fld     ft6, (24+24)(sp)
    fld     ft7, (28+28)(sp)
    fld     fa0, (32+32)(sp)
    fld     fa1, (36+36)(sp)
    fld     fa2, (40+40)(sp)
    fld     fa3, (44+44)(sp)
    fld     fa4, (48+48)(sp)
    fld     fa5, (52+52)(sp)
    fld     fa6, (56+56)(sp)
    fld     fa7, (60+60)(sp)
    fld     ft8, (64+64)(sp)
    fld     ft9, (68+68)(sp)
    fld     ft10,(72+72)(sp)
    fld     ft11,(76+76)(sp)

    addi    sp, sp, 160

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


    .align  2
    .global Sirq_Handler
    .weak   Sirq_Handler
    .type   Sirq_Handler, %function
Sirq_Handler:
    csrw    mscratch, sp
    la      sp, g_top_irqstack

    addi    sp, sp, -(76+76)
    sd      t0, (4+4)(sp)
    sd      t1, (8+8)(sp)
    sd      t2, (12+12)(sp)

    csrr    t0, sepc
    sd      t0, (68+68)(sp)
    sd      t2, (72+72)(sp)
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

    addi    sp,  sp, -160
    fsd     ft0, (0 +0 )(sp)
    fsd     ft1, (4 +4 )(sp)
    fsd     ft2, (8 +8 )(sp)
    fsd     ft3, (12+12)(sp)
    fsd     ft4, (16+16)(sp)
    fsd     ft5, (20+20)(sp)
    fsd     ft6, (24+24)(sp)
    fsd     ft7, (28+28)(sp)
    fsd     fa0, (32+32)(sp)
    fsd     fa1, (36+36)(sp)
    fsd     fa2, (40+40)(sp)
    fsd     fa3, (44+44)(sp)
    fsd     fa4, (48+48)(sp)
    fsd     fa5, (52+52)(sp)
    fsd     fa6, (56+56)(sp)
    fsd     fa7, (60+60)(sp)
    fsd     ft8, (64+64)(sp)
    fsd     ft9, (68+68)(sp)
    fsd     ft10,(72+72)(sp)
    fsd     ft11,(76+76)(sp)

    andi    t2, t2, 0x3FF
    slli    t2, t2, 3

    li      t0, 7
    la      t2, do_irq
    jalr    t2

    /* write PLIC_CLAIM and exit interrupt */
    ld      a1, (72+72+160)(sp)
    andi    a0, a1, 0x3FF

    fld     ft0, (0 +0 )(sp)
    fld     ft1, (4 +4 )(sp)
    fld     ft2, (8 +8 )(sp)
    fld     ft3, (12+12)(sp)
    fld     ft4, (16+16)(sp)
    fld     ft5, (20+20)(sp)
    fld     ft6, (24+24)(sp)
    fld     ft7, (28+28)(sp)
    fld     fa0, (32+32)(sp)
    fld     fa1, (36+36)(sp)
    fld     fa2, (40+40)(sp)
    fld     fa3, (44+44)(sp)
    fld     fa4, (48+48)(sp)
    fld     fa5, (52+52)(sp)
    fld     fa6, (56+56)(sp)
    fld     fa7, (60+60)(sp)
    fld     ft8, (64+64)(sp)
    fld     ft9, (68+68)(sp)
    fld     ft10,(72+72)(sp)
    fld     ft11,(76+76)(sp)

    addi    sp, sp, 160

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
    csrr    sp, mscratch
    sret


    .align  2
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
    sd      t2, (72+72)(sp)
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

    addi    sp,  sp, -160
    fsd     ft0, (0 +0 )(sp)
    fsd     ft1, (4 +4 )(sp)
    fsd     ft2, (8 +8 )(sp)
    fsd     ft3, (12+12)(sp)
    fsd     ft4, (16+16)(sp)
    fsd     ft5, (20+20)(sp)
    fsd     ft6, (24+24)(sp)
    fsd     ft7, (28+28)(sp)
    fsd     fa0, (32+32)(sp)
    fsd     fa1, (36+36)(sp)
    fsd     fa2, (40+40)(sp)
    fsd     fa3, (44+44)(sp)
    fsd     fa4, (48+48)(sp)
    fsd     fa5, (52+52)(sp)
    fsd     fa6, (56+56)(sp)
    fsd     fa7, (60+60)(sp)
    fsd     ft8, (64+64)(sp)
    fsd     ft9, (68+68)(sp)
    fsd     ft10,(72+72)(sp)
    fsd     ft11,(76+76)(sp)

    SAVE_VECTOR_REGISTERS

    la      t2, do_irq
    jalr    t2
_interrupt_return_address:
    RESTORE_VECTOR_REGISTERS

    fld     ft0, (0 +0 )(sp)
    fld     ft1, (4 +4 )(sp)
    fld     ft2, (8 +8 )(sp)
    fld     ft3, (12+12)(sp)
    fld     ft4, (16+16)(sp)
    fld     ft5, (20+20)(sp)
    fld     ft6, (24+24)(sp)
    fld     ft7, (28+28)(sp)
    fld     fa0, (32+32)(sp)
    fld     fa1, (36+36)(sp)
    fld     fa2, (40+40)(sp)
    fld     fa3, (44+44)(sp)
    fld     fa4, (48+48)(sp)
    fld     fa5, (52+52)(sp)
    fld     fa6, (56+56)(sp)
    fld     fa7, (60+60)(sp)
    fld     ft8, (64+64)(sp)
    fld     ft9, (68+68)(sp)
    fld     ft10,(72+72)(sp)
    fld     ft11,(76+76)(sp)

    addi    sp, sp, 160

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
    .align  2
    .global trap
    .type   trap, %function
trap:
    csrw    mscratch, sp
    la      t0, g_trap_sp
    addi    t0, t0, -(132+132)
    sd      x1, ( 0 + 0 )(t0)
    sd      x2, ( 4 + 4 )(t0)
    sd      x3, ( 8 + 8 )(t0)
    sd      x4, ( 12+ 12)(t0)
    sd      x5, ( 16+ 16)(t0)
    sd      x6, ( 20+ 20)(t0)
    sd      x7, ( 24+ 24)(t0)
    sd      x8, ( 28+ 28)(t0)
    sd      x9, ( 32+ 32)(t0)
    sd      x10,( 36+ 36)(t0)
    sd      x11,( 40+ 40)(t0)
    sd      x12,( 44+ 44)(t0)
    sd      x13,( 48+ 48)(t0)
    sd      x14,( 52+ 52)(t0)
    sd      x15,( 56+ 56)(t0)
    sd      x16,( 60+ 60)(t0)
    sd      x17,( 64+ 64)(t0)
    sd      x18,( 68+ 68)(t0)
    sd      x19,( 72+ 72)(t0)
    sd      x20,( 76+ 76)(t0)
    sd      x21,( 80+ 80)(t0)
    sd      x22,( 84+ 84)(t0)
    sd      x23,( 88+ 88)(t0)
    sd      x24,( 92+ 92)(t0)
    sd      x25,( 96+ 96)(t0)
    sd      x26,(100+100)(t0)
    sd      x27,(104+104)(t0)
    sd      x28,(108+108)(t0)
    sd      x29,(112+112)(t0)
    sd      x30,(116+116)(t0)
    sd      x31,(120+120)(t0)
    csrr    a0, mepc
    sd      a0, (124+124)(t0)
    csrr    a0, mstatus
    sd      a0, (128+128)(t0)

    mv      a0, t0
    mv      sp, a0

    la      a1, exceptionHandler
    jalr    a1

    .align  6
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
