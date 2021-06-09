    .syntax unified
    .arch armv6-m

    .section .text.SVC_Handler
    .thumb
    .thumb_func
    .align 1
    .globl SVC_Handler
    .type SVC_Handler,%function
SVC_Handler:
    MOVS R0,#4
    MOV R1,LR
    TST R0,R1
    BEQ stacking_used_MSP
    MRS R0,PSP
    B SVC_C_Routine
stacking_used_MSP:
    MRS R0,MSP
SVC_C_Routine:
    LDR R1,=SVC_Handler_C
    BX R1
    .size	SVC_Handler, . - SVC_Handler
