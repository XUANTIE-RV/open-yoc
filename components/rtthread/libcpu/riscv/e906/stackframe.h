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
#define FPU_CTX_F1_OFF   4   /* offsetof(fpu_context_t, fpustatus.f[1])  - offsetof(fpu_context_t, fpustatus.f[0]) */
#define FPU_CTX_F2_OFF   8   /* offsetof(fpu_context_t, fpustatus.f[2])  - offsetof(fpu_context_t, fpustatus.f[0]) */
#define FPU_CTX_F3_OFF   12  /* offsetof(fpu_context_t, fpustatus.f[3])  - offsetof(fpu_context_t, fpustatus.f[0]) */
#define FPU_CTX_F4_OFF   16  /* offsetof(fpu_context_t, fpustatus.f[4])  - offsetof(fpu_context_t, fpustatus.f[0]) */
#define FPU_CTX_F5_OFF   20  /* offsetof(fpu_context_t, fpustatus.f[5])  - offsetof(fpu_context_t, fpustatus.f[0]) */
#define FPU_CTX_F6_OFF   24  /* offsetof(fpu_context_t, fpustatus.f[6])  - offsetof(fpu_context_t, fpustatus.f[0]) */
#define FPU_CTX_F7_OFF   28  /* offsetof(fpu_context_t, fpustatus.f[7])  - offsetof(fpu_context_t, fpustatus.f[0]) */
#define FPU_CTX_F8_OFF   32  /* offsetof(fpu_context_t, fpustatus.f[8])  - offsetof(fpu_context_t, fpustatus.f[0]) */
#define FPU_CTX_F9_OFF   36  /* offsetof(fpu_context_t, fpustatus.f[9])  - offsetof(fpu_context_t, fpustatus.f[0]) */
#define FPU_CTX_F10_OFF  40  /* offsetof(fpu_context_t, fpustatus.f[10]) - offsetof(fpu_context_t, fpustatus.f[0]) */
#define FPU_CTX_F11_OFF  44  /* offsetof(fpu_context_t, fpustatus.f[11]) - offsetof(fpu_context_t, fpustatus.f[0]) */
#define FPU_CTX_F12_OFF  48  /* offsetof(fpu_context_t, fpustatus.f[12]) - offsetof(fpu_context_t, fpustatus.f[0]) */
#define FPU_CTX_F13_OFF  52  /* offsetof(fpu_context_t, fpustatus.f[13]) - offsetof(fpu_context_t, fpustatus.f[0]) */
#define FPU_CTX_F14_OFF  56  /* offsetof(fpu_context_t, fpustatus.f[14]) - offsetof(fpu_context_t, fpustatus.f[0]) */
#define FPU_CTX_F15_OFF  60  /* offsetof(fpu_context_t, fpustatus.f[15]) - offsetof(fpu_context_t, fpustatus.f[0]) */
#define FPU_CTX_F16_OFF  64  /* offsetof(fpu_context_t, fpustatus.f[16]) - offsetof(fpu_context_t, fpustatus.f[0]) */
#define FPU_CTX_F17_OFF  68  /* offsetof(fpu_context_t, fpustatus.f[17]) - offsetof(fpu_context_t, fpustatus.f[0]) */
#define FPU_CTX_F18_OFF  72  /* offsetof(fpu_context_t, fpustatus.f[18]) - offsetof(fpu_context_t, fpustatus.f[0]) */
#define FPU_CTX_F19_OFF  76  /* offsetof(fpu_context_t, fpustatus.f[19]) - offsetof(fpu_context_t, fpustatus.f[0]) */
#define FPU_CTX_F20_OFF  80  /* offsetof(fpu_context_t, fpustatus.f[20]) - offsetof(fpu_context_t, fpustatus.f[0]) */
#define FPU_CTX_F21_OFF  84  /* offsetof(fpu_context_t, fpustatus.f[21]) - offsetof(fpu_context_t, fpustatus.f[0]) */
#define FPU_CTX_F22_OFF  88  /* offsetof(fpu_context_t, fpustatus.f[22]) - offsetof(fpu_context_t, fpustatus.f[0]) */
#define FPU_CTX_F23_OFF  92  /* offsetof(fpu_context_t, fpustatus.f[23]) - offsetof(fpu_context_t, fpustatus.f[0]) */
#define FPU_CTX_F24_OFF  96  /* offsetof(fpu_context_t, fpustatus.f[24]) - offsetof(fpu_context_t, fpustatus.f[0]) */
#define FPU_CTX_F25_OFF  100 /* offsetof(fpu_context_t, fpustatus.f[25]) - offsetof(fpu_context_t, fpustatus.f[0]) */
#define FPU_CTX_F26_OFF  104 /* offsetof(fpu_context_t, fpustatus.f[26]) - offsetof(fpu_context_t, fpustatus.f[0]) */
#define FPU_CTX_F27_OFF  108 /* offsetof(fpu_context_t, fpustatus.f[27]) - offsetof(fpu_context_t, fpustatus.f[0]) */
#define FPU_CTX_F28_OFF  112 /* offsetof(fpu_context_t, fpustatus.f[28]) - offsetof(fpu_context_t, fpustatus.f[0]) */
#define FPU_CTX_F29_OFF  116 /* offsetof(fpu_context_t, fpustatus.f[29]) - offsetof(fpu_context_t, fpustatus.f[0]) */
#define FPU_CTX_F30_OFF  120 /* offsetof(fpu_context_t, fpustatus.f[30]) - offsetof(fpu_context_t, fpustatus.f[0]) */
#define FPU_CTX_F31_OFF  124 /* offsetof(fpu_context_t, fpustatus.f[31]) - offsetof(fpu_context_t, fpustatus.f[0]) */
#endif /* ENABLE_FPU */

