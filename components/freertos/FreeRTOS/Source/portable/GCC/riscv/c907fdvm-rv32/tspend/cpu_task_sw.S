 /*
 * Copyright (C) 2017-2024 Alibaba Group Holding Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <csi_config.h>
#include <riscv_csr.h>

#ifdef CONFIG_VIC_TSPDR
#define VIC_TSPDR CONFIG_VIC_TSPDR
#else
#ifdef CONFIG_RISCV_SMODE
#define VIC_TSPDR   0xE400C000
#else
#define VIC_TSPDR   0xE4000000
#endif /*CONFIG_RISCV_SMODE*/
#endif /*CONFIG_VIC_TSPDR*/

.global cpu_is_irq_enable
.global cpu_intrpt_save
.global cpu_intrpt_restore
.global vPortStartTask
.global Mtspend_Handler
.global Stspend_Handler
.extern pxCurrentTCB
.extern vTaskSwitchContext
.extern g_top_irqstack

.equ RISCV_MSTATUS_MIE,        (1<<3)       /*machine-level interrupt bit*/
.equ RISCV_SSTATUS_SIE,        (1<<1)       /*supervisor-level interrupt bit*/

#if defined(CONFIG_RISCV_SMODE) && CONFIG_RISCV_SMODE
.equ RISCV_VIC_TSPDR,          (VIC_TSPDR + 0xC000)       /*soft irq register*/
#else
.equ RISCV_VIC_TSPDR,          (VIC_TSPDR)                /*soft irq register*/
#endif

#if defined(CONFIG_RISCV_SMODE) && CONFIG_RISCV_SMODE
cpu_intrpt_save:
    csrrci a0, MODE_PREFIX(status), RISCV_SSTATUS_SIE
    ret

cpu_is_irq_enable:
    csrr a0, MODE_PREFIX(status)
    andi a0, a0, RISCV_SSTATUS_SIE
    ret
#else
cpu_intrpt_save:
    csrrci a0, MODE_PREFIX(status), RISCV_MSTATUS_MIE
    ret

cpu_is_irq_enable:
    csrr a0, MODE_PREFIX(status)
    andi a0, a0, RISCV_MSTATUS_MIE
    ret
#endif

cpu_intrpt_restore:
    csrw MODE_PREFIX(status), a0
    ret

#if CONFIG_CHECK_FPU_DIRTY || CONFIG_CHECK_VECTOR_DIRTY || CONFIG_CHECK_MATRIX_DIRTY
.macro FREERTOS_RESTORE_MSTATUS
    li       t1, 0
#if __riscv_matrix || __riscv_xtheadmatrix
#if __riscv_xlen == 64
    addi     t1, t1, 24
#else
    addi     t1, t1, 12
#endif /*__riscv_xlen */
    csrr     t0, xmlenb
    slli     t0, t0, 3
    add      t1, t1, t0
#endif /* __riscv_matrix || __riscv_xtheadmatrix */

#ifdef __riscv_vector
    csrr     t0, vlenb
    slli     t0, t0, 5
    add      t1, t1, t0
#if __riscv_xlen == 64
    addi     t1, t1, 40
#else
    addi     t1, t1, 20
#endif  /* __riscv_xlen */
#endif  /* __riscv_vector */

#if __riscv_flen
#if __riscv_xlen == 64
    addi     t1, t1, (256 + 8)
#else
    addi     t1, t1, (256 + 4)
#endif /* __riscv_xlen */
#endif /* __riscv_flen */

#if __riscv_xlen == 64
    addi     t1, t1, (256 - 8)
#else
    addi     t1, t1, (128 - 4)
#endif /* __riscv_xlen */

    add      sp, sp, t1
    load_x   t3, (0)(sp)
    csrw     mstatus, t3
    sub      sp, sp, t1
.endm
#endif /* CONFIG_CHECK_FPU_DIRTY || CONFIG_CHECK_VECTOR_DIRTY || CONFIG_CHECK_MATRIX_DIRTY */

/********************************************************************
 * Functions: vPortYield
 *
 ********************************************************************/
