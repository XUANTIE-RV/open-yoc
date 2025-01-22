/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-02-02     lizhirui     first version
 * 2021-02-11     lizhirui     fixed gp save/store bug
 * 2021-11-18     JasonHu      add fpu registers save/restore
 */

#ifndef __STACKFRAME_H__
#define __STACKFRAME_H__

#include "cpuport.h"
#include "riscv_csr.h"

#ifdef ENABLE_FPU
#define FPU_CTX_F0_OFF   0   /* offsetof(fpu_context_t, fpustatus.f[0])  - offsetof(fpu_context_t, fpustatus.f[0]) */
#define FPU_CTX_F1_OFF   8   /* offsetof(fpu_context_t, fpustatus.f[1])  - offsetof(fpu_context_t, fpustatus.f[0]) */
#define FPU_CTX_F2_OFF   16  /* offsetof(fpu_context_t, fpustatus.f[2])  - offsetof(fpu_context_t, fpustatus.f[0]) */
#define FPU_CTX_F3_OFF   24  /* offsetof(fpu_context_t, fpustatus.f[3])  - offsetof(fpu_context_t, fpustatus.f[0]) */
#define FPU_CTX_F4_OFF   32  /* offsetof(fpu_context_t, fpustatus.f[4])  - offsetof(fpu_context_t, fpustatus.f[0]) */
#define FPU_CTX_F5_OFF   40  /* offsetof(fpu_context_t, fpustatus.f[5])  - offsetof(fpu_context_t, fpustatus.f[0]) */
#define FPU_CTX_F6_OFF   48  /* offsetof(fpu_context_t, fpustatus.f[6])  - offsetof(fpu_context_t, fpustatus.f[0]) */
#define FPU_CTX_F7_OFF   56  /* offsetof(fpu_context_t, fpustatus.f[7])  - offsetof(fpu_context_t, fpustatus.f[0]) */
#define FPU_CTX_F8_OFF   64  /* offsetof(fpu_context_t, fpustatus.f[8])  - offsetof(fpu_context_t, fpustatus.f[0]) */
#define FPU_CTX_F9_OFF   72  /* offsetof(fpu_context_t, fpustatus.f[9])  - offsetof(fpu_context_t, fpustatus.f[0]) */
#define FPU_CTX_F10_OFF  80  /* offsetof(fpu_context_t, fpustatus.f[10]) - offsetof(fpu_context_t, fpustatus.f[0]) */
#define FPU_CTX_F11_OFF  88  /* offsetof(fpu_context_t, fpustatus.f[11]) - offsetof(fpu_context_t, fpustatus.f[0]) */
#define FPU_CTX_F12_OFF  96  /* offsetof(fpu_context_t, fpustatus.f[12]) - offsetof(fpu_context_t, fpustatus.f[0]) */
#define FPU_CTX_F13_OFF  104 /* offsetof(fpu_context_t, fpustatus.f[13]) - offsetof(fpu_context_t, fpustatus.f[0]) */
#define FPU_CTX_F14_OFF  112 /* offsetof(fpu_context_t, fpustatus.f[14]) - offsetof(fpu_context_t, fpustatus.f[0]) */
#define FPU_CTX_F15_OFF  120 /* offsetof(fpu_context_t, fpustatus.f[15]) - offsetof(fpu_context_t, fpustatus.f[0]) */
#define FPU_CTX_F16_OFF  128 /* offsetof(fpu_context_t, fpustatus.f[16]) - offsetof(fpu_context_t, fpustatus.f[0]) */
#define FPU_CTX_F17_OFF  136 /* offsetof(fpu_context_t, fpustatus.f[17]) - offsetof(fpu_context_t, fpustatus.f[0]) */
#define FPU_CTX_F18_OFF  144 /* offsetof(fpu_context_t, fpustatus.f[18]) - offsetof(fpu_context_t, fpustatus.f[0]) */
#define FPU_CTX_F19_OFF  152 /* offsetof(fpu_context_t, fpustatus.f[19]) - offsetof(fpu_context_t, fpustatus.f[0]) */
#define FPU_CTX_F20_OFF  160 /* offsetof(fpu_context_t, fpustatus.f[20]) - offsetof(fpu_context_t, fpustatus.f[0]) */
#define FPU_CTX_F21_OFF  168 /* offsetof(fpu_context_t, fpustatus.f[21]) - offsetof(fpu_context_t, fpustatus.f[0]) */
#define FPU_CTX_F22_OFF  176 /* offsetof(fpu_context_t, fpustatus.f[22]) - offsetof(fpu_context_t, fpustatus.f[0]) */
#define FPU_CTX_F23_OFF  184 /* offsetof(fpu_context_t, fpustatus.f[23]) - offsetof(fpu_context_t, fpustatus.f[0]) */
#define FPU_CTX_F24_OFF  192 /* offsetof(fpu_context_t, fpustatus.f[24]) - offsetof(fpu_context_t, fpustatus.f[0]) */
#define FPU_CTX_F25_OFF  200 /* offsetof(fpu_context_t, fpustatus.f[25]) - offsetof(fpu_context_t, fpustatus.f[0]) */
#define FPU_CTX_F26_OFF  208 /* offsetof(fpu_context_t, fpustatus.f[26]) - offsetof(fpu_context_t, fpustatus.f[0]) */
#define FPU_CTX_F27_OFF  216 /* offsetof(fpu_context_t, fpustatus.f[27]) - offsetof(fpu_context_t, fpustatus.f[0]) */
#define FPU_CTX_F28_OFF  224 /* offsetof(fpu_context_t, fpustatus.f[28]) - offsetof(fpu_context_t, fpustatus.f[0]) */
#define FPU_CTX_F29_OFF  232 /* offsetof(fpu_context_t, fpustatus.f[29]) - offsetof(fpu_context_t, fpustatus.f[0]) */
#define FPU_CTX_F30_OFF  240 /* offsetof(fpu_context_t, fpustatus.f[30]) - offsetof(fpu_context_t, fpustatus.f[0]) */
#define FPU_CTX_F31_OFF  248 /* offsetof(fpu_context_t, fpustatus.f[31]) - offsetof(fpu_context_t, fpustatus.f[0]) */
#endif /* ENABLE_FPU */

