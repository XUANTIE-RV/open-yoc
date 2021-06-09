    .syntax unified
    .arch armv6-m

    .text
    .thumb
    .thumb_func
    .align 1
    .globl int_call_base
    .type int_call_base,%function
int_call_base:
    MOVS R2,#4
    MOV R1,LR
    TST R2,R1
    BEQ stacking_used_MSP
    MRS R0,PSP
    B Int_Call_C_Routine
stacking_used_MSP:
    MRS R0,MSP
Int_Call_C_Routine:
    BX R3
    .size	int_call_base, . - int_call_base

    
	
	.globl SVC_Handler_ASM
	.type SVC_Handler_ASM,%function
SVC_Handler_ASM:
	LDR R3,=SVC_Handler_C
	B int_call_base
	
    .size	SVC_Handler_ASM, . - SVC_Handler_ASM

	.globl SWINT_Handler_ASM
	.type SWINT_Handler_ASM,%function
SWINT_Handler_ASM:
	LDR R3,=SWINT_Handler_C
	B int_call_base

    .size	SWINT_Handler_ASM, . - SWINT_Handler_ASM
