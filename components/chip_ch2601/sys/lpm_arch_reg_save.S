/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

 /****************************************************************************
 FILE_NAME           : arch_reg_save.S
 FUNCTION            : save the cpu context:
 ******************************************************************************/
#include<csi_config.h>

.global arch_do_cpu_save
.global arch_do_cpu_resume
.global arch_resume_context

#undef  CSKY_LPM_BASE
#define CSKY_LPM_BASE   0x40000000

arch_resume_context:
    la      t0, CSKY_LPM_BASE
    lw      t1, 0(t0)
    li      t2, 0x10
    and     t1, t1, t2
    beq     t1, t2, arch_sp_resume_from_standby

arch_sp_resume_from_stop:
    la      t0, g_arch_cpu_saved
    lw      t1, 0x38(t0)
    jal     arch_do_cpu_resume

arch_sp_resume_from_standby:
    jal     Reset_Handler

arch_do_cpu_save:
    la      x10, g_arch_cpu_saved
    sw      x1, 0(x10)
    sw      x2, 4(x10)
    sw      x3, 8(x10)
    sw      x4, 12(x10)
    sw      x4, 16(x10)
    sw      x6, 20(x10)
    sw      x7, 24(x10)
    sw      x8, 28(x10)
    sw      x9, 32(x10)
    sw      x11, 36(x10)
    sw      x12, 40(x10)
    sw      x13, 44(x10)
    sw      x14, 48(x10)
    sw      x15, 52(x10)
    sw      x16, 56(x10)
    sw      x17, 60(x10)
    sw      x18, 64(x10)
    sw      x19, 68(x10)
    sw      x20, 72(x10)
    sw      x21, 76(x10)
    sw      x22, 80(x10)
    sw      x23, 84(x10)
    sw      x24, 88(x10)
    sw      x25, 92(x10)
    sw      x26, 96(x10)
    sw      x27, 100(x10)
    sw      x28, 104(x10)
    sw      x29, 108(x10)
    sw      x30, 112(x10)
    sw      x31, 116(x10)

    csrr    t0, mepc
    sw      t0, 120(x10)
    csrr    t0, mstatus
    sw      t0, 124(x10)
    csrr    t0, mcause
    sw      t0, 128(x10)
    csrr    t0, mie
    sw      t0, 132(x10)
    csrr    t0, mtvec
    sw      t0, 136(x10)
    csrr    t0, mtvt
    sw      t0, 140(x10)
    csrr    t0, 0x307
    sw      t0, 144(x10)
    csrr    t0, mxstatus
    sw      t0, 148(x10)
    csrr    t0, mhcr
    sw      t0, 152(x10)
    dcache.ciall
    wfi

arch_do_cpu_resume:
    la      x10, g_arch_cpu_saved

    lw      t0, 120(x10)
    csrw    mepc, t0

    lw      t0, 124(x10)
    csrw    mstatus, t0

    lw      t0, 128(x10)
    csrw    mcause, t0

    lw      t0, 132(x10)
    csrw    mie, t0

    lw      t0, 136(x10)
    csrw    mtvec, t0

    lw      t0, 140(x10)
    csrw    mtvt, t0

    lw      t0, 144(x10)
    csrw    0x307, t0

    lw      t0, 148(x10)
    csrw    mxstatus, t0

    lw      t0, 152(x10)
    csrw    mhcr, t0

    lw      x1, 0(x10)
    lw      x2, 4(x10)
    lw      x3, 8(x10)
    lw      x4, 12(x10)
    lw      x5, 16(x10)
    lw      x6, 20(x10)
    lw      x7, 24(x10)
    lw      x8, 28(x10)
    lw      x9, 32(x10)
    lw      x11, 36(x10)
    lw      x12, 40(x10)
    lw      x13, 44(x10)
    lw      x14, 48(x10)
    lw      x15, 52(x10)
    lw      x16, 56(x10)
    lw      x17, 60(x10)
    lw      x18, 64(x10)
    lw      x19, 68(x10)
    lw      x20, 72(x10)
    lw      x21, 76(x10)
    lw      x22, 80(x10)
    lw      x23, 84(x10)
    lw      x24, 88(x10)
    lw      x25, 92(x10)
    lw      x26, 96(x10)
    lw      x27, 100(x10)
    lw      x28, 104(x10)
    lw      x29, 108(x10)
    lw      x30, 112(x10)
    lw      x31, 116(x10)
    ret
