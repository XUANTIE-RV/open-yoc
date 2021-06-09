    .option rvc
    .align 1
    .globl rv32_delay_asm
    .type rv32_delay_asm,%function
rv32_delay_asm:
    sub a0,a0,a1
    bnez a0,rv32_delay_asm
    ret
    .size	rv32_delay_asm, . - rv32_delay_asm
    
    .end
    