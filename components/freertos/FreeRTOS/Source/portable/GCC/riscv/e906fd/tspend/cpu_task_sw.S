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

#include "riscv_csr.h"

/* Enable interrupts when returning from the handler */
#define MSTATUS_PRV1 0x1880
.equ RISCV_MSTATUS_MIE,        (1<<3)       /*machine-level interrupt bit*/
/******************************************************************************
 * Functions:
 *     size_t cpu_intrpt_save(void);
 *     void   cpu_intrpt_restore(size_t psr);
 ******************************************************************************/

.global cpu_intrpt_save
.type cpu_intrpt_save, %function
cpu_intrpt_save:
    csrr    a0, mstatus
    csrc    mstatus, 8
    ret

.global cpu_intrpt_restore
.type cpu_intrpt_restore, %function
cpu_intrpt_restore:
    csrw    mstatus, a0
    ret

.global cpu_is_irq_enable
.type cpu_is_irq_enable, %function
cpu_is_irq_enable:
    csrr a0, mstatus
    andi a0, a0, RISCV_MSTATUS_MIE
    ret

#if CONFIG_CHECK_FPU_DIRTY
.macro FREERTOS_RESTORE_MSTATUS
    li       t1, 0

#ifdef __riscv_dsp
    addi     t1, t1, 4
#endif

#if __riscv_flen == 64
    addi     t1, t1, 260
#elif __riscv_flen == 32
    addi     t1, t1, 132
#endif
    addi     t1, t1, (128-4)

    add      sp, sp, t1
    lw       t3, (0)(sp)
    csrw     mstatus, t3
    sub      sp, sp, t1
.endm
#endif /* CONFIG_CHECK_FPU_DIRTY */

/********************************************************************
 * Functions: vPortYield
 *
 ********************************************************************/
.global vPortYield
.type   vPortYield, %function
vPortYield:
    li      t0, 0xE080100C
    lb      t1, (t0)
    li      t2, 0x01
    or      t1, t1, t2
    sb      t1, (t0)

    /* FIXME: trriger soft-irq quickly */
    fence
    nop
    nop
    nop
    nop
    nop

    ret

/********************************************************************
 * Functions: vPortStartTask
 *
 ********************************************************************/
.global vPortStartTask
.type   vPortStartTask, %function
vPortStartTask:
    j       __vPortFistTask

/********************************************************************
 * Functions: tspend_handler
 *
 ********************************************************************/
.global tspend_handler
.type   tspend_handler, %function
tspend_handler:
#if CONFIG_CHECK_FPU_DIRTY
    addi    sp, sp, -128
#else
    addi    sp, sp, -124
#endif
    sw      x1, 0(sp)
    sw      x3, 4(sp)
    sw      x4, 8(sp)
    sw      x5, 12(sp)
    sw      x6, 16(sp)
    sw      x7, 20(sp)
    sw      x8, 24(sp)
    sw      x9, 28(sp)
    sw      x10, 32(sp)
    sw      x11, 36(sp)
    sw      x12, 40(sp)
    sw      x13, 44(sp)
    sw      x14, 48(sp)
    sw      x15, 52(sp)
    sw      x16, 56(sp)
    sw      x17, 60(sp)
    sw      x18, 64(sp)
    sw      x19, 68(sp)
    sw      x20, 72(sp)
    sw      x21, 76(sp)
    sw      x22, 80(sp)
    sw      x23, 84(sp)
    sw      x24, 88(sp)
    sw      x25, 92(sp)
    sw      x26, 96(sp)
    sw      x27, 100(sp)
    sw      x28, 104(sp)
    sw      x29, 108(sp)
    sw      x30, 112(sp)
    sw      x31, 116(sp)
    csrr    t0, mepc
    sw      t0, 120(sp)
#if CONFIG_CHECK_FPU_DIRTY
    csrr    t3, mstatus
    sw      t3, 124(sp)
#endif

#if __riscv_flen == 64
#if CONFIG_CHECK_FPU_DIRTY
    li      t1, SR_FS_DIRTY
    and     t3, t3, t1
    bne     t3, t1, 1f
