/* ****************************************************************************
 *                                                                          *
 * C-Sky Microsystems Confidential                                          *
 * -------------------------------                                          *
 * This file and all its contents are properties of C-Sky Microsystems. The *
 * information contained herein is confidential and proprietary and is not  *
 * to be disclosed outside of C-Sky Microsystems except under a             *
 * Non-Disclosured Agreement (NDA).                                         *
 *                                                                          *
 ****************************************************************************
 FILE_NAME           : tee_lpm.S
 FUNCTION            : save the cpu context:
 ******************************************************************************/

#define PMU_IRAM_AON_BASE       (0x20110000)
#define WAKE_SCPU_SAVE_ADDRESS  (PMU_IRAM_AON_BASE+0x174)

.global eflash_powon
.global arch_tw_do_cpu_save
.global arch_tw_do_cpu_resume

eflash_powon:
    //br .
    lrw  r0,  0x168
    ld.w r0, (r0,0x0)
    ldw r15, (r0, 0x3c)
    ldw  r2, (r0, 0x6c)
    mtcr r2, cr<8, 3>
    //br .
    lrw  r0,  0x16c
    ld.w r0, (r0,0x0)
    ld.w r1, (r0, 0x0)
    bseti r1, 5
    st.w r1, (r0, 0x0)
    //br .
    lrw  r0,  0x170
    ld.w r0, (r0,0x0)
    ld.w r1, (r0, 0x70)
    ori  r1, r1, 7
    st.w r1, (r0, 0x70)

    movi    r0,    1
    jmp r15

arch_tw_do_cpu_save:
    subi    r14, 4
    stw r0, (r14, 0x0)
    lrw r0, WAKE_SCPU_SAVE_ADDRESS
    stw r1, (r0, 0x4)
    stw r2, (r0, 0x8)
    stw r3, (r0, 0xc)
    stw r4, (r0, 0x10)
    stw r5, (r0, 0x14)
    stw r6, (r0, 0x18)
    stw r7, (r0, 0x1c)
    stw r8, (r0, 0x20)
    stw r9, (r0, 0x24)
    stw r10, (r0, 0x28)
    stw r11, (r0, 0x2c)
    stw r12, (r0, 0x30)
    stw r13, (r0, 0x34)
    stw r15, (r0, 0x3c)
    mfcr    r2, psr
    stw r2, (r0, 0x40)
    mfcr    r2, vbr
    stw r2, (r0, 0x44)
    mfcr    r2, epsr
    stw r2, (r0, 0x48)
    mfcr    r2, epc
    stw r2, (r0, 0x4c)
    // save NT_SSP
    mfcr    r2, cr<6, 3>
    stw r2, (r0, 0x50)
    // save NT_PSR
    mfcr    r2, cr<0, 3>
    stw r2, (r0, 0x54)
    // save NT_VBR
    mfcr    r2, cr<1, 3>
    stw r2, (r0, 0x58)
    // save NT_EPSR
    mfcr    r2, cr<2, 3>
    stw r2, (r0, 0x5c)
    // save NT_EPC
    mfcr    r2, cr<4, 3>
    stw r2, (r0, 0x60)
    // save NT_EBR
    mfcr    r2, cr<10,3>
    stw r2, (r0, 0x64)
    // save T_EBR
    mfcr    r2, cr<1,1>
    stw r2, (r0, 0x68)
    //save jtag
    mfcr    r2, cr<8, 3>
    stw r2, (r0, 0x6c)

    ldw r2, (r14, 0x0)
    addi    r14, 4
    stw r14, (r0, 0x38)
    stw r2, (r0, 0x0)
    movi    r0, 0
    jmp r15

arch_tw_do_cpu_resume:
    //movi r1, 0x3
    //mtcr r1, cr<8, 3>
    lrw    r0,    WAKE_SCPU_SAVE_ADDRESS
    ldw  r2, (r0, 0x6c)
    mtcr r2, cr<8, 3>
    ldw r1, (r0, 0x4)
    ldw r3, (r0, 0xc)
    ldw r4, (r0, 0x10)
    ldw r5, (r0, 0x14)
    ldw r6, (r0, 0x18)
    ldw r7, (r0, 0x1c)
    ldw r8, (r0, 0x20)
    ldw r9, (r0, 0x24)
    ldw r10, (r0, 0x28)
    ldw r11, (r0, 0x2c)
    ldw r12, (r0, 0x30)
    ldw r13, (r0, 0x34)
    ldw r14, (r0, 0x38)
    //ldw r15, (r0, 0x3c)
    ldw r2, (r0, 0x40)
    mtcr    r2, psr
    ldw r2, (r0, 0x44)
    mtcr    r2, vbr
    ldw r2, (r0, 0x48)
    mtcr    r2, epsr
    ldw r2, (r0, 0x4c)
    mtcr    r2, epc
    ldw r2, (r0, 0x50)
    mtcr    r2, cr<6, 3>
    ldw r2, (r0, 0x54)
    mtcr    r2, cr<0, 3>
    ldw r2, (r0, 0x58)
    mtcr    r2, cr<1, 3>
    ldw r2, (r0, 0x5c)
    mtcr    r2, cr<2, 3>
    ldw r2, (r0, 0x60)
    mtcr    r2, cr<4, 3>
    ldw r2, (r0, 0x64)
    mtcr    r2, cr<10, 3>
    ldw r2, (r0, 0x68)
    mtcr    r2, cr<1, 1>
    ldw r2, (r0, 0x8)
    ldw r0, (r0, 0x0)
    //movi    r0, 1
    jmp r15

