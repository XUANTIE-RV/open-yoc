    .syntax unified
    .arch armv6-m

    .section .xip_banned,"x"
    .thumb
    .thumb_func
    .align 1
    .globl arm_cm_delay_asm
    .type arm_cm_delay_asm,%function
arm_cm_delay_asm:
    SUBS r0,r0,0x1
    CMP r0,0x0
    BNE arm_cm_delay_asm
    BX LR
    
    .size	arm_cm_delay_asm, . - arm_cm_delay_asm