.global vPortYield
.type   vPortYield, %function
vPortYield:
    li       t0, RISCV_VIC_TSPDR
    li       t2, 0x1
    sw       t2, 0(t0)

__loop:
    lw       t2, 0(t0)
    seqz     t1, t2
#if defined(CONFIG_RISCV_SMODE) && CONFIG_RISCV_SMODE
    csrr     t2, sip
    andi     t2, t2, 0x2
#else
    csrr     t2, mip
    andi     t2, t2, 0x8
#endif
    snez     t3, t2
    add      t2, t3, t1
    beqz     t2, __loop
    ret


.align 8
vPortStartTask:
    j        __vPortFistTask

.align 8
Stspend_Handler:
Mtspend_Handler:
#if __riscv_xlen == 64
    addi     sp, sp, -(128+128)
    store_x  x1,  (0  +0  )(sp)
    store_x  x3,  (4  +4  )(sp)
    store_x  x4,  (8  +8  )(sp)
    store_x  x5,  (12 +12 )(sp)
    store_x  x6,  (16 +16 )(sp)
    store_x  x7,  (20 +20 )(sp)
    store_x  x8,  (24 +24 )(sp)
    store_x  x9,  (28 +28 )(sp)
    store_x  x10, (32 +32 )(sp)
    store_x  x11, (36 +36 )(sp)
    store_x  x12, (40 +40 )(sp)
    store_x  x13, (44 +44 )(sp)
    store_x  x14, (48 +48 )(sp)
    store_x  x15, (52 +52 )(sp)
    store_x  x16, (56 +56 )(sp)
    store_x  x17, (60 +60 )(sp)
    store_x  x18, (64 +64 )(sp)
    store_x  x19, (68 +68 )(sp)
    store_x  x20, (72 +72 )(sp)
    store_x  x21, (76 +76 )(sp)
    store_x  x22, (80 +80 )(sp)
    store_x  x23, (84 +84 )(sp)
    store_x  x24, (88 +88 )(sp)
    store_x  x25, (92 +92 )(sp)
    store_x  x26, (96 +96 )(sp)
    store_x  x27, (100+100)(sp)
    store_x  x28, (104+104)(sp)
    store_x  x29, (108+108)(sp)
    store_x  x30, (112+112)(sp)
    store_x  x31, (116+116)(sp)

    csrr     t0, MODE_PREFIX(epc)
    store_x  t0, (120+120)(sp)
#if CONFIG_CHECK_FPU_DIRTY || CONFIG_CHECK_VECTOR_DIRTY || CONFIG_CHECK_MATRIX_DIRTY
    csrr     t3, MODE_PREFIX(status)
    store_x  t3, (124+124)(sp)
#else
    csrr     t0, MODE_PREFIX(status)
    store_x  t0, (124+124)(sp)
#endif /* CONFIG_CHECK_FPU_DIRTY, CONFIG_CHECK_VECTOR_DIRTY, CONFIG_CHECK_MATRIX_DIRTY */
#else /* __riscv_xlen == 32 */
    addi     sp, sp, -128
    store_x  x1,  (0  )(sp)
    store_x  x3,  (4  )(sp)
    store_x  x4,  (8  )(sp)
    store_x  x5,  (12 )(sp)
    store_x  x6,  (16 )(sp)
    store_x  x7,  (20 )(sp)
    store_x  x8,  (24 )(sp)
    store_x  x9,  (28 )(sp)
    store_x  x10, (32 )(sp)
    store_x  x11, (36 )(sp)
    store_x  x12, (40 )(sp)
    store_x  x13, (44 )(sp)
    store_x  x14, (48 )(sp)
    store_x  x15, (52 )(sp)
    store_x  x16, (56 )(sp)
    store_x  x17, (60 )(sp)
    store_x  x18, (64 )(sp)
    store_x  x19, (68 )(sp)
    store_x  x20, (72 )(sp)
    store_x  x21, (76 )(sp)
    store_x  x22, (80 )(sp)
    store_x  x23, (84 )(sp)
    store_x  x24, (88 )(sp)
    store_x  x25, (92 )(sp)
    store_x  x26, (96 )(sp)
    store_x  x27, (100)(sp)
    store_x  x28, (104)(sp)
    store_x  x29, (108)(sp)
    store_x  x30, (112)(sp)
    store_x  x31, (116)(sp)

    csrr     t0, MODE_PREFIX(epc)
    store_x  t0, (120)(sp)
