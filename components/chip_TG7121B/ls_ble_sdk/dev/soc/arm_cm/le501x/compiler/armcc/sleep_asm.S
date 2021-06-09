    PRESERVE8
    THUMB
    AREA    |.xip_banned|, CODE, READONLY
cpu_sleep_asm PROC
    EXPORT cpu_sleep_asm
    IMPORT before_wfi
    mov r0, r8
    mov r1, r9
    mov r2, r10
    mov r3, r11
    PUSH {R0,R1,R2,R3,R4,R5,R6,R7,LR}
    MOVS R1,#0
    MOV R2,SP
    STR R2,[R1,#0]
    BL before_wfi
    WFI
    ENDP

    AREA    |.xip_banned|, CODE, READONLY
cpu_recover_asm PROC
    EXPORT cpu_recover_asm
    IMPORT after_wfi
    BL after_wfi
    POP {R0,R1,R2,R3,R4,R5,R6,R7}
    mov r8, r0
    mov r9, r1
    mov r10, r2
    mov r11, r3
    POP {PC}    
    ENDP
        
    END