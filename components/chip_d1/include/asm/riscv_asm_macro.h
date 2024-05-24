/*
 * Copyright (C) 2018-2023 Alibaba Group Holding Limited
 */

/*
 * attention: don't modify this file as a suggest
 * you should copy from chip_riscv_dummy/include/asm/riscv_asm_macro.h and keep it newer
 * please contact xuantie-rtos os team if have question 
 */

#ifndef __RISCV_ASM_MACRO_H__
#define __RISCV_ASM_MACRO_H__

#if __riscv_xlen == 64
.macro SAVE_VECTOR_REGISTERS
    /* t0,t1 saved before using */
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
#endif /*__riscv_xlen */


.macro SAVE_FLOAT_REGISTERS
    /* t0 saved before using */
#if __riscv_flen == 64
#if __riscv_xlen == 64
    addi     sp, sp, -(4+4)
    frcsr    t0
    sd       t0, (0  +0  )(sp)
#else
    addi     sp, sp, -4
    frcsr    t0
    sw       t0, 0(sp)
#endif /*__riscv_xlen */

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
#elif __riscv_flen == 32
    addi     sp, sp, -4
    frcsr    t0
    sw       t0, 0(sp)

    addi    sp,  sp, -80
    fsw     ft0, 0(sp)
    fsw     ft1, 4(sp)
    fsw     ft2, 8(sp)
    fsw     ft3, 12(sp)
    fsw     ft4, 16(sp)
    fsw     ft5, 20(sp)
    fsw     ft6, 24(sp)
    fsw     ft7, 28(sp)
    fsw     fa0, 32(sp)
    fsw     fa1, 36(sp)
    fsw     fa2, 40(sp)
    fsw     fa3, 44(sp)
    fsw     fa4, 48(sp)
    fsw     fa5, 52(sp)
    fsw     fa6, 56(sp)
    fsw     fa7, 60(sp)
    fsw     ft8, 64(sp)
    fsw     ft9, 68(sp)
    fsw     ft10,72(sp)
    fsw     ft11,76(sp)
#endif /*__riscv_flen */
.endm

.macro RESTORE_FLOAT_REGISTERS
    /* t0 not restored before using */
#if __riscv_flen == 64
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

#if __riscv_xlen == 64
    ld       t0, (0 +0)(sp)
    fscsr    t0
    addi     sp, sp, (4+4)
#else
    lw       t0, 0(sp)
    fscsr    t0
    addi     sp, sp, 4
#endif /*__riscv_xlen */
#elif __riscv_flen == 32
    flw     ft0, 0(sp)
    flw     ft1, 4(sp)
    flw     ft2, 8(sp)
    flw     ft3, 12(sp)
    flw     ft4, 16(sp)
    flw     ft5, 20(sp)
    flw     ft6, 24(sp)
    flw     ft7, 28(sp)
    flw     fa0, 32(sp)
    flw     fa1, 36(sp)
    flw     fa2, 40(sp)
    flw     fa3, 44(sp)
    flw     fa4, 48(sp)
    flw     fa5, 52(sp)
    flw     fa6, 56(sp)
    flw     fa7, 60(sp)
    flw     ft8, 64(sp)
    flw     ft9, 68(sp)
    flw     ft10,72(sp)
    flw     ft11,76(sp)
    addi    sp, sp, 80

    lw       t0, 0(sp)
    fscsr    t0
    addi     sp, sp, 4
#endif /*__riscv_flen */
.endm


#endif /* __RISCV_ASM_MACRO_H__ */

