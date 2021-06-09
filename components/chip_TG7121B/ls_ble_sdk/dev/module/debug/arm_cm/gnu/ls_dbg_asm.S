    .syntax unified
    .arch armv6-m

    .thumb
    .thumb_func
    .align 1
    .globl stack_assert_asm
    .type stack_assert_asm,%function
stack_assert_asm:

    PUSH {LR}
    MOV R3,LR
    BL stack_assert_c
    POP {PC}

    .end
    