.macro SAVE_ALL
    /* save general registers */
    addi sp, sp, -CTX_GENERAL_REG_NR * REGBYTES
    STORE x1,   0 * REGBYTES(sp)
    STORE x3,   2 * REGBYTES(sp)
    STORE x4,   3 * REGBYTES(sp)
    STORE x5,   4 * REGBYTES(sp)
    STORE x6,   5 * REGBYTES(sp)
    STORE x7,   6 * REGBYTES(sp)
    STORE x8,   7 * REGBYTES(sp)
    STORE x9,   8 * REGBYTES(sp)
    STORE x10,  9 * REGBYTES(sp)
    STORE x11, 10 * REGBYTES(sp)
    STORE x12, 11 * REGBYTES(sp)
    STORE x13, 12 * REGBYTES(sp)
    STORE x14, 13 * REGBYTES(sp)
    STORE x15, 14 * REGBYTES(sp)
    STORE x16, 15 * REGBYTES(sp)
    STORE x17, 16 * REGBYTES(sp)
    STORE x18, 17 * REGBYTES(sp)
    STORE x19, 18 * REGBYTES(sp)
    STORE x20, 19 * REGBYTES(sp)
    STORE x21, 20 * REGBYTES(sp)
    STORE x22, 21 * REGBYTES(sp)
    STORE x23, 22 * REGBYTES(sp)
    STORE x24, 23 * REGBYTES(sp)
    STORE x25, 24 * REGBYTES(sp)
    STORE x26, 25 * REGBYTES(sp)
    STORE x27, 26 * REGBYTES(sp)
    STORE x28, 27 * REGBYTES(sp)
    STORE x29, 28 * REGBYTES(sp)
    STORE x30, 29 * REGBYTES(sp)
    STORE x31, 30 * REGBYTES(sp)
    csrr  x1, mepc
    STORE x1,  31 * REGBYTES(sp)
#if CONFIG_CHECK_FPU_DIRTY || CONFIG_CHECK_VECTOR_DIRTY
    csrr  t3, mstatus
    STORE t3,  32 * REGBYTES(sp)
#else
    csrr  x1, mstatus
    STORE x1,  32 * REGBYTES(sp)
#endif /* CONFIG_CHECK_FPU_DIRTY || CONFIG_CHECK_VECTOR_DIRTY */

#if defined(__riscv_flen) && defined(ENABLE_FPU)
#if CONFIG_CHECK_FPU_DIRTY
    li       t1, SR_FS_DIRTY
    and      t4, t3, t1
    bne      t4, t1, 1f