#endif
    addi     sp, sp, -4
    frcsr    t0
    sw       t0, 0(sp)

    addi    sp, sp, -256
    fsd     f31, (0  +0  )(sp)
    fsd     f30, (4  +4  )(sp)
    fsd     f29, (8  +8  )(sp)
    fsd     f28, (12 +12 )(sp)
    fsd     f27, (16 +16 )(sp)
    fsd     f26, (20 +20 )(sp)
    fsd     f25, (24 +24 )(sp)
    fsd     f24, (28 +28 )(sp)
    fsd     f23, (32 +32 )(sp)
    fsd     f22, (36 +36 )(sp)
    fsd     f21, (40 +40 )(sp)
    fsd     f20, (44 +44 )(sp)
    fsd     f19, (48 +48 )(sp)
    fsd     f18, (52 +52 )(sp)
    fsd     f17, (56 +56 )(sp)
    fsd     f16, (60 +60 )(sp)
    fsd     f15, (64 +64 )(sp)
    fsd     f14, (68 +68 )(sp)
    fsd     f13, (72 +72 )(sp)
    fsd     f12, (76 +76 )(sp)
    fsd     f11, (80 +80 )(sp)
    fsd     f10, (84 +84 )(sp)
    fsd     f9,  (88 +88 )(sp)
    fsd     f8,  (92 +92 )(sp)
    fsd     f7,  (96 +96 )(sp)
    fsd     f6,  (100+100)(sp)
    fsd     f5,  (104+104)(sp)
    fsd     f4,  (108+108)(sp)
    fsd     f3,  (112+112)(sp)
    fsd     f2,  (116+116)(sp)
    fsd     f1,  (120+120)(sp)
    fsd     f0,  (124+124)(sp)
#if CONFIG_CHECK_FPU_DIRTY
    j       2f
1:
    addi    sp, sp, -260
2:
#endif
#elif __riscv_flen == 32
#if CONFIG_CHECK_FPU_DIRTY
    li      t1, SR_FS_DIRTY
    and     t3, t3, t1
    bne     t3, t1, 1f
#endif
    addi     sp, sp, -4
    frcsr    t0
    sw       t0, 0(sp)

    addi    sp, sp, -128
    fsw     f31, 0(sp)
    fsw     f30, 4(sp)
    fsw     f29, 8(sp)
    fsw     f28, 12(sp)
    fsw     f27, 16(sp)
    fsw     f26, 20(sp)
    fsw     f25, 24(sp)
    fsw     f24, 28(sp)
    fsw     f23, 32(sp)
    fsw     f22, 36(sp)
    fsw     f21, 40(sp)
    fsw     f20, 44(sp)
    fsw     f19, 48(sp)
    fsw     f18, 52(sp)
    fsw     f17, 56(sp)
    fsw     f16, 60(sp)
    fsw     f15, 64(sp)
    fsw     f14, 68(sp)
    fsw     f13, 72(sp)
    fsw     f12, 76(sp)
    fsw     f11, 80(sp)
    fsw     f10, 84(sp)
    fsw     f9, 88(sp)
    fsw     f8, 92(sp)
    fsw     f7, 96(sp)
    fsw     f6, 100(sp)
    fsw     f5, 104(sp)
    fsw     f4, 108(sp)
    fsw     f3, 112(sp)
    fsw     f2, 116(sp)
    fsw     f1, 120(sp)
    fsw     f0, 124(sp)
#if CONFIG_CHECK_FPU_DIRTY
    j       2f
1:
    addi    sp, sp, -132
2:
#endif
#endif /* __riscv_flen */

#if __riscv_dsp
    addi    sp, sp, -4
    csrr    t0, vxsat
    sw      t0, 0(sp)
#endif /*__riscv_dsp */

    la      a1, pxCurrentTCB
    lw      a1, (a1)
    sw      sp, (a1)

    jal     vTaskSwitchContext

__vPortFistTask:
    la      a1, pxCurrentTCB
    lw      a1, (a1)
    lw      sp, (a1)

#if CONFIG_CHECK_FPU_DIRTY
    FREERTOS_RESTORE_MSTATUS
#endif

#if __riscv_dsp
    lw      t0, 0(sp)
    csrw    vxsat, t0
    addi    sp, sp, 4
#endif /*__riscv_dsp */

#if __riscv_flen == 64
#if CONFIG_CHECK_FPU_DIRTY
    li      t1, SR_FS_DIRTY
    and     t3, t3, t1
    bne     t3, t1, 11f