.macro SAVE_ALL
#if defined(__riscv_flen) && defined(ENABLE_FPU)
    addi sp, sp, -(CTX_FPU_REG_NR + 1) * REGBYTES
#endif /* __riscv_flen && ENABLE_FPU */

    /* save general registers */
    addi sp, sp, -CTX_GENERAL_REG_NR * REGBYTES
    STORE x1,   0 * REGBYTES(sp)
    STORE x2,   1 * REGBYTES(sp)
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
    STORE x1,   31 * REGBYTES(sp)
#if CONFIG_CHECK_FPU_DIRTY
    csrr  t3, mstatus
    STORE t3,   32 * REGBYTES(sp)
#else
    csrr  x1, mstatus
    STORE x1,   32 * REGBYTES(sp)
#endif
#if defined(__riscv_flen) && defined(ENABLE_FPU)
#if CONFIG_CHECK_FPU_DIRTY
    /* skip if not dirty */
    li       t1, SR_FS_DIRTY
    and      t3, t3, t1
    bne      t3, t1, 1f
#endif  /*CONFIG_CHECK_FPU_DIRTY*/
    /* save float registers */
    mv t1, sp
    addi t1, t1, CTX_GENERAL_REG_NR * REGBYTES
    /* save fcsr registers */
    frcsr    t0
    sd       t0, (0)(t1)
    addi t1, t1, (1) * REGBYTES

    fsd f0,  FPU_CTX_F0_OFF(t1)
    fsd f1,  FPU_CTX_F1_OFF(t1)
    fsd f2,  FPU_CTX_F2_OFF(t1)
    fsd f3,  FPU_CTX_F3_OFF(t1)
    fsd f4,  FPU_CTX_F4_OFF(t1)
    fsd f5,  FPU_CTX_F5_OFF(t1)
    fsd f6,  FPU_CTX_F6_OFF(t1)
    fsd f7,  FPU_CTX_F7_OFF(t1)
    fsd f8,  FPU_CTX_F8_OFF(t1)
    fsd f9,  FPU_CTX_F9_OFF(t1)
    fsd f10, FPU_CTX_F10_OFF(t1)
    fsd f11, FPU_CTX_F11_OFF(t1)
    fsd f12, FPU_CTX_F12_OFF(t1)
    fsd f13, FPU_CTX_F13_OFF(t1)
    fsd f14, FPU_CTX_F14_OFF(t1)
    fsd f15, FPU_CTX_F15_OFF(t1)
    fsd f16, FPU_CTX_F16_OFF(t1)
    fsd f17, FPU_CTX_F17_OFF(t1)
    fsd f18, FPU_CTX_F18_OFF(t1)
    fsd f19, FPU_CTX_F19_OFF(t1)
    fsd f20, FPU_CTX_F20_OFF(t1)
    fsd f21, FPU_CTX_F21_OFF(t1)
    fsd f22, FPU_CTX_F22_OFF(t1)
    fsd f23, FPU_CTX_F23_OFF(t1)
    fsd f24, FPU_CTX_F24_OFF(t1)
    fsd f25, FPU_CTX_F25_OFF(t1)
    fsd f26, FPU_CTX_F26_OFF(t1)
    fsd f27, FPU_CTX_F27_OFF(t1)
    fsd f28, FPU_CTX_F28_OFF(t1)
    fsd f29, FPU_CTX_F29_OFF(t1)
    fsd f30, FPU_CTX_F30_OFF(t1)
    fsd f31, FPU_CTX_F31_OFF(t1)
