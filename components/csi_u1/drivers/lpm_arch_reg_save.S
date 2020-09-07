/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */
 /****************************************************************************
 FILE_NAME           : arch_reg_save.S
 FUNCTION            : save the cpu context:
 ******************************************************************************/

.global arch_do_cpu_save
.global arch_do_cpu_resume
.global arch_resume_context

#define EFLASH_CONTROL_BASE 0x4003f000
#define CSKY_LPM_BASE   0x40002000

#ifndef CONFIG_TEE_CA
arch_resume_context:
    mfcr    r0, cr<8, 3>
    bseti   r0, 1
    bseti   r0, 0
    mtcr    r0, cr<8, 3>
    lrw     r0, EFLASH_CONTROL_BASE
    movi    r1, 0x35
    stw     r1, (r0, 0x24)
    movi    r1, 0x16
    stw     r1, (r0, 0x28)
    movi    r1, 0x35
    stw     r1, (r0, 0x2c)
    movi    r1, 0x1b9
    stw     r1, (r0, 0x30)
    movi    r1, 0x8b10
    stw     r1, (r0, 0x34)

    lrw     r0, CSKY_LPM_BASE
    ldw     r1, (r0, 0x0)
    btsti   r1, 3
    bt      arch_sp_resume_from_stop
    bf      arch_sp_resume_from_standby

arch_sp_resume_from_stop:
    lrw     r0, g_arch_cpu_saved
    ldw     sp, (r0, 0x38)
    jbr     resume_context_from_stop_mode

arch_sp_resume_from_standby:
    jbr     Reset_Handler
#endif

arch_do_cpu_save:
    subi    r14, 4
    stw     r0, (r14, 0x0)
    lrw     r0, g_arch_cpu_saved
    addi    r0, 4
    stm     r1-r15, (r0)
    subi    r0, 4
    mfcr    r2, psr
    stw     r2, (r0, 0x40)
    mfcr    r2, vbr
    stw     r2, (r0, 0x44)
    mfcr    r2, epsr
    stw     r2, (r0, 0x48)
    mfcr    r2, epc
    stw     r2, (r0, 0x4c)
    /* save NT_SSP */
    mfcr    r2, cr<6, 3>
    stw     r2, (r0, 0x50)
    /* save NT_PSR */
    mfcr    r2, cr<0, 3>
    stw     r2, (r0, 0x54)
    /* save NT_VBR */
    mfcr    r2, cr<1, 3>
    stw     r2, (r0, 0x58)
    /* save NT_EPSR */
    mfcr    r2, cr<2, 3>
    stw     r2, (r0, 0x5c)
    /* save NT_EPC */
    mfcr    r2, cr<4, 3>
    stw     r2, (r0, 0x60)
    /* save NT_EBR */
    mfcr    r2, cr<10,3>
    stw     r2, (r0, 0x64)
    /* save T_EBR */
    mfcr    r2, cr<1,1>
    stw     r2, (r0, 0x68)
    /* save jtag */
    mfcr    r2, cr<8, 3>
    stw     r2, (r0, 0x6c)

    ldw     r2, (r14, 0x0)
    addi    r14, 4
    stw     r14, (r0, 0x38)
    stw     r2, (r0, 0x0)
    movi    r0, 0
    jmp     r15

arch_do_cpu_resume:
    lrw     r0, g_arch_cpu_saved
    ldw     r2, (r0, 0x6c)
    mtcr    r2, cr<8, 3>
    addi    r0, 4
    ldm     r1-r15, (r0)
    subi    r0, 4
    ldw     r2, (r0, 0x40)
    mtcr    r2, psr
    ldw     r2, (r0, 0x44)
    mtcr    r2, vbr
    ldw     r2, (r0, 0x48)
    mtcr    r2, epsr
    ldw     r2, (r0, 0x4c)
    mtcr    r2, epc
    ldw     r2, (r0, 0x50)
    mtcr    r2, cr<6, 3>
    ldw     r2, (r0, 0x54)
    mtcr    r2, cr<0, 3>
    ldw     r2, (r0, 0x58)
    mtcr    r2, cr<1, 3>
    ldw     r2, (r0, 0x5c)
    mtcr    r2, cr<2, 3>
    ldw     r2, (r0, 0x60)
    mtcr    r2, cr<4, 3>
    ldw     r2, (r0, 0x64)
    mtcr    r2, cr<10, 3>
    ldw     r2, (r0, 0x68)
    mtcr    r2, cr<1, 1>
    ldw     r2, (r0, 0x8)
    ldw     r0, (r0, 0x0)
    movi    r0, 1
    jmp     r15