#endif /* CONFIG_CHECK_FPU_DIRTY */
    /* save fcsr registers */
    addi sp, sp, -(CTX_FPU_CSR_REG_NR) * REGBYTES
    frcsr    t0
    sd       t0, (0)(sp)
    /* save fpu registers*/
    addi sp, sp, -(CTX_FPU_REG_NR) * REGBYTES
    fsd f0,  FPU_CTX_F0_OFF(sp)
    fsd f1,  FPU_CTX_F1_OFF(sp)
    fsd f2,  FPU_CTX_F2_OFF(sp)
    fsd f3,  FPU_CTX_F3_OFF(sp)
    fsd f4,  FPU_CTX_F4_OFF(sp)
    fsd f5,  FPU_CTX_F5_OFF(sp)
    fsd f6,  FPU_CTX_F6_OFF(sp)
    fsd f7,  FPU_CTX_F7_OFF(sp)
    fsd f8,  FPU_CTX_F8_OFF(sp)
    fsd f9,  FPU_CTX_F9_OFF(sp)
    fsd f10, FPU_CTX_F10_OFF(sp)
    fsd f11, FPU_CTX_F11_OFF(sp)
    fsd f12, FPU_CTX_F12_OFF(sp)
    fsd f13, FPU_CTX_F13_OFF(sp)
    fsd f14, FPU_CTX_F14_OFF(sp)
    fsd f15, FPU_CTX_F15_OFF(sp)
    fsd f16, FPU_CTX_F16_OFF(sp)
    fsd f17, FPU_CTX_F17_OFF(sp)
    fsd f18, FPU_CTX_F18_OFF(sp)
    fsd f19, FPU_CTX_F19_OFF(sp)
    fsd f20, FPU_CTX_F20_OFF(sp)
    fsd f21, FPU_CTX_F21_OFF(sp)
    fsd f22, FPU_CTX_F22_OFF(sp)
    fsd f23, FPU_CTX_F23_OFF(sp)
    fsd f24, FPU_CTX_F24_OFF(sp)
    fsd f25, FPU_CTX_F25_OFF(sp)
    fsd f26, FPU_CTX_F26_OFF(sp)
    fsd f27, FPU_CTX_F27_OFF(sp)
    fsd f28, FPU_CTX_F28_OFF(sp)
    fsd f29, FPU_CTX_F29_OFF(sp)
    fsd f30, FPU_CTX_F30_OFF(sp)
    fsd f31, FPU_CTX_F31_OFF(sp)
#if CONFIG_CHECK_FPU_DIRTY
    j   2f
1:
    /* don't store, move sp only */
    addi sp, sp, -(CTX_FPU_CSR_REG_NR + CTX_FPU_REG_NR) * REGBYTES
2:
#endif /* CONFIG_CHECK_FPU_DIRTY */
#endif /* __riscv_flen && ENABLE_FPU */

#if defined(__riscv_vector) && defined(ENABLE_VECTOR)
#if CONFIG_CHECK_VECTOR_DIRTY
    /* check if VS filed of MSTATUS is 'dirty' */
    li      t1, SR_VS_DIRTY
    and     t4, t3, t1
    bne     t4, t1, 3f
#endif
    /* save vector csr registers */
    addi    sp, sp, -(CTX_VECTOR_CSR_REG_NR) * REGBYTES
    csrr    t0, vl
    sd      t0,  (0)(sp)
    csrr    t0, vtype
    sd      t0,  (8)(sp)
    csrr    t0, vstart
    sd      t0,  (16)(sp)
    csrr    t0, vxsat
    sd      t0,  (24)(sp)
    csrr    t0, vxrm
    sd      t0,  (32)(sp)
    /* save vector registers */
    csrr t0, vlenb
    li   t1, CTX_VECTOR_REG_NR
    mul  t2, t0, t1
    sub  sp, sp, t2
    slli t0, t0, 3
    mv   t1, sp
#if (__riscv_v == 7000)
    vsetvli  zero, zero, e8, m8
    vsb.v    v0, (t1)
    add      t1, t1, t0
    vsb.v    v8, (t1)
    add      t1, t1, t0
    vsb.v    v16, (t1)
    add      t1, t1, t0
    vsb.v    v24, (t1)
#elif (__riscv_v == 1000000)
    vsetvli  zero, zero, e8, m8, ta, ma
    vs8r.v   v0, (t1)
    add      t1, t1, t0
    vs8r.v   v8, (t1)
    add      t1, t1, t0
    vs8r.v   v16, (t1)
    add      t1, t1, t0
    vs8r.v   v24, (t1)
#endif
#if CONFIG_CHECK_VECTOR_DIRTY
    j        4f
3:
    /* don't save, move sp only */
    addi     sp, sp, -(CTX_VECTOR_CSR_REG_NR) * REGBYTES
    csrr     t0, vlenb
    li       t1, CTX_VECTOR_REG_NR
    mul      t2, t0, t1
    sub      sp, sp, t2
