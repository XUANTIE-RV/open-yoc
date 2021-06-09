    PRESERVE8
    THUMB
    AREA    |STACK_ASSERT_ASM|, CODE, READONLY
    FRAME UNWIND ON
stack_assert_asm PROC
    EXPORT stack_assert_asm
    IMPORT stack_assert_c
    PUSH {LR}
    FRAME PUSH {LR}
    MOV R3,LR
    BL stack_assert_c
    POP {PC}
    ENDP

    ALIGN
    AREA    |LS_DELAY_ASM|, CODE, READONLY
    FRAME UNWIND ON
ls_delay_asm PROC
    EXPORT ls_delay_asm
    SUBS R0,R0,#1
    CMP R0,#0
    BNE ls_delay_asm
    BX LR
    ENDP                

    END
    
