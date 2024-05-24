/*
 * Copyright (C) 2014-2023 Alibaba Group Holding Limited
 */

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
    addi    sp, sp, -60
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
    csrr    t0, mepc
    sw      t0, 56(sp)

    la      a1, pxCurrentTCB
    lw      a1, (a1)
    sw      sp, (a1)

    jal     vTaskSwitchContext

__vPortFistTask:
    la      a1, pxCurrentTCB
    lw      a1, (a1)
    lw      sp, (a1)

    /* Run in machine mode */
    li      t0, MSTATUS_PRV1
    csrs    mstatus, t0

    lw      t0, 56(sp)
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
    addi    sp, sp, 60

    mret