4:
#endif /* CONFIG_CHECK_VECTOR_DIRTY */
#endif /* __riscv_vector && ENABLE_VECTOR */
.endm

#if CONFIG_CHECK_FPU_DIRTY || CONFIG_CHECK_VECTOR_DIRTY
.macro RESTORE_MSTATUS
    li       t1, 0

#if defined(__riscv_vector) && defined(ENABLE_VECTOR)
    addi     t1, t1, (CTX_VECTOR_CSR_REG_NR) * REGBYTES
    csrr     t0, vlenb
    li       t2, CTX_VECTOR_REG_NR
    mul      t2, t0, t2
    add      t1, t1, t2
#endif

#if defined(__riscv_flen) && defined(ENABLE_FPU)
    addi     t1, t1, (CTX_FPU_CSR_REG_NR + CTX_FPU_REG_NR) * REGBYTES
#endif

    /* general regs */
    addi     t1, t1, (CTX_GENERAL_REG_NR - 1) * REGBYTES

    /* restore mstatus */
    add      sp, sp, t1
    ld       t3, (0)(sp)
    csrw     mstatus, t3
    sub      sp, sp, t1
.endm
#endif

.macro RESTORE_ALL
#if CONFIG_CHECK_FPU_DIRTY || CONFIG_CHECK_VECTOR_DIRTY
    RESTORE_MSTATUS
#endif /* CONFIG_CHECK_FPU_DIRTY || CONFIG_CHECK_VECTOR_DIRTY */
#if defined(__riscv_vector) && defined(ENABLE_VECTOR)
#if CONFIG_CHECK_VECTOR_DIRTY
    /* restore mstatus first */
    /* check if VS filed of MSTATUS is 'dirty' */
    li       t1, SR_VS_DIRTY
    and      t4, t3, t1
    bne      t4, t1, 1f
#endif
    /* restore vector registers */
    csrr     t0, vlenb
    slli     t0, t0, 3
#if (__riscv_v == 7000)
    vsetvli  zero, zero, e8, m8
    vlb.v    v0, (sp)
    add      sp, sp, t0
    vlb.v    v8, (sp)
    add      sp, sp, t0
    vlb.v    v16, (sp)
    add      sp, sp, t0
    vlb.v    v24, (sp)
    add      sp, sp, t0
#elif (__riscv_v == 1000000)
    vsetvli  zero, zero, e8, m8, ta, ma
    vl8r.v   v0, (sp)
    add      sp, sp, t0
    vl8r.v   v8, (sp)
    add      sp, sp, t0
    vl8r.v   v16, (sp)
    add      sp, sp, t0
    vl8r.v   v24, (sp)
    add      sp, sp, t0
#endif
    /* restore vector csr registers */
    ld       t0, (0)(sp)
    ld       t1, (8)(sp)
    ld       t2, (16)(sp)
    vsetvl   zero, t0, t1
    csrw     vstart, t2
    ld       t2, (24)(sp)
    csrw     vxsat, t2
    ld       t2, (32)(sp)
    csrw     vxrm, t2
    addi     sp, sp, (CTX_VECTOR_CSR_REG_NR) * REGBYTES
#if CONFIG_CHECK_VECTOR_DIRTY
    j        2f
1:
    csrr     t0, vlenb
    li       t1, CTX_VECTOR_REG_NR
    mul      t2, t0, t1
    add      sp, sp, t2
    addi     sp, sp, (CTX_VECTOR_CSR_REG_NR) * REGBYTES
2:
#endif /* CONFIG_CHECK_VECTOR_DIRTY */
#endif /* __riscv_vector && ENABLE_VECTOR */

#if defined(__riscv_flen) && defined(ENABLE_FPU)
#if CONFIG_CHECK_FPU_DIRTY
    /* check if FS filed of MSTATUS is 'dirty' */
    li       t1, SR_FS_DIRTY
    and      t4, t3, t1
    bne      t4, t1, 3f
