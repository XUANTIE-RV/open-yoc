    .syntax unified
    .arch armv6-m
    
    .section .xip_banned,"x"
    .thumb
    .thumb_func
    .globl cpu_sleep_asm
    .type cpu_sleep_asm,%function
cpu_sleep_asm:
    mov r0, r8
    mov r1, r9
    mov r2, r10
    mov r3, r11
    PUSH {R0,R1,R2,R3,R4,R5,R6,R7,LR}
    BL store_msp
    BL before_wfi
    WFI
    .size	cpu_sleep_asm, . - cpu_sleep_asm

    .section .xip_banned,"x"
    .thumb
    .thumb_func
    .globl cpu_recover_asm
    .type cpu_recover_asm,%function
cpu_recover_asm:
    BL restore_msp
    BL after_wfi
    POP {R0,R1,R2,R3,R4,R5,R6,R7}
    mov r8, r0
    mov r9, r1
    mov r10, r2
    mov r11, r3
    POP {PC}
    .size	cpu_recover_asm, . - cpu_recover_asm

    