#if CONFIG_CHECK_FPU_DIRTY || CONFIG_CHECK_VECTOR_DIRTY || CONFIG_CHECK_MATRIX_DIRTY
    csrr     t3, MODE_PREFIX(status)
    store_x  t3, (124)(sp)
#else
    csrr     t0, MODE_PREFIX(status)
    store_x  t0, (124)(sp)
#endif /* CONFIG_CHECK_FPU_DIRTY, CONFIG_CHECK_VECTOR_DIRTY, CONFIG_CHECK_MATRIX_DIRTY */
#endif /* __riscv_xlen */

#ifdef __riscv_flen
#if __riscv_xlen == 64
#if CONFIG_CHECK_FPU_DIRTY
    li       t1, SR_FS_DIRTY
    and      t4, t3, t1
    bne      t4, t1, 1f
#endif /* CONFIG_CHECK_FPU_DIRTY */

    addi     sp, sp, -(4+4)
    frcsr    t0
    store_x  t0, (0  +0  )(sp)
#else
    addi     sp, sp, -(4)
    frcsr    t0
    store_x  t0, (0)(sp)
#endif /* __riscv_xlen == 64 */

    addi     sp, sp, -(128+128)
    fstore_x f31, (0  +0  )(sp)
    fstore_x f30, (4  +4  )(sp)
    fstore_x f29, (8  +8  )(sp)
    fstore_x f28, (12 +12 )(sp)
    fstore_x f27, (16 +16 )(sp)
    fstore_x f26, (20 +20 )(sp)
    fstore_x f25, (24 +24 )(sp)
    fstore_x f24, (28 +28 )(sp)
    fstore_x f23, (32 +32 )(sp)
    fstore_x f22, (36 +36 )(sp)
    fstore_x f21, (40 +40 )(sp)
    fstore_x f20, (44 +44 )(sp)
    fstore_x f19, (48 +48 )(sp)
    fstore_x f18, (52 +52 )(sp)
    fstore_x f17, (56 +56 )(sp)
    fstore_x f16, (60 +60 )(sp)
    fstore_x f15, (64 +64 )(sp)
    fstore_x f14, (68 +68 )(sp)
    fstore_x f13, (72 +72 )(sp)
    fstore_x f12, (76 +76 )(sp)
    fstore_x f11, (80 +80 )(sp)
    fstore_x f10, (84 +84 )(sp)
    fstore_x f9,  (88 +88 )(sp)
    fstore_x f8,  (92 +92 )(sp)
    fstore_x f7,  (96 +96 )(sp)
    fstore_x f6,  (100+100)(sp)
    fstore_x f5,  (104+104)(sp)
    fstore_x f4,  (108+108)(sp)
    fstore_x f3,  (112+112)(sp)
    fstore_x f2,  (116+116)(sp)
    fstore_x f1,  (120+120)(sp)
    fstore_x f0,  (124+124)(sp)
#if CONFIG_CHECK_FPU_DIRTY
    j        2f
1:
#if __riscv_xlen == 64
    addi     sp, sp, -264
#else
    addi     sp, sp, -260
#endif /* __riscv_xlen == 64 */
2:
#endif /* CONFIG_CHECK_FPU_DIRTY */
#endif /* __riscv_flen */

#ifdef __riscv_vector
#if CONFIG_CHECK_VECTOR_DIRTY
    li       t1, SR_VS_DIRTY
    and      t4, t3, t1
    bne      t4, t1, 3f
#endif
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
    addi     sp, sp, -(20)
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
#endif

    csrr     t0, vlenb
    slli     t0, t0, 3
    slli     t1, t0, 2
    sub      sp, sp, t1