#endif
    /* restore float register */
    fld f0, FPU_CTX_F0_OFF(sp)
    fld f1, FPU_CTX_F1_OFF(sp)
    fld f2, FPU_CTX_F2_OFF(sp)
    fld f3, FPU_CTX_F3_OFF(sp)
    fld f4, FPU_CTX_F4_OFF(sp)
    fld f5, FPU_CTX_F5_OFF(sp)
    fld f6, FPU_CTX_F6_OFF(sp)
    fld f7, FPU_CTX_F7_OFF(sp)
    fld f8, FPU_CTX_F8_OFF(sp)
    fld f9, FPU_CTX_F9_OFF(sp)
    fld f10,FPU_CTX_F10_OFF(sp)
    fld f11,FPU_CTX_F11_OFF(sp)
    fld f12,FPU_CTX_F12_OFF(sp)
    fld f13,FPU_CTX_F13_OFF(sp)
    fld f14,FPU_CTX_F14_OFF(sp)
    fld f15,FPU_CTX_F15_OFF(sp)
    fld f16,FPU_CTX_F16_OFF(sp)
    fld f17,FPU_CTX_F17_OFF(sp)
    fld f18,FPU_CTX_F18_OFF(sp)
    fld f19,FPU_CTX_F19_OFF(sp)
    fld f20,FPU_CTX_F20_OFF(sp)
    fld f21,FPU_CTX_F21_OFF(sp)
    fld f22,FPU_CTX_F22_OFF(sp)
    fld f23,FPU_CTX_F23_OFF(sp)
    fld f24,FPU_CTX_F24_OFF(sp)
    fld f25,FPU_CTX_F25_OFF(sp)
    fld f26,FPU_CTX_F26_OFF(sp)
    fld f27,FPU_CTX_F27_OFF(sp)
    fld f28,FPU_CTX_F28_OFF(sp)
    fld f29,FPU_CTX_F29_OFF(sp)
    fld f30,FPU_CTX_F30_OFF(sp)
    fld f31,FPU_CTX_F31_OFF(sp)
    addi     sp, sp, CTX_FPU_REG_NR * REGBYTES
    /* restore fcsr registers */
    ld       t0, 0(sp)
    fscsr    t0
    addi     sp, sp, CTX_FPU_CSR_REG_NR * REGBYTES
#if CONFIG_CHECK_FPU_DIRTY
    j        4f
3:
    addi     sp, sp, CTX_FPU_REG_NR * REGBYTES
    addi     sp, sp, CTX_FPU_CSR_REG_NR * REGBYTES
4:
#endif /* CONFIG_CHECK_FPU_DIRTY */
#endif /* __riscv_flen && ENABLE_FPU */

    /* restore general registers */
    LOAD x1,  31 * REGBYTES(sp)
    csrw mepc, x1
#if (!CONFIG_CHECK_FPU_DIRTY) && (!CONFIG_CHECK_VECTOR_DIRTY)
    LOAD x1,  32 * REGBYTES(sp)
    csrw mstatus, x1
#endif
    LOAD x1,   0 * REGBYTES(sp)
    LOAD x3,   2 * REGBYTES(sp)
    LOAD x4,   3 * REGBYTES(sp)
    LOAD x5,   4 * REGBYTES(sp)
    LOAD x6,   5 * REGBYTES(sp)
    LOAD x7,   6 * REGBYTES(sp)
    LOAD x8,   7 * REGBYTES(sp)
    LOAD x9,   8 * REGBYTES(sp)
    LOAD x10,  9 * REGBYTES(sp)
    LOAD x11, 10 * REGBYTES(sp)
    LOAD x12, 11 * REGBYTES(sp)
    LOAD x13, 12 * REGBYTES(sp)
    LOAD x14, 13 * REGBYTES(sp)
    LOAD x15, 14 * REGBYTES(sp)
    LOAD x16, 15 * REGBYTES(sp)
    LOAD x17, 16 * REGBYTES(sp)
    LOAD x18, 17 * REGBYTES(sp)
    LOAD x19, 18 * REGBYTES(sp)
    LOAD x20, 19 * REGBYTES(sp)
    LOAD x21, 20 * REGBYTES(sp)
    LOAD x22, 21 * REGBYTES(sp)
    LOAD x23, 22 * REGBYTES(sp)
    LOAD x24, 23 * REGBYTES(sp)
    LOAD x25, 24 * REGBYTES(sp)
    LOAD x26, 25 * REGBYTES(sp)
    LOAD x27, 26 * REGBYTES(sp)
    LOAD x28, 27 * REGBYTES(sp)
    LOAD x29, 28 * REGBYTES(sp)
    LOAD x30, 29 * REGBYTES(sp)
    LOAD x31, 30 * REGBYTES(sp)
    addi sp, sp, CTX_GENERAL_REG_NR * REGBYTES
.endm

.macro RESTORE_SYS_GP
    .option push
    .option norelax
        la gp, __global_pointer$
    .option pop
.endm

.macro OPEN_INTERRUPT
    csrsi mstatus, 8
.endm

.macro CLOSE_INTERRUPT
    csrci mstatus, 8
.endm

#endif