#if CONFIG_CHECK_FPU_DIRTY
1:
#endif  /*CONFIG_CHECK_FPU_DIRTY*/
#endif /* __riscv_flen && ENABLE_FPU */
.endm

#if CONFIG_CHECK_FPU_DIRTY
.macro RESTORE_MSTATUS
    li       t1, 0
    /* general regs */
    li       t0, (CTX_GENERAL_REG_NR - 1) * REGBYTES
    add      t1, t1, t0
    /* restore mstatus */
    add      sp, sp, t1
    LOAD     t3, (0)(sp)
    csrw     mstatus, t3
    sub      sp, sp, t1
.endm
#endif

.macro RESTORE_ALL
#if CONFIG_CHECK_FPU_DIRTY
    RESTORE_MSTATUS
#endif
#if defined(__riscv_flen) && defined(ENABLE_FPU)
#if CONFIG_CHECK_FPU_DIRTY
    /* skip if not dirty */
    li       t1, SR_FS_DIRTY
    and      t3, t3, t1
    bne      t3, t1, 1f
#endif  /*CONFIG_CHECK_FPU_DIRTY*/
    /* restore float register  */
    addi t2, sp, (CTX_GENERAL_REG_NR + 1) * REGBYTES
    fld f0, FPU_CTX_F0_OFF(t2)
    fld f1, FPU_CTX_F1_OFF(t2)
    fld f2, FPU_CTX_F2_OFF(t2)
    fld f3, FPU_CTX_F3_OFF(t2)
    fld f4, FPU_CTX_F4_OFF(t2)
    fld f5, FPU_CTX_F5_OFF(t2)
    fld f6, FPU_CTX_F6_OFF(t2)
    fld f7, FPU_CTX_F7_OFF(t2)
    fld f8, FPU_CTX_F8_OFF(t2)
    fld f9, FPU_CTX_F9_OFF(t2)
    fld f10,FPU_CTX_F10_OFF(t2)
    fld f11,FPU_CTX_F11_OFF(t2)
    fld f12,FPU_CTX_F12_OFF(t2)
    fld f13,FPU_CTX_F13_OFF(t2)
    fld f14,FPU_CTX_F14_OFF(t2)
    fld f15,FPU_CTX_F15_OFF(t2)
    fld f16,FPU_CTX_F16_OFF(t2)
    fld f17,FPU_CTX_F17_OFF(t2)
    fld f18,FPU_CTX_F18_OFF(t2)
    fld f19,FPU_CTX_F19_OFF(t2)
    fld f20,FPU_CTX_F20_OFF(t2)
    fld f21,FPU_CTX_F21_OFF(t2)
    fld f22,FPU_CTX_F22_OFF(t2)
    fld f23,FPU_CTX_F23_OFF(t2)
    fld f24,FPU_CTX_F24_OFF(t2)
    fld f25,FPU_CTX_F25_OFF(t2)
    fld f26,FPU_CTX_F26_OFF(t2)
    fld f27,FPU_CTX_F27_OFF(t2)
    fld f28,FPU_CTX_F28_OFF(t2)
    fld f29,FPU_CTX_F29_OFF(t2)
    fld f30,FPU_CTX_F30_OFF(t2)
    fld f31,FPU_CTX_F31_OFF(t2)

    /* restore fcsr registers */
    addi     t2, sp, CTX_GENERAL_REG_NR * REGBYTES
    ld       t0, 0(t2)
    fscsr    t0
#if CONFIG_CHECK_FPU_DIRTY
1:
#endif  /*CONFIG_CHECK_FPU_DIRTY*/
#endif /* __riscv_flen && ENABLE_FPU */

    /* reserve general registers */
    LOAD x1,   31 * REGBYTES(sp)
    csrw mepc, x1
#if !CONFIG_CHECK_FPU_DIRTY
    LOAD x1,   32 * REGBYTES(sp)
    csrw mstatus, x1
#endif /* CONFIG_CHECK_FPU_DIRTY */
    LOAD x1,   0 * REGBYTES(sp)
    LOAD x2,   1 * REGBYTES(sp)
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

#if defined(__riscv_flen) && defined(ENABLE_FPU)
    addi sp, sp, (CTX_FPU_REG_NR + 1) * REGBYTES
#endif /* __riscv_flen && ENABLE_FPU */
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
