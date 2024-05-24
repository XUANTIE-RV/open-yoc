/*
 * Copyright (C) 2018-2024 Alibaba Group Holding Limited
 */

/*
 * attention: don't modify this file as a suggest
 * you should copy from chip_riscv_dummy/include/asm/riscv_asm_macro.h and keep it newer
 * please contact xuantie-rtos os team if have question 
 */

#ifndef __RISCV_ASM_MACRO_H__
#define __RISCV_ASM_MACRO_H__

#include "riscv_csr.h"

.macro SAVE_VECTOR_REGISTERS
    /* t0,t1 saved before using */
#ifdef __riscv_vector
#if __riscv_xlen == 64
    addi     sp, sp, -(20+20)
    csrr     t0, vl
    store_x  t0,  (0  +0  )(sp)
    csrr     t0, vtype
    store_x  t0,  (4  +4  )(sp)
    csrr     t0, vstart
    store_x  t0,  (8  +8  )(sp)
    csrr     t0, vxsat
    store_x  t0,  (12 +12 )(sp)
    csrr     t0, vxrm
    store_x  t0,  (16 +16 )(sp)
#else
    addi     sp, sp, -20
    csrr     t0, vl
    store_x  t0,  (0)(sp)
    csrr     t0, vtype
    store_x  t0,  (4)(sp)
    csrr     t0, vstart
    store_x  t0,  (8)(sp)
    csrr     t0, vxsat
    store_x  t0,  (12)(sp)
    csrr     t0, vxrm
    store_x  t0,  (16)(sp)
#endif /*__riscv_xlen */

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
    /* t0,t1,t2 not restored before using */
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
#if __riscv_xlen == 64
    load_x   t0, (0 +0)(sp)
    load_x   t1, (4 +4)(sp)
    load_x   t2, (8 +8)(sp)
    vsetvl   zero, t0, t1
    csrw     vstart, t2
    load_x   t2, (12 +12)(sp)
    csrw     vxsat, t2
    load_x   t2, (16 +16)(sp)
    csrw     vxrm, t2
    addi     sp, sp, (20+20)
#else
    load_x   t0, (0)(sp)
    load_x   t1, (4)(sp)
    load_x   t2, (8)(sp)
    vsetvl   zero, t0, t1
    csrw     vstart, t2
    load_x   t2, (12)(sp)
    csrw     vxsat, t2
    load_x   t2, (16)(sp)
    csrw     vxrm, t2
    addi     sp, sp, 20
#endif /*__riscv_xlen */

#endif /*__riscv_vector*/
.endm


.macro SAVE_FLOAT_REGISTERS
    /* t0 saved before using */
#if __riscv_flen == 64
#if __riscv_xlen == 64
    addi     sp, sp, -(4+4)
    frcsr    t0
    store_x  t0, (0  +0  )(sp)
#else
    addi     sp, sp, -4
    frcsr    t0
    store_x  t0, 0(sp)
#endif /*__riscv_xlen */

    addi     sp,  sp, -160
    fstore_x ft0, (0 +0 )(sp)
    fstore_x ft1, (4 +4 )(sp)
    fstore_x ft2, (8 +8 )(sp)
    fstore_x ft3, (12+12)(sp)
    fstore_x ft4, (16+16)(sp)
    fstore_x ft5, (20+20)(sp)
    fstore_x ft6, (24+24)(sp)
    fstore_x ft7, (28+28)(sp)
    fstore_x fa0, (32+32)(sp)
    fstore_x fa1, (36+36)(sp)
    fstore_x fa2, (40+40)(sp)
    fstore_x fa3, (44+44)(sp)
    fstore_x fa4, (48+48)(sp)
    fstore_x fa5, (52+52)(sp)
    fstore_x fa6, (56+56)(sp)
    fstore_x fa7, (60+60)(sp)
    fstore_x ft8, (64+64)(sp)
    fstore_x ft9, (68+68)(sp)
    fstore_x ft10,(72+72)(sp)
    fstore_x ft11,(76+76)(sp)
#elif __riscv_flen == 32
    addi     sp, sp, -4
    frcsr    t0
    store_x  t0, 0(sp)

    addi     sp,  sp, -80
    fstore_x ft0, 0(sp)
    fstore_x ft1, 4(sp)
    fstore_x ft2, 8(sp)
    fstore_x ft3, 12(sp)
    fstore_x ft4, 16(sp)
    fstore_x ft5, 20(sp)
    fstore_x ft6, 24(sp)
    fstore_x ft7, 28(sp)
    fstore_x fa0, 32(sp)
    fstore_x fa1, 36(sp)
    fstore_x fa2, 40(sp)
    fstore_x fa3, 44(sp)
    fstore_x fa4, 48(sp)
    fstore_x fa5, 52(sp)
    fstore_x fa6, 56(sp)
    fstore_x fa7, 60(sp)
    fstore_x ft8, 64(sp)
    fstore_x ft9, 68(sp)
    fstore_x ft10,72(sp)
    fstore_x ft11,76(sp)