#endif
    fld     f31, (0  +0  )(sp)
    fld     f30, (4  +4  )(sp)
    fld     f29, (8  +8  )(sp)
    fld     f28, (12 +12 )(sp)
    fld     f27, (16 +16 )(sp)
    fld     f26, (20 +20 )(sp)
    fld     f25, (24 +24 )(sp)
    fld     f24, (28 +28 )(sp)
    fld     f23, (32 +32 )(sp)
    fld     f22, (36 +36 )(sp)
    fld     f21, (40 +40 )(sp)
    fld     f20, (44 +44 )(sp)
    fld     f19, (48 +48 )(sp)
    fld     f18, (52 +52 )(sp)
    fld     f17, (56 +56 )(sp)
    fld     f16, (60 +60 )(sp)
    fld     f15, (64 +64 )(sp)
    fld     f14, (68 +68 )(sp)
    fld     f13, (72 +72 )(sp)
    fld     f12, (76 +76 )(sp)
    fld     f11, (80 +80 )(sp)
    fld     f10, (84 +84 )(sp)
    fld     f9,  (88 +88 )(sp)
    fld     f8,  (92 +92 )(sp)
    fld     f7,  (96 +96 )(sp)
    fld     f6,  (100+100)(sp)
    fld     f5,  (104+104)(sp)
    fld     f4,  (108+108)(sp)
    fld     f3,  (112+112)(sp)
    fld     f2,  (116+116)(sp)
    fld     f1,  (120+120)(sp)
    fld     f0,  (124+124)(sp)
    addi    sp, sp, 256

    lw       t0, 0(sp)
    fscsr    t0
    addi     sp, sp, 4
#if CONFIG_CHECK_FPU_DIRTY
    j        12f
11:
    addi     sp, sp, 260
12:
#endif /* CONFIG_CHECK_FPU_DIRTY */
#elif __riscv_flen == 32
#if CONFIG_CHECK_FPU_DIRTY
    li      t1, SR_FS_DIRTY
    and     t3, t3, t1
    bne     t3, t1, 11f
#endif
    flw     f31, 0(sp)
    flw     f30, 4(sp)
    flw     f29, 8(sp)
    flw     f28, 12(sp)
    flw     f27, 16(sp)
    flw     f26, 20(sp)
    flw     f25, 24(sp)
    flw     f24, 28(sp)
    flw     f23, 32(sp)
    flw     f22, 36(sp)
    flw     f21, 40(sp)
    flw     f20, 44(sp)
    flw     f19, 48(sp)
    flw     f18, 52(sp)
    flw     f17, 56(sp)
    flw     f16, 60(sp)
    flw     f15, 64(sp)
    flw     f14, 68(sp)
    flw     f13, 72(sp)
    flw     f12, 76(sp)
    flw     f11, 80(sp)
    flw     f10, 84(sp)
    flw     f9, 88(sp)
    flw     f8, 92(sp)
    flw     f7, 96(sp)
    flw     f6, 100(sp)
    flw     f5, 104(sp)
    flw     f4, 108(sp)
    flw     f3, 112(sp)
    flw     f2, 116(sp)
    flw     f1, 120(sp)
    flw     f0, 124(sp)
    addi    sp, sp, 128

    lw       t0, 0(sp)
    fscsr    t0
    addi     sp, sp, 4
#if CONFIG_CHECK_FPU_DIRTY
    j        12f
11:
    addi     sp, sp, 132
12:
#endif /* CONFIG_CHECK_FPU_DIRTY */
#endif /* __riscv_flen == 32 */

    /* Run in machine mode */
    li      t0, MSTATUS_PRV1
    csrs    mstatus, t0

    lw      t0, 120(sp)
    csrw    mepc, t0

    lw      x1, 0(sp)
    lw      x3, 4(sp)
    lw      x4, 8(sp)
    lw      x5, 12(sp)
    lw      x6, 16(sp)
    lw      x7, 20(sp)
    lw      x8, 24(sp)
    lw      x9, 28(sp)
    lw      x10, 32(sp)
    lw      x11, 36(sp)
    lw      x12, 40(sp)
    lw      x13, 44(sp)
    lw      x14, 48(sp)
    lw      x15, 52(sp)
    lw      x16, 56(sp)
    lw      x17, 60(sp)
    lw      x18, 64(sp)
    lw      x19, 68(sp)
    lw      x20, 72(sp)
    lw      x21, 76(sp)
    lw      x22, 80(sp)
    lw      x23, 84(sp)
    lw      x24, 88(sp)
    lw      x25, 92(sp)
    lw      x26, 96(sp)
    lw      x27, 100(sp)
    lw      x28, 104(sp)
    lw      x29, 108(sp)
    lw      x30, 112(sp)
    lw      x31, 116(sp)
#if CONFIG_CHECK_FPU_DIRTY
    addi    sp, sp, 128
#else
    addi    sp, sp, 124
#endif
    mret

    .size   tspend_handler, . - tspend_handler