#if (__riscv_v == 7000)
    vsetvli  zero, zero, e8, m8
    vsb.v    v0, (sp)
    add      sp, sp, t0
    vsb.v    v8, (sp)
    add      sp, sp, t0
    vsb.v    v16, (sp)
    add      sp, sp, t0
    vsb.v    v24, (sp)
#elif (__riscv_v == 1000000)
    vsetvli  zero, zero, e8, m8, ta, ma
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
#if CONFIG_CHECK_VECTOR_DIRTY
    j        4f
3:
#if __riscv_xlen == 64
    addi     sp, sp, -40
#else
    addi     sp, sp, -20
#endif /* __riscv_xlen == 64 */
    csrr     t0, vlenb
    slli     t0, t0, 5
    sub      sp, sp, t0
4:
#endif /* CONFIG_CHECK_VECTOR_DIRTY */
#endif /*__riscv_vector*/

#if __riscv_matrix || __riscv_xtheadmatrix
#if CONFIG_CHECK_MATRIX_DIRTY
    li       t1, SR_MS_DIRTY
    and      t4, t3, t1
    bne      t4, t1, 5f
#endif
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
#if CONFIG_CHECK_MATRIX_DIRTY
    j        6f
5:
#if __riscv_xlen == 64
    addi     sp, sp, -24
#else
    addi     sp, sp, -12
#endif
    csrr     t0, xmlenb
    slli     t1, t0, 3
    sub      sp, sp, t1
6:
#endif /* CONFIG_CHECK_MATRIX_DIRTY */
#endif /* __riscv_matrix || __riscv_xtheadmatrix */

    la       a1, pxCurrentTCB
    load_x   a1, (a1)
    store_x  sp, (a1)

    jal      vTaskSwitchContext

__vPortFistTask:
    la       a1, pxCurrentTCB
    load_x   a1, (a1)
    load_x   sp, (a1)

#if CONFIG_CHECK_FPU_DIRTY || CONFIG_CHECK_VECTOR_DIRTY || CONFIG_CHECK_MATRIX_DIRTY
    FREERTOS_RESTORE_MSTATUS
#endif

    /*clear software interrupt*/
    li       t0, (RISCV_VIC_TSPDR)
    li       t2, 0x0
    sw       t2, 0(t0)

#if __riscv_matrix || __riscv_xtheadmatrix
#if CONFIG_CHECK_MATRIX_DIRTY
    li       t1, SR_MS_DIRTY
    and      t4, t3, t1
    bne      t4, t1, 11f
#endif

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
#if CONFIG_CHECK_MATRIX_DIRTY
    j        12f
11:
    csrr     t0, xmlenb
    slli     t1, t0, 3
    add      sp, sp, t1
#if __riscv_xlen == 64
    addi     sp, sp, 24
#else
    addi     sp, sp, 12
#endif
12:
#endif /* CONFIG_CHECK_MATRIX_DIRTY */
#endif /* __riscv_matrix || __riscv_xtheadmatrix */

#ifdef __riscv_vector
#if CONFIG_CHECK_VECTOR_DIRTY
    li       t1, SR_VS_DIRTY
    and      t4, t3, t1
    bne      t4, t1, 13f
#endif

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
    addi     sp, sp, (20)
#endif /*__riscv_xlen*/
#if CONFIG_CHECK_VECTOR_DIRTY
    j        14f
13:
    /* don't restore, move sp only */
#if __riscv_xlen == 64
    addi     sp, sp, (20+20)
#else
    addi     sp, sp, (20)
#endif
    csrr     t0, vlenb
    slli     t0, t0, 5
    add      sp, sp, t0
14:
#endif /* CONFIG_CHECK_VECTOR_DIRTY */
#endif /*__riscv_vector*/

#ifdef __riscv_flen
#if CONFIG_CHECK_FPU_DIRTY
    li       t1, SR_FS_DIRTY
    and      t4, t3, t1
    bne      t4, t1, 15f