#endif /*__riscv_flen */
.endm

.macro RESTORE_FLOAT_REGISTERS
    /* t0 not restored before using */
#if __riscv_flen == 64
    fload_x  ft0, (0 +0 )(sp)
    fload_x  ft1, (4 +4 )(sp)
    fload_x  ft2, (8 +8 )(sp)
    fload_x  ft3, (12+12)(sp)
    fload_x  ft4, (16+16)(sp)
    fload_x  ft5, (20+20)(sp)
    fload_x  ft6, (24+24)(sp)
    fload_x  ft7, (28+28)(sp)
    fload_x  fa0, (32+32)(sp)
    fload_x  fa1, (36+36)(sp)
    fload_x  fa2, (40+40)(sp)
    fload_x  fa3, (44+44)(sp)
    fload_x  fa4, (48+48)(sp)
    fload_x  fa5, (52+52)(sp)
    fload_x  fa6, (56+56)(sp)
    fload_x  fa7, (60+60)(sp)
    fload_x  ft8, (64+64)(sp)
    fload_x  ft9, (68+68)(sp)
    fload_x  ft10,(72+72)(sp)
    fload_x  ft11,(76+76)(sp)
    addi     sp, sp, 160

#if __riscv_xlen == 64
    load_x   t0, (0 +0)(sp)
    fscsr    t0
    addi     sp, sp, (4+4)
#else
    load_x   t0, 0(sp)
    fscsr    t0
    addi     sp, sp, 4
#endif /*__riscv_xlen */
#elif __riscv_flen == 32
    fload_x  ft0, 0(sp)
    fload_x  ft1, 4(sp)
    fload_x  ft2, 8(sp)
    fload_x  ft3, 12(sp)
    fload_x  ft4, 16(sp)
    fload_x  ft5, 20(sp)
    fload_x  ft6, 24(sp)
    fload_x  ft7, 28(sp)
    fload_x  fa0, 32(sp)
    fload_x  fa1, 36(sp)
    fload_x  fa2, 40(sp)
    fload_x  fa3, 44(sp)
    fload_x  fa4, 48(sp)
    fload_x  fa5, 52(sp)
    fload_x  fa6, 56(sp)
    fload_x  fa7, 60(sp)
    fload_x  ft8, 64(sp)
    fload_x  ft9, 68(sp)
    fload_x  ft10,72(sp)
    fload_x  ft11,76(sp)
    addi     sp, sp, 80

    load_x   t0, 0(sp)
    fscsr    t0
    addi     sp, sp, 4
#endif /*__riscv_flen */
.endm

.macro SAVE_MATRIX_REGISTERS
    /* t0,t1 saved before using */
#ifdef __riscv_matrix
#if __riscv_xlen == 64
    addi     sp, sp, -(12+12)
    csrr     t0, xmrstart
    store_x  t0,  (0  +0  )(sp)
    csrr     t0, xmcsr
    store_x  t0,  (4  +4  )(sp)
    csrr     t0, xmsize
    store_x  t0,  (8  +8  )(sp)
#else
    addi     sp, sp, -12
    csrr     t0, xmrstart
    store_x  t0,  (0)(sp)
    csrr     t0, xmcsr
    store_x  t0,  (4)(sp)
    csrr     t0, xmsize
    store_x  t0,  (8)(sp)
#endif /*__riscv_xlen */

    csrr     t0, xmlenb
    slli     t1, t0, 3
    sub      sp, sp, t1
    csrw     xmrstart, x0
    mst8mb   m0, (sp)
#endif /*__riscv_matrix*/
.endm

.macro RESTORE_MATRIX_REGISTERS
    /* t0,t1 not restored before using */
#ifdef __riscv_matrix
    csrr     t0, xmlenb
    slli     t1, t0, 3
    csrw     xmrstart, x0
    mld8mb   m0, (sp)
    add      sp, sp, t1
#if __riscv_xlen == 64
    load_x   t0, (0 + 0)(sp)
    csrw     xmrstart, t0
    load_x   t0, (4 + 4)(sp)
    csrw     xmcsr, t0
    load_x   t0, (8 + 8)(sp)
    csrw     xmsize, t0
    addi     sp, sp, (12+12)
#else
    load_x   t0, (0)(sp)
    csrw     xmrstart, t0
    load_x   t0, (4)(sp)
    csrw     xmcsr, t0
    load_x   t0, (8)(sp)
    csrw     xmsize, t0
    addi     sp, sp, 12
#endif /*__riscv_xlen */

#endif /*__riscv_matrix*/
.endm



#endif /* __RISCV_ASM_MACRO_H__ */

