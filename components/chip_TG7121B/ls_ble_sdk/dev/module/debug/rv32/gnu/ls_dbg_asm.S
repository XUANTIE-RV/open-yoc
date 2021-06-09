    .option rvc
    .align 1
    .globl stack_assert_asm
    .type stack_assert_asm,%function
stack_assert_asm:
    mv a3,ra
    j stack_assert_c
    .size	stack_assert_asm, . - stack_assert_asm

    .end
