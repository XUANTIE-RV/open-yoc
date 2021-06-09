    .text

    .align 1
	.globl SWINT_Handler_ASM
	.type SWINT_Handler_ASM,%function
SWINT_Handler_ASM:
    mv a0,sp
	jal SWINT_Handler_C

    .size	SWINT_Handler_ASM, . - SWINT_Handler_ASM