#endif
    fload_x  f31,( 0 + 0 )(sp)
    fload_x  f30,( 4 + 4 )(sp)
    fload_x  f29,( 8 + 8 )(sp)
    fload_x  f28,( 12+ 12)(sp)
    fload_x  f27,( 16+ 16)(sp)
    fload_x  f26,( 20+ 20)(sp)
    fload_x  f25,( 24+ 24)(sp)
    fload_x  f24,( 28+ 28)(sp)
    fload_x  f23,( 32+ 32)(sp)
    fload_x  f22,( 36+ 36)(sp)
    fload_x  f21,( 40+ 40)(sp)
    fload_x  f20,( 44+ 44)(sp)
    fload_x  f19,( 48+ 48)(sp)
    fload_x  f18,( 52+ 52)(sp)
    fload_x  f17,( 56+ 56)(sp)
    fload_x  f16,( 60+ 60)(sp)
    fload_x  f15,( 64+ 64)(sp)
    fload_x  f14,( 68+ 68)(sp)
    fload_x  f13,( 72+ 72)(sp)
    fload_x  f12,( 76+ 76)(sp)
    fload_x  f11,( 80+ 80)(sp)
    fload_x  f10,( 84+ 84)(sp)
    fload_x  f9, ( 88+ 88)(sp)
    fload_x  f8, ( 92+ 92)(sp)
    fload_x  f7, ( 96+ 96)(sp)
    fload_x  f6, (100+100)(sp)
    fload_x  f5, (104+104)(sp)
    fload_x  f4, (108+108)(sp)
    fload_x  f3, (112+112)(sp)
    fload_x  f2, (116+116)(sp)
    fload_x  f1, (120+120)(sp)
    fload_x  f0, (124+124)(sp)
    addi     sp, sp, (128+128)

#if __riscv_xlen == 64
    load_x   t0, (0 +0)(sp)
    fscsr    t0
    addi     sp, sp, (4+4)
#else
    load_x   t0, (0)(sp)
    fscsr    t0
    addi     sp, sp, (4)
#endif /*__riscv_xlen*/
#if CONFIG_CHECK_FPU_DIRTY
    j        16f
15:
#if __riscv_xlen == 64
    addi     sp, sp, 264
#else
    addi     sp, sp, 260
#endif
16:
#endif /* CONFIG_CHECK_FPU_DIRTY */
#endif /*__riscv_flen*/

#if __riscv_xlen == 64
#if (!CONFIG_CHECK_FPU_DIRTY) && (!CONFIG_CHECK_VECTOR_DIRTY) && (!CONFIG_CHECK_MATRIX_DIRTY)
    load_x   t0, (124 +124)(sp)
    csrw     MODE_PREFIX(status), t0
#endif

    load_x   t0, (120 +120)(sp)
    csrw     MODE_PREFIX(epc), t0

    load_x   x1,  (0  +0  )(sp)
    load_x   x3,  (4  +4  )(sp)
    load_x   x4,  (8  +8  )(sp)
    load_x   x5,  (12 +12 )(sp)
    load_x   x6,  (16 +16 )(sp)
    load_x   x7,  (20 +20 )(sp)
    load_x   x8,  (24 +24 )(sp)
    load_x   x9,  (28 +28 )(sp)
    load_x   x10, (32 +32 )(sp)
    load_x   x11, (36 +36 )(sp)
    load_x   x12, (40 +40 )(sp)
    load_x   x13, (44 +44 )(sp)
    load_x   x14, (48 +48 )(sp)
    load_x   x15, (52 +52 )(sp)
    load_x   x16, (56 +56 )(sp)
    load_x   x17, (60 +60 )(sp)
    load_x   x18, (64 +64 )(sp)
    load_x   x19, (68 +68 )(sp)
    load_x   x20, (72 +72 )(sp)
    load_x   x21, (76 +76 )(sp)
    load_x   x22, (80 +80 )(sp)
    load_x   x23, (84 +84 )(sp)
    load_x   x24, (88 +88 )(sp)
    load_x   x25, (92 +92 )(sp)
    load_x   x26, (96 +96 )(sp)
    load_x   x27, (100+100)(sp)
    load_x   x28, (104+104)(sp)
    load_x   x29, (108+108)(sp)
    load_x   x30, (112+112)(sp)
    load_x   x31, (116+116)(sp)
    addi     sp, sp, (128+128)
#else
#if (!CONFIG_CHECK_FPU_DIRTY) && (!CONFIG_CHECK_VECTOR_DIRTY) && (!CONFIG_CHECK_MATRIX_DIRTY)
    load_x   t0, (124)(sp)
    csrw     MODE_PREFIX(status), t0
#endif

    load_x   t0, (120)(sp)
    csrw     MODE_PREFIX(epc), t0

    load_x   x1,  (0  )(sp)
    load_x   x3,  (4  )(sp)
    load_x   x4,  (8  )(sp)
    load_x   x5,  (12 )(sp)
    load_x   x6,  (16 )(sp)
    load_x   x7,  (20 )(sp)
    load_x   x8,  (24 )(sp)
    load_x   x9,  (28 )(sp)
    load_x   x10, (32 )(sp)
    load_x   x11, (36 )(sp)
    load_x   x12, (40 )(sp)
    load_x   x13, (44 )(sp)
    load_x   x14, (48 )(sp)
    load_x   x15, (52 )(sp)
    load_x   x16, (56 )(sp)
    load_x   x17, (60 )(sp)
    load_x   x18, (64 )(sp)
    load_x   x19, (68 )(sp)
    load_x   x20, (72 )(sp)
    load_x   x21, (76 )(sp)
    load_x   x22, (80 )(sp)
    load_x   x23, (84 )(sp)
    load_x   x24, (88 )(sp)
    load_x   x25, (92 )(sp)
    load_x   x26, (96 )(sp)
    load_x   x27, (100)(sp)
    load_x   x28, (104)(sp)
    load_x   x29, (108)(sp)
    load_x   x30, (112)(sp)
    load_x   x31, (116)(sp)
    addi     sp, sp, (128)
#endif /*__riscv_xlen*/

    MODE_PREFIX(ret)

   .size   Mtspend_Handler, . - Mtspend_Handler
   .size   Stspend_Handler, . - Stspend_Handler
/*-----------------------------------------------------------*/

/*
 * Unlike other ports pxPortInitialiseStack() is written in assembly code as it
 * needs access to the portasmADDITIONAL_CONTEXT_SIZE constant.  The prototype
 * for the function is as per the other ports:
 * StackType_t *pxPortInitialiseStack( StackType_t *pxTopOfStack, TaskFunction_t pxCode, void *pvParameters );
 *
 * As per the standard RISC-V ABI pxTopcOfStack is passed in in a0, pxCode in
 * a1, and pvParameters in a2.  The new top of stack is passed out in a0.
 *
 * RISC-V maps registers to ABI names as follows (X1 to X31 integer registers
 * for the 'I' profile, X1 to X15 for the 'E' profile, currently I assumed).
 *
 * Register		ABI Name	Description						Saver
 * x0			zero		Hard-wired zero					-
 * x1			ra			Return address					Caller
 * x2			sp			Stack pointer					Callee
 * x3			gp			Global pointer					-
 * x4			tp			Thread pointer					-
 * x5-7			t0-2		Temporaries						Caller
 * x8			s0/fp		Saved register/Frame pointer	Callee
 * x9			s1			Saved register					Callee
 * x10-11		a0-1		Function Arguments/return values Caller
 * x12-17		a2-7		Function arguments				Caller
 * x18-27		s2-11		Saved registers					Callee
 * x28-31		t3-6		Temporaries						Caller
 *
 * The RISC-V context is saved t FreeRTOS tasks in the following stack frame,
 * where the global and thread pointers are currently assumed to be constant so
 * are not saved:
 *
 * mstatus
 * x31
 * x30
 * x29
 * x28
 * x27
 * x26
 * x25
 * x24
 * x23
 * x22
 * x21
 * x20
 * x19
 * x18
 * x17
 * x16
 * x15
 * x14
 * x13
 * x12
 * x11
 * pvParameters
 * x9
 * x8
 * x7
 * x6
 * x5
 * portTASK_RETURN_ADDRESS
 * [chip specific registers go here]
 * pxCode
 */

/*-----------------------------------------------------------*/
